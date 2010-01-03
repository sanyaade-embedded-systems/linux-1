/*
 * arch/arm/mach-omap2/serial.c
 *
 * OMAP2 serial support.
 *
 * Copyright (C) 2005-2008 Nokia Corporation
 * Author: Paul Mundt <paul.mundt@nokia.com>
 *
 * Major rework for PM support by Kevin Hilman
 *
 * Based off of arch/arm/mach-omap/omap1/serial.c
 *
 * Copyright (C) 2009 Texas Instruments
 * Added OMAP4 support - Santosh Shilimkar <santosh.shilimkar@ti.com
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <mach/common.h>
#include <mach/board.h>
#include <mach/clock.h>
#include <mach/control.h>

#ifdef CONFIG_SERIAL_OMAP
#include <mach/dma.h>
#include <mach/omap-serial.h>
#endif

#include "prm.h"
#include "pm.h"
#include "prm-regbits-34xx.h"

#define UART_OMAP_NO_EMPTY_FIFO_READ_IP_REV	0x52
#define UART_OMAP_WER		0x17	/* Wake-up enable register */

#define DEFAULT_TIMEOUT (5 * HZ)

#ifdef CONFIG_SERIAL_8250
struct omap_uart_state {
	int num;
	int can_sleep;
	struct timer_list timer;
	u32 timeout;

	void __iomem *wk_st;
	void __iomem *wk_en;
	u32 wk_mask;
	u32 padconf;

	struct clk *ick;
	struct clk *fck;
	int clocked;

	struct plat_serial8250_port *p;
	struct list_head node;
	struct platform_device pdev;

#if defined(CONFIG_ARCH_OMAP3) && defined(CONFIG_PM)
	int context_valid;

	/* Registers to be saved/restored for OFF-mode */
	u16 dll;
	u16 dlh;
	u16 ier;
	u16 sysc;
	u16 scr;
	u16 wer;
#endif
};

static LIST_HEAD(uart_list);

static struct plat_serial8250_port serial_platform_data0[] = {
	{
		.membase	= OMAP2_L4_IO_ADDRESS(OMAP_UART1_BASE),
		.mapbase	= OMAP_UART1_BASE,
		.irq		= 72,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP24XX_BASE_BAUD * 16,
	}, {
		.flags		= 0
	}
};

static struct plat_serial8250_port serial_platform_data1[] = {
	{
		.membase	= OMAP2_L4_IO_ADDRESS(OMAP_UART2_BASE),
		.mapbase	= OMAP_UART2_BASE,
		.irq		= 73,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP24XX_BASE_BAUD * 16,
	}, {
		.flags		= 0
	}
};

static struct plat_serial8250_port serial_platform_data2[] = {
	{
		.membase	= OMAP2_L4_IO_ADDRESS(OMAP_UART3_BASE),
		.mapbase	= OMAP_UART3_BASE,
		.irq		= 74,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP24XX_BASE_BAUD * 16,
	}, {
		.flags		= 0
	}
};
#ifdef CONFIG_ARCH_OMAP4
static struct plat_serial8250_port serial_platform_data3[] = {
	{
		.membase	= OMAP2_L4_IO_ADDRESS(OMAP_UART4_BASE),
		.mapbase	= OMAP_UART4_BASE,
		.irq		= 70,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP24XX_BASE_BAUD * 16,
	}, {
		.flags		= 0
	}
};
#endif
static inline unsigned int serial_read_reg(struct plat_serial8250_port *up,
					   int offset)
{
	offset <<= up->regshift;
	return (unsigned int)__raw_readb(up->membase + offset);
}

static inline void serial_write_reg(struct plat_serial8250_port *p, int offset,
				    int value)
{
	offset <<= p->regshift;
	__raw_writeb(value, p->membase + offset);
}

/*
 * Internal UARTs need to be initialized for the 8250 autoconfig to work
 * properly. Note that the TX watermark initialization may not be needed
 * once the 8250.c watermark handling code is merged.
 */
