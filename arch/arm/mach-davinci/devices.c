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
#include <mach/omapl1x7.h>
#include <mach/edma.h>
#include <mach/emac.h>
#include <mach/i2c.h>
#include <mach/cpu.h>
#include <mach/mux.h>
#include <mach/mmc.h>

#include "clock.h"


#define DAVINCI_I2C_BASE	     0x01C21000
#define DAVINCI_EMAC_CNTRL_REGS_BASE 0x01C80000
#define DAVINCI_EMAC_CNTRL_MOD_REGS_BASE	0x01C81000
#define DAVINCI_EMAC_CNTRL_RAM_BASE		0x01C82000
#define DAVINCI_EMAC_MDIO_REGS_BASE		0x01C84000
#define DAVINCI_MMCSD0_BASE	     0x01E10000
#define DM355_MMCSD0_BASE	     0x01E11000
#define DM355_MMCSD1_BASE	     0x01E00000
#define OMAPL1X7_MMC_SD0_BASE	     0x01C40000

#ifndef CONFIG_MACH_OMAPL1X7_EVM
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
#else
static struct resource omapl1x7_i2c_resources0[] = {
	{
		.start          = OMAPL1X7_I2C0_BASE,
		.end            = OMAPL1X7_I2C0_BASE + SZ_4K - 1,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = IRQ_OMAPL1X7_I2CINT0,
		.end            = IRQ_OMAPL1X7_I2CINT0,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct resource omapl1x7_i2c_resources1[] = {
	{
		.start          = OMAPL1X7_I2C1_BASE,
		.end            = OMAPL1X7_I2C1_BASE + SZ_4K - 1,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = IRQ_OMAPL1X7_I2CINT1,
		.end            = IRQ_OMAPL1X7_I2CINT1,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct platform_device omapl1x7_i2c_device[] = {
	[0] = {
		.name           = "i2c_davinci",
		.id             = 1,
		.num_resources  = ARRAY_SIZE(omapl1x7_i2c_resources0),
		.resource       = omapl1x7_i2c_resources0,
	},
	[1] = {
		.name           = "i2c_davinci",
		.id             = 2,
		.num_resources  = ARRAY_SIZE(omapl1x7_i2c_resources1),
		.resource       = omapl1x7_i2c_resources1,
	}
};
#endif

void __init davinci_init_i2c(struct davinci_i2c_platform_data *pdata)
{
	int i = 0;
	if (cpu_is_davinci_dm644x())
		davinci_cfg_reg(DM644X_I2C);

	if (cpu_is_omapl1x7()) {
		davinci_cfg_reg(OMAPL1X7_I2C1_SCL);
		davinci_cfg_reg(OMAPL1X7_I2C1_SDA);
		davinci_cfg_reg(OMAPL1X7_I2C0_SDA);
		davinci_cfg_reg(OMAPL1X7_I2C0_SCL);		
	}

#ifndef CONFIG_MACH_OMAPL1X7_EVM
	davinci_i2c_device.dev.platform_data = pdata;
	(void) platform_device_register(&davinci_i2c_device);
#else
	for (i = 0; i < 2; i++) {
		omapl1x7_i2c_device[i].dev.platform_data = pdata;
		(void) platform_device_register(&omapl1x7_i2c_device[i]);
	}
#endif
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
		.start = DAVINCI_DMA_MMCRXEVT,
		.flags = IORESOURCE_DMA,
	}, {
		.start = DAVINCI_DMA_MMCTXEVT,
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
		.start = 30,	/* rx */
		.flags = IORESOURCE_DMA,
	}, {
		.start = 31,	/* tx */
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

static struct resource omapl1x7_mmc_resources[] = {
	{		 /* registers */
		.start	= OMAPL1X7_MMC_SD0_BASE,
		.end	= OMAPL1X7_MMC_SD0_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{		 /* interrupt */
		.start	= IRQ_OMAPL1X7_MMCSDINT0,
		.end	= IRQ_OMAPL1X7_MMCSDINT0,
		.flags	= IORESOURCE_IRQ,
	},
	{		 /* DMA RX */
		.start	= 16,
		.end	= 16,
		.flags	= IORESOURCE_DMA,
	},
	{		 /* DMA TX */
		.start	= 17,
		.end	= 17,
		.flags	= IORESOURCE_DMA,
	},
};

static struct platform_device omapl1x7_mmc_device = {
	.name		= "davinci_mmc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(omapl1x7_mmc_resources),
	.resource	= omapl1x7_mmc_resources,
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

		else if (cpu_is_omapl1x7()) {
			pdev = &omapl1x7_mmc_device;
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_0);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_1);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_2);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_3);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_4);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_5);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_6);
			davinci_cfg_reg(OMAPL1X7_MMCSD_DAT_7);
			davinci_cfg_reg(OMAPL1X7_MMCSD_CLK);
			davinci_cfg_reg(OMAPL1X7_MMCSD_CMD);
		} else
			pdev = &davinci_mmcsd0_device;

		clockname = cpu_is_davinci_dm355() ? "mmcsd0" : "mmcsd";
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

