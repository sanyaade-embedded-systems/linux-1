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
#include <linux/usb/otg.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <plat/mux.h>

#include <plat/mcspi.h>
#include <plat/board.h>
#include <plat/common.h>
#include <plat/control.h>
#include <plat/timer-gp.h>
#include <plat/usb.h>
#include <plat/syntm12xx.h>
#include <plat/keypad.h>
#include <plat/display.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/cache-l2x0.h>
#include <linux/i2c/twl.h>
#include <linux/regulator/machine.h>
#include "mmc-twl4030.h"
#include <linux/delay.h>

#include <plat/omap_device.h>
#include <plat/omap_hwmod.h>

static int ts_gpio;

#define OMAP4_KBDOCP_BASE               0x4A31C000

static int omap_keymap[] = {
	KEY(0, 0, KEY_E),
	KEY(0, 1, KEY_D),
	KEY(0, 2, KEY_X),
	KEY(0, 3, KEY_Z),
	KEY(0, 4, KEY_W),
	KEY(0, 5, KEY_S),
	KEY(0, 6, KEY_Q),
	KEY(0, 7, KEY_UNKNOWN),

	KEY(1, 0, KEY_R),
	KEY(1, 1, KEY_F),
	KEY(1, 2, KEY_C),
	KEY(1, 3, KEY_KPPLUS),
	KEY(1, 4, KEY_Y),
	KEY(1, 5, KEY_H),
	KEY(1, 6, KEY_A),
	KEY(1, 7, KEY_UNKNOWN),

	KEY(2, 0, KEY_T),
	KEY(2, 1, KEY_G),
	KEY(2, 2, KEY_V),
	KEY(2, 3, KEY_B),
	KEY(2, 4, KEY_U),
	KEY(2, 5, KEY_J),
	KEY(2, 6, KEY_N),
	KEY(2, 7, KEY_UNKNOWN),

	KEY(3, 0, KEY_HOME),
	KEY(3, 1, KEY_SEND),
	KEY(3, 2, KEY_END),
	KEY(3, 3, KEY_F1),
	KEY(3, 4, KEY_LEFTSHIFT),
	KEY(3, 5, KEY_F3),
	KEY(3, 6, KEY_BACK),
	KEY(3, 7, KEY_UNKNOWN),

	KEY(4, 0, KEY_UNKNOWN),
	KEY(4, 1, KEY_UNKNOWN),
	KEY(4, 2, KEY_UNKNOWN),
	KEY(4, 3, KEY_UNKNOWN),
	KEY(4, 4, KEY_VOLUMEUP),
	KEY(4, 5, KEY_UNKNOWN),
	KEY(4, 6, KEY_BACKSPACE),
	KEY(4, 7, KEY_F4),

	KEY(5, 0, KEY_UNKNOWN),
	KEY(5, 1, KEY_UNKNOWN),
	KEY(5, 2, KEY_UNKNOWN),
	KEY(5, 3, KEY_UNKNOWN),
	KEY(5, 4, KEY_UNKNOWN),
	KEY(5, 5, KEY_VOLUMEDOWN),
	KEY(5, 6, KEY_UNKNOWN),
	KEY(5, 7, KEY_UNKNOWN),

	KEY(6, 0, KEY_I),
	KEY(6, 1, KEY_K),
	KEY(6, 2, KEY_DOT),
	KEY(6, 3, KEY_O),
	KEY(6, 4, KEY_L),
	KEY(6, 5, KEY_M),
	KEY(6, 6, KEY_P),
	KEY(6, 7, KEY_OK),

	KEY(7, 0, KEY_UNKNOWN),
	KEY(7, 1, KEY_ENTER),
	KEY(7, 2, KEY_CAPSLOCK),
	KEY(7, 3, KEY_SPACE),
	KEY(7, 4, KEY_LEFT),
	KEY(7, 5, KEY_RIGHT),
	KEY(7, 6, KEY_UP),
	KEY(7, 7, KEY_DOWN),
	0,
};

static struct resource sdp4430_kp_resources[] = {
	{
		.start  = OMAP4_KBDOCP_BASE,
		.end    = OMAP4_KBDOCP_BASE,
		.flags  = IORESOURCE_MEM,
	},
};

static struct omap_kp_platform_data omap_kp_data = {
	.rows		= 8,
	.cols		= 8,
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
	.num_resources	= ARRAY_SIZE(sdp4430_kp_resources),
	.resource	= sdp4430_kp_resources,
};

