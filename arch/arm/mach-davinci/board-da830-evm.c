/*
 * TI DaVinci DA830 EVM board
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
#include <linux/i2c/at24.h>
#include <linux/etherdevice.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <mach/emac.h>
#include <mach/nand.h>
#include <mach/mmc.h>
#include <mach/cp_intc.h>

#include "clock.h"

#define DA830_EVM_PHY_MASK		(0x0)	/* No Phy */
#define DA830_EVM_MDIO_FREQUENCY	(2200000) /* PHY bus frequency */

static struct emac_platform_data da830_evm_emac_pdata = {
	.phy_mask	= DA830_EVM_PHY_MASK,
	.mdio_max_freq	= DA830_EVM_MDIO_FREQUENCY,
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

struct davinci_spi_config_t w25x32_spi_cfg = {
	.wdelay		= 0,
	.odd_parity	= 0,
	.parity_enable	= 0,
	.wait_enable	= 0,
	.lsb_first	= 0,
	.timer_disable	= 0,
	.clk_high	= 0,
	.phase_in	= 1,
	.clk_internal	= 1,
	.loop_back	= 0,
	.cs_hold	= 1,
	.intr_level	= 0,
	.pin_op_modes	= SPI_OPMODE_SPISCS_4PIN,
#ifndef CONFIG_SPI_INTERRUPT
	.poll_mode	= 1,
#endif
};

static struct flash_platform_data spi_flash_data = {
	.name = "m25p80",
	.parts = spi_flash_partitions,
	.nr_parts = ARRAY_SIZE(spi_flash_partitions),
	.type = "w25x32",
};

static struct spi_board_info da8xx_spi_board_info0[] = {
	[0] = {
		.modalias = "m25p80",
		.platform_data = &spi_flash_data,
		.controller_data = &w25x32_spi_cfg,
		.mode = SPI_MODE_0,
		.max_speed_hz = 30000000,	/* max sample rate at 3V */
		.bus_num = 0,
		.chip_select = 0,
	},
};
struct mtd_partition da830_evm_nandflash_partition[] = {
	/* 5 MB space at the beginning for bootloader and kernel */
	{
		.name           = "NAND filesystem",
		.offset         = 3 * SZ_1M,
		.size           = MTDPART_SIZ_FULL,
		.mask_flags     = 0,
	}
};

#if defined (CONFIG_MTD_NAND_DAVINCI) || defined(CONFIG_MTD_NAND_DAVINCI_MODULE)
static struct davinci_nand_pdata da830_evm_nandflash_data = {
	.parts		= da830_evm_nandflash_partition,
	.nr_parts	= ARRAY_SIZE(da830_evm_nandflash_partition),
	.ecc_mode	= NAND_ECC_HW_SYNDROME,
	.options	= NAND_USE_FLASH_BBT,
};

#define SZ_32K	(32 * 1024)

static struct resource da830_evm_nandflash_resource[] = {
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

static struct platform_device da830_evm_nandflash_device = {
	.name		= "davinci_nand",
	.id		= 1,
	.dev		= {
		.platform_data  = &da830_evm_nandflash_data,
	},
	.num_resources	= ARRAY_SIZE(da830_evm_nandflash_resource),
	.resource	= da830_evm_nandflash_resource,
};
#endif

/* Most of this EEPROM is unused, but U-Boot uses some data:
 *  - 0x7f00, 6 bytes Ethernet Address
 *  - ... newer boards may have more
 */
static struct at24_iface *at24_if;

static int at24_setup(struct at24_iface *iface, void *context)
{
	DECLARE_MAC_BUF(mac_str);
	char mac_addr[6];

	at24_if = iface;

	/* Read MAC addr from EEPROM */
	if (at24_if->read(at24_if, mac_addr, 0x7f00, 6) == 6) {
		printk(KERN_INFO "Read MAC addr from EEPROM: %s\n",
		print_mac(mac_str, mac_addr));

		memcpy(da830_evm_emac_pdata.mac_addr, mac_addr, 6);
	}
	return 0;
}

static struct at24_platform_data eeprom_info = {
	.byte_len	= (256*1024) / 8,
	.page_size	= 64,
	.flags		= AT24_FLAG_ADDR16,
	.setup		= at24_setup,
};

int da830evm_eeprom_read(void *buf, off_t off, size_t count)
{
	if (at24_if)
		return at24_if->read(at24_if, buf, off, count);
	return -ENODEV;
}
EXPORT_SYMBOL(da830evm_eeprom_read);

int da830evm_eeprom_write(void *buf, off_t off, size_t count)
{
	if (at24_if)
		return at24_if->write(at24_if, buf, off, count);
	return -ENODEV;
}
EXPORT_SYMBOL(da830evm_eeprom_write);

#if defined(CONFIG_MMC_DAVINCI) || defined(CONFIG_MMC_DAVINCI_MODULE)
static int da830_evm_mmc_get_ro(int index)
{
	int val, status, gpio_num = 33;

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

static int da830_evm_mmc_get_cd(int index)
{
	int val, status, gpio_num = 34;
       
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

static struct davinci_mmc_config da830_mmc_config = {
	.get_ro         = da830_evm_mmc_get_ro,
	.get_cd		= da830_evm_mmc_get_cd,
	.wires          = 4,
	.version	= MMC_CTLR_VERSION_2,
	.cc_inst	= EDMA_CC_INST_0
};
#endif

static struct i2c_board_info __initdata i2c_info[] =  {
	{
		I2C_BOARD_INFO("24c256", 0x50),
		.platform_data  = &eeprom_info,
	},
	{
		I2C_BOARD_INFO("pcf8574a", 0x3f),
	},
};

static struct davinci_i2c_platform_data da830_i2c_data0 = {
	.bus_freq       = 100,
	.bus_delay      = 0,
};

static struct davinci_uart_config uart_config __initdata = {
	.enabled_uarts = ((1 << 0) | (1 << 1) | (1 << 2)),
};

static void __init da830_evm_init_i2c(void)
{
	davinci_init_i2c(&da830_i2c_data0);
	i2c_register_board_info(1, i2c_info, ARRAY_SIZE(i2c_info));
}

static struct platform_device *da830_evm_devices[] __initdata = {
#if defined (CONFIG_MTD_NAND_DAVINCI) || defined(CONFIG_MTD_NAND_DAVINCI_MODULE)
	&da830_evm_nandflash_device,
#endif
};

static void __init da830_map_io(void)
{
	davinci_map_common_io();
	da830_init();
}

int da830_lcd_hw_init(void)
{
	struct i2c_client *client;
	struct i2c_adapter *adap = i2c_get_adapter(1);
	u32 val = 0;

	list_for_each_entry(client, &adap->clients, list) {
		if (!strcmp(client->name, "pcf8574a"))
			break;
	}
	if (client == NULL) {
		printk(KERN_ERR "PCF8574A Client not found\n");
		return -ENODEV;
	}
	val = i2c_smbus_read_byte(client);
	val &= ~(0x01 << 6);
	i2c_smbus_write_byte(client, val);

	return 0;
}
EXPORT_SYMBOL(da830_lcd_hw_init);

static __init void da830_evm_init(void)
{
	davinci_cfg_reg(DA830_UART2_RXD);
	davinci_cfg_reg(DA830_UART2_TXD);
	davinci_serial_init(&uart_config);

#if defined (CONFIG_MTD_NAND_DAVINCI) || defined(CONFIG_MTD_NAND_DAVINCI_MODULE)
	davinci_cfg_reg(DA830_EMA_D_0);
	davinci_cfg_reg(DA830_EMA_D_1);
	davinci_cfg_reg(DA830_EMA_D_2);
	davinci_cfg_reg(DA830_EMA_D_3);
	davinci_cfg_reg(DA830_EMA_D_4);
	davinci_cfg_reg(DA830_EMA_D_5);
	davinci_cfg_reg(DA830_EMA_D_6);
	davinci_cfg_reg(DA830_EMA_D_7);
	davinci_cfg_reg(DA830_EMA_D_8);
	davinci_cfg_reg(DA830_EMA_D_9);
	davinci_cfg_reg(DA830_EMA_D_10);
	davinci_cfg_reg(DA830_EMA_D_11);
	davinci_cfg_reg(DA830_EMA_D_12);
	davinci_cfg_reg(DA830_EMA_D_13);
	davinci_cfg_reg(DA830_EMA_D_14);
	davinci_cfg_reg(DA830_EMA_D_15);
	davinci_cfg_reg(DA830_EMA_A_0);
	davinci_cfg_reg(DA830_EMA_A_1);
	davinci_cfg_reg(DA830_EMA_A_2);
	davinci_cfg_reg(DA830_EMA_A_3);
	davinci_cfg_reg(DA830_EMA_A_4);
	davinci_cfg_reg(DA830_EMA_A_5);
	davinci_cfg_reg(DA830_EMA_A_6);
	davinci_cfg_reg(DA830_EMA_A_7);
	davinci_cfg_reg(DA830_EMA_A_8);
	davinci_cfg_reg(DA830_EMA_A_9);
	davinci_cfg_reg(DA830_EMA_A_10);
	davinci_cfg_reg(DA830_EMA_A_11);
	davinci_cfg_reg(DA830_EMA_A_12);
	davinci_cfg_reg(DA830_EMA_BA_0);
	davinci_cfg_reg(DA830_EMA_BA_1);
	davinci_cfg_reg(DA830_EMA_CLK);
	davinci_cfg_reg(DA830_EMA_SDCKE);
	davinci_cfg_reg(DA830_NEMA_CAS);
	davinci_cfg_reg(DA830_NEMA_RAS);
	davinci_cfg_reg(DA830_NEMA_WE);
	davinci_cfg_reg(DA830_NEMA_CS_0);
	davinci_cfg_reg(DA830_NEMA_CS_2);
	davinci_cfg_reg(DA830_NEMA_CS_3);
	davinci_cfg_reg(DA830_NEMA_CAS);
	davinci_cfg_reg(DA830_NEMA_RAS);
	davinci_cfg_reg(DA830_NEMA_OE);
	davinci_cfg_reg(DA830_NEMA_WE_DQM_1);
	davinci_cfg_reg(DA830_NEMA_WE_DQM_0);
	davinci_cfg_reg(DA830_EMA_WAIT_0);
#endif

	platform_add_devices(da830_evm_devices,
			     ARRAY_SIZE(da830_evm_devices));

	da830_evm_init_i2c();

#if defined(CONFIG_MMC_DAVINCI) || defined(CONFIG_MMC_DAVINCI_MODULE)
	davinci_setup_mmc(0, &da830_mmc_config);
#endif

	da8xx_init_spi0(NULL, 1, da8xx_spi_board_info0,
		ARRAY_SIZE(da8xx_spi_board_info0));

	da8xx_init_rtc();

	da830_init_mcasp1();

	da830_init_emac(&da830_evm_emac_pdata);
}

static __init void da830_evm_irq_init(void)
{
	cp_intc_init((void __iomem*)IO_Dx_INTC_VIRT, IRQ_DA8XX_ARMCLKSTOPREQ + 1, NULL);
}

MACHINE_START(DAVINCI_DA8XX_EVM, "DaVinci DA830 EVM")
	.phys_io	= IO_PHYS,
	.io_pg_offst	= (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
	.boot_params	= (0xC0000100),
	.map_io		= da830_map_io,
	.init_irq	= da830_evm_irq_init,
	.timer		= &davinci_timer,
	.init_machine	= da830_evm_init,
MACHINE_END