static inline void __init omap_uart_reset(struct omap_uart_state *uart)
{
	struct plat_serial8250_port *p = uart->p;

	serial_write_reg(p, UART_OMAP_MDR1, 0x07);
	serial_write_reg(p, UART_OMAP_SCR, 0x08);
	serial_write_reg(p, UART_OMAP_MDR1, 0x00);
	serial_write_reg(p, UART_OMAP_SYSC, (0x02 << 3) | (1 << 2) | (1 << 0));
}

#if defined(CONFIG_PM) && defined(CONFIG_ARCH_OMAP3)

static int enable_off_mode; /* to be removed by full off-mode patches */

static void omap_uart_save_context(struct omap_uart_state *uart)
{
	u16 lcr = 0;
	struct plat_serial8250_port *p = uart->p;

	if (!enable_off_mode)
		return;

	lcr = serial_read_reg(p, UART_LCR);
	serial_write_reg(p, UART_LCR, 0xBF);
	uart->dll = serial_read_reg(p, UART_DLL);
	uart->dlh = serial_read_reg(p, UART_DLM);
	serial_write_reg(p, UART_LCR, lcr);
	uart->ier = serial_read_reg(p, UART_IER);
	uart->sysc = serial_read_reg(p, UART_OMAP_SYSC);
	uart->scr = serial_read_reg(p, UART_OMAP_SCR);
	uart->wer = serial_read_reg(p, UART_OMAP_WER);

	uart->context_valid = 1;
}

static void omap_uart_restore_context(struct omap_uart_state *uart)
{
	u16 efr = 0;
	struct plat_serial8250_port *p = uart->p;

	if (!enable_off_mode)
		return;

	if (!uart->context_valid)
		return;

	uart->context_valid = 0;

	serial_write_reg(p, UART_OMAP_MDR1, 0x7);
	serial_write_reg(p, UART_LCR, 0xBF); /* Config B mode */
	efr = serial_read_reg(p, UART_EFR);
	serial_write_reg(p, UART_EFR, UART_EFR_ECB);
	serial_write_reg(p, UART_LCR, 0x0); /* Operational mode */
	serial_write_reg(p, UART_IER, 0x0);
	serial_write_reg(p, UART_LCR, 0xBF); /* Config B mode */
	serial_write_reg(p, UART_DLL, uart->dll);
	serial_write_reg(p, UART_DLM, uart->dlh);
	serial_write_reg(p, UART_LCR, 0x0); /* Operational mode */
	serial_write_reg(p, UART_IER, uart->ier);
	serial_write_reg(p, UART_FCR, 0xA1);
	serial_write_reg(p, UART_LCR, 0xBF); /* Config B mode */
	serial_write_reg(p, UART_EFR, efr);
	serial_write_reg(p, UART_LCR, UART_LCR_WLEN8);
	serial_write_reg(p, UART_OMAP_SCR, uart->scr);
	serial_write_reg(p, UART_OMAP_WER, uart->wer);
	serial_write_reg(p, UART_OMAP_SYSC, uart->sysc);
	serial_write_reg(p, UART_OMAP_MDR1, 0x00); /* UART 16x mode */
}
#else
static inline void omap_uart_save_context(struct omap_uart_state *uart) {}
static inline void omap_uart_restore_context(struct omap_uart_state *uart) {}
#endif /* CONFIG_PM && CONFIG_ARCH_OMAP3 */

static inline void omap_uart_enable_clocks(struct omap_uart_state *uart)
{
	if (uart->clocked)
		return;

	clk_enable(uart->ick);
	clk_enable(uart->fck);
	uart->clocked = 1;
	omap_uart_restore_context(uart);
}

#ifdef CONFIG_PM

static inline void omap_uart_disable_clocks(struct omap_uart_state *uart)
{
	if (!uart->clocked)
		return;

	omap_uart_save_context(uart);
	uart->clocked = 0;
	clk_disable(uart->ick);
	clk_disable(uart->fck);
}

