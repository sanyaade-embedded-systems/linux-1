/*
 * TI DA850/OMAP-L138 EVM board
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Derived from: arch/arm/mach-davinci/board-da830-evm.c
 * Original Copyrights follow:
 *
 * 2007, 2009 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/i2c/pca953x.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/davinci_spi_master.h>
#include <linux/regulator/machine.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/common.h>
#include <mach/irqs.h>
#include <mach/cp_intc.h>
#include <mach/da8xx.h>
#include <mach/nand.h>
#include <mach/mux.h>
#include <mach/usb.h>

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
static int da850_evm_usb1_vbus(unsigned port, int on);
static irqreturn_t da850_usb1_oc_handler(int irq, void *dev);
static int da850_evm_usb1_ocic_notify(da830_ocic_handler_t handler);
static int da850_evm_usb1_get_oci(unsigned port);
extern struct da830_ohci_root_hub da850_ohci_rh_data;
da830_ocic_handler_t usb1_root_hub_oc;
static int usb1_oci;
#endif

#define DA850_EVM_PHY_MASK		0x1
#define DA850_EVM_MDIO_FREQUENCY	2200000 /* PHY bus frequency */

#define DA850_LCD_PWR_PIN		GPIO_TO_PIN(2, 8)
#define DA850_LCD_BL_PIN		GPIO_TO_PIN(2, 15)

#define DA850_MMCSD_CD_PIN		GPIO_TO_PIN(4, 0)
#define DA850_MMCSD_WP_PIN		GPIO_TO_PIN(4, 1)

#define DA850_EMAC_RMII_PIN		GPIO_TO_PIN(2, 6)

#define DA850_USB1_VBUS_PIN		GPIO_TO_PIN(2, 4)
#define DA850_USB1_OC_PIN		GPIO_TO_PIN(6, 13)

static struct mtd_partition da850_evm_norflash_partition[] = {
	{
		.name           = "bootloaders + env",
		.offset         = 0,
		.size           = SZ_512K,
		.mask_flags     = MTD_WRITEABLE,
	},
	{
		.name           = "kernel",
		.offset         = MTDPART_OFS_APPEND,
		.size           = SZ_2M,
		.mask_flags     = 0,
	},
	{
		.name           = "filesystem",
		.offset         = MTDPART_OFS_APPEND,
		.size           = MTDPART_SIZ_FULL,
		.mask_flags     = 0,
	},
};

static struct physmap_flash_data da850_evm_norflash_data = {
	.width		= 2,
	.parts		= da850_evm_norflash_partition,
	.nr_parts	= ARRAY_SIZE(da850_evm_norflash_partition),
};

