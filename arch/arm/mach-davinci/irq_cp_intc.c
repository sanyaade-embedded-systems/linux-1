/*
 * TI CP INTC (common platform interrupt controller) driver
 *
 * 2008, MontaVista Software, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <asm/atomic.h>
#include <mach/hardware.h>
#include <mach/common.h>
#include <mach/irq_cp_intc.h>

/* register offsets */
#define CP_INTC_REV                    0x00
#define CP_INTC_CNTRL                  0x04
#define CP_INTC_HOST_CNTRL             0x0C
#define CP_INTC_GLOB_EN                0x10
#define CP_INTC_GLOB_NEST_EN           0x1C
#define CP_INTC_SYS_STAT_IDX_SET       0x20
#define CP_INTC_SYS_STAT_IDX_CLR       0x24
#define CP_INTC_SYS_EN_IDX_SET         0x28
#define CP_INTC_SYS_EN_IDX_CLR         0x2C
#define CP_INTC_GLB_WKUP_EN            0x30
#define CP_INTC_HOST_EN_IDX_SET        0x34
#define CP_INTC_HOST_EN_IDX_CLR        0x38
#define CP_INTC_PCR_PRESCALE           0x40
#define CP_INTC_VCTR_BASE              0x50
#define CP_INTC_VCTR_SIZE              0x54
#define CP_INTC_VCTR_NULL              0x58
#define CP_INTC_PRI_IDX                0x80
#define CP_INTC_PRI_VCTR               0x84
#define CP_INTC_SECU_EN                0x90
#define CP_INTC_SECU_PRI               0x94
#define CP_INTC_PCR_PARAM              0x100
#define CP_INTC_SYS_RAW_STAT(n)        (0x200 + (n << 2))
#define CP_INTC_SYS_STAT_CLR(n)        (0x280 + (n << 2))
#define CP_INTC_SYS_EN_SET(n)          (0x300 + (n << 2))
#define CP_INTC_SYS_EN_CLR(n)          (0x380 + (n << 2))
#define CP_INTC_CHAN_MAP(n)            (0x400 + (n << 2))
#define CP_INTC_HOST_MAP(n)            (0x800 + (n << 2))
#define CP_INTC_HOST_PRI_IDX           0x900
#define CP_INTC_SYS_POLARITY(n)        (0xD00 + (n << 2))
#define CP_INTC_SYS_TYPE(n)            (0xD80 + (n << 2))
#define CP_INTC_WKUP_EN                0xE00
#define CP_INTC_DBG_SEL                0xF00
#define CP_INTC_SYS_SECU_EN            0x1000
#define CP_INTC_HOST_NEST_LVL          0x1100
#define CP_INTC_HOST_EN(n)             (0x1500 + (n << 2))
#define CP_INTC_HOST_PRI_VCTR          0x1600
#define CP_INTC_VCTR_ADDR              0x2000

static struct cp_intc_irq_data *cp_intc_data;

static inline unsigned int cp_intc_readl(int offset)
{
	return __raw_readl(cp_intc_data->base + offset);
}

static inline void cp_intc_writel(unsigned long value, int offset)
{
	__raw_writel(value, cp_intc_data->base + offset);
}

/* Disable interrupt */
static void cp_intc_mask_irq(unsigned int irq)
{
	cp_intc_writel(cp_intc_data->host_irq, CP_INTC_HOST_EN_IDX_CLR);
	cp_intc_writel(irq, CP_INTC_SYS_EN_IDX_CLR);
	cp_intc_writel(cp_intc_data->host_irq, CP_INTC_HOST_EN_IDX_SET);
}

/* Enable interrupt */
static void cp_intc_unmask_irq(unsigned int irq)
{
	cp_intc_writel(irq, CP_INTC_SYS_EN_IDX_SET);
}

static void cp_intc_ack_irq(unsigned int irq)
{
	cp_intc_writel(irq, CP_INTC_SYS_STAT_IDX_CLR);
}

static int cp_intc_set_irq_type(unsigned int irq, unsigned int flow_type)
{
	u32 reg, mask, pol_mask, type_mask;

	reg = irq / 32;
	mask = irq % 32;

	pol_mask = cp_intc_readl(CP_INTC_SYS_POLARITY(reg));
	type_mask = cp_intc_readl(CP_INTC_SYS_TYPE(reg));

	switch (flow_type) {
	case IRQ_TYPE_EDGE_RISING:
		pol_mask |= mask;
		type_mask |= mask;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		pol_mask &= ~mask;
		type_mask |= mask;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		pol_mask |= mask;
		type_mask &= ~mask;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		pol_mask &= ~mask;
		type_mask &= ~mask;
		break;
	default:
		return -1;
	}

	cp_intc_writel(pol_mask, CP_INTC_SYS_POLARITY(reg));
	cp_intc_writel(type_mask, CP_INTC_SYS_TYPE(reg));
	return 0;
}