static void omap_uart_enable_wakeup(struct omap_uart_state *uart)
{
	/* Set wake-enable bit */
	if (uart->wk_en && uart->wk_mask) {
		u32 v = __raw_readl(uart->wk_en);
		v |= uart->wk_mask;
		__raw_writel(v, uart->wk_en);
	}

	/* Ensure IOPAD wake-enables are set */
	if (cpu_is_omap34xx() && uart->padconf) {
		u16 v = omap_ctrl_readw(uart->padconf);
		v |= OMAP3_PADCONF_WAKEUPENABLE0;
		omap_ctrl_writew(v, uart->padconf);
	}
}

static void omap_uart_disable_wakeup(struct omap_uart_state *uart)
{
	/* Clear wake-enable bit */
	if (uart->wk_en && uart->wk_mask) {
		u32 v = __raw_readl(uart->wk_en);
		v &= ~uart->wk_mask;
		__raw_writel(v, uart->wk_en);
	}

	/* Ensure IOPAD wake-enables are cleared */
	if (cpu_is_omap34xx() && uart->padconf) {
		u16 v = omap_ctrl_readw(uart->padconf);
		v &= ~OMAP3_PADCONF_WAKEUPENABLE0;
		omap_ctrl_writew(v, uart->padconf);
	}
}

static void omap_uart_smart_idle_enable(struct omap_uart_state *uart,
					  int enable)
{
	struct plat_serial8250_port *p = uart->p;
	u16 sysc;

	sysc = serial_read_reg(p, UART_OMAP_SYSC) & 0x7;
	if (enable)
		sysc |= 0x2 << 3;
	else
		sysc |= 0x1 << 3;

	serial_write_reg(p, UART_OMAP_SYSC, sysc);
}

static void omap_uart_block_sleep(struct omap_uart_state *uart)
{
	omap_uart_enable_clocks(uart);

	omap_uart_smart_idle_enable(uart, 0);
	uart->can_sleep = 0;
	if (uart->timeout)
		mod_timer(&uart->timer, jiffies + uart->timeout);
	else
		del_timer(&uart->timer);
}

static void omap_uart_allow_sleep(struct omap_uart_state *uart)
{
	if (device_may_wakeup(&uart->pdev.dev))
		omap_uart_enable_wakeup(uart);
	else
		omap_uart_disable_wakeup(uart);

	if (!uart->clocked)
		return;

	omap_uart_smart_idle_enable(uart, 1);
	uart->can_sleep = 1;
	del_timer(&uart->timer);
}

static void omap_uart_idle_timer(unsigned long data)
{
	struct omap_uart_state *uart = (struct omap_uart_state *)data;

	omap_uart_allow_sleep(uart);
}

void omap_uart_prepare_idle(int num)
{
	struct omap_uart_state *uart;

	list_for_each_entry(uart, &uart_list, node) {
		if (num == uart->num && uart->can_sleep) {
			omap_uart_disable_clocks(uart);
			return;
		}
	}
}

void omap_uart_resume_idle(int num)
{
	struct omap_uart_state *uart;

	list_for_each_entry(uart, &uart_list, node) {
		if (num == uart->num) {
			omap_uart_enable_clocks(uart);

			/* Check for IO pad wakeup */
			if (cpu_is_omap34xx() && uart->padconf) {
				u16 p = omap_ctrl_readw(uart->padconf);

				if (p & OMAP3_PADCONF_WAKEUPEVENT0)
					omap_uart_block_sleep(uart);
			}

			/* Check for normal UART wakeup */
			if (__raw_readl(uart->wk_st) & uart->wk_mask)
				omap_uart_block_sleep(uart);
			return;
		}
	}
}

void omap_uart_prepare_suspend(void)
{
	struct omap_uart_state *uart;

	list_for_each_entry(uart, &uart_list, node) {
		omap_uart_allow_sleep(uart);
	}
}

int omap_uart_can_sleep(void)
{
	struct omap_uart_state *uart;
	int can_sleep = 1;

	list_for_each_entry(uart, &uart_list, node) {
		if (!uart->clocked)
			continue;

		if (!uart->can_sleep) {
			can_sleep = 0;
			continue;
		}

		/* This UART can now safely sleep. */
		omap_uart_allow_sleep(uart);
	}

	return can_sleep;
}