static void davinci_init_wdt(void)
{
	platform_device_register(&davinci_wdt_device);
}

/*-------------------------------------------------------------------------*/

#if defined(CONFIG_TI_DAVINCI_EMAC) || defined(CONFIG_TI_DAVINCI_EMAC_MODULE)

static struct resource emac_resources[] = {
	{
		.start	= DAVINCI_EMAC_CNTRL_REGS_BASE,
		.end	= DAVINCI_EMAC_CNTRL_REGS_BASE + 0x0fff,
		.flags	= IORESOURCE_MEM,
		.name	= "ctrl_regs"
	},
	{
		.start	= DAVINCI_EMAC_CNTRL_MOD_REGS_BASE,
		.end	= DAVINCI_EMAC_CNTRL_MOD_REGS_BASE + 0x0fff,
		.flags	= IORESOURCE_MEM,
		.name	= "ctrl_module_regs"
	},
	{
		.start	= DAVINCI_EMAC_CNTRL_RAM_BASE,
		.end	= DAVINCI_EMAC_CNTRL_RAM_BASE + 0x1fff,
		.flags	= IORESOURCE_MEM,
		.name	= "ctrl_ram"
	},
	{
		.start	= DAVINCI_EMAC_MDIO_REGS_BASE,
		.end	= DAVINCI_EMAC_MDIO_REGS_BASE + 0x07ff,
		.flags	= IORESOURCE_MEM,
		.name	= "mdio_regs"
	},
	{
		.start = IRQ_EMACINT,
		.end   = IRQ_EMACINT,
		.flags = IORESOURCE_IRQ,
	},
};

static struct emac_platform_data emac_pdata;

static struct platform_device davinci_emac_device = {
       .name		= "davinci_emac",
       .id		= 1,
       .num_resources	= ARRAY_SIZE(emac_resources),
       .resource	= emac_resources,
       .dev = {
		.platform_data = &emac_pdata,
	}
};