static struct resource da850_evm_norflash_resource[] = {
	{
		.start	= DA8XX_AEMIF_CS2_BASE,
		.end	= DA8XX_AEMIF_CS2_BASE + SZ_32M - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da850_evm_norflash_device = {
	.name		= "physmap-flash",
	.id		= 0,
	.dev		= {
		.platform_data  = &da850_evm_norflash_data,
	},
	.num_resources	= 1,
	.resource	= da850_evm_norflash_resource,
};

/* DA850/OMAP-L138 EVM includes a 512 MByte large-page NAND flash
 * (128K blocks). It may be used instead of the (default) SPI flash
 * to boot, using TI's tools to install the secondary boot loader
 * (UBL) and U-Boot.
 */
struct mtd_partition da850_evm_nandflash_partition[] = {
	{
		.name		= "u-boot env",
		.offset		= 0,
		.size		= SZ_128K,
		.mask_flags	= MTD_WRITEABLE,
	 },
	{
		.name		= "UBL",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_128K,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "u-boot",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 4 * SZ_128K,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "kernel",
		.offset		= 0x200000,
		.size		= SZ_2M,
		.mask_flags	= 0,
	},
	{
		.name		= "filesystem",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
		.mask_flags	= 0,
	},
};

static struct davinci_nand_pdata da850_evm_nandflash_data = {
	.parts		= da850_evm_nandflash_partition,
	.nr_parts	= ARRAY_SIZE(da850_evm_nandflash_partition),
	.ecc_mode	= NAND_ECC_HW,
	.ecc_bits	= 4,
	.options	= NAND_USE_FLASH_BBT,
};

static struct resource da850_evm_nandflash_resource[] = {
	{
		.start	= DA8XX_AEMIF_CS3_BASE,
		.end	= DA8XX_AEMIF_CS3_BASE + SZ_512K + 2 * SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= DA8XX_AEMIF_CTL_BASE,
		.end	= DA8XX_AEMIF_CTL_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da850_evm_nandflash_device = {
	.name		= "davinci_nand",
	.id		= 1,
	.dev		= {
		.platform_data	= &da850_evm_nandflash_data,
	},
	.num_resources	= ARRAY_SIZE(da850_evm_nandflash_resource),
	.resource	= da850_evm_nandflash_resource,
};

static struct mtd_partition spi_flash_partitions[] = {
	[0] = {
		.name = "U-Boot",
		.offset = 0,
		.size = SZ_256K,
		.mask_flags = MTD_WRITEABLE,
	},
	[1] = {
		.name = "U-Boot Environment",
		.offset = MTDPART_OFS_APPEND,
		.size = SZ_64K,
		.mask_flags = MTD_WRITEABLE,
	},
	[2] = {
		.name = "Linux",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_8M - (SZ_256K + SZ_64K + SZ_64K),
		.mask_flags = 0,
	},
	[3] = {
		.name = "MAC Address",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_64K,
		.mask_flags = MTD_WRITEABLE,
		.setup = davinci_get_mac_addr,
		.context = (void *)0,
	},
};

struct davinci_spi_config_t m25p64_spi_cfg = {
	.wdelay         = 0,
	.odd_parity     = 0,
	.parity_enable  = 0,
	.wait_enable    = 0,
	.lsb_first      = 0,
	.timer_disable  = 0,
	.clk_high       = 0,
	.phase_in       = 1,
	.clk_internal   = 1,
	.loop_back      = 0,
	.cs_hold        = 1,
	.intr_level     = 0,
	.pin_op_modes   = SPI_OPMODE_SPISCS_4PIN,
#ifndef CONFIG_SPI_INTERRUPT
	.poll_mode      = 1,
#endif
};

static struct flash_platform_data spi_flash_data = {
	.name = "m25p80",
	.parts = spi_flash_partitions,
	.nr_parts = ARRAY_SIZE(spi_flash_partitions),
	.type = "m25p64",
};

static struct spi_board_info da850_spi_board_info[] = {
	[0] = {
		.modalias = "m25p80",
		.platform_data = &spi_flash_data,
		.controller_data = &m25p64_spi_cfg,
		.mode = SPI_MODE_0,
		.max_speed_hz = 30000000,       /* max sample rate at 3V */
		.bus_num = 1,
		.chip_select = 0,
	},
};

static struct davinci_i2c_platform_data da850_evm_i2c_0_pdata = {
	.bus_freq	= 100,	/* kHz */
	.bus_delay	= 0,	/* usec */
};

static struct davinci_uart_config da850_evm_uart_config __initdata = {
	.enabled_uarts = 0x7,
};

static struct platform_device *da850_evm_devices[] __initdata = {
	&da850_evm_nandflash_device,
	&da850_evm_norflash_device,
};

/* davinci da850 evm audio machine driver */
static u8 da850_iis_serializer_direction[] = {
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	TX_MODE,
	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};

static struct snd_platform_data da850_evm_snd_data = {
	.tx_dma_offset	= 0x2000,
	.rx_dma_offset	= 0x2000,
	.op_mode	= DAVINCI_MCASP_IIS_MODE,
	.num_serializer	= ARRAY_SIZE(da850_iis_serializer_direction),
	.tdm_slots	= 2,
	.serial_dir	= da850_iis_serializer_direction,
	.eventq_no	= EVENTQ_1,
	.version	= MCASP_VERSION_2,
	.txnumevt	= 1,
	.rxnumevt	= 1,
};

static struct davinci_mcbsp_platform_data da850_mcbsp0_config = {
	.inst	= 0,
};

static struct davinci_mcbsp_platform_data da850_mcbsp1_config = {
	.inst	= 1,
};

static int da850_evm_mmc_get_ro(int index)
{
	return gpio_get_value(DA850_MMCSD_WP_PIN);
}

static int da850_evm_mmc_get_cd(int index)
{
	return !gpio_get_value(DA850_MMCSD_CD_PIN);
}

static struct davinci_mmc_config da850_mmc_config = {
	.get_ro		= da850_evm_mmc_get_ro,
	.get_cd		= da850_evm_mmc_get_cd,
	.wires		= 4,
	.version	= MMC_CTLR_VERSION_2,
};

static int da850_lcd_hw_init(void)
{
	int status;

	status = gpio_request(DA850_LCD_BL_PIN, "lcd bl\n");
	if (status < 0)
		return status;

	status = gpio_request(DA850_LCD_PWR_PIN, "lcd pwr\n");
	if (status < 0) {
		gpio_free(DA850_LCD_BL_PIN);
		return status;
	}

	gpio_direction_output(DA850_LCD_BL_PIN, 0);
	gpio_direction_output(DA850_LCD_PWR_PIN, 0);

	/* disable lcd backlight */
	gpio_set_value(DA850_LCD_BL_PIN, 0);

	/* disable lcd power */
	gpio_set_value(DA850_LCD_PWR_PIN, 0);

	/* enable lcd power */
	gpio_set_value(DA850_LCD_PWR_PIN, 1);

	/* enable lcd backlight */
	gpio_set_value(DA850_LCD_BL_PIN, 1);

	return 0;
}

#define DA8XX_AEMIF_CE2CFG_OFFSET	0x10
#define DA8XX_AEMIF_ASIZE_16BIT		0x1

static void __init da850_evm_init_nor(void)
{
	void __iomem *aemif_addr;

	aemif_addr = ioremap(DA8XX_AEMIF_CTL_BASE, SZ_32K);

	/* Configure data bus width of CS2 to 16 bit */
	writel(readl(aemif_addr + DA8XX_AEMIF_CE2CFG_OFFSET) |
		DA8XX_AEMIF_ASIZE_16BIT,
		aemif_addr + DA8XX_AEMIF_CE2CFG_OFFSET);

	iounmap(aemif_addr);
}

/* TPS65070 voltage regulator support */

/* 3.3V */
struct regulator_consumer_supply tps65070_dcdc1_consumers[] = {
	{
		.supply = "usb0_vdda33",
	},
	{
		.supply = "usb1_vdda33",
	},
};

/* 3.3V or 1.8V */
struct regulator_consumer_supply tps65070_dcdc2_consumers[] = {
	{
		.supply = "dvdd3318_a",
	},
	{
		.supply = "dvdd3318_b",
	},
	{
		.supply = "dvdd3318_c",
	},
};

/* 1.2V */
struct regulator_consumer_supply tps65070_dcdc3_consumers[] = {
	{
		.supply = "cvdd",
	},
};

/* 1.8V LDO */
struct regulator_consumer_supply tps65070_ldo1_consumers[] = {
	{
		.supply = "sata_vddr",
	},
	{
		.supply = "usb0_vdda18",
	},
	{
		.supply = "usb1_vdda18",
	},
	{
		.supply = "ddr_dvdd18",
	},
};

/* 1.2V LDO */
struct regulator_consumer_supply tps65070_ldo2_consumers[] = {
	{
		.supply = "sata_vdd",
	},
	{
		.supply = "pll0_vdda",
	},
	{
		.supply = "pll1_vdda",
	},
	{
		.supply = "usbs_cvdd",
	},
	{
		.supply = "vddarnwa1",
	},
};

struct regulator_init_data tps65070_regulator_data[] = {
	/* dcdc1 */
	{
		.constraints = {
			.min_uV = 3150000,
			.max_uV = 3450000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_dcdc1_consumers),
		.consumer_supplies = tps65070_dcdc1_consumers,
	},

