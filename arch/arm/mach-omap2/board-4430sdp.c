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
#include <linux/input.h>

#include <mach/keypad.h>
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
#include <linux/i2c/twl.h>
#include <linux/regulator/machine.h>

#define OMAP4_KBDOCP_BASE               0x4A31C000

static int ts_gpio;

static int omap_keymap[] = {
	KEY(0, 0, KEY_SENDFILE),
	KEY(0, 1, KEY_1),
	KEY(0, 2, KEY_4),
	KEY(0, 3, KEY_7),
	KEY(1, 0, KEY_END),
	KEY(1, 1, KEY_2),
	KEY(1, 2, KEY_5),
	KEY(1, 3, KEY_8),
	KEY(1, 4, KEY_0),
	KEY(2, 1, KEY_3),
	KEY(2, 2, KEY_6),
	KEY(2, 3, KEY_9),
	KEY(3, 1, KEY_HOME),
	KEY(3, 2, KEY_BACK),
	KEY(3, 3, KEY_VOLUMEUP),
	KEY(3, 4, KEY_VOLUMEDOWN),
	KEY(4, 0, KEY_UP),
	KEY(4, 1, KEY_RIGHT),
	KEY(4, 2, KEY_LEFT),
	KEY(4, 3, KEY_DOWN),
	KEY(0, 4, KEY_DOT),
	KEY(2, 4, KEY_LEFT),
	KEY(4, 4, KEY_ENTER),
	KEY(5, 0, KEY_SCROLLUP),
	KEY(5, 1, KEY_SCROLLDOWN),
	KEY(5, 2, KEY_RIGHT),
	KEY(5, 3, KEY_RECORD),
	0,
};

static struct resource omap_kp_resources[] = {
	[0] = {
		.start	= OMAP4_KBDOCP_BASE,
		.end	= OMAP4_KBDOCP_BASE,
		.flags	= IORESOURCE_MEM,
	},
};

static struct omap_kp_platform_data omap_kp_data = {
	.rows		= 5,
	.cols		= 6,
	.keymap		= omap_keymap,
	.keymapsize	= ARRAY_SIZE(omap_keymap),
	.delay		= 4,
	.rep		= 1,
};

static struct platform_device omap_kp_device = {
	.name		= "omap-keypad",
	.id		= -1,
	.dev		= {
		.platform_data = &omap_kp_data,
	},
	.num_resources  = ARRAY_SIZE(omap_kp_resources),
	.resource       = omap_kp_resources,
};

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
	&omap_kp_device,
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

#ifdef CONFIG_TWL6030_CORE
static struct regulator_init_data sdp4430_vaux1 = {
	.constraints = {
		.min_uV		 	= 1000000,
		.max_uV		 	= 3000000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vaux2 = {
	.constraints = {
		.min_uV		 	= 1200000,
		.max_uV		 	= 2800000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vaux3 = {
	.constraints = {
		.min_uV		 	= 1000000,
		.max_uV		 	= 3000000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vmmc = {
	.constraints = {
		.min_uV		 	= 1200000,
		.max_uV		 	= 3000000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vpp = {
	.constraints = {
		.min_uV		 	= 1800000,
		.max_uV		 	= 2500000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vusim = {
	.constraints = {
		.min_uV		 	= 1200000,
		.max_uV		 	= 2900000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vana = {
	.constraints = {
		.min_uV		 	= 2100000,
		.max_uV		 	= 2100000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vcxio = {
	.constraints = {
		.min_uV		 	= 1800000,
		.max_uV		 	= 1800000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vdac = {
	.constraints = {
		.min_uV		 	= 1800000,
		.max_uV		 	= 1800000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 = REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct regulator_init_data sdp4430_vusb = {
	.constraints = {
		.min_uV		 	= 3300000,
		.max_uV		 	= 3300000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask	 =	REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
};

static struct twl_platform_data sdp4430_twldata = {
	.irq_base	= TWL6030_IRQ_BASE,
	.irq_end	= TWL6030_IRQ_END,

	/* Regulators */
	.vmmc		= &sdp4430_vmmc,
	.vpp		= &sdp4430_vpp,
	.vusim		= &sdp4430_vusim,
	.vana		= &sdp4430_vana,
	.vcxio		= &sdp4430_vcxio,
	.vdac		= &sdp4430_vdac,
	.vusb		= &sdp4430_vusb,
	.vaux1		= &sdp4430_vaux1,
	.vaux2		= &sdp4430_vaux2,
	.vaux3		= &sdp4430_vaux3,
};
#endif

static struct i2c_board_info __initdata sdp4430_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO("twl6030", 0x48),
		.flags = I2C_CLIENT_WAKE,
		.irq = INT_44XX_SYS_NIRQ,
#ifdef CONFIG_TWL6030_CORE
		.platform_data = &sdp4430_twldata,
#endif
	},
};

static int __init omap4_i2c_init(void)
{
	omap_register_i2c_bus(1, 2600, sdp4430_i2c_boardinfo,
				ARRAY_SIZE(sdp4430_i2c_boardinfo));
	omap_register_i2c_bus(2, 400, NULL, 0);
	omap_register_i2c_bus(3, 400, NULL, 0);
	return 0;
}
static void omap_mcbsp_init(void)
{
	omap_cfg_reg(AA3_4430_McBSP1_CLK);
	omap_cfg_reg(Y3_4430_McBSP1_DR);
	omap_cfg_reg(Y2_4430_McBSP1_DX);
	omap_cfg_reg(Y4_4430_McBSP1_FSX);
}
void omap_kp_init(void)
{
	omap_cfg_reg(PAD0_4430_UNIPRO_TX0);
	omap_cfg_reg(PAD1_4430_UNIPRO_TY0);
	omap_cfg_reg(PAD0_4430_UNIPRO_TX1);
	omap_cfg_reg(PAD1_4430_UNIPRO_TY1);
	omap_cfg_reg(PAD0_4430_UNIPRO_TX2);
	omap_cfg_reg(PAD1_4430_UNIPRO_TY2);

	omap_cfg_reg(PAD0_4430_UNIPRO_RX0);
	omap_cfg_reg(PAD1_4430_UNIPRO_RY0);
	omap_cfg_reg(PAD0_4430_UNIPRO_RX1);
	omap_cfg_reg(PAD1_4430_UNIPRO_RY1);
	omap_cfg_reg(PAD0_4430_UNIPRO_RX2);
	omap_cfg_reg(PAD1_4430_UNIPRO_RY2);
}

static void omap_phoenix_init(void)
{
	omap_cfg_reg(PAD1_4430_SYS_NIRQ1);
	omap_cfg_reg(PAD0_4430_SYS_NIRQ2);
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
	omap_mcbsp_init();
	omap_kp_init();
	omap_phoenix_init();

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
