#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/interrupt.h>


/*The #defines and #includes below can be removed when merged with dsi.c*/

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/kthread.h>
#include <linux/wait.h>

#include <mach/board.h>
#include <mach/display.h>
#include <mach/clock.h>
#include "dss.h"

void __iomem  *dsi_bases;
extern void __iomem  *dss_base;
int lcd_ix = 0;

struct dsi_reg { u16 idx; };
struct dss_reg { u16 idx; };

#define DSI_REG(idx)		((const struct dsi_reg) { idx })
#define DSS_REG(idx)		((const struct dss_reg) { idx })

#define DSI_SZ_REGS		SZ_1K
/* DSI Protocol Engine */
#define DSS_CONTROL 			DSS_REG(0x0040)
#define DSI_REVISION			DSI_REG(0x0000)
#define DSI_SYSCONFIG			DSI_REG(0x0010)
#define DSI_SYSSTATUS			DSI_REG(0x0014)
#define DSI_IRQSTATUS			DSI_REG(0x0018)
#define DSI_IRQENABLE			DSI_REG(0x001C)
#define DSI_CTRL			DSI_REG(0x0040)
#define DSI_COMPLEXIO_CFG1		DSI_REG(0x0048)
#define DSI_COMPLEXIO_IRQ_STATUS	DSI_REG(0x004C)
#define DSI_COMPLEXIO_IRQ_ENABLE	DSI_REG(0x0050)
#define DSI_CLK_CTRL			DSI_REG(0x0054)
#define DSI_TIMING1			DSI_REG(0x0058)
#define DSI_TIMING2			DSI_REG(0x005C)
#define DSI_VM_TIMING1			DSI_REG(0x0060)
#define DSI_VM_TIMING2			DSI_REG(0x0064)
#define DSI_VM_TIMING3			DSI_REG(0x0068)
#define DSI_CLK_TIMING			DSI_REG(0x006C)
#define DSI_TX_FIFO_VC_SIZE		DSI_REG(0x0070)
#define DSI_RX_FIFO_VC_SIZE		DSI_REG(0x0074)
#define DSI_COMPLEXIO_CFG2		DSI_REG(0x0078)
#define DSI_RX_FIFO_VC_FULLNESS		DSI_REG(0x007C)
#define DSI_VM_TIMING4			DSI_REG(0x0080)
#define DSI_TX_FIFO_VC_EMPTINESS	DSI_REG(0x0084)
#define DSI_VM_TIMING5			DSI_REG(0x0088)
#define DSI_VM_TIMING6			DSI_REG(0x008C)
#define DSI_VM_TIMING7			DSI_REG(0x0090)
#define DSI_STOPCLK_TIMING		DSI_REG(0x0094)
#define DSI_VC_CTRL(n)			DSI_REG(0x0100 + (n * 0x20))
#define DSI_VC_TE(n)			DSI_REG(0x0104 + (n * 0x20))
#define DSI_VC_LONG_PACKET_HEADER(n)	DSI_REG(0x0108 + (n * 0x20))
#define DSI_VC_LONG_PACKET_PAYLOAD(n)	DSI_REG(0x010C + (n * 0x20))
#define DSI_VC_SHORT_PACKET_HEADER(n)	DSI_REG(0x0110 + (n * 0x20))
#define DSI_VC_IRQSTATUS(n)		DSI_REG(0x0118 + (n * 0x20))
#define DSI_VC_IRQENABLE(n)		DSI_REG(0x011C + (n * 0x20))

/* DSIPHY_SCP */

#define DSI_DSIPHY_CFG0			DSI_REG(0x200 + 0x0000)
#define DSI_DSIPHY_CFG1			DSI_REG(0x200 + 0x0004)
#define DSI_DSIPHY_CFG2			DSI_REG(0x200 + 0x0008)
#define DSI_DSIPHY_CFG5			DSI_REG(0x200 + 0x0014)

/* DSI_PLL_CTRL_SCP */

#define DSI_PLL_CONTROL			DSI_REG(0x300 + 0x0000)
#define DSI_PLL_STATUS			DSI_REG(0x300 + 0x0004)
#define DSI_PLL_GO			DSI_REG(0x300 + 0x0008)
#define DSI_PLL_CONFIGURATION1		DSI_REG(0x300 + 0x000C)
#define DSI_PLL_CONFIGURATION2		DSI_REG(0x300 + 0x0010)