/* Begin Synaptic Touchscreen TM-01217 */

static char *tm12xx_idev_names[] = {
	"Synaptic TM12XX TouchPoint 1",
	"Synaptic TM12XX TouchPoint 2",
	"Synaptic TM12XX TouchPoint 3",
	"Synaptic TM12XX TouchPoint 4",
	"Synaptic TM12XX TouchPoint 5",
	"Synaptic TM12XX TouchPoint 6",
	NULL,
};

static u8 tm12xx_button_map[] = {
	KEY_F1,
	KEY_F2,
};

static struct tm12xx_ts_platform_data tm12xx_platform_data[] = {
	[0] = { /* Primary Controller */
		.gpio_intr = 35,
		.idev_name = tm12xx_idev_names,
		.button_map = tm12xx_button_map,
		.num_buttons = ARRAY_SIZE(tm12xx_button_map),
		.repeat = 0,
		.swap_xy = 1,
	},
	[1] = { /* Secondary Controller */
		.gpio_intr = 36,
		.idev_name = tm12xx_idev_names,
		.button_map = tm12xx_button_map,
		.num_buttons = ARRAY_SIZE(tm12xx_button_map),
		.repeat = 0,
		.swap_xy = 1,
	},
};

/* End Synaptic Touchscreen TM-01217 */

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
/* Display */
static int sdp4430_panel_enable_lcd(struct omap_dss_device *dssdev) {
	if (dssdev->channel == OMAP_DSS_CHANNEL_LCD2) {

		gpio_request(DSI2_GPIO_104, "dsi2_en_gpio");
		gpio_direction_output(DSI2_GPIO_104, 0);
		mdelay(500);
		gpio_set_value(DSI2_GPIO_104, 1);
		mdelay(500);
		gpio_set_value(DSI2_GPIO_104, 0);
		mdelay(500);
		gpio_set_value(DSI2_GPIO_104, 1);

		twl_i2c_write_u8(TWL_MODULE_PWM, 0xFF, PWM2ON); /*0xBD = 0xFF*/
		twl_i2c_write_u8(TWL_MODULE_PWM, 0x7F, PWM2OFF); /*0xBE = 0x7F*/
		twl_i2c_write_u8(TWL6030_MODULE_ID1, 0x30, TOGGLE3);

		gpio_request(DSI2_GPIO_59, "dsi2_bl_gpio");
		gpio_direction_output(DSI2_GPIO_59, 1);
		mdelay(120);
		gpio_set_value(DSI2_GPIO_59, 0);
		mdelay(120);
		gpio_set_value(DSI2_GPIO_59, 1);

	} else {
		gpio_request(DSI1_GPIO_102, "dsi1_en_gpio");
		gpio_direction_output(DSI1_GPIO_102, 0);
		mdelay(500);
		gpio_set_value(DSI1_GPIO_102, 1);
		mdelay(500);
		gpio_set_value(DSI1_GPIO_102, 0);
		mdelay(500);
		gpio_set_value(DSI1_GPIO_102, 1);

		twl_i2c_write_u8(TWL_MODULE_PWM, 0xFF, PWM2ON); /*0xBD = 0xFF*/
		twl_i2c_write_u8(TWL_MODULE_PWM, 0x7F, PWM2OFF); /*0xBE = 0x7F*/
		twl_i2c_write_u8(TWL6030_MODULE_ID1, 0x30, TOGGLE3);

		gpio_request(DSI1_GPIO_27, "dsi1_bl_gpio");
		gpio_direction_output(DSI1_GPIO_27, 1);
		mdelay(120);
		gpio_set_value(DSI1_GPIO_27, 0);
		mdelay(120);
		gpio_set_value(DSI1_GPIO_27, 1);

	}

	return 0;
}

static int sdp4430_panel_disable_lcd(struct omap_dss_device *dssdev) {

	if (dssdev->channel == OMAP_DSS_CHANNEL_LCD2) {
		gpio_set_value(DSI2_GPIO_104, 1);
		gpio_set_value(DSI2_GPIO_59, 0);
	} else {
		gpio_set_value(DSI1_GPIO_102, 1);
		gpio_set_value(DSI1_GPIO_27, 0);
	}
	return 0;
}

static struct omap_device_pm_latency omap_dss_latency[] = {
	[0] = {
		.deactivate_func	= omap_device_idle_hwmods,
		.activate_func		= omap_device_enable_hwmods,
	},
};