/**
 * omap_uart_interrupt()
 *
 * This handler is used only to detect that *any* UART interrupt has
 * occurred.  It does _nothing_ to handle the interrupt.  Rather,
 * any UART interrupt will trigger the inactivity timer so the
 * UART will not idle or sleep for its timeout period.
 *
 **/
static irqreturn_t omap_uart_interrupt(int irq, void *dev_id)
{
	struct omap_uart_state *uart = dev_id;

	omap_uart_block_sleep(uart);

	return IRQ_NONE;
}

static void omap_uart_idle_init(struct omap_uart_state *uart)
{
	struct plat_serial8250_port *p = uart->p;
	int ret;

	uart->can_sleep = 0;
	uart->timeout = DEFAULT_TIMEOUT;
	setup_timer(&uart->timer, omap_uart_idle_timer,
		    (unsigned long) uart);
	mod_timer(&uart->timer, jiffies + uart->timeout);
	omap_uart_smart_idle_enable(uart, 0);

	if (cpu_is_omap34xx()) {
		u32 mod = (uart->num == 2) ? OMAP3430_PER_MOD : CORE_MOD;
		u32 wk_mask = 0;
		u32 padconf = 0;

		uart->wk_en = OMAP34XX_PRM_REGADDR(mod, PM_WKEN1);
		uart->wk_st = OMAP34XX_PRM_REGADDR(mod, PM_WKST1);
		switch (uart->num) {
		case 0:
			wk_mask = OMAP3430_ST_UART1_MASK;
			padconf = 0x182;
			break;
		case 1:
			wk_mask = OMAP3430_ST_UART2_MASK;
			padconf = 0x17a;
			break;
		case 2:
			wk_mask = OMAP3430_ST_UART3_MASK;
			padconf = 0x19e;
			break;
		}
		uart->wk_mask = wk_mask;
		uart->padconf = padconf;
	} else if (cpu_is_omap24xx()) {
		u32 wk_mask = 0;

		if (cpu_is_omap2430()) {
			uart->wk_en = OMAP2430_PRM_REGADDR(CORE_MOD, PM_WKEN1);
			uart->wk_st = OMAP2430_PRM_REGADDR(CORE_MOD, PM_WKST1);
		} else if (cpu_is_omap2420()) {
			uart->wk_en = OMAP2420_PRM_REGADDR(CORE_MOD, PM_WKEN1);
			uart->wk_st = OMAP2420_PRM_REGADDR(CORE_MOD, PM_WKST1);
		}
		switch (uart->num) {
		case 0:
			wk_mask = OMAP24XX_ST_UART1_MASK;
			break;
		case 1:
			wk_mask = OMAP24XX_ST_UART2_MASK;
			break;
		case 2:
			wk_mask = OMAP24XX_ST_UART3_MASK;
			break;
		}
		uart->wk_mask = wk_mask;
	} else {
		uart->wk_en = 0;
		uart->wk_st = 0;
		uart->wk_mask = 0;
		uart->padconf = 0;
	}

	p->flags |= UPF_SHARE_IRQ;
	ret = request_irq(p->irq, omap_uart_interrupt, IRQF_SHARED,
			  "serial idle", (void *)uart);
	WARN_ON(ret);
}

void omap_uart_enable_irqs(int enable)
{
	int ret;
	struct omap_uart_state *uart;

	list_for_each_entry(uart, &uart_list, node) {
		if (enable)
			ret = request_irq(uart->p->irq, omap_uart_interrupt,
				IRQF_SHARED, "serial idle", (void *)uart);
		else
			free_irq(uart->p->irq, (void *)uart);
	}
}

static ssize_t sleep_timeout_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct omap_uart_state *uart = container_of(pdev,
					struct omap_uart_state, pdev);

	return sprintf(buf, "%u\n", uart->timeout / HZ);
}

static ssize_t sleep_timeout_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t n)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct omap_uart_state *uart = container_of(pdev,
					struct omap_uart_state, pdev);
	unsigned int value;

	if (sscanf(buf, "%u", &value) != 1) {
		printk(KERN_ERR "sleep_timeout_store: Invalid value\n");
		return -EINVAL;
	}

	uart->timeout = value * HZ;
	if (uart->timeout)
		mod_timer(&uart->timer, jiffies + uart->timeout);
	else
		/* A zero value means disable timeout feature */
		omap_uart_block_sleep(uart);

	return n;
}

