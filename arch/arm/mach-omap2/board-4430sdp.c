/*
 * Board support file for OMAP4430 SDP.
 *
 * Copyright (C) 2009 Texas Instruments
 *
 * Author: Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Based on mach-omap2/board-3430sdp.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <mach/mux.h>

#include <mach/mcspi.h>
#include <mach/board.h>
#include <mach/common.h>
#include <mach/control.h>
#include <mach/timer-gp.h>
#include <asm/hardware/gic.h>

static int ts_gpio;

static struct omap2_mcspi_device_config tsc2046_mcspi_config = {
	.turbo_mode     = 0,
	.single_channel = 1,  /* 0: slave, 1: master */
};

static struct omap2_mcspi_device_config dummy1_mcspi_config = {
	.turbo_mode     = 0,
	.single_channel = 1,  /* 0: slave, 1: master */
};

#ifdef CONFIG_SPI_TI_OMAP_TEST
static struct omap2_mcspi_device_config dummy2_mcspi_config = {
	.turbo_mode     = 0,
	.single_channel = 0,  /* 0: slave, 1: master */
};
#endif

static struct platform_device sdp4430_lcd_device = {
	.name		= "sdp4430_lcd",
	.id		= -1,
};

static struct platform_device *sdp4430_devices[] __initdata = {
	&sdp4430_lcd_device,
};

static struct omap_uart_config sdp4430_uart_config __initdata = {
	.enabled_uarts	= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
};

static struct omap_lcd_config sdp4430_lcd_config __initdata = {
	.ctrl_name	= "internal",
};

static struct omap_board_config_kernel sdp4430_config[] __initdata = {
	{ OMAP_TAG_UART,	&sdp4430_uart_config },
	{ OMAP_TAG_LCD,		&sdp4430_lcd_config },
};

static void __init gic_init_irq(void)
{
	gic_dist_init(0, IO_ADDRESS(OMAP44XX_GIC_DIST_BASE), 29);
	gic_cpu_init(0, IO_ADDRESS(OMAP44XX_GIC_CPU_BASE));
}

static struct spi_board_info sdp4430_spi_board_info[] __initdata = {
	[0] = {
		.modalias		= "spitst",
		.bus_num		= 1,
		.chip_select		= 0,
		.max_speed_hz		= 1500000,
	},
	[1] = {
		.modalias		= "ads7846",
		.bus_num		= 1,
		.chip_select		= 0,
		.max_speed_hz		= 1500000,
	},
#ifdef CONFIG_SPI_TI_OMAP_TEST
	[2] = {
		.modalias		= "spidev",
		.bus_num		= 3,
		.chip_select		= 0,
		.max_speed_hz		= 6000000,
		.controller_data	= &dummy2_mcspi_config, /* Slave */
	},
	[3] = {
		.modalias		= "dummydevice1",
		.bus_num		= 2,
		.chip_select		= 0,
		.max_speed_hz		= 1500000,
		.controller_data	= &dummy1_mcspi_config, /*Master */
	},
#endif
};

static void __init omap_4430sdp_init_irq(void)
{
	omap2_init_common_hw(NULL, NULL);
#ifdef CONFIG_OMAP_32K_TIMER
	omap2_gp_clockevent_set_gptimer(1);
#endif
	gic_init_irq();
	omap_gpio_init();
}
static int __init omap4_i2c_init(void)
{
	omap_register_i2c_bus(1, 2600, NULL, 0);
	omap_register_i2c_bus(2, 400, NULL, 0);
	omap_register_i2c_bus(3, 400, NULL, 0);
	return 0;
}

static void __init omap_4430sdp_init(void)
{
	omap4_i2c_init();
	platform_add_devices(sdp4430_devices, ARRAY_SIZE(sdp4430_devices));
	omap_board_config = sdp4430_config;
	omap_board_config_size = ARRAY_SIZE(sdp4430_config);
	omap_serial_init();
	sdp4430_spi_board_info[0].irq = OMAP_GPIO_IRQ(ts_gpio);
	spi_register_board_info(sdp4430_spi_board_info,
				ARRAY_SIZE(sdp4430_spi_board_info));

}

static void __init omap_4430sdp_map_io(void)
{
	omap2_set_globals_443x();
	omap2_map_common_io();
}

MACHINE_START(OMAP_4430SDP, "OMAP4430 4430SDP board")
	/* Maintainer: Santosh Shilimkar - Texas Instruments Inc */
	.phys_io	= 0x48000000,
	.io_pg_offst	= ((0xd8000000) >> 18) & 0xfffc,
	.boot_params	= 0x80000100,
	.map_io		= omap_4430sdp_map_io,
	.init_irq	= omap_4430sdp_init_irq,
	.init_machine	= omap_4430sdp_init,
	.timer		= &omap_timer,
MACHINE_END
