/*
 * TI DaVinci DA850 EVM board
 *
 * Derived from: arch/arm/mach-davinci/board-evm.c
 * Copyright (C) 2006 Texas Instruments.
 *
 * (C) 2007-2008, MontaVista Software, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 */

/**************************************************************************
 * Included Files
 **************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/root_dev.h>
#include <linux/dma-mapping.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/io.h>
#include <linux/console.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/davinci_spi_master.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <mach/irqs.h>
#include <mach/edma.h>
#include <linux/kgdb.h>
#include <mach/common.h>
#include <mach/cpu.h>
#include <mach/clock.h>
#include <mach/psc.h>
#include <mach/da8xx.h>
#include <mach/soc-da8xx.h>
#include <mach/serial.h>
#include <mach/mux.h>
#include <mach/i2c.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/etherdevice.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <mach/emac.h>
#include <mach/nand.h>
#include <mach/mmc.h>
#include <mach/cp_intc.h>
#include <mach/asp.h>

#include "clock.h"

#define DA850_EVM_PHY_MASK		(0x1)
#define DA850_EVM_MDIO_FREQUENCY	(2200000) /* PHY bus frequency */

#define PCF8575_PORT0	(0)
#define PCF8575_PORT1	(8)
#define VIDEO_SEL	BIT(PCF8575_PORT0 + 5)
#define CAMERA_SEL	BIT(PCF8575_PORT0 + 6)
#define ADC_SEL		BIT(PCF8575_PORT0 + 7)

static struct emac_platform_data da850_evm_emac_pdata = {
	.phy_mask	= DA850_EVM_PHY_MASK,
	.mdio_max_freq	= DA850_EVM_MDIO_FREQUENCY,
	.rmii_en	= 0,
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
		.size = SZ_16K,
		.mask_flags = MTD_WRITEABLE,
	},
	[2] = {
		.name = "Linux",
		.offset = MTDPART_OFS_NXTBLK,
		.size = MTDPART_SIZ_FULL,
		.mask_flags = 0,
	},
};