	/* dcdc2 */
	{
		.constraints = {
			.min_uV = 1710000,
			.max_uV = 3450000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_dcdc2_consumers),
		.consumer_supplies = tps65070_dcdc2_consumers,
	},

	/* dcdc3 */
	{
		.constraints = {
			.min_uV = 950000,
			.max_uV = 1320000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_dcdc3_consumers),
		.consumer_supplies = tps65070_dcdc3_consumers,
	},

	/* ldo1 */
	{
		.constraints = {
			.min_uV = 1710000,
			.max_uV = 1890000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_ldo1_consumers),
		.consumer_supplies = tps65070_ldo1_consumers,
	},

	/* ldo2 */
	{
		.constraints = {
			.min_uV = 1140000,
			.max_uV = 1320000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_ldo2_consumers),
		.consumer_supplies = tps65070_ldo2_consumers,
	},
};

static struct i2c_board_info __initdata da850evm_tps65070_info[] = {
	{
		I2C_BOARD_INFO("tps6507x", 0x48),
		.platform_data = &tps65070_regulator_data[0],
	},
};

static int __init pmic_tps65070_init(void)
{
	return i2c_register_board_info(1, da850evm_tps65070_info,
					ARRAY_SIZE(da850evm_tps65070_info));
}

#if defined(CONFIG_MTD_PHYSMAP) || \
    defined(CONFIG_MTD_PHYSMAP_MODULE)
#define HAS_NOR 1
#else
#define HAS_NOR 0
#endif

#if defined(CONFIG_MMC_DAVINCI) || \
    defined(CONFIG_MMC_DAVINCI_MODULE)
#define HAS_MMC 1
#else
#define HAS_MMC 0
#endif

#if defined(CONFIG_DAVINCI_MCBSP0)
#define HAS_MCBSP0 1
#else
#define HAS_MCBSP0 0
#endif

#if defined(CONFIG_DAVINCI_MCBSP1)
#define HAS_MCBSP1 1
#else
#define HAS_MCBSP1 0
#endif

#if defined(CONFIG_TI_DAVINCI_EMAC) || \
	defined(CONFIG_TI_DAVINCI_EMAC_MODULE)
#define HAS_EMAC 1
#else
#define HAS_EMAC 0
#endif

#if defined(CONFIG_PARPORT_DA8XX) || \
	defined(CONFIG_PARPORT_DA8XX_MODULE)
#define HAS_CHAR_LCD 1
#else
#define HAS_CHAR_LCD 0
#endif

static int gpio_exp_setup(struct i2c_client *client, unsigned gpio,
						unsigned ngpio, void *c)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;
	int sel_a, sel_b, sel_c;