static struct omap_dss_device sdp4430_lcd_device = {
	.name			= "lcd",
	.driver_name		= "panel-taal",
	.type			= OMAP_DISPLAY_TYPE_DSI,
	.reset_gpio		= 78,
	.phy.dsi		= {
		.clk_lane	= 1,
		.clk_pol	= 0,
		.data1_lane	= 2,
		.data1_pol	= 0,
		.data2_lane	= 3,
		.data2_pol	= 0,
		.ext_te		= false,
		.ext_te_gpio	= 101,
		.div		= {
			.regm		= 150,
			.regn		= 20,
			.regm3		= 4,
			.regm4		= 4,
			.lck_div	= 1,
			.pck_div	= 6,
			.lp_clk_div = 6,
		},
	},
	.platform_enable	=	sdp4430_panel_enable_lcd,
	.platform_disable	=	sdp4430_panel_disable_lcd,
	.channel			=	OMAP_DSS_CHANNEL_LCD,
};

static struct omap_dss_device sdp4430_lcd2_device = {
	.name			= "2lcd",
	.driver_name		= "panel-taal2",
	.type			= OMAP_DISPLAY_TYPE_DSI,
	.reset_gpio		= 78,
	.phy.dsi		= {
		.clk_lane	= 1,
		.clk_pol	= 0,
		.data1_lane	= 2,
		.data1_pol	= 0,
		.data2_lane	= 3,
		.data2_pol	= 0,
		.ext_te		= false,
		.ext_te_gpio	= 103,
		.div		= {
			.regm		= 150,
			.regn		= 20,
			.regm3		= 4,
			.regm4		= 4,
			.lck_div	= 1,
			.pck_div	= 6,
			.lp_clk_div = 6,
		},
	},
	.platform_enable	=	sdp4430_panel_enable_lcd,
	.platform_disable	=	sdp4430_panel_disable_lcd,
	.channel			=	OMAP_DSS_CHANNEL_LCD2,
};

static int sdp4430_panel_enable_hdmi(struct omap_dss_device *dssdev)
{
	gpio_request(HDMI_GPIO_60 , "hdmi_gpio_60");
	gpio_request(HDMI_GPIO_41 , "hdmi_gpio_41");
	gpio_direction_output(HDMI_GPIO_60, 0);
	gpio_direction_output(HDMI_GPIO_41, 0);
	gpio_set_value(HDMI_GPIO_60, 1);
	gpio_set_value(HDMI_GPIO_41, 1);
	gpio_set_value(HDMI_GPIO_60, 0);

	return 0;
}

static int sdp4430_panel_disable_hdmi(struct omap_dss_device *dssdev)
{
	gpio_set_value(HDMI_GPIO_60, 1);
	gpio_set_value(HDMI_GPIO_41, 1);

	return 0;
}
static void __init sdp4430_hdmi_init(void)
{
	return;
}

static struct omap_dss_device sdp4430_hdmi_device = {
	.name = "hdmi",
	.driver_name = "hdmi_panel",
	.type = OMAP_DISPLAY_TYPE_HDMI,
	.phy.dpi.data_lines = 24,
	.platform_enable = sdp4430_panel_enable_hdmi,
	.platform_disable = sdp4430_panel_disable_hdmi,
};

static int sdp4430_panel_enable_pico_DLP(struct omap_dss_device *dssdev)
{
	int i = 0;
	gpio_request(DLP_4430_GPIO_59, "DLP DISPLAY SEL");
	gpio_direction_output(DLP_4430_GPIO_59, 0);
	gpio_request(DLP_4430_GPIO_45, "DLP PARK");
	gpio_direction_output(DLP_4430_GPIO_45, 0);
	gpio_request(DLP_4430_GPIO_40, "DLP PHY RESET");
	gpio_direction_output(DLP_4430_GPIO_40, 0);
	gpio_request(DLP_4430_GPIO_44, "DLP READY RESET");
	gpio_direction_input(DLP_4430_GPIO_44);
	mdelay(500);

	gpio_set_value(DLP_4430_GPIO_59, 1);
	gpio_set_value(DLP_4430_GPIO_45, 1);
	mdelay(1000);

	gpio_set_value(DLP_4430_GPIO_40, 1);
	mdelay(1000);

	/*FIXME with the MLO gpio changes , gpio read is not retuning correct value even though 		it is  set in hardware so the check is comment till the problem is fixed */
	/*while(i == 0){
	i=gpio_get_value(DLP_4430_GPIO_44);
	printk("wait for ready bit %d\n",i);
	}*/
	printk("%d ready bit ", i);
	mdelay(2000);
	return 0;
}