struct davinci_spi_config_t w25x64_spi_cfg = {
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

static struct spi_board_info da850_spi_board_info0[] = {
	[0] = {
		.modalias = "m25p80",
		.platform_data = &spi_flash_data,
		.controller_data = &w25x64_spi_cfg,
		.mode = SPI_MODE_0,
		.max_speed_hz = 30000000,       /* max sample rate at 3V */
		.bus_num = 1,
		.chip_select = 0,
	},
};

struct mtd_partition da850_evm_nandflash_partition[] = {
	/* 5 MB space at the beginning for bootloader and kernel */
	{
		.name           = "NAND filesystem",
		.offset         = 3 * SZ_1M,
		.size           = SZ_4M,
		.mask_flags     = 0,
	}
};

#if defined (CONFIG_MTD_NAND_DAVINCI) || defined(CONFIG_MTD_NAND_DAVINCI_MODULE)
static struct davinci_nand_pdata da850_evm_nandflash_data = {
	.parts		= da850_evm_nandflash_partition,
	.nr_parts	= ARRAY_SIZE(da850_evm_nandflash_partition),
	.ecc_mode	= NAND_ECC_HW_SYNDROME,
	.options	= NAND_USE_FLASH_BBT,
};

#define SZ_32K	(32 * 1024)

static struct resource da850_evm_nandflash_resource[] = {
	{
		.start	= 0x62000000,
		.end	= 0x62000000 + SZ_512K + 2 * SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= 0x68000000,
		.end	= 0x68000000 + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da850_evm_nandflash_device = {
	.name		= "davinci_nand",
	.id		= 1,
	.dev		= {
		.platform_data  = &da850_evm_nandflash_data,
	},
	.num_resources	= ARRAY_SIZE(da850_evm_nandflash_resource),
	.resource	= da850_evm_nandflash_resource,
};
#endif

static int eth_addr_setup(char *str)
{
	int i;

	if (str == NULL)
		return 0;

	/* Conversion of a MAC address from a string (AA:BB:CC:DD:EE:FF)
	 * to a 6 bytes array. */
	for (i = 0; i < ETH_ALEN; i++)
		da850_evm_emac_pdata.mac_addr[i] = simple_strtol(&str[i*3],
							   (char **)NULL, 16);

	return 1;
}
/* Get MAC address from kernel boot parameter eth=AA:BB:CC:DD:EE:FF */
__setup("eth=", eth_addr_setup);

#if defined(CONFIG_MMC_DAVINCI) || defined(CONFIG_MMC_DAVINCI_MODULE)
static int da850_evm_mmc_get_ro(int index)
{
	/* GPIO 4[1] is used for MMC/SD WP - 16 * 4 + 1 = 65 */
	int val, status, gpio_num = 65;

	status = gpio_request(gpio_num, "MMC WP\n");
	if (status < 0) {
		printk(KERN_WARNING "%s can not open GPIO %d\n", __func__,
				gpio_num);
		return 0;
	}
	gpio_direction_input(gpio_num);
	val = gpio_get_value(gpio_num);
	gpio_free(gpio_num);
	return val;
}

static int da850_evm_mmc_get_cd(int index)
{
	/* GPIO 4[0] is used for MMC/SD WP - 16 * 4 + 0 = 64 */
	int val, status, gpio_num = 64;
       
	status = gpio_request(gpio_num, "MMC CD\n");
	if (status < 0) {
		printk(KERN_WARNING "%s can not open GPIO %d\n", __func__,
				gpio_num);
		return 0;
	}
	gpio_direction_input(gpio_num);
	val = gpio_get_value(gpio_num);
	gpio_free(gpio_num);
	return !val;
}

static struct davinci_mmc_config da850_mmc_config = {
	.get_ro         = da850_evm_mmc_get_ro,
	.get_cd		= da850_evm_mmc_get_cd,
	.wires          = 4,
	.version	= MMC_CTLR_VERSION_2,
	.cc_inst	= EDMA_CC_INST_0
};
#endif

static struct i2c_board_info __initdata i2c_info[] =  {
	{
		I2C_BOARD_INFO("pcf8575", 0x20),
	},
};

static struct davinci_i2c_platform_data da850_i2c_data0 = {
	.bus_freq       = 100,
	.bus_delay      = 0,
};

static struct davinci_uart_config uart_config __initdata = {
	.enabled_uarts = ((1 << 0) | (1 << 1) | (1 << 2)),
};

static void __init da850_evm_init_i2c(void)
{
	davinci_init_i2c(&da850_i2c_data0);
	i2c_register_board_info(1, i2c_info, ARRAY_SIZE(i2c_info));
}

static struct platform_device *da850_evm_devices[] __initdata = {
#if defined (CONFIG_MTD_NAND_DAVINCI) || defined(CONFIG_MTD_NAND_DAVINCI_MODULE)
	&da850_evm_nandflash_device,
#endif
};

static void __init da850_map_io(void)
{
	davinci_map_common_io();
	da850_init();
}

static __init void da850_evm_init(void)
{
	unsigned int *addr = IO_ADDRESS(DA8XX_CFGCHIP3);

	davinci_cfg_reg(DA850_UART2_RXD);
	davinci_cfg_reg(DA850_UART2_TXD);
	davinci_serial_init(&uart_config);
	/*
	 * shut down uart 0 and 1; they are not used on the board and
         * accessing them causes endless "too much work in irq53" messages
	 * with arago fs
         */
	__raw_writel(0, IO_ADDRESS(DA8XX_UART1_BASE) + 0x30);
	__raw_writel(0, IO_ADDRESS(DA8XX_UART0_BASE) + 0x30);

#if defined (CONFIG_MTD_NAND_DAVINCI) || defined(CONFIG_MTD_NAND_DAVINCI_MODULE)
	davinci_cfg_reg(DA850_EMA_D_0);
	davinci_cfg_reg(DA850_EMA_D_1);
	davinci_cfg_reg(DA850_EMA_D_2);
	davinci_cfg_reg(DA850_EMA_D_3);
	davinci_cfg_reg(DA850_EMA_D_4);
	davinci_cfg_reg(DA850_EMA_D_5);
	davinci_cfg_reg(DA850_EMA_D_6);
	davinci_cfg_reg(DA850_EMA_D_7);
	davinci_cfg_reg(DA850_NEMA_OE);
	davinci_cfg_reg(DA850_NEMA_CS_3);
	davinci_cfg_reg(DA850_NEMA_CS_4);
	davinci_cfg_reg(DA850_EMA_A_1);
	davinci_cfg_reg(DA850_EMA_A_2);
	davinci_cfg_reg(DA850_NEMA_WE);
#elif defined (CONFIG_MTD_CFI) || defined (CONFIG_MTD_CFI_MODULE)
	davinci_cfg_reg(DA850_EMA_D_0);
	davinci_cfg_reg(DA850_EMA_D_1);
	davinci_cfg_reg(DA850_EMA_D_2);
	davinci_cfg_reg(DA850_EMA_D_3);
	davinci_cfg_reg(DA850_EMA_D_4);
	davinci_cfg_reg(DA850_EMA_D_5);
	davinci_cfg_reg(DA850_EMA_D_6);
	davinci_cfg_reg(DA850_EMA_D_7);
	davinci_cfg_reg(DA850_EMA_D_8);
	davinci_cfg_reg(DA850_EMA_D_9);
	davinci_cfg_reg(DA850_EMA_D_10);
	davinci_cfg_reg(DA850_EMA_D_11);
	davinci_cfg_reg(DA850_EMA_D_12);
	davinci_cfg_reg(DA850_EMA_D_13);
	davinci_cfg_reg(DA850_EMA_D_14);
	davinci_cfg_reg(DA850_EMA_D_15);
	davinci_cfg_reg(DA850_EMA_A_0);
	davinci_cfg_reg(DA850_EMA_A_1);
	davinci_cfg_reg(DA850_EMA_A_2);
	davinci_cfg_reg(DA850_EMA_A_3);
	davinci_cfg_reg(DA850_EMA_A_4);
	davinci_cfg_reg(DA850_EMA_A_5);
	davinci_cfg_reg(DA850_EMA_A_6);
	davinci_cfg_reg(DA850_EMA_A_7);
	davinci_cfg_reg(DA850_EMA_A_8);
	davinci_cfg_reg(DA850_EMA_A_9);
	davinci_cfg_reg(DA850_EMA_A_10);
	davinci_cfg_reg(DA850_EMA_A_11);
	davinci_cfg_reg(DA850_EMA_A_12);
	davinci_cfg_reg(DA850_EMA_BA_1);
	davinci_cfg_reg(DA850_NEMA_WE);
	davinci_cfg_reg(DA850_NEMA_CS_2);
	davinci_cfg_reg(DA850_NEMA_OE);
#endif

	platform_add_devices(da850_evm_devices,
			     ARRAY_SIZE(da850_evm_devices));

	da850_evm_init_i2c();

#if defined(CONFIG_MMC_DAVINCI) || defined(CONFIG_MMC_DAVINCI_MODULE)
	/* Mux the GPIO required for WP feature */
	davinci_cfg_reg(DA850_GPIO4_0);
	davinci_cfg_reg(DA850_GPIO4_1);
	davinci_setup_mmc(0, &da850_mmc_config);
#endif

	da850_init_spi1(NULL, 1, da850_spi_board_info0,
			ARRAY_SIZE(da850_spi_board_info0));

	da8xx_init_rtc();

	da850_init_mcasp();

	if (da850_evm_emac_pdata.rmii_en) {
		/* RMII */
		__raw_writel((__raw_readl(addr)) | BIT(8), addr);
		davinci_cfg_reg(DA850_RMII_TXD_0);
		davinci_cfg_reg(DA850_RMII_TXD_1);
		davinci_cfg_reg(DA850_RMII_TXEN);
		davinci_cfg_reg(DA850_RMII_CRS_DV);
		davinci_cfg_reg(DA850_RMII_RXD_0);
		davinci_cfg_reg(DA850_RMII_RXD_1);
		davinci_cfg_reg(DA850_RMII_RXER);
		davinci_cfg_reg(DA850_RMII_MHZ_50_CLK);

	} else {
		/*MII */
		__raw_writel((__raw_readl(addr)) & ~BIT(8), addr);
		davinci_cfg_reg(DA850_MII_TXEN);
		davinci_cfg_reg(DA850_MII_TXCLK);
		davinci_cfg_reg(DA850_MII_COL);
		davinci_cfg_reg(DA850_MII_TXD_3);
		davinci_cfg_reg(DA850_MII_TXD_2);
		davinci_cfg_reg(DA850_MII_TXD_1);
		davinci_cfg_reg(DA850_MII_TXD_0);
		davinci_cfg_reg(DA850_MII_RXER);
		davinci_cfg_reg(DA850_MII_CRS);
		davinci_cfg_reg(DA850_MII_RXCLK);
		davinci_cfg_reg(DA850_MII_RXDV);
		davinci_cfg_reg(DA850_MII_RXD_3);
		davinci_cfg_reg(DA850_MII_RXD_2);
		davinci_cfg_reg(DA850_MII_RXD_1);
		davinci_cfg_reg(DA850_MII_RXD_0);
	}

	davinci_cfg_reg(DA850_MDIO_CLK);
	davinci_cfg_reg(DA850_MDIO_D);

	da850_init_emac(&da850_evm_emac_pdata);
}

static __init void da850_evm_irq_init(void)
{
	cp_intc_init((void __iomem*)IO_Dx_INTC_VIRT, IRQ_DA850_MCBSP1XINT + 1, NULL);
}

MACHINE_START(DAVINCI_DA850_EVM, "DaVinci DA850 EVM")
	.phys_io	= IO_PHYS,
	.io_pg_offst	= (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
	.boot_params	= (0xC0000100),
	.map_io		= da850_map_io,
	.init_irq	= da850_evm_irq_init,
	.timer		= &davinci_timer,
	.init_machine	= da850_evm_init,
MACHINE_END