#define REG_GET(idx, start, end) \
	FLD_GET(dsi_read_reg(idx), start, end)

#define REG_FLD_MOD(idx, val, start, end) \
	dsi_write_reg(idx, FLD_MOD(dsi_read_reg(idx), val, start, end))

/* Global interrupts */
#define DSI_IRQ_VC0		(1 << 0)
#define DSI_IRQ_VC1		(1 << 1)
#define DSI_IRQ_VC2		(1 << 2)
#define DSI_IRQ_VC3		(1 << 3)
#define DSI_IRQ_WAKEUP		(1 << 4)
#define DSI_IRQ_RESYNC		(1 << 5)
#define DSI_IRQ_PLL_LOCK	(1 << 7)
#define DSI_IRQ_PLL_UNLOCK	(1 << 8)
#define DSI_IRQ_PLL_RECALL	(1 << 9)
#define DSI_IRQ_COMPLEXIO_ERR	(1 << 10)
#define DSI_IRQ_HS_TX_TIMEOUT	(1 << 14)
#define DSI_IRQ_LP_RX_TIMEOUT	(1 << 15)
#define DSI_IRQ_TE_TRIGGER	(1 << 16)
#define DSI_IRQ_ACK_TRIGGER	(1 << 17)
#define DSI_IRQ_SYNC_LOST	(1 << 18)
#define DSI_IRQ_LDO_POWER_GOOD	(1 << 19)
#define DSI_IRQ_TA_TIMEOUT	(1 << 20)
#define DSI_IRQ_ERROR_MASK \
	(DSI_IRQ_HS_TX_TIMEOUT | DSI_IRQ_LP_RX_TIMEOUT | DSI_IRQ_SYNC_LOST | \
	DSI_IRQ_TA_TIMEOUT)
#define DSI_IRQ_CHANNEL_MASK	0xf



static inline void dsi_write_reg(const struct dsi_reg idx, u32 val)
{
	__raw_writel(val, dsi_bases + idx.idx);
}

static inline u32 dsi_read_reg(const struct dsi_reg idx)
{
	return __raw_readl(dsi_bases + idx.idx);
}

static inline void dss_write_reg(const struct dss_reg idx, u32 val)
{
	__raw_writel(val, dss_base + idx.idx);
}

static inline u32 dss_read_reg(const struct dss_reg idx)
{
	return __raw_readl(dss_base + idx.idx);
}

/*all can be removed from above*/

#define GPIO_OE         				0x48059134
#define GPIO_DATAOUT    				0x4805913C
#define OMAP24XX_GPIO_CLEARDATAOUT      0x48059190
#define OMAP24XX_GPIO_SETDATAOUT        0x48059194
#define DSICLKCTRLLPCLKDIVISOR    		1<<0| 1<<1| 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10 | 1<<11 | 1<<12
#define DSICLKCTRLPWRSTATUS 			1<<28| 1<<29
#define DSIPLLSTATUSRESETDONE			1<<0
#define COMPLEXIOCFG1PWRSTATUS          1<<26 | 1<<25

#define gVidModeVc 0

void modify_register(const struct dsi_reg idx, u32 clearmask,
                                                u32 setmask)
{
	unsigned int val;
		val = dsi_read_reg(idx);
		val &= ~(clearmask);
		val |= (setmask);
		dsi_write_reg(idx,val);

}

void modify_dss_register(const struct dss_reg idx, u32 clearmask,
                                                u32 setmask)
{
	unsigned int val;
		val = dss_read_reg(idx);
		val &= ~(clearmask);
		val |= (setmask);
		dss_write_reg(idx,val);

}

