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
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/etherdevice.h>
#include <mach/emac.h>

#include "clock.h"

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

static struct i2c_board_info __initdata i2c_info[] =  {
	{
		I2C_BOARD_INFO("24c256", 0x50),
		.platform_data  = &eeprom_info,
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

static void __init omapl1x7_map_io(void)
{
	davinci_map_common_io();
	omapl1x7_init();
}

static __init void omapl1x7_evm_init(void)
{
	davinci_cfg_reg(OMAPL1X7_UART2_RXD);
	davinci_cfg_reg(OMAPL1X7_UART2_TXD);
	davinci_serial_init(&uart_config);
	omapl1x7_evm_init_i2c();
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