static int sdp4430_panel_disable_pico_DLP(struct omap_dss_device *dssdev)
{
	gpio_set_value(DLP_4430_GPIO_40, 0);
	gpio_set_value(DLP_4430_GPIO_45, 0);

	return 0;
}

static struct omap_dss_device sdp4430_picoDLP_device = {
	.name			            = "pico_DLP",
	.driver_name		        = "picoDLP_panel",
	.type			            = OMAP_DISPLAY_TYPE_DPI,
	.phy.dpi.data_lines	        = 24,
	.platform_enable	        = sdp4430_panel_enable_pico_DLP,
	.platform_disable	        = sdp4430_panel_disable_pico_DLP,
	.channel			= OMAP_DSS_CHANNEL_LCD2,
};



static struct omap_dss_device *sdp4430_dss_devices[] = {
	&sdp4430_lcd_device,
	&sdp4430_lcd2_device,
#ifdef CONFIG_OMAP2_DSS_HDMI
	&sdp4430_hdmi_device,
#endif
#ifdef CONFIG_PANEL_PICO_DLP
	&sdp4430_picoDLP_device,
#endif
};

static struct omap_dss_board_info sdp4430_dss_data = {
	.num_devices	=	ARRAY_SIZE(sdp4430_dss_devices),
	.devices	=	sdp4430_dss_devices,
	.default_device	=	&sdp4430_lcd_device,
};

static struct platform_device sdp4430_dss_device = {
	.name	=	"omapdss",
	.id	=	-1,
	.dev	= {
		.platform_data = &sdp4430_dss_data,
	},
};

#define MAX_OMAP_DSS_HWMOD_NAME_LEN 16
static const char name[] = "omapdss";
struct omap_device *od;

static void __init sdp4430_display_init(void) {
	struct omap_hwmod *oh;
	char oh_name[MAX_OMAP_DSS_HWMOD_NAME_LEN];
	int l, idx;
	struct omap_dss_platform_data *pdata;
	int bus_id = 1;
	idx = 1;

	l = snprintf(oh_name, MAX_OMAP_DSS_HWMOD_NAME_LEN,
			"dss");
	WARN(l >= MAX_OMAP_DSS_HWMOD_NAME_LEN,
		"String buffer overflow in DSS device setup\n");

	oh = omap_hwmod_lookup(oh_name);
	if (!oh) {
		pr_err("Could not look up %s\n", oh_name);
		return -EEXIST;
	}

	od = omap_device_build(name, -1, oh, &sdp4430_dss_data,
			sizeof(struct omap_dss_board_info),
			omap_dss_latency,
			ARRAY_SIZE(omap_dss_latency), 0);

	WARN(IS_ERR(od), "Could not build omap_device for %s %s\n",
			name, oh_name);

	return;
}

/* end Display */

static struct regulator_consumer_supply sdp4430_vdda_dac_supply = {
	.supply		= "vdda_dac",
	.dev		= &sdp4430_dss_device.dev,
};
static struct platform_device *sdp4430_devices[] __initdata = {
	&omap_kp_device,
};

static struct omap_uart_config sdp4430_uart_config __initdata = {
	.enabled_uarts	= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
};

static struct omap_lcd_config sdp4430_lcd_config __initdata = {
	.ctrl_name	= "internal",
};

static struct omap_board_config_kernel sdp4430_config[] __initdata = {
	{ OMAP_TAG_LCD,		&sdp4430_lcd_config },
};

static struct twl4030_hsmmc_info mmc[] = {
	{
		.mmc            = 1,
		/* 8 bits (default) requires S6.3 == ON,
		 * so the SIM card isn't used; else 4 bits.
		 */
		.wires          = 8,
		.gpio_wp        = 4,
	},
	{
		.mmc            = 2,
		.wires          = 8,
		.nonremovable   = true,
		.gpio_cd        = -EINVAL,
		.gpio_wp        = -EINVAL,
	},
	{
		.mmc            = 3,
		.wires          = -EINVAL,
		.gpio_cd        = -EINVAL,
		.gpio_wp        = -EINVAL,
	},
	{
		.mmc            = 4,
		.wires          = -EINVAL,
		.gpio_cd        = -EINVAL,
		.gpio_wp        = -EINVAL,
	},
	{
		.mmc            = 5,
		.wires          = 8,
		.gpio_cd        = -EINVAL,
		.gpio_wp        = 4,
	},
	{}      /* Terminator */
};