void lock_dsi_pll(void)
{
		unsigned int val;

		/* Select the manual mode of PLL update*/

		val = dsi_read_reg(DSI_PLL_CONTROL);
		val = val & ~(1<<0);
		dsi_write_reg(DSI_PLL_CONTROL,val);

		/* DSIPHY clock is disabled and HSDIV in bypass mode*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val & ~(1<<14);
		val = val | (1<<20);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Input reference clock to PLL is SYSCLK (DSS2_FCLK)*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val & ~(1<<11);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Enable the HIGHFREQ divider if input clock is greater than 32MHz*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (0<<12);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Configure the divisor values*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION1);
		val = val |

		  (3<<26) |
		  (3<<21) |
		  (102<<9) |
		  (18<<1) |
		  (1<<0);

		dsi_write_reg(DSI_PLL_CONFIGURATION1,val);

		/* Enable the DSI proto engine clock divider from HSDIV*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (1<<18);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Enable the DSS clock divider from HSDIV*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (1<<16);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Select the refernce as SYSCLK*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (0x3<<21);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Select the DCO frequency range to 500-1000MHz*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (0x2<<1);
		/*SetRegister32(KHwBaseDsiPllController + KHoDsiPllConfig2,val);*/
		modify_register(DSI_PLL_CONFIGURATION2,(1<<1 | 1<<2 | 1<<3),val);

		/* Enable PLL reference clock*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (1<<13);
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Enable DSIPHY clock and HSDIV in normal mode*/
		val = dsi_read_reg(DSI_PLL_CONFIGURATION2);
		val = val | (1<<14);
		val = val & (~(1<<20));
		dsi_write_reg(DSI_PLL_CONFIGURATION2,val);

		/* Start the PLL locking by setting PLL GO*/
		val = 1;
		dsi_write_reg(DSI_PLL_GO,val);

		/* Waiting for the lock request to be issued to PLL*/
		while(dsi_read_reg(DSI_PLL_GO) != 0);

		/* Waiting for the PLL to be locked*/
		while( ( (dsi_read_reg(DSI_PLL_STATUS) & 0x02) != 0x02));
		printk(" PLL finished \n");
	return;
}

void dsi_cplx_io_pwron(void)
{
	int u;
	int val;

	/*Kern::Printf("dsiMux = %x",dsiMux);*/
	/*turn Cio clock on*/
	modify_register(DSI_CLK_CTRL,0x00000000,1<<14);

	/* send power command to complexio module*/
	val = dsi_read_reg(DSI_COMPLEXIO_CFG1);
	val = ((val & 0xE7FFFFFF) |
	      (1 << 27));

	dsi_write_reg(DSI_COMPLEXIO_CFG1,val);

	/* Wait loop*/
	for (u = 0; u<100000;u++);

	val = dsi_read_reg(DSI_COMPLEXIO_CFG1);
	val = val | (1<<30);
	dsi_write_reg(DSI_COMPLEXIO_CFG1,val);

	/* Wait loop*/
	for (u = 0; u<100000;u++);

	/* Check whether the power status is changed*/
	do
	{
		val = dsi_read_reg(DSI_COMPLEXIO_CFG1);
		val = ((val & (COMPLEXIOCFG1PWRSTATUS)) >> 25);
		/* Wait loop*/
		for (u = 0; u<100000;u++);
	}while((val != 1));
}