DEVICE_ATTR(sleep_timeout, 0644, sleep_timeout_show, sleep_timeout_store);
#define DEV_CREATE_FILE(dev, attr) WARN_ON(device_create_file(dev, attr))
#else
static inline void omap_uart_idle_init(struct omap_uart_state *uart) {}
#define DEV_CREATE_FILE(dev, attr)
#endif /* CONFIG_PM */

static struct omap_uart_state omap_uart[OMAP_MAX_NR_PORTS] = {
	{
		.pdev = {
			.name			= "serial8250",
			.id			= PLAT8250_DEV_PLATFORM,
			.dev			= {
				.platform_data	= serial_platform_data0,
			},
		},
	}, {
		.pdev = {
			.name			= "serial8250",
			.id			= PLAT8250_DEV_PLATFORM1,
			.dev			= {
				.platform_data	= serial_platform_data1,
			},
		},
	}, {
		.pdev = {
			.name			= "serial8250",
			.id			= PLAT8250_DEV_PLATFORM2,
			.dev			= {
				.platform_data	= serial_platform_data2,
			},
		},
	},
#ifdef CONFIG_ARCH_OMAP4
	{
		.pdev = {
			.name			= "serial8250",
			.id			= 3,
			.dev			= {
				.platform_data	= serial_platform_data3,
			},
		},
	},
#endif
};

/*
 * Override the default 8250 read handler: mem_serial_in()
 * Empty RX fifo read causes an abort on omap3630 and omap4
 * This function makes sure that an empty rx fifo is not read on these silicons
 * (OMAP1/2/3430 are not affected)
 */
static unsigned int serial_in_override(struct uart_port *up, int offset)
{
	if (UART_RX == offset) {
	unsigned int lsr;
	lsr = serial_read_reg(omap_uart[up->line].p, UART_LSR);
	if (!(lsr & UART_LSR_DR))
	return -EPERM;
	}
	return serial_read_reg(omap_uart[up->line].p, offset);
 }
static void serial_out_override(struct uart_port *up, int offset, int value)
{
	unsigned int status, tmout = 10000;
	/* Wait up to 10ms for the character(s) to be sent. */
	do {
		status = serial_read_reg(omap_uart[up->line].p, UART_LSR);
		if (--tmout == 0)
			break;
		udelay(1);
	} while (!(status & UART_LSR_THRE));
	serial_write_reg(omap_uart[up->line].p, offset, value);
}
void __init omap_serial_init(void)
{
	int i;
	const struct omap_uart_config *info;
	char name[16];

	/*
	 * Make sure the serial ports are muxed on at this point.
	 * You have to mux them off in device drivers later on
	 * if not needed.
	 */

	info = omap_get_config(OMAP_TAG_UART, struct omap_uart_config);

	if (info == NULL)
		return;

	for (i = 0; i < OMAP_MAX_NR_PORTS; i++) {
		struct omap_uart_state *uart = &omap_uart[i];
		struct platform_device *pdev = &uart->pdev;
		struct device *dev = &pdev->dev;
		struct plat_serial8250_port *p = dev->platform_data;

		if (!(info->enabled_uarts & (1 << i))) {
			p->membase = NULL;
			p->mapbase = 0;
			continue;
		}

		sprintf(name, "uart%d_ick", i+1);
		uart->ick = clk_get(NULL, name);
		if (IS_ERR(uart->ick)) {
			printk(KERN_ERR "Could not get uart%d_ick\n", i+1);
			uart->ick = NULL;
		}

		sprintf(name, "uart%d_fck", i+1);
		uart->fck = clk_get(NULL, name);
		if (IS_ERR(uart->fck)) {
			printk(KERN_ERR "Could not get uart%d_fck\n", i+1);
			uart->fck = NULL;
		}

		/* FIXME: Remove this once the clkdev is ready */
		if (!cpu_is_omap44xx()) {
			if (!uart->ick || !uart->fck)
				continue;
		}

		uart->num = i;
		p->private_data = uart;
		uart->p = p;
		list_add_tail(&uart->node, &uart_list);

		if (cpu_is_omap44xx())
			p->irq += 32;

		omap_uart_enable_clocks(uart);
		omap_uart_reset(uart);
		omap_uart_idle_init(uart);

		if (WARN_ON(platform_device_register(pdev)))
			continue;
		if ((cpu_is_omap34xx() && uart->padconf) ||
		    (uart->wk_en && uart->wk_mask)) {
			device_init_wakeup(dev, true);
			DEV_CREATE_FILE(dev, &dev_attr_sleep_timeout);
		}

		/* omap44xx: Never read empty UART fifo
		 * omap3xxx: Never read empty UART fifo on UARTs
		 * with IP rev >=0x52
		 */
		if (cpu_is_omap44xx()) {
			uart->p->serial_in = serial_in_override;
			uart->p->serial_out = serial_out_override;
		}
		else if ((serial_read_reg(uart->p, UART_OMAP_MVER) & 0xFF)
				>= UART_OMAP_NO_EMPTY_FIFO_READ_IP_REV) {
			uart->p->serial_in = serial_in_override;
			uart->p->serial_out = serial_out_override;
		}
	}
}
#endif