static struct regulator_consumer_supply sdp4430_vmmc_supply[] = {
	{
		.supply = "vmmc",
	},
	{
		.supply = "vmmc",
	},
	{
		.supply = "vmmc",
	},
	{
		.supply = "vmmc",
	},
	{
		.supply = "vmmc",
	},
};

static int __init sdp4430_mmc_init(void)
{
	/* TODO: Fix Hard Coding */
	mmc[0].gpio_cd = 384 ;

	twl4030_mmc_init(mmc);
	/* link regulators to MMC adapters ... we "know" the
	 * regulators will be set up only *after* we return.
	 */
	sdp4430_vmmc_supply[0].dev = mmc[0].dev;
	sdp4430_vmmc_supply[1].dev = mmc[1].dev;
	sdp4430_vmmc_supply[2].dev = mmc[2].dev;
	sdp4430_vmmc_supply[3].dev = mmc[3].dev;
	sdp4430_vmmc_supply[4].dev = mmc[4].dev;
	return 0;
}

#ifdef CONFIG_CACHE_L2X0
static int __init omap_l2_cache_init(void)
{
	void __iomem *l2cache_base;

	/* Static mapping, never released */
	l2cache_base = ioremap(OMAP44XX_L2CACHE_BASE, SZ_4K);
	BUG_ON(!l2cache_base);

	/* Enable L2 Cache using secure api
	 * Save/Restore relevant registers
	 */
	__asm__ __volatile__(
		"stmfd r13!, {r0-r12, r14}\n"
		"mov r0, #1\n"
		"ldr r12, =0x102\n"
		"dsb\n"
		"smc\n"
		"ldmfd r13!, {r0-r12, r14}");

	/* 32KB way size, 16-way associativity,
	* parity disabled
	*/
	l2x0_init(l2cache_base, 0x0e050000, 0xc0000fff);

	return 0;
}
early_initcall(omap_l2_cache_init);
#endif

static void __init gic_init_irq(void)
{
	void __iomem *base;

	/* Static mapping, never released */
	base = ioremap(OMAP44XX_GIC_DIST_BASE, SZ_4K);
	BUG_ON(!base);
	gic_dist_init(0, base, 29);

	/* Static mapping, never released */
	gic_cpu_base_addr = ioremap(OMAP44XX_GIC_CPU_BASE, SZ_512);
	BUG_ON(!gic_cpu_base_addr);
	gic_cpu_init(0, gic_cpu_base_addr);
}

static void __init omap_4430sdp_init_irq(void)
{
	omap_board_config = sdp4430_config;
	omap_board_config_size = ARRAY_SIZE(sdp4430_config);
	gic_init_irq();
	omap2_init_common_hw(NULL, NULL);
#ifdef CONFIG_OMAP_32K_TIMER
	omap2_gp_clockevent_set_gptimer(1);
#endif
}

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

/* VMMC1 for MMC1 card */
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
	.num_consumer_supplies  = 5,
	.consumer_supplies      = &sdp4430_vmmc_supply,
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
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &sdp4430_vdda_dac_supply,
};

/* VPLL2 for digital video outputs */
static struct regulator_consumer_supply sdp4430_vpll2_supplies[] = {
	{
		.supply		= "vdvi",
		.dev		= &sdp4430_lcd_device.dev,
	},
	{
		.supply		= "vdds_dsi",
		.dev		= &sdp4430_dss_device.dev,
	}
};

static struct regulator_init_data sdp4430_vpll2 = {
	.constraints = {
		.name			= "VDVI",
		.min_uV			= 1800000,
		.max_uV			= 1800000,
		.apply_uV		= true,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= ARRAY_SIZE(sdp4430_vpll2_supplies),
	.consumer_supplies	= sdp4430_vpll2_supplies,
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

static struct twl4030_madc_platform_data sdp4430_gpadc_data = {
	.irq_line	= 1,
};

static struct twl4030_bci_platform_data sdp4430_bci_data = {
};

static struct twl4030_platform_data sdp4430_twldata = {
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
	.madc           = &sdp4430_gpadc_data,
	.bci            = &sdp4430_bci_data,
};

static struct pico_platform_data picodlp_platform_data[] = {
	[0] = { /* DLP Controller */
		.gpio_intr = 40,
	},
};

static struct i2c_board_info __initdata sdp4430_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO("twl6030", 0x48),
		.flags = I2C_CLIENT_WAKE,
		.irq = INT_44XX_SYS_NIRQ,
		.platform_data = &sdp4430_twldata,
	},
};

