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

#include <mach/common.h>
#include <mach/board.h>

static struct plat_serial8250_port serial_platform_data[] = {
	{
		.membase	= 0xD8610E00,
		.mapbase	= 0x08610E00,
		.irq		= 7,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= 33300000,
	}, {
		.flags		= 0
	}
};


static struct platform_device serial_device = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_PLATFORM,
	.dev			= {
		.platform_data	= serial_platform_data,
	},
};

static int __init omap_init(void)
{
	int ret;

	ret = platform_device_register(&serial_device);

#ifdef CONFIG_PM
	if (!ret)
		ret = sysfs_create_file(&serial_device.dev.kobj,
					&sleep_timeout_attr.attr);
#endif
	return ret;
}
arch_initcall(omap_init);
