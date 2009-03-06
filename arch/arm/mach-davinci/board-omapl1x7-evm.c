/*
 * TI DaVinci OMAPL1X7 EVM board
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

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>
#include <mach/irqs.h>
#include <mach/edma.h>
#include <linux/kgdb.h>
#include <mach/common.h>
#include <mach/cpu.h>
#include <mach/clock.h>
#include <mach/psc.h>
#include <mach/omapl1x7.h>
#include <mach/soc-omapl1x7.h>
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
#include <mach/mmc.h>

#include "clock.h"

struct mtd_partition omapl1x7_evm_nandflash_partition[] = {
	/* 5 MB space at the beginning for bootloader and kernel */
	{
		.name           = "NAND filesystem",
		.offset         = 3 * SZ_1M,
		.size           = MTDPART_SIZ_FULL,
		.mask_flags     = 0,
	}
};

static struct flash_platform_data omapl1x7_evm_nandflash_data = {
	.parts		= omapl1x7_evm_nandflash_partition,
	.nr_parts	= ARRAY_SIZE(omapl1x7_evm_nandflash_partition),
};

#define SZ_32K	(32 * 1024)

static struct resource omapl1x7_evm_nandflash_resource[] = {
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

static struct platform_device omapl1x7_evm_nandflash_device = {
	.name		= "davinci_nand",
	.id		= 0,
	.dev		= {
		.platform_data  = &omapl1x7_evm_nandflash_data,
	},
	.num_resources	= ARRAY_SIZE(omapl1x7_evm_nandflash_resource),
	.resource	= omapl1x7_evm_nandflash_resource,
};

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

		davinci_init_emac(mac_addr);
	}
	return 0;
}

static struct at24_platform_data eeprom_info = {
	.byte_len	= (256*1024) / 8,
	.page_size	= 64,
	.flags		= AT24_FLAG_ADDR16,
	.setup		= at24_setup,
};

int omapl1x7evm_eeprom_read(void *buf, off_t off, size_t count)
{
	if (at24_if)
		return at24_if->read(at24_if, buf, off, count);
	return -ENODEV;
}
EXPORT_SYMBOL(omapl1x7evm_eeprom_read);

int omapl1x7evm_eeprom_write(void *buf, off_t off, size_t count)
{
	if (at24_if)
		return at24_if->write(at24_if, buf, off, count);
	return -ENODEV;
}
EXPORT_SYMBOL(omapl1x7evm_eeprom_write);

static int omapl1x7_evm_mmc_get_ro(int index)
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

static int omapl1x7_evm_mmc_get_cd(int index)
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

static struct davinci_mmc_config omapl1x7_mmc_config = {
	.get_ro         = omapl1x7_evm_mmc_get_ro,
	.get_cd		= omapl1x7_evm_mmc_get_cd,
	.wires          = 4
};

static struct i2c_board_info __initdata i2c_info[] =  {
	{
		I2C_BOARD_INFO("24c256", 0x50),
		.platform_data  = &eeprom_info,
	},
	{
		I2C_BOARD_INFO("pcf8574a", 0x3f),
	},
};

static struct davinci_i2c_platform_data omapl1x7_i2c_data0 = {
	.bus_freq       = 100,
	.bus_delay      = 0,
};

static struct davinci_uart_config uart_config __initdata = {
	.enabled_uarts = ((1 << 0) | (1 << 1) | (1 << 2)),
};

static void __init omapl1x7_evm_init_i2c(void)
{
	davinci_init_i2c(&omapl1x7_i2c_data0);
	i2c_register_board_info(1, i2c_info, ARRAY_SIZE(i2c_info));
}

static struct platform_device *omapl1x7_evm_devices[] __initdata = {
	&omapl1x7_evm_nandflash_device,
};

static void __init omapl1x7_map_io(void)
{
	davinci_map_common_io();
	omapl1x7_init();
}

int omapl1x7_lcd_hw_init(void)
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
EXPORT_SYMBOL(omapl1x7_lcd_hw_init);