	sel_a = gpio + 7;
	sel_b = gpio + 6;
	sel_c = gpio + 5;

	/* deselect all fucntionalities */
	gpio_request(sel_a, "sel_a");
	gpio_direction_output(sel_a, 1);

	gpio_request(sel_b, "sel_b");
	gpio_direction_output(sel_b, 1);

	gpio_request(sel_c, "sel_c");
	gpio_direction_output(sel_c, 1);

	if (soc_info->emac_pdata->rmii_en) {
		/* enable RMII */
		gpio_direction_output(sel_a, 0);
		gpio_direction_output(sel_b, 1);
		gpio_direction_output(sel_c, 1);
	} else if (HAS_CHAR_LCD) {
		/* enable CHAR LCD */
		gpio_direction_output(sel_a, 0);
		gpio_direction_output(sel_b, 0);
		gpio_direction_output(sel_c, 0);
	} else {
		/* do nothing */
	}

	return 0;
}

static int gpio_exp_teardown(struct i2c_client *client, unsigned gpio,
						unsigned ngpio, void *c)
{
	gpio_free(gpio + 5);
	gpio_free(gpio + 6);
	gpio_free(gpio + 7);

	return 0;
}

static struct pca953x_platform_data gpio_exp = {
	.gpio_base	= DAVINCI_N_GPIO,
	.setup		= gpio_exp_setup,
	.teardown	= gpio_exp_teardown,
};

static struct i2c_board_info __initdata i2c_info[] =  {
	{
		I2C_BOARD_INFO("tca6416", 0x20),
		.platform_data = &gpio_exp,
	},
};

static void __init da850_evm_config_emac(u8 rmii_en)
{
	void __iomem *cfg_chip3_base;
	int ret;
	u32 val;

	cfg_chip3_base = DA8XX_SYSCFG_VIRT(DA8XX_CFGCHIP3_REG);

	/* configure the CFGCHIP3 register for RMII or MII */
	val = readl(cfg_chip3_base);
	if (rmii_en)
		val |= BIT(8);
	else
		val &= ~BIT(8);

	writel(val, cfg_chip3_base);

	if (!rmii_en)
		ret = da8xx_pinmux_setup(da850_cpgmac_pins);
	else
		ret = da8xx_pinmux_setup(da850_rmii_pins);
	if (ret)
		pr_warning("da850_evm_init: cpgmac/rmii mux setup failed: %d\n",
				ret);

	if (rmii_en) {
		/* Disable MII clock */
		davinci_cfg_reg(DA850_GPIO2_6);
		gpio_request(DA850_EMAC_RMII_PIN, "mdio_clk_en");
		gpio_direction_output(DA850_EMAC_RMII_PIN, 1);
	}
}

static __init void da850_evm_init(void)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;
	int ret;

#ifdef CONFIG_DA850_RMII
	if (HAS_CHAR_LCD)
		pr_warning("WARNING: both Character LCD and RMII are "
			"enabled, but only one can work.\n"
			"\tUse MII support on base board instead of "
			"RMII.\n");
	else
		soc_info->emac_pdata->rmii_en = 1;
#else
	soc_info->emac_pdata->rmii_en = 0;
#endif