static struct i2c_board_info __initdata sdp4430_i2c_2_boardinfo[] = {
	{
		I2C_BOARD_INFO("tm12xx_ts_primary", 0x4b),
		.platform_data = &tm12xx_platform_data[0],
	},
    {
		I2C_BOARD_INFO("picoDLP_i2c_driver", 0x1b),
		.platform_data = &picodlp_platform_data[0],
	},
};

static struct i2c_board_info __initdata sdp4430_i2c_3_boardinfo[] = {
	{
		I2C_BOARD_INFO("tm12xx_ts_secondary", 0x4b),
		.platform_data = &tm12xx_platform_data[1],
	},
};

static int __init omap4_i2c_init(void)
{
	/* Phoenix Audio IC needs I2C1 to srat with 400 KHz and less */
	omap_register_i2c_bus(1, 400, sdp4430_i2c_boardinfo,
				ARRAY_SIZE(sdp4430_i2c_boardinfo));
	omap_register_i2c_bus(2, 400, sdp4430_i2c_2_boardinfo,
				ARRAY_SIZE(sdp4430_i2c_2_boardinfo));
	omap_register_i2c_bus(3, 400, sdp4430_i2c_3_boardinfo,
				ARRAY_SIZE(sdp4430_i2c_3_boardinfo));

	return 0;
}

static struct spi_board_info sdp4430_spi_board_info[] __initdata = {
	[0] = {
		.modalias		= "ks8851",
		.bus_num		= 1,
		.chip_select		= 0,
		.max_speed_hz		= 24000000,
		.irq			= 34,
	},
	[1] = {
		.modalias		= "spitst",
		.bus_num		= 4,
		.chip_select		= 0,
		.max_speed_hz		= 1500000,
	},
};

static struct omap_musb_board_data musb_board_data = {
	.interface_type         = MUSB_INTERFACE_UTMI,
#ifdef CONFIG_USB_MUSB_OTG
	.mode                   = MUSB_OTG,
#elif defined(CONFIG_USB_MUSB_HDRC_HCD)
	.mode                   = MUSB_HOST,
#elif defined(CONFIG_USB_GADGET_MUSB_HDRC)
	.mode                   = MUSB_PERIPHERAL,
 #endif
	.power                  = 100,
 };

static void omap_ethernet_init(void)
{
	gpio_request(48, "ethernet");
	gpio_direction_output(48, 1);
	gpio_request(138, "quart");
	gpio_direction_output(138, 1);
	gpio_request(34, "ks8851");
	gpio_direction_input(34);
}

static void __init omap_4430sdp_init(void)
{
	omap4_i2c_init();
	platform_add_devices(sdp4430_devices, ARRAY_SIZE(sdp4430_devices));
	omap_serial_init();
	/* OMAP4 SDP uses internal transceiver so register nop transceiver */
	sdp4430_mmc_init();
	usb_nop_xceiv_register();
	usb_musb_init(&musb_board_data);
	omap_ethernet_init();
	sdp4430_spi_board_info[0].irq = gpio_to_irq(34);
	spi_register_board_info(sdp4430_spi_board_info,
			ARRAY_SIZE(sdp4430_spi_board_info));
	sdp4430_display_init();
}

static void __init omap_4430sdp_map_io(void)
{
	omap2_set_globals_443x();
	omap2_map_common_io();
}

MACHINE_START(OMAP_4430SDP, "OMAP4430 4430SDP board")
	/* Maintainer: Santosh Shilimkar - Texas Instruments Inc */
	.phys_io	= 0x48000000,
	.io_pg_offst	= ((0xfa000000) >> 18) & 0xfffc,
	.boot_params	= 0x80000100,
	.map_io		= omap_4430sdp_map_io,
	.init_irq	= omap_4430sdp_init_irq,
	.init_machine	= omap_4430sdp_init,
	.timer		= &omap_timer,
MACHINE_END