static __init void omapl1x7_evm_init(void)
{
	davinci_cfg_reg(OMAPL1X7_UART2_RXD);
	davinci_cfg_reg(OMAPL1X7_UART2_TXD);
	davinci_serial_init(&uart_config);

	davinci_cfg_reg(OMAPL1X7_EMA_D_0);
	davinci_cfg_reg(OMAPL1X7_EMA_D_1);
	davinci_cfg_reg(OMAPL1X7_EMA_D_2);
	davinci_cfg_reg(OMAPL1X7_EMA_D_3);
	davinci_cfg_reg(OMAPL1X7_EMA_D_4);
	davinci_cfg_reg(OMAPL1X7_EMA_D_5);
	davinci_cfg_reg(OMAPL1X7_EMA_D_6);
	davinci_cfg_reg(OMAPL1X7_EMA_D_7);
	davinci_cfg_reg(OMAPL1X7_EMA_D_8);
	davinci_cfg_reg(OMAPL1X7_EMA_D_9);
	davinci_cfg_reg(OMAPL1X7_EMA_D_10);
	davinci_cfg_reg(OMAPL1X7_EMA_D_11);
	davinci_cfg_reg(OMAPL1X7_EMA_D_12);
	davinci_cfg_reg(OMAPL1X7_EMA_D_13);
	davinci_cfg_reg(OMAPL1X7_EMA_D_14);
	davinci_cfg_reg(OMAPL1X7_EMA_D_15);
	davinci_cfg_reg(OMAPL1X7_EMA_A_0);
	davinci_cfg_reg(OMAPL1X7_EMA_A_1);
	davinci_cfg_reg(OMAPL1X7_EMA_A_2);
	davinci_cfg_reg(OMAPL1X7_EMA_A_3);
	davinci_cfg_reg(OMAPL1X7_EMA_A_4);
	davinci_cfg_reg(OMAPL1X7_EMA_A_5);
	davinci_cfg_reg(OMAPL1X7_EMA_A_6);
	davinci_cfg_reg(OMAPL1X7_EMA_A_7);
	davinci_cfg_reg(OMAPL1X7_EMA_A_8);
	davinci_cfg_reg(OMAPL1X7_EMA_A_9);
	davinci_cfg_reg(OMAPL1X7_EMA_A_10);
	davinci_cfg_reg(OMAPL1X7_EMA_A_11);
	davinci_cfg_reg(OMAPL1X7_EMA_A_12);
	davinci_cfg_reg(OMAPL1X7_EMA_BA_0);
	davinci_cfg_reg(OMAPL1X7_EMA_BA_1);
	davinci_cfg_reg(OMAPL1X7_EMA_CLK);
	davinci_cfg_reg(OMAPL1X7_EMA_SDCKE);
	davinci_cfg_reg(OMAPL1X7_NEMA_CAS);
	davinci_cfg_reg(OMAPL1X7_NEMA_RAS);
	davinci_cfg_reg(OMAPL1X7_NEMA_WE);
	davinci_cfg_reg(OMAPL1X7_NEMA_CS_0);
	davinci_cfg_reg(OMAPL1X7_NEMA_CS_2);
	davinci_cfg_reg(OMAPL1X7_NEMA_CS_3);
	davinci_cfg_reg(OMAPL1X7_NEMA_CAS);
	davinci_cfg_reg(OMAPL1X7_NEMA_RAS);
	davinci_cfg_reg(OMAPL1X7_NEMA_OE);
	davinci_cfg_reg(OMAPL1X7_NEMA_WE_DQM_1);
	davinci_cfg_reg(OMAPL1X7_NEMA_WE_DQM_0);
	davinci_cfg_reg(OMAPL1X7_EMA_WAIT_0);

	platform_add_devices(omapl1x7_evm_devices,
			     ARRAY_SIZE(omapl1x7_evm_devices));

	omapl1x7_evm_init_i2c();

	davinci_setup_mmc(0, &omapl1x7_mmc_config);

	omapl1x7_init_rtc();
}

static __init void omapl1x7_evm_irq_init(void)
{
	soc_omapl1x7_irq_init();
}

MACHINE_START(DAVINCI_DA8XX_EVM, "DaVinci OMAPL1X7 EVM")
	.phys_io	= IO_PHYS,
	.io_pg_offst	= (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
	.boot_params	= (0xC0000100),
	.map_io		= omapl1x7_map_io,
	.init_irq	= omapl1x7_evm_irq_init,
	.timer		= &davinci_timer,
	.init_machine	= omapl1x7_evm_init,
MACHINE_END