	ret = pmic_tps65070_init();
	if (ret)
		pr_warning("da850_evm_init: TPS65070 PMIC init failed: %d\n",
				ret);

	ret = da8xx_pinmux_setup(da850_nand_pins);
	if (ret)
		pr_warning("da850_evm_init: nand mux setup failed: %d\n",
				ret);

	ret = da8xx_pinmux_setup(da850_nor_pins);
	if (ret)
		pr_warning("da850_evm_init: nor mux setup failed: %d\n",
				ret);

	da850_evm_init_nor();

	platform_add_devices(da850_evm_devices,
				ARRAY_SIZE(da850_evm_devices));

	ret = da8xx_register_edma();
	if (ret)
		pr_warning("da850_evm_init: edma registration failed: %d\n",
				ret);

	i2c_register_board_info(1, i2c_info, ARRAY_SIZE(i2c_info));

	ret = da8xx_pinmux_setup(da850_i2c0_pins);
	if (ret)
		pr_warning("da850_evm_init: i2c0 mux setup failed: %d\n",
				ret);

	ret = da8xx_register_i2c(0, &da850_evm_i2c_0_pdata);
	if (ret)
		pr_warning("da850_evm_init: i2c0 registration failed: %d\n",
				ret);

	soc_info->emac_pdata->phy_mask = DA850_EVM_PHY_MASK;
	soc_info->emac_pdata->mdio_max_freq = DA850_EVM_MDIO_FREQUENCY;
	da850_evm_config_emac(soc_info->emac_pdata->rmii_en);
	ret = da8xx_register_emac();
	if (ret)
		pr_warning("da850_evm_init: emac registration failed: %d\n",
				ret);