void config_cmplx_io(void)
{
	unsigned int val;
	int u;
	int count;
	void __iomem*phymux_base;

	/* Do The Mux*/
	unsigned int dsimux = 0xFFFFFFFF;
	/*((1 << 14) | (1 << 19) | (0x1F << 24) | (0x0 << 29));
		Pulling Down DSI lanes, Enabling DSI Lanes*/

	phymux_base = ioremap(0x4A100000,0x1000);

	/*turn Cio clock on*/
	modify_register(DSI_CLK_CTRL,0x00000000,1<<14);

	/* Turning on DSI PHY Mux*/
	__raw_writel(dsimux,phymux_base+0x618);
	dsimux = __raw_readl(phymux_base+0x618);

	/* COnfigure the lanes*/
	val = dsi_read_reg(DSI_COMPLEXIO_CFG1);
	val	= val |
	      (
		   (0<<11)|
	       (3<<8)| /* LAne 2*/
	       (0<<7)|
	       (2<<4)| /* Lane 1*/
	       (0<<3)|
	       (1<<0)); /* Clk Pos*/
	dsi_write_reg(DSI_COMPLEXIO_CFG1,val);

	/*DsiCplxIoPwrOn();*/

	/*Timing Configurations
	 Configure the DSI PHY timing parameters*/
	val = ((9<<24)|
		   (20<<16)|
		   (6<<8)|
		   (15<<0));
	dsi_write_reg(DSI_DSIPHY_CFG0,val);

	val = dsi_read_reg(DSI_DSIPHY_CFG1);
	val = val & ~(0x007FFFFF);
	val = ((2<<29)|
	       (0<<27)|
	       (2<<24)|
	       (3<<16)|
	       (6<<8)|
	       (26<<0) );
	dsi_write_reg(DSI_DSIPHY_CFG1,val);

	val = dsi_read_reg(DSI_DSIPHY_CFG2);
	/* this is required to preserve the reset data */
	val = val & ~(0x000000FF);
	val = val | (7 << 0);
	dsi_write_reg(DSI_DSIPHY_CFG2,val);

	/* Set the GO bit*/

	val = dsi_read_reg(DSI_COMPLEXIO_CFG1);
	val = val | (1<<30);
	dsi_write_reg(DSI_COMPLEXIO_CFG1,val);

	count = 100;

	/* Waiting for the Go bit to be reset by HW*/
	while( (dsi_read_reg(DSI_COMPLEXIO_CFG1) & (1<<30) ) && (--count) )
	{
		for (u = 0; u<100000;u++);
	}

	if(count == 0)
	{
		/*__KTRACE_OPT(KDLL,Kern::Printf("config_dphy: dsi_complexio_HW_reset_status incomplete !!!! \n"));*/
	}

	/*do
	{
		val = Register32(KHwBaseDsiProtoEng + KHoComplexIOCfg1);
		// Wait loop
		for (u = 0; u<100000;u++);
	}while(!(val & KHtComplexIOCfg1ResetDone)); // Check the RESET done bit*/

	/* enable the COMPLEXIO interrupts*/
	dsi_write_reg(DSI_COMPLEXIO_IRQ_STATUS,0xFFFFFFFF); /* clear the events*/
	dsi_write_reg(DSI_COMPLEXIO_IRQ_ENABLE,0x0); /* enable the interrupt events*/

	dsi_cplx_io_pwron();

	/* Timer configuration*/
	val = 0;
	val = dsi_read_reg(DSI_TIMING1);
	val = val |
	      (0 << 31)	| /* TA_TO : 0x0 turn around is off*/
	      (1 << 15)	| /* Control of ForceTxStopMode signal*/
	      (1 << 14)	| /* STOP_STATE_X16_IO*/
	      (0x999);	  /* STOP_STATE_COUNTER_IO*/

	dsi_write_reg(DSI_TIMING1,val);

	/* Disable Command Mode*/
	val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc));
	val = val & ~(1<<0);
	dsi_write_reg(DSI_VC_CTRL(gVidModeVc),val);

	/* Enable Command Mode*/
	val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc));
	val = val | (1<<0);
	dsi_write_reg(DSI_VC_CTRL(gVidModeVc),val);

	val = dsi_read_reg(DSI_TIMING2);
	val = val |
	      (0<<31)|		/* HS_TX_TO*/
	      (0<<15);		/* LP_RX_TO*/

	dsi_write_reg(DSI_TIMING2,val);

	/* config DDR CLK timer*/
	val = dsi_read_reg(DSI_CLK_TIMING);
	val = (10 << 8) |
	      (9 << 0);
	dsi_write_reg(DSI_CLK_TIMING,val);

	return ;

}

int reset_dsi_proto_eng (void)
{
	unsigned int val;
	/*Reset the DSI protocol engine*/
	val = dsi_read_reg(DSI_SYSCONFIG);
	val = val | (1<<1);
	dsi_write_reg(DSI_SYSCONFIG,val);

	/*wait for reset to complete*/
	while(dsi_read_reg(DSI_SYSSTATUS)!=0x1);
	printk("\n reset complete \n");
	return 1;
}

