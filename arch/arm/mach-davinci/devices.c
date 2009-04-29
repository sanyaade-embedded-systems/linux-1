/*
 * mach-davinci/devices.c
 *
 * DaVinci platform device setup/initialization
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/random.h>
#include <linux/etherdevice.h>

#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/mach/map.h>

#include <mach/hardware.h>
#include <mach/da8xx.h>
#include <mach/edma.h>
#include <mach/emac.h>
#include <mach/i2c.h>
#include <mach/cpu.h>
#include <mach/mux.h>
#include <mach/mmc.h>
#include <mach/da8xx_lcdc.h>
#include <mach/dm646x.h>
#include <mach/da8xx.h>

#include "clock.h"


#define DAVINCI_I2C_BASE	     0x01C21000
#define DAVINCI_EMAC_CNTRL_REGS_BASE 0x01C80000
#define DAVINCI_EMAC_CNTRL_MOD_REGS_BASE	0x01C81000
#define DAVINCI_EMAC_CNTRL_RAM_BASE		0x01C82000
#define DAVINCI_EMAC_MDIO_REGS_BASE		0x01C84000
#define DAVINCI_MMCSD0_BASE	     0x01E10000
#define DM355_MMCSD0_BASE	     0x01E11000
#define DM355_MMCSD1_BASE	     0x01E00000
#define DA830_MMC_SD0_BASE	     0x01C40000
#define DA830_RTC_BASE	     0x01C23000

#define DA850_RMII_SEL	(0x01 << 8)

static struct resource i2c_resources[] = {
	{
		.start		= DAVINCI_I2C_BASE,
		.end		= DAVINCI_I2C_BASE + 0x40,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= IRQ_I2C,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device davinci_i2c_device = {
	.name           = "i2c_davinci",
	.id             = 1,
	.num_resources	= ARRAY_SIZE(i2c_resources),
	.resource	= i2c_resources,
};

static struct resource da8xx_i2c_resources0[] = {
	{
		.start          = DA8XX_I2C0_BASE,
		.end            = DA8XX_I2C0_BASE + SZ_4K - 1,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = IRQ_DA8XX_I2CINT0,
		.end            = IRQ_DA8XX_I2CINT0,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct resource da8xx_i2c_resources1[] = {
	{
		.start          = DA8XX_I2C1_BASE,
		.end            = DA8XX_I2C1_BASE + SZ_4K - 1,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = IRQ_DA8XX_I2CINT1,
		.end            = IRQ_DA8XX_I2CINT1,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_i2c_device[] = {
	[0] = {
		.name           = "i2c_davinci",
		.id             = 1,
		.num_resources  = ARRAY_SIZE(da8xx_i2c_resources0),
		.resource       = da8xx_i2c_resources0,
	},
	[1] = {
		.name           = "i2c_davinci",
		.id             = 2,
		.num_resources  = ARRAY_SIZE(da8xx_i2c_resources1),
		.resource       = da8xx_i2c_resources1,
	}
};

void __init davinci_init_i2c(struct davinci_i2c_platform_data *pdata)
{
	int i = 0;
	if (cpu_is_davinci_dm644x())
		davinci_cfg_reg(DM644X_I2C);

	if (cpu_is_da830()) {
		davinci_cfg_reg(DA830_I2C1_SCL);
		davinci_cfg_reg(DA830_I2C1_SDA);
		davinci_cfg_reg(DA830_I2C0_SDA);
		davinci_cfg_reg(DA830_I2C0_SCL);		
	}

	if (cpu_is_da850()) {
		davinci_cfg_reg(DA850_I2C1_SCL);
		davinci_cfg_reg(DA850_I2C1_SDA);
		davinci_cfg_reg(DA850_I2C0_SDA);
		davinci_cfg_reg(DA850_I2C0_SCL);		
	}

	if (cpu_is_davinci_dm644x() ||
	    cpu_is_davinci_dm646x() ||
	    cpu_is_davinci_dm355()) {
		davinci_i2c_device.dev.platform_data = pdata;
		(void) platform_device_register(&davinci_i2c_device);
	} else if (cpu_is_da8xx()) {
		for (i = 0; i < 2; i++) {
			da8xx_i2c_device[i].dev.platform_data = pdata;
			(void) platform_device_register(&da8xx_i2c_device[i]);
		}
	}
}

#if	defined(CONFIG_MMC_DAVINCI) || defined(CONFIG_MMC_DAVINCI_MODULE)

static u64 mmcsd0_dma_mask = DMA_32BIT_MASK;

static struct resource mmcsd0_resources[] = {
	{
		/* different on dm355 */
		.start = DAVINCI_MMCSD0_BASE,
		.end   = DAVINCI_MMCSD0_BASE + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	/* IRQs:  MMC/SD, then SDIO */
	{
		.start = IRQ_MMCINT,
		.flags = IORESOURCE_IRQ,
	}, {
		/* different on dm355 */
		.start = IRQ_SDIOINT,
		.flags = IORESOURCE_IRQ,
	},
	/* DMA channels: RX, then TX */
	{
		.start = EDMA_CTLR_CHAN(0, DAVINCI_DMA_MMCRXEVT),
		.flags = IORESOURCE_DMA,
	}, {
		.start = EDMA_CTLR_CHAN(0, DAVINCI_DMA_MMCTXEVT),
		.flags = IORESOURCE_DMA,
	},
};