#ifdef CONFIG_SERIAL_OMAP
static struct uart_dma_info uart1_dma_state = {
#ifdef CONFIG_SERIAL_OMAP_DMA_UART1
		.dma_enabled	= 1,
		.rx_dma_bufsize	=
			CONFIG_SERIAL_OMAP_UART1_RXDMA_BUFSIZE,
		.rx_timeout	=
			CONFIG_SERIAL_OMAP_UART1_RXDMA_TIMEOUT,
#endif
};

static struct uart_dma_info uart2_dma_state = {
#ifdef CONFIG_SERIAL_OMAP_DMA_UART2
		.dma_enabled	= 1,
		.rx_dma_bufsize	=
			CONFIG_SERIAL_OMAP_UART2_RXDMA_BUFSIZE,
		.rx_timeout	=
			CONFIG_SERIAL_OMAP_UART2_RXDMA_TIMEOUT,
#endif
};

static struct uart_dma_info uart3_dma_state = {
#ifdef CONFIG_SERIAL_OMAP_DMA_UART3
		.dma_enabled	= 1,
		.rx_dma_bufsize	=
			CONFIG_SERIAL_OMAP_UART3_RXDMA_BUFSIZE,
		.rx_timeout	=
			CONFIG_SERIAL_OMAP_UART3_RXDMA_TIMEOUT,
#endif
};


#ifdef CONFIG_ARCH_OMAP4
static struct uart_dma_info uart4_dma_state = {
#ifdef CONFIG_SERIAL_OMAP_DMA_UART4
		.dma_enabled	= 1,
		.rx_dma_bufsize	=
			CONFIG_SERIAL_OMAP_UART4_RXDMA_BUFSIZE,
		.rx_timeout	=
			CONFIG_SERIAL_OMAP_UART4_RXDMA_TIMEOUT,
#endif
};
#endif

static struct resource omap2_uart1_resources[] = {
	{
		.start          = OMAP_UART1_BASE,
		.end            = OMAP_UART1_BASE + 0x3ff,
		.flags          = IORESOURCE_MEM,
	}, {
		/* UART1 IRQ - 72*/
		.start          = INT_24XX_UART1_IRQ,
		.flags          = IORESOURCE_IRQ,
	}, {
		/* UART1 TX DMA CHANNEL -S_DMA_48- */
		.start		= OMAP24XX_DMA_UART1_TX,
		.flags		= IORESOURCE_DMA,
	}, {
		/* UART1 RX DMA CHANNEL -S_DMA_49- */
		.start		= OMAP24XX_DMA_UART1_RX,
		.flags		= IORESOURCE_DMA,
	}
};