static struct resource dm646x_emac_resources[] = {
	{
		.start	= DAVINCI_EMAC_CNTRL_REGS_BASE,
		.end	= DAVINCI_EMAC_CNTRL_REGS_BASE + 0x0fff,
		.flags	= IORESOURCE_MEM,
		.name	= "ctrl_regs"
	},
	{
		.start	= DAVINCI_EMAC_CNTRL_MOD_REGS_BASE,
		.end	= DAVINCI_EMAC_CNTRL_MOD_REGS_BASE + 0x0fff,
		.flags	= IORESOURCE_MEM,
		.name	= "ctrl_module_regs"
	},
	{
		.start	= DAVINCI_EMAC_CNTRL_RAM_BASE,
		.end	= DAVINCI_EMAC_CNTRL_RAM_BASE + 0x1fff,
		.flags	= IORESOURCE_MEM,
		.name	= "ctrl_ram"
	},
	{
		.start	= DAVINCI_EMAC_MDIO_REGS_BASE,
		.end	= DAVINCI_EMAC_MDIO_REGS_BASE + 0x07ff,
		.flags	= IORESOURCE_MEM,
		.name	= "mdio_regs"
	},
	{
		.start	= IRQ_DM646X_EMACRXTHINT,
		.end	= IRQ_DM646X_EMACRXTHINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DM646X_EMACRXINT,
		.end	= IRQ_DM646X_EMACRXINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DM646X_EMACTXINT,
		.end	= IRQ_DM646X_EMACTXINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DM646X_EMACMISCINT,
		.end	= IRQ_DM646X_EMACMISCINT,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device dm646x_emac_device = {
	.name		= "davinci_emac",
	.id		= 1,
	.num_resources	= ARRAY_SIZE(dm646x_emac_resources),
	.resource	= dm646x_emac_resources,
	.dev = {
		.platform_data = &emac_pdata,
	}
};

#define RESOURCE_IRQ	4

static struct resource omapl1x7_emac_resources [] = {
        {
                .start  = OMAPL1X7_EMAC_CPGMAC_BASE,
                .end    = OMAPL1X7_EMAC_CPGMAC_BASE + 0xfff,
                .flags  = IORESOURCE_MEM,
                .name   = "ctrl_regs",
        },
        {
                .start  = OMAPL1X7_EMAC_CPGMACSS_BASE,
                .end    = OMAPL1X7_EMAC_CPGMACSS_BASE + 0xfff,
                .flags  = IORESOURCE_MEM,
                .name   = "ctrl_module_regs",
        },
        {
                .start  = OMAPL1X7_EMAC_CPPI_PORT_BASE,
                .end    = OMAPL1X7_EMAC_CPPI_PORT_BASE + 0x1fff,
                .flags  = IORESOURCE_MEM,
                .name   = "ctrl_ram",
        },
        {
                .start  = OMAPL1X7_EMAC_MDIO_BASE,
                .end    = OMAPL1X7_EMAC_MDIO_BASE + 0xfff,
                .flags  = IORESOURCE_MEM,
                .name   = "mdio_regs",
        },
        [RESOURCE_IRQ] = {
                .start  = IRQ_OMAPL1X7_C0_RX_THRESH_PULSE,
                .end	= IRQ_OMAPL1X7_C0_RX_THRESH_PULSE,
                .flags  = IORESOURCE_IRQ,
                .name   = "mac_rx_threshold",
        },
        {
                .start  = IRQ_OMAPL1X7_C0_RX_PULSE,
                .end	= IRQ_OMAPL1X7_C0_RX_PULSE,
                .flags  = IORESOURCE_IRQ,
                .name   = "mac_rx",
        },
        {
                .start  = IRQ_OMAPL1X7_C0_TX_PULSE,
                .end	= IRQ_OMAPL1X7_C0_TX_PULSE,
                .flags  = IORESOURCE_IRQ,
                .name   = "mac_tx",
        },
        {
                .start  = IRQ_OMAPL1X7_C0_MISC_PULSE,
                .end	= IRQ_OMAPL1X7_C0_MISC_PULSE,
                .flags  = IORESOURCE_IRQ,
                .name   = "mac_misc",
        },
};
static struct emac_platform_data omapl1x7_emac_pdata = {
	.phy_mask = 0x0,	/* No PHY */
};

static struct platform_device omapl1x7_emac_device = {
        .name = "davinci_emac",
        .id = 1,
        .num_resources = ARRAY_SIZE(omapl1x7_emac_resources),
        .resource = omapl1x7_emac_resources,
        .dev = {
                .platform_data = &omapl1x7_emac_pdata,
        }
};

void davinci_init_emac(char *mac_addr)
{
	DECLARE_MAC_BUF(buf);

	if (!(cpu_is_davinci_dm644x() || cpu_is_davinci_dm646x()
		|| cpu_is_omapl1x7()))
		return;

	if (cpu_is_omapl1x7()) {
		davinci_cfg_reg(OMAPL1X7_RMII_TXD_0);
		davinci_cfg_reg(OMAPL1X7_RMII_TXD_1);
		davinci_cfg_reg(OMAPL1X7_RMII_TXEN);
		davinci_cfg_reg(OMAPL1X7_RMII_CRS_DV);
		davinci_cfg_reg(OMAPL1X7_RMII_RXD_0);
		davinci_cfg_reg(OMAPL1X7_RMII_RXD_1);
		davinci_cfg_reg(OMAPL1X7_RMII_RXER);
		davinci_cfg_reg(OMAPL1X7_MDIO_CLK);
		davinci_cfg_reg(OMAPL1X7_MDIO_D);
	}

	/* if valid MAC exists, don't re-register */
	if (is_valid_ether_addr(emac_pdata.mac_addr))
		return;

	if (mac_addr && is_valid_ether_addr(mac_addr))
		memcpy(emac_pdata.mac_addr, mac_addr, 6);
	else {
		/* Use random MAC if none passed */
		random_ether_addr(emac_pdata.mac_addr);

		printk(KERN_WARNING "%s: using random MAC addr: %s\n",
		       __func__, print_mac(buf, emac_pdata.mac_addr));
	}
	if ((cpu_is_davinci_dm644x()))
		(void) platform_device_register(&davinci_emac_device);
	else if (cpu_is_davinci_dm646x())
		(void) platform_device_register(&dm646x_emac_device);
	else
		(void) platform_device_register(&omapl1x7_emac_device);
}

#else

void davinci_init_emac(char *unused) {}

#endif

/*-------------------------------------------------------------------------*/

static int __init davinci_init_devices(void)
{
	/* please keep these calls, and their implementations above,
	 * in alphabetical order so they're easier to sort through.
	 */
	davinci_init_wdt();

	return 0;
}
arch_initcall(davinci_init_devices);

static int __init davinci_init_devices_late(void)
{
	/* This is a backup call in case board code did not call init func */
	davinci_init_emac(NULL);

	return 0;
}
late_initcall(davinci_init_devices_late);