void config_dsi_proto_engine(void)
{
	int u;
	unsigned int val=0;

	printk("reset_dsi_proto_eng \n");

	/* 1) Reset the DSI protocol engine*/
	if (reset_dsi_proto_eng() != 1)
	return;

	printk("clear the reset bit \n");

	/* 2) Clear the reset Bit*/
	val = dsi_read_reg(DSI_SYSCONFIG);
	val = val & ~(1<<0);
	dsi_write_reg(DSI_SYSCONFIG,val);

	/* 3) Make sure the Virtual channels are disabled before
		configuration*/
	val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc));
	val = val & ~(1<<0);
	dsi_write_reg(DSI_VC_CTRL(gVidModeVc),val);

	val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc+1));
	val = val & ~(1<<0);
	dsi_write_reg(DSI_VC_CTRL(gVidModeVc+1),val);

	/* 4) Setup the clock control*/
	val = dsi_read_reg(DSI_CLK_CTRL);
	val = val & ~(DSICLKCTRLLPCLKDIVISOR);

	val = val |
	      (1<<21) |
	      (1<<20) |
	      (1<<18) |
	      (0<<16) |
	      (0<<15) |  /* 0 -> NULL packet generation disabled */
	      (1<<14) |
	      (1<<13) |
	      (6<<0); /* 50/7 => 7MHz */

	dsi_write_reg(DSI_CLK_CTRL,val);

	#if 1 /*HS mode*/

	/*Config VideoMode Timing */
	dsi_write_reg(DSI_VM_TIMING1,0x02004006);
	dsi_write_reg(DSI_VM_TIMING2,0x04010001);
	dsi_write_reg(DSI_VM_TIMING3,0x036F01E0);
	dsi_write_reg(DSI_VM_TIMING4,0x00487296);
	dsi_write_reg(DSI_VM_TIMING5,0x0082DF3B);
	dsi_write_reg(DSI_VM_TIMING6,0x7A6731D1);
	dsi_write_reg(DSI_VM_TIMING7,0x00090007);

	/*Config VC channel */
	dsi_write_reg(DSI_VC_CTRL(gVidModeVc+1),0x60809382);  /*video channel*/

	/*Clear all IRQ*/
	dsi_write_reg(DSI_VC_IRQSTATUS(gVidModeVc+1),0xFF);
	dsi_write_reg(DSI_VC_IRQENABLE(gVidModeVc+1),0x0);

	#endif /*HS mode */

	/* 5) Configure Command Mode Channel*/
	/* configure the VC channel to command mode (command mode, L4 as input, HS, no DMA ) */
	val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc));
	val = val & ~((1<<4)| (1<<1));
	val = val |
	      (4<<27) |
	      (4<<21) |
	      (3<<10) |
	      (3<<17) |
	      (1<<8) |
	      (1<<7) |
	      (1<<8) |
	      (0<<9) ;

	dsi_write_reg(DSI_VC_CTRL(gVidModeVc),val);

	/* Enable interrupt events for Command mode channel*/
	dsi_write_reg(DSI_VC_IRQSTATUS(gVidModeVc),0xFF);/* clear the events */
	dsi_write_reg(DSI_VC_IRQENABLE(gVidModeVc),0x0);/* enable the events */

	/* 6) Configure the RX and TX fifo*/
	val = (0<<28) | 			/* VC3 Not used*/
	      (0<<24) | 			/* VC3 Not used*/
	      (0<<20) | 			/* VC2 Not used*/
	      (0<<16) | 			/* VC2 Not used*/
	      (4<<12) | 			/* VC1 FIFO length*/
	      (0<<8) | 				/* VC1 FIFO start address*/
	      (4<<4) | 				/* VC0 FIFO length*/
	      (0<<0); 				/* VC0 FIFO start address*/
		dsi_write_reg(DSI_TX_FIFO_VC_SIZE,val);
	val = (0<<28) | 			/* VC3 Not used*/
	      (0<<24) | 			/* VC3 Not used*/
	      (0<<20) | 			/* VC2 Not used*/
	      (0<<16) | 			/* VC2 Not used*/
	      (1<<12) | 			/* VC1 FIFO length*/
	      (0<<8) | 				/* VC1 FIFO start address*/
	      (1<<4) | 				/* VC0 Not used for BTA*/
	      (0<<0);				/* VC0 Not used for BTA*/
		dsi_write_reg(DSI_RX_FIFO_VC_SIZE,val);

		/* 7) enable DSI IRQ*/
		dsi_write_reg(DSI_IRQSTATUS,0x001FFFFF);

		printk("configure the pll \n");

		/* 8) Configure DSI PLL*/
			/* 1) Send PWR_ON command*/
		/* send the power command*/
		val = dsi_read_reg(DSI_CLK_CTRL);
		val = ((val & ~(3 << 30)) | (2 << 30));
		dsi_write_reg(DSI_CLK_CTRL,val);

		/* Check whether the power status is changed*/
		do
		{
			val = dsi_read_reg(DSI_CLK_CTRL);
			val = ((val & (DSICLKCTRLPWRSTATUS)) >> 28);
			/* Wait loop*/
			for (u = 0; u<100000;u++);
		}while((val != 2));

		/* 2) Wait for the PLL to get reset*/
		do
		{
			val = dsi_read_reg(DSI_PLL_STATUS);
		}while(!(val & DSIPLLSTATUSRESETDONE)); /* Check the RESET done bit*/

		/* 3) Send PWR_ON command after reset*/
		val = dsi_read_reg(DSI_CLK_CTRL);
		val = ((val & ~(3 << 30)) | (2 << 30));
		dsi_write_reg(DSI_CLK_CTRL,val);

		/* Check whether the power status is changed*/
		do
		{
			val = dsi_read_reg(DSI_CLK_CTRL);
			val = ((val & (DSICLKCTRLPWRSTATUS)) >> 28);
			/* Wait loop*/
			for (u = 0; u<100000;u++);
		}while( (val != 2));

		/* 4) Lock DSI PLL*/
		lock_dsi_pll();

		/*Switch the dispc to DSI PLL*/
		/*__raw_writel(0x103,dss_base+0x0040);?*/

		if(lcd_ix == 1){
		printk("switch dsi2 to pll \n");
		modify_dss_register(DSS_CONTROL, 0x00000000, (1<<12 | 1<<10));
		}
	else {
		printk("switch dsi to pll \n");
		modify_dss_register(DSS_CONTROL, 0x00000000, 0x003); /*svovo3*/
		}

		for (u = 0; u<100000;u++);

		/* Configure ComplexIO*/
		config_cmplx_io();

		/* Configure DSI registers */
		val = 0x6A98;
		dsi_write_reg(DSI_CTRL,val);

		/* Enable OMAP-DSI Interface*/
		val = dsi_read_reg(DSI_CTRL);
		val = (val | (1<<0));
		dsi_write_reg(DSI_CTRL,val);

		/* Enable Command Mode*/
		val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc));
		val = val | (1<<0);
		dsi_write_reg(DSI_VC_CTRL(gVidModeVc),val);

		#if 1 /* HS mode*/
		/* Enable Video Mode*/
		val = dsi_read_reg(DSI_VC_CTRL(gVidModeVc+1));
		val = val | (1<<0);
		dsi_write_reg(DSI_VC_CTRL(gVidModeVc+1),val);
		#endif

		/* Around 100ms delay for TAAL to stabilize*/
		for (val=0; val <10; val++)
		for (u = 0; u < 100000; u++);

	/* On 3430 SDP, Power to taal is controlled by GPIO Pin*/
	/* This is Implemented in VarintLcdPowerUp();*/

	printk("configure complexio");

	val = 0;
	/* Register 12*/
	val = val | (0x58 << 0);
	dsi_write_reg(DSI_REG(0x200 + 0x30),val);

	/* Register 14*/
	val = 0;
	val = val | (1 << 31) | 1<<11 | (0x54 << 23) | 1<<19 | 1<<18 | (0x7 << 14);
	dsi_write_reg(DSI_REG(0x200 + 0x38),val);

	/* Register 8*/
	val = 0;
	val = val | (1 << 11) | (16 << 6) | 1<<5 | (0xE << 0);
	dsi_write_reg(DSI_REG(0x200 + 0x20),val);

	/* Around 100ms delay for TAAL to stabilize*/
	for (val=0; val < 10; val++)
	for (u = 0; u < 100000; u++);

	mdelay(200);
	/* send short packet*/
}

void Setup_SDP(void *dsi_bases_param, int lcd_index)
{
	dsi_bases = dsi_bases_param;
	lcd_ix = lcd_index;
	config_dsi_proto_engine();
}