static struct irq_chip cp_intc_irq_chip = {
	.name           = "CP_INTC",
	.ack            = cp_intc_ack_irq,
	.mask           = cp_intc_mask_irq,
	.unmask         = cp_intc_unmask_irq,
	.set_type       = cp_intc_set_irq_type,
};

/* FIQ are pri 0-1; otherwise 2-7, with 7 lowest priority */
static u8 omapl1xx_default_priorities[DAVINCI_N_AINTC_IRQ] = {
	[IRQ_DA8XX_COMMTX]              = 7,
	[IRQ_DA8XX_COMMRX]              = 7,
	[IRQ_DA8XX_NINT]                = 7,
	[IRQ_DA8XX_EVTOUT0]             = 7,
	[IRQ_DA8XX_EVTOUT1]             = 7,
	[IRQ_DA8XX_EVTOUT2]             = 7,
	[IRQ_DA8XX_EVTOUT3]             = 7,
	[IRQ_DA8XX_EVTOUT4]             = 7,
	[IRQ_DA8XX_EVTOUT5]             = 7,
	[IRQ_DA8XX_EVTOUT6]             = 7,
	[IRQ_DA8XX_EVTOUT7]             = 7,
	[IRQ_DA8XX_CCINT0]              = 7,
	[IRQ_DA8XX_CCERRINT]            = 7,
	[IRQ_DA8XX_TCERRINT0]           = 7,
	[IRQ_DA8XX_AEMIFINT]            = 7,
	[IRQ_DA8XX_I2CINT0]             = 7,
	[IRQ_DA8XX_MMCSDINT0]           = 7,
	[IRQ_DA8XX_MMCSDINT1]           = 7,
	[IRQ_DA8XX_ALLINT0]             = 7,
	[IRQ_DA8XX_RTC]                 = 7,
	[IRQ_DA8XX_SPINT0]              = 7,
	[IRQ_DA8XX_TINT12_0]            = 7,
	[IRQ_DA8XX_TINT34_0]            = 7,
	[IRQ_DA8XX_TINT12_1]            = 7,
	[IRQ_DA8XX_TINT34_1]            = 7,
	[IRQ_DA8XX_UARTINT0]            = 7,
	[IRQ_DA8XX_KEYMGRINT]           = 7,
	[IRQ_DA8XX_SECINT]              = 7,
	[IRQ_DA8XX_SECKEYERR]           = 7,
	[IRQ_DA830_MPUERR]              = 7,
	[IRQ_DA830_IOPUERR]             = 7,
	[IRQ_DA830_BOOTCFGERR]          = 7,
	[IRQ_DA8XX_CHIPINT0]            = 7,
	[IRQ_DA8XX_CHIPINT1]            = 7,
	[IRQ_DA8XX_CHIPINT2]            = 7,
	[IRQ_DA8XX_CHIPINT3]            = 7,
	[IRQ_DA8XX_TCERRINT1]           = 7,
	[IRQ_DA8XX_C0_RX_THRESH_PULSE]  = 7,
	[IRQ_DA8XX_C0_RX_PULSE]         = 7,
	[IRQ_DA8XX_C0_TX_PULSE]         = 7,
	[IRQ_DA8XX_C0_MISC_PULSE]       = 7,
	[IRQ_DA8XX_C1_RX_THRESH_PULSE]  = 7,
	[IRQ_DA8XX_C1_RX_PULSE]         = 7,
	[IRQ_DA8XX_C1_TX_PULSE]         = 7,
	[IRQ_DA8XX_C1_MISC_PULSE]       = 7,
	[IRQ_DA8XX_MEMERR]              = 7,
	[IRQ_DA8XX_GPIO0]               = 7,
	[IRQ_DA8XX_GPIO1]               = 7,
	[IRQ_DA8XX_GPIO2]               = 7,
	[IRQ_DA8XX_GPIO3]               = 7,
	[IRQ_DA8XX_GPIO4]               = 7,
	[IRQ_DA8XX_GPIO5]               = 7,
	[IRQ_DA8XX_GPIO6]               = 7,
	[IRQ_DA8XX_GPIO7]               = 7,
	[IRQ_DA8XX_GPIO8]               = 7,
	[IRQ_DA8XX_I2CINT1]             = 7,
	[IRQ_DA8XX_LCDINT]              = 7,
	[IRQ_DA8XX_UARTINT1]            = 7,
	[IRQ_DA8XX_MCASPINT]            = 7,
	[IRQ_DA8XX_ALLINT1]             = 7,
	[IRQ_DA8XX_SPINT1]              = 7,
	[IRQ_DA8XX_UHPI_INT1]           = 7,
	[IRQ_DA8XX_USB_INT]             = 7,
	[IRQ_DA8XX_IRQN]                = 7,
	[IRQ_DA8XX_RWAKEUP]             = 7,
	[IRQ_DA8XX_UARTINT2]            = 7,
	[IRQ_DA8XX_DFTSSINT]            = 7,
	[IRQ_DA8XX_EHRPWM0]             = 7,
	[IRQ_DA8XX_EHRPWM0TZ]           = 7,
	[IRQ_DA8XX_EHRPWM1]             = 7,
	[IRQ_DA8XX_EHRPWM1TZ]           = 7,
	[IRQ_DA830_EHRPWM2]             = 7,
	[IRQ_DA830_EHRPWM2TZ]           = 7,
	[IRQ_DA8XX_ECAP0]               = 7,
	[IRQ_DA8XX_ECAP1]               = 7,
	[IRQ_DA8XX_ECAP2]               = 7,
	[IRQ_DA830_EQEP0]               = 7,
	[IRQ_DA830_EQEP1]               = 7,
	[IRQ_DA830_T12CMPINT0_0]        = 7,
	[IRQ_DA830_T12CMPINT1_0]        = 7,
	[IRQ_DA830_T12CMPINT2_0]        = 7,
	[IRQ_DA830_T12CMPINT3_0]        = 7,
	[IRQ_DA830_T12CMPINT4_0]        = 7,
	[IRQ_DA830_T12CMPINT5_0]        = 7,
	[IRQ_DA830_T12CMPINT6_0]        = 7,
	[IRQ_DA830_T12CMPINT7_0]        = 7,
	[IRQ_DA830_T12CMPINT0_1]        = 7,
	[IRQ_DA830_T12CMPINT1_1]        = 7,
	[IRQ_DA830_T12CMPINT2_1]        = 7,
	[IRQ_DA830_T12CMPINT3_1]        = 7,
	[IRQ_DA830_T12CMPINT4_1]        = 7,
	[IRQ_DA830_T12CMPINT5_1]        = 7,
	[IRQ_DA830_T12CMPINT6_1]        = 7,
	[IRQ_DA830_T12CMPINT7_1]        = 7,
	[IRQ_DA8XX_ARMCLKSTOPREQ]       = 7,
};