static struct resource omap2_uart2_resources[] = {
	{
		.start          = OMAP_UART2_BASE,
		.end            = OMAP_UART2_BASE + 0x3ff,
		.flags          = IORESOURCE_MEM,
	}, {
		/* UART2 IRQ - 73*/
		.start          = INT_24XX_UART2_IRQ,
		.flags          = IORESOURCE_IRQ,
	}, {
		/* UART2 TX DMA CHANNEL -S_DMA_50- */
		.start		= OMAP24XX_DMA_UART2_TX,
		.flags		= IORESOURCE_DMA,
	}, {
		/* UART2 RX DMA CHANNEL -S_DMA_51- */
		.start		= OMAP24XX_DMA_UART2_RX,
		.flags		= IORESOURCE_DMA,
	}
};

static struct resource omap2_uart3_resources[] = {
	{
		.start          = OMAP_UART3_BASE,
		.end            = OMAP_UART3_BASE + 0x3ff,
		.flags          = IORESOURCE_MEM,
	}, {
		/* UART3 IRQ - 74*/
		.start          = INT_24XX_UART3_IRQ,
		.flags          = IORESOURCE_IRQ,
	}, {
		/* UART3 TX DMA CHANNEL -S_DMA_52- */
		.start		= OMAP24XX_DMA_UART3_TX,
		.flags		= IORESOURCE_DMA,
	}, {
		/* UART3 RX DMA CHANNEL -S_DMA_53- */
		.start		= OMAP24XX_DMA_UART3_RX,
		.flags		= IORESOURCE_DMA,
	}
};

#ifdef CONFIG_ARCH_OMAP4
static struct resource omap2_uart4_resources[] = {
	{
		.start          = OMAP_UART4_BASE,
		.end            = OMAP_UART4_BASE + 0x3ff,
		.flags          = IORESOURCE_MEM,
	}, {
		/* UART4 IRQ */
		.start          = 70,
		.flags          = IORESOURCE_IRQ,
	}, {
		/* UART4 TX DMA CHANNEL -S_DMA_54- */
		.start		= OMAP44XX_DMA_UART4_DMA_TX,
		.flags		= IORESOURCE_DMA,
	}, {
		/* UART4 RX DMA CHANNEL -S_DMA_55- */
		.start		= OMAP44XX_DMA_UART4_DMA_RX,
		.flags		= IORESOURCE_DMA,
	}
};
#endif

/* OMAP UART platform structure */
static struct platform_device uart1_device = {
	.name			= "omap-uart",
	.id			= 1,
	.num_resources		= ARRAY_SIZE(omap2_uart1_resources),
	.resource		= omap2_uart1_resources,
	.dev			= {
		.platform_data  = &uart1_dma_state,
	},
};
static struct platform_device uart2_device = {
	.name			= "omap-uart",
	.id			= 2,
	.num_resources		= ARRAY_SIZE(omap2_uart2_resources),
	.resource		= omap2_uart2_resources,
	.dev			= {
		.platform_data  = &uart2_dma_state,
	},
};
static struct platform_device uart3_device = {
	.name			= "omap-uart",
	.id			= 3,
	.num_resources		= ARRAY_SIZE(omap2_uart3_resources),
	.resource		= omap2_uart3_resources,
	.dev			= {
		.platform_data  = &uart3_dma_state,
	},
};

#ifdef CONFIG_ARCH_OMAP4
static struct platform_device uart4_device = {
	.name			= "omap-uart",
	.id			= 4,
	.num_resources		= ARRAY_SIZE(omap2_uart4_resources),
	.resource		= omap2_uart4_resources,
	.dev			= {
		.platform_data  = &uart4_dma_state,
	},
};
#endif

static struct platform_device *uart_devices[] = {
	&uart1_device,
	&uart2_device,
	&uart3_device,
#ifdef CONFIG_ARCH_OMAP4
	&uart4_device,
#endif
};

void __init omap_serial_init(void)
{
	int ret = 0, i;

	/*
	 * FIX-ME: Replace with correct clk node when clk
	 * framework is available
	 */
	if (!cpu_is_omap44xx())
		omap_serial_early_init();

	for (i = 0; i < OMAP_MAX_NR_PORTS; i++)
		omap_uart_idle_init(i);

	ret = platform_add_devices(uart_devices, ARRAY_SIZE(uart_devices));
}
#endif