	ret = da8xx_register_watchdog();
	if (ret)
		pr_warning("da830_evm_init: watchdog registration failed: %d\n",
				ret);

	if (HAS_MMC) {
		if (HAS_NOR)
			pr_warning("WARNING: both NOR Flash and MMC/SD are "
				"enabled, but they share AEMIF pins.\n"
				"\tDisable one of them.\n");

		ret = da8xx_pinmux_setup(da850_mmcsd0_pins);
		if (ret)
			pr_warning("da850_evm_init: mmcsd0 mux setup failed:"
					" %d\n", ret);

		ret = gpio_request(DA850_MMCSD_CD_PIN, "MMC CD\n");
		if (ret)
			pr_warning("da850_evm_init: can not open GPIO %d\n",
					DA850_MMCSD_CD_PIN);
		gpio_direction_input(DA850_MMCSD_CD_PIN);

		ret = gpio_request(DA850_MMCSD_WP_PIN, "MMC WP\n");
		if (ret)
			pr_warning("da850_evm_init: can not open GPIO %d\n",
					DA850_MMCSD_WP_PIN);
		gpio_direction_input(DA850_MMCSD_WP_PIN);

		ret = da8xx_register_mmcsd0(&da850_mmc_config);
		if (ret)
			pr_warning("da850_evm_init: mmcsd0 registration failed:"
					" %d\n", ret);
	}

	davinci_serial_init(&da850_evm_uart_config);

	/*
	 * shut down uart 0 and 1; they are not used on the board and
	 * accessing them causes endless "too much work in irq53" messages
	 * with arago fs
	 */
	__raw_writel(0, IO_ADDRESS(DA8XX_UART1_BASE) + 0x30);
	__raw_writel(0, IO_ADDRESS(DA8XX_UART0_BASE) + 0x30);

	ret = da8xx_pinmux_setup(da850_mcasp_pins);
	if (ret)
		pr_warning("da850_evm_init: mcasp mux setup failed: %d\n",
				ret);

	da8xx_init_mcasp(0, &da850_evm_snd_data);

	ret = da8xx_pinmux_setup(da850_lcdcntl_pins);
	if (ret)
		pr_warning("da850_evm_init: lcdcntl mux setup failed: %d\n",
				ret);

	ret = da850_lcd_hw_init();
	if (ret)
		pr_warning("da850_evm_init: lcd initialization failed: %d\n",
				ret);

	ret = da8xx_register_lcdc();
	if (ret)
		pr_warning("da850_evm_init: lcdc registration failed: %d\n",
				ret);

	if (HAS_MCBSP0) {
		if (HAS_EMAC)
			pr_warning("WARNING: both MCBSP0 and EMAC are "
				"enabled, but they share pins.\n"
				"\tDisable one of them.\n");

		ret = da850_init_mcbsp(&da850_mcbsp0_config);
		if (ret)
			pr_warning("da850_evm_init: mcbsp0 registration"
					"failed: %d\n",	ret);
	}

	if (HAS_MCBSP1) {
		ret = da850_init_mcbsp(&da850_mcbsp1_config);
		if (ret)
			pr_warning("da850_evm_init: mcbsp1 registration"
					" failed: %d\n", ret);
	}

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	/* Own the VBUS line for USB1 interface */
	ret = da8xx_pinmux_setup(da850_usb11_pins);
	if (ret)
		pr_warning("da850_evm_init: usb1 mux setup failed: %d\n",
				ret);

	if (gpio_request(DA850_USB1_VBUS_PIN, "USB1 VBUS\n") < 0)
		pr_warning("da850_evm_init: usb1 vbus gpio ownership failed\n");