void __init cp_intc_init(struct cp_intc_irq_data *irq_data)
{
	int i, j;
	int num_reg;
	int count;
	u8 *irq_prio;
	u32 val;

	irq_prio = omapl1xx_default_priorities;

	cp_intc_data = irq_data;

	cp_intc_writel(0, CP_INTC_GLOB_EN);

	/* disable all host interrupts */
	for (i = 0; i <= cp_intc_data->num_host_irqs; i++)
		cp_intc_writel(0, CP_INTC_HOST_EN(i));

	/* disable system interrupts */
	num_reg = cp_intc_data->max_irq >> 5; /* 32 system ints per register */
	for (i = 0; i <= num_reg; i++)
		cp_intc_writel(0, CP_INTC_SYS_EN_CLR(i));

	/* set to normal mode, no nesting, no priority hold */
	cp_intc_writel(0, CP_INTC_CNTRL);
	cp_intc_writel(0, CP_INTC_HOST_CNTRL);

	/* clear system interrupt status */
	num_reg = cp_intc_data->max_irq >> 5; /* 32 status ints per register */
	for (i = 0; i <= num_reg; i++)
		cp_intc_writel(0xffffffff, CP_INTC_SYS_STAT_CLR(i));

	/* enable host interrupt */
	cp_intc_writel(cp_intc_data->host_irq, CP_INTC_HOST_EN_IDX_SET);

	/* priority is determined by host channel.  Lower channel number
	   has higher priority i.e. chan 0 has highest priority and channel
	   31 had the lowest priority */
	num_reg = cp_intc_data->max_irq >> 2; /* 4 channels per register */
	if (irq_prio) {
		count = 0;
		for (i = 0; i < num_reg; i++) {
			val = 0;
			for (j = 0; j < 4; j++, count++)
				val = (val << 8) |
			(irq_prio[count] & 0x1F);

			cp_intc_writel(val, CP_INTC_CHAN_MAP(i));
		}
	} else {
		/* default everything to channel 16 if prioirty not specified */
		/* Note that channel 0-1 are mapped to FIQ and channels 2-31
		   are mapped to IRQ */
		for (i = 0; i < num_reg; i++)
			cp_intc_writel(0x0f0f0f0f, CP_INTC_CHAN_MAP(i));
	}

	/* set up genirq dispatch for ARM INTC */
	for (i = 0; i < cp_intc_data->max_irq; i++) {
		set_irq_chip(i, &cp_intc_irq_chip);
		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
		set_irq_handler(i, handle_edge_irq);
	}

	/* enable global interrupt */
	cp_intc_writel(1, CP_INTC_GLOB_EN);
}