static struct platform_device davinci_mmcsd0_device = {
	.name = "davinci_mmc",
	.id = 0,
	.dev = {
		.dma_mask = &mmcsd0_dma_mask,
		.coherent_dma_mask = DMA_32BIT_MASK,
	},
	.num_resources = ARRAY_SIZE(mmcsd0_resources),
	.resource = mmcsd0_resources,
};

static u64 mmcsd1_dma_mask = DMA_32BIT_MASK;

static struct resource mmcsd1_resources[] = {
	{
		.start = DM355_MMCSD1_BASE,
		.end   = DM355_MMCSD1_BASE + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	/* IRQs:  MMC/SD, then SDIO */
	{
		.start = IRQ_DM355_MMCINT1,
		.flags = IORESOURCE_IRQ,
	}, {
		.start = IRQ_DM355_SDIOINT1,
		.flags = IORESOURCE_IRQ,
	},
	/* DMA channels: RX, then TX */
	{
		.start = EDMA_CTLR_CHAN(0, 30),	/* rx */
		.flags = IORESOURCE_DMA,
	}, {
		.start = EDMA_CTLR_CHAN(0, 31),	/* tx */
		.flags = IORESOURCE_DMA,
	},
};

static struct platform_device davinci_mmcsd1_device = {
	.name = "davinci_mmc",
	.id = 1,
	.dev = {
		.dma_mask = &mmcsd1_dma_mask,
		.coherent_dma_mask = DMA_32BIT_MASK,
	},
	.num_resources = ARRAY_SIZE(mmcsd1_resources),
	.resource = mmcsd1_resources,
};

static struct resource da830_mmc_resources[] = {
	{		 /* registers */
		.start	= DA830_MMC_SD0_BASE,
		.end	= DA830_MMC_SD0_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{		 /* interrupt */
		.start	= IRQ_DA8XX_MMCSDINT0,
		.end	= IRQ_DA8XX_MMCSDINT0,
		.flags	= IORESOURCE_IRQ,
	},
	{		 /* DMA RX */
		.start	= EDMA_CTLR_CHAN(0, 16),
		.end	= EDMA_CTLR_CHAN(0, 16),
		.flags	= IORESOURCE_DMA,
	},
	{		 /* DMA TX */
		.start	= EDMA_CTLR_CHAN(0, 17),
		.end	= EDMA_CTLR_CHAN(0, 17),
		.flags	= IORESOURCE_DMA,
	},
};

static struct platform_device da830_mmc_device = {
	.name		= "davinci_mmc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da830_mmc_resources),
	.resource	= da830_mmc_resources,
};

static struct resource da850_mmc_resources[] = {
	{		 /* registers */
		.start	= 0x01c40000,
		.end	= 0x01c40000 + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{		 /* interrupt */
		.start	= 16,
		.end	= 16,
		.flags	= IORESOURCE_IRQ,
	},
	{		 /* DMA RX */
		.start	= EDMA_CTLR_CHAN(0, 16),
		.end	= EDMA_CTLR_CHAN(0, 16),
		.flags	= IORESOURCE_DMA,
	},
	{		 /* DMA TX */
		.start	= EDMA_CTLR_CHAN(0, 17),
		.end	= EDMA_CTLR_CHAN(0, 17),
		.flags	= IORESOURCE_DMA,
	},
};

static struct platform_device da850_mmc_device = {
	.name		= "davinci_mmc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da850_mmc_resources),
	.resource	= da850_mmc_resources,
};

void __init davinci_setup_mmc(int module, struct davinci_mmc_config *config)
{
	struct platform_device	*pdev = NULL;

	if (WARN_ON(cpu_is_davinci_dm646x()))
		return;

	/* REVISIT: update PINMUX, ARM_IRQMUX, and EDMA_EVTMUX here too;
	 * for example if MMCSD1 is used for SDIO, maybe DAT2 is unused.
	 *
	 * FIXME dm6441 (no MMC/SD), dm357 (one), and dm335 (two) are
	 * not handled right here ...
	 */
	switch (module) {
	case 1:
		if (!cpu_is_davinci_dm355())
			break;

		/* REVISIT we may not need all these pins if e.g. this
		 * is a hard-wired SDIO device...
		 */
		davinci_cfg_reg(DM355_SD1_CMD);
		davinci_cfg_reg(DM355_SD1_CLK);
		davinci_cfg_reg(DM355_SD1_DATA0);
		davinci_cfg_reg(DM355_SD1_DATA1);
		davinci_cfg_reg(DM355_SD1_DATA2);
		davinci_cfg_reg(DM355_SD1_DATA3);

		pdev = &davinci_mmcsd1_device;
		break;
	case 0:
		if (cpu_is_davinci_dm355()) {
			mmcsd0_resources[0].start = DM355_MMCSD0_BASE;
			mmcsd0_resources[0].end = DM355_MMCSD0_BASE + SZ_4K - 1;
			mmcsd0_resources[2].start = IRQ_DM355_SDIOINT0;

			/* expose all 6 MMC0 signals:  CLK, CMD, DATA[0..3] */
			davinci_cfg_reg(DM355_MMCSD0);

			/* enable RX EDMA */
			davinci_cfg_reg(DM355_EVT26_MMC0_RX);
		}

		else if (cpu_is_davinci_dm644x()) {
			/* REVISIT: should this be in board-init code? */
			void __iomem *base =
				IO_ADDRESS(DAVINCI_SYSTEM_MODULE_BASE);

			/* Power-on 3.3V IO cells */
			__raw_writel(0, base + DM64XX_VDD3P3V_PWDN);
			/*Set up the pull regiter for MMC */
			davinci_cfg_reg(DM644X_MSTK);
		}

		if (cpu_is_da830()) {
			pdev = &da830_mmc_device;
			davinci_cfg_reg(DA830_MMCSD_DAT_0);
			davinci_cfg_reg(DA830_MMCSD_DAT_1);
			davinci_cfg_reg(DA830_MMCSD_DAT_2);
			davinci_cfg_reg(DA830_MMCSD_DAT_3);
			davinci_cfg_reg(DA830_MMCSD_DAT_4);
			davinci_cfg_reg(DA830_MMCSD_DAT_5);
			davinci_cfg_reg(DA830_MMCSD_DAT_6);
			davinci_cfg_reg(DA830_MMCSD_DAT_7);
			davinci_cfg_reg(DA830_MMCSD_CLK);
			davinci_cfg_reg(DA830_MMCSD_CMD);
		} else if (cpu_is_da850()) {
			pdev = &da850_mmc_device;
			davinci_cfg_reg(DA850_MMCSD0_DAT_0);
			davinci_cfg_reg(DA850_MMCSD0_DAT_1);
			davinci_cfg_reg(DA850_MMCSD0_DAT_2);
			davinci_cfg_reg(DA850_MMCSD0_DAT_3);
			davinci_cfg_reg(DA850_MMCSD0_DAT_4);
			davinci_cfg_reg(DA850_MMCSD0_DAT_5);
			davinci_cfg_reg(DA850_MMCSD0_DAT_6);
			davinci_cfg_reg(DA850_MMCSD0_DAT_7);
			davinci_cfg_reg(DA850_MMCSD0_CLK);
			davinci_cfg_reg(DA850_MMCSD0_CMD);
		} else
			pdev = &davinci_mmcsd0_device;

		break;
	}

	if (WARN_ON(!pdev))
		return;

	pdev->dev.platform_data = config;
	platform_device_register(pdev);
}

#else

void __init davinci_setup_mmc(int module, struct davinci_mmc_config *config)
{
}

#endif

static struct resource da8xx_rtc_resources[] = {
	[0] = {		/* registers */
		.start  = DA830_RTC_BASE,
		.end    = DA830_RTC_BASE + SZ_4K - 1,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {		/* interrupt */
		.start  = IRQ_DA8XX_RTC,
		.end    = IRQ_DA8XX_RTC,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_rtc_device = {
	.name		= "rtc-da830",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da8xx_rtc_resources),
	.resource	= da8xx_rtc_resources,
};

void da8xx_init_rtc(void)
{
        (void) platform_device_register(&da8xx_rtc_device);
}

/*-------------------------------------------------------------------------*/

static struct resource wdt_resources[] = {
	{
		.start	= 0x01c21c00,
		.end	= 0x01c21fff,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device davinci_wdt_device = {
	.name		= "watchdog",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(wdt_resources),
	.resource	= wdt_resources,
};

static struct resource da8xx_wdt_resources[] = {
	{
		.start	= 0x01c21000,
		.end	= 0x01c21fff,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da8xx_wdt_device = {
	.name		= "watchdog",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(da8xx_wdt_resources),
	.resource	= da8xx_wdt_resources,
};

static void davinci_init_wdt(void)
{
	if (cpu_is_da8xx())
		platform_device_register(&da8xx_wdt_device);
	else
		platform_device_register(&davinci_wdt_device);
}

/*-------------------------------------------------------------------------*/
static struct da8xx_lcdc_platform_data da8xx_evm_lcdc_pdata = {
	.lcdc_clk_name  = "LCDCTRLCLK",
};
static struct resource da8xx_lcdc_resources[] = {
	[0] = {	/* registers */
		.start	= DA8XX_LCD_CNTRL_BASE,
		.end	= DA8XX_LCD_CNTRL_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
/* XXX Next resource seems wrong.  Should use dma_* calls? -- Remove */
	[1] = {	/* frame buffer and palette */
		.start	= DA8XX_EMIF30_ASYNC_DATA_CE5_BASE,
		.end	= DA8XX_EMIF30_ASYNC_DATA_CE5_BASE + SZ_512M - 1,
		.flags	= IORESOURCE_MEM,
	},
/* END Remove */
	[2] = {	/* interrupt */
		.start	= IRQ_DA8XX_LCDINT,
		.end	= IRQ_DA8XX_LCDINT,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_lcdc_device = {
	.name		= "da8xx_lcdc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da8xx_lcdc_resources),
	.resource	= da8xx_lcdc_resources,
	.dev = {
		.platform_data = &da8xx_evm_lcdc_pdata,
	}
};

static int __init davinci_init_devices(void)
{
	int ret;

	/* please keep these calls, and their implementations above,
	 * in alphabetical order so they're easier to sort through.
	 */
	davinci_init_wdt();
	if (cpu_is_da8xx()) {
		ret = platform_device_register(&da8xx_lcdc_device);

		if (ret)
			return ret;
		else if (cpu_is_da830()) {
			davinci_cfg_reg(DA830_LCD_D_0);
			davinci_cfg_reg(DA830_LCD_D_1);
			davinci_cfg_reg(DA830_LCD_D_2);
			davinci_cfg_reg(DA830_LCD_D_3);
			davinci_cfg_reg(DA830_LCD_D_4);
			davinci_cfg_reg(DA830_LCD_D_5);
			davinci_cfg_reg(DA830_LCD_D_6);
			davinci_cfg_reg(DA830_LCD_D_7);
			davinci_cfg_reg(DA830_LCD_D_8);
			davinci_cfg_reg(DA830_LCD_D_9);
			davinci_cfg_reg(DA830_LCD_D_10);
			davinci_cfg_reg(DA830_LCD_D_11);
			davinci_cfg_reg(DA830_LCD_D_12);
			davinci_cfg_reg(DA830_LCD_D_13);
			davinci_cfg_reg(DA830_LCD_D_14);
			davinci_cfg_reg(DA830_LCD_D_15);
			davinci_cfg_reg(DA830_LCD_PCLK);
			davinci_cfg_reg(DA830_LCD_HSYNC);
			davinci_cfg_reg(DA830_LCD_VSYNC);
			davinci_cfg_reg(DA830_NLCD_AC_ENB_CS);
			davinci_cfg_reg(DA830_LCD_MCLK);			
		} else if (cpu_is_da850()) {
			davinci_cfg_reg(DA850_LCD_D_0);
			davinci_cfg_reg(DA850_LCD_D_1);
			davinci_cfg_reg(DA850_LCD_D_2);
			davinci_cfg_reg(DA850_LCD_D_3);
			davinci_cfg_reg(DA850_LCD_D_4);
			davinci_cfg_reg(DA850_LCD_D_5);
			davinci_cfg_reg(DA850_LCD_D_6);
			davinci_cfg_reg(DA850_LCD_D_7);
			davinci_cfg_reg(DA850_LCD_D_8);
			davinci_cfg_reg(DA850_LCD_D_9);
			davinci_cfg_reg(DA850_LCD_D_10);
			davinci_cfg_reg(DA850_LCD_D_11);
			davinci_cfg_reg(DA850_LCD_D_12);
			davinci_cfg_reg(DA850_LCD_D_13);
			davinci_cfg_reg(DA850_LCD_D_14);
			davinci_cfg_reg(DA850_LCD_D_15);
			davinci_cfg_reg(DA850_LCD_PCLK);
			davinci_cfg_reg(DA850_LCD_HSYNC);
			davinci_cfg_reg(DA850_LCD_VSYNC);
			davinci_cfg_reg(DA850_NLCD_AC_ENB_CS);
			davinci_cfg_reg(DA850_LCD_MCLK);			
		}
	}

	return 0;
}
arch_initcall(davinci_init_devices);