	if (gpio_request(DA850_USB1_OC_PIN, "USB1 OC\n") < 0)
		pr_warning("da850_evm_init: usb1 oc gpio ownership failed\n");

	gpio_direction_output(DA850_USB1_VBUS_PIN, 0);
	gpio_direction_output(DA850_USB1_OC_PIN, 1);

	if (request_irq(gpio_to_irq(DA850_USB1_OC_PIN), da850_usb1_oc_handler,
			0, "usb1_oc_handler", NULL))
		pr_warning("da850_evm_init: usb1 OC irq registration failed\n");

	set_irq_type(gpio_to_irq(DA850_USB1_OC_PIN), IRQ_TYPE_LEVEL_LOW);

	da850_ohci_rh_data.set_power = da850_evm_usb1_vbus;
	da850_ohci_rh_data.ocic_notify = da850_evm_usb1_ocic_notify;
	da850_ohci_rh_data.get_oci = da850_evm_usb1_get_oci;
#endif

	ret = da8xx_register_ohci();
	if (ret)
		pr_warning("da850_evm_init: OHCI registration failed: %d\n",
				ret);

#if  defined(CONFIG_USB_MUSB_HDRC) || defined(CONFIG_USB_MUSB_HDRC_MODULE)
	/* Setup the USB0 interface w.r.t platform infrastructure */
	setup_usb(500, 8);
#endif

	ret = da8xx_pinmux_setup(da850_spi1_pins);
	if (ret)
		pr_warning("da850_evm_init: spi1 mux setup failed: %d\n",
				ret);

	da850_init_spi1(NULL, 1, da850_spi_board_info,
			ARRAY_SIZE(da850_spi_board_info));

	da8xx_register_rtc();
	
	ret = da8xx_register_sata();
	if (ret)
		pr_warning("da850_evm_init: sata registration failed: %d\n",
				ret);
}

#ifdef CONFIG_SERIAL_8250_CONSOLE
static int __init da850_evm_console_init(void)
{
	return add_preferred_console("ttyS", 2, "115200");
}
console_initcall(da850_evm_console_init);
#endif

static __init void da850_evm_irq_init(void)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;

	cp_intc_init((void __iomem *)DA8XX_CP_INTC_VIRT, DA850_N_CP_INTC_IRQ,
			soc_info->intc_irq_prios);
}

static void __init da850_evm_map_io(void)
{
	da850_init();
}

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
/*
 * USB1 VBUS control on OMAPL138 EVM.  This functiion uses the GPIO2[4] line
 * to control the USB1 VBUS line.
 */
static int da850_evm_usb1_vbus(unsigned port, int on)
{
	if (on)
		usb1_oci = 0;

	gpio_set_value(DA850_USB1_VBUS_PIN, on);
	return 0;
}

/*
 * Call the related OHCI module handler in the event of a OC condition.
 * OC indication is provided by GPIO6[13] line.
 */
static irqreturn_t da850_usb1_oc_handler(int irq, void *dev)
{
	usb1_oci = 1;
	usb1_root_hub_oc(&da850_ohci_rh_data, 0);
	return IRQ_HANDLED;
}

/*
 * Register for a OHCI OC handler
 */
static int da850_evm_usb1_ocic_notify(da830_ocic_handler_t handler)
{
	usb1_root_hub_oc = handler;
	return 0;
}

/*
 * Provide the OC status for the root hub port.
 */
static int da850_evm_usb1_get_oci(unsigned port)
{
	return usb1_oci;
}
#endif

MACHINE_START(DAVINCI_DA850_EVM, "DaVinci DA850/OMAP-L138 EVM")
	.phys_io	= IO_PHYS,
	.io_pg_offst	= (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
	.boot_params	= (DA8XX_DDR_BASE + 0x100),
	.map_io		= da850_evm_map_io,
	.init_irq	= da850_evm_irq_init,
	.timer		= &davinci_timer,
	.init_machine	= da850_evm_init,
MACHINE_END
