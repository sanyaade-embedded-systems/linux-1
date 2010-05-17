/*
 * omap_hwmod_2430_data.c - hardware modules present on the OMAP2430 chips
 *
 * Copyright (C) 2009-2010 Nokia Corporation
 * Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * XXX handle crossbar/shared link difference for L3?
 * XXX these should be marked initdata for multi-OMAP kernels
 */
#include <plat/omap_hwmod.h>
#include <mach/irqs.h>
#include <plat/cpu.h>
#include <plat/dma.h>
#include <plat/gpio.h>

#include "omap_hwmod_common_data.h"

#include "prm-regbits-24xx.h"

/*
 * OMAP2430 hardware module integration data
 *
 * ALl of the data in this section should be autogeneratable from the
 * TI hardware database or other technical documentation.  Data that
 * is driver-specific or driver-kernel integration-specific belongs
 * elsewhere.
 */

static struct omap_hwmod omap2430_mpu_hwmod;
static struct omap_hwmod omap2430_l3_hwmod;
static struct omap_hwmod omap2430_l4_core_hwmod;

/* L3 -> L4_CORE interface */
static struct omap_hwmod_ocp_if omap2430_l3__l4_core = {
	.master	= &omap2430_l3_hwmod,
	.slave	= &omap2430_l4_core_hwmod,
	.user	= OCP_USER_MPU | OCP_USER_SDMA,
};

/* MPU -> L3 interface */
static struct omap_hwmod_ocp_if omap2430_mpu__l3 = {
	.master = &omap2430_mpu_hwmod,
	.slave	= &omap2430_l3_hwmod,
	.user	= OCP_USER_MPU,
};

/* Slave interfaces on the L3 interconnect */
static struct omap_hwmod_ocp_if *omap2430_l3_slaves[] = {
	&omap2430_mpu__l3,
};

/* Master interfaces on the L3 interconnect */
static struct omap_hwmod_ocp_if *omap2430_l3_masters[] = {
	&omap2430_l3__l4_core,
};

/* L3 */
static struct omap_hwmod omap2430_l3_hwmod = {
	.name		= "l3_hwmod",
	.class		= &l3_hwmod_class,
	.masters	= omap2430_l3_masters,
	.masters_cnt	= ARRAY_SIZE(omap2430_l3_masters),
	.slaves		= omap2430_l3_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_l3_slaves),
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430)
};

static struct omap_hwmod omap2430_l4_wkup_hwmod;
static struct omap_hwmod omap2430_mmc1_hwmod;
static struct omap_hwmod omap2430_mmc2_hwmod;
static struct omap_hwmod omap2430_gpio1_hwmod;
static struct omap_hwmod omap2430_gpio2_hwmod;
static struct omap_hwmod omap2430_gpio3_hwmod;
static struct omap_hwmod omap2430_gpio4_hwmod;
static struct omap_hwmod omap2430_gpio5_hwmod;

/* L4_CORE -> L4_WKUP interface */
static struct omap_hwmod_ocp_if omap2430_l4_core__l4_wkup = {
	.master	= &omap2430_l4_core_hwmod,
	.slave	= &omap2430_l4_wkup_hwmod,
	.user	= OCP_USER_MPU | OCP_USER_SDMA,
};

/* L4 WKUP -> GPIO1 interface */
static struct omap_hwmod_addr_space omap2430_gpio1_addr_space[] = {
	{
		.pa_start	= OMAP243X_GPIO1_BASE,
		.pa_end		= OMAP243X_GPIO1_BASE + SZ_4K - 1,
		.flags		= ADDR_TYPE_RT
	},
};

static struct omap_hwmod_ocp_if omap2430_l4_wkup__gpio1 = {
	.master		= &omap2430_l4_wkup_hwmod,
	.slave		= &omap2430_gpio1_hwmod,
	.clk		= "gpios_ick",
	.addr		= omap2430_gpio1_addr_space,
	.addr_cnt	= ARRAY_SIZE(omap2430_gpio1_addr_space),
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* L4 WKUP -> GPIO2 interface */
static struct omap_hwmod_addr_space omap2430_gpio2_addr_space[] = {
	{
		.pa_start	= OMAP243X_GPIO2_BASE,
		.pa_end		= OMAP243X_GPIO2_BASE + SZ_4K - 1,
		.flags		= ADDR_TYPE_RT
	},
};

static struct omap_hwmod_ocp_if omap2430_l4_wkup__gpio2 = {
	.master		= &omap2430_l4_wkup_hwmod,
	.slave		= &omap2430_gpio2_hwmod,
	.clk		= "gpios_ick",
	.addr		= omap2430_gpio2_addr_space,
	.addr_cnt	= ARRAY_SIZE(omap2430_gpio2_addr_space),
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* L4 WKUP -> GPIO3 interface */
static struct omap_hwmod_addr_space omap2430_gpio3_addr_space[] = {
	{
		.pa_start	= OMAP243X_GPIO3_BASE,
		.pa_end		= OMAP243X_GPIO3_BASE + SZ_4K - 1,
		.flags		= ADDR_TYPE_RT
	},
};

static struct omap_hwmod_ocp_if omap2430_l4_wkup__gpio3 = {
	.master		= &omap2430_l4_wkup_hwmod,
	.slave		= &omap2430_gpio3_hwmod,
	.clk		= "gpios_ick",
	.addr		= omap2430_gpio3_addr_space,
	.addr_cnt	= ARRAY_SIZE(omap2430_gpio3_addr_space),
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* L4 WKUP -> GPIO4 interface */
static struct omap_hwmod_addr_space omap2430_gpio4_addr_space[] = {
	{
		.pa_start	= OMAP243X_GPIO4_BASE,
		.pa_end		= OMAP243X_GPIO4_BASE + SZ_4K - 1,
		.flags		= ADDR_TYPE_RT
	},
};

static struct omap_hwmod_ocp_if omap2430_l4_wkup__gpio4 = {
	.master		= &omap2430_l4_wkup_hwmod,
	.slave		= &omap2430_gpio4_hwmod,
	.clk		= "gpios_ick",
	.addr		= omap2430_gpio4_addr_space,
	.addr_cnt	= ARRAY_SIZE(omap2430_gpio4_addr_space),
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* L4 CORE -> GPIO5 interface */
static struct omap_hwmod_addr_space omap2430_gpio5_addr_space[] = {
	{
		.pa_start	= OMAP243X_GPIO5_BASE,
		.pa_end		= OMAP243X_GPIO5_BASE + SZ_4K - 1,
		.flags		= ADDR_TYPE_RT
	},
};

static struct omap_hwmod_ocp_if omap2430_l4_core__gpio5 = {
	.master		= &omap2430_l4_core_hwmod,
	.slave		= &omap2430_gpio5_hwmod,
	.clk		= "gpio5_ick",
	.addr		= omap2430_gpio5_addr_space,
	.addr_cnt	= ARRAY_SIZE(omap2430_gpio5_addr_space),
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* Slave interfaces on the L4_CORE interconnect */
static struct omap_hwmod_ocp_if *omap2430_l4_core_slaves[] = {
	&omap2430_l3__l4_core,
	&omap2430_l4_core__gpio5,
};

/* Master interfaces on the L4_CORE interconnect */
static struct omap_hwmod_ocp_if *omap2430_l4_core_masters[] = {
	&omap2430_l4_core__l4_wkup,
};

/* L4 CORE */
static struct omap_hwmod omap2430_l4_core_hwmod = {
	.name		= "l4_core_hwmod",
	.class		= &l4_hwmod_class,
	.masters	= omap2430_l4_core_masters,
	.masters_cnt	= ARRAY_SIZE(omap2430_l4_core_masters),
	.slaves		= omap2430_l4_core_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_l4_core_slaves),
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430)
};

/* Slave interfaces on the L4_WKUP interconnect */
static struct omap_hwmod_ocp_if *omap2430_l4_wkup_slaves[] = {
	&omap2430_l4_core__l4_wkup,
	&omap2430_l4_wkup__gpio1,
	&omap2430_l4_wkup__gpio2,
	&omap2430_l4_wkup__gpio3,
	&omap2430_l4_wkup__gpio4,
};

/* Master interfaces on the L4_WKUP interconnect */
static struct omap_hwmod_ocp_if *omap2430_l4_wkup_masters[] = {
};

/* L4 WKUP */
static struct omap_hwmod omap2430_l4_wkup_hwmod = {
	.name		= "l4_wkup_hwmod",
	.class		= &l4_hwmod_class,
	.masters	= omap2430_l4_wkup_masters,
	.masters_cnt	= ARRAY_SIZE(omap2430_l4_wkup_masters),
	.slaves		= omap2430_l4_wkup_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_l4_wkup_slaves),
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430)
};

/* Master interfaces on the MPU device */
static struct omap_hwmod_ocp_if *omap2430_mpu_masters[] = {
	&omap2430_mpu__l3,
};

/* MPU */
static struct omap_hwmod omap2430_mpu_hwmod = {
	.name		= "mpu",
	.class		= &mpu_hwmod_class,
	.main_clk	= "mpu_ck",
	.masters	= omap2430_mpu_masters,
	.masters_cnt	= ARRAY_SIZE(omap2430_mpu_masters),
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

/* GPIO common */

static struct omap_gpio_dev_attr gpio_dev_attr = {
	.gpio_bank_count = 5,
	.gpio_bank_bits = 32,
	.dbck_flag = false,
	.fck_flag = true,
};

static struct omap_hwmod_class_sysconfig gpio_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_SIDLEMODE |
			   SYSC_HAS_ENAWAKEUP | SYSC_HAS_SOFTRESET |
			   SYSC_HAS_AUTOIDLE),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields    = &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class gpio_class = {
	.name = "gpio",
	.sysc = &gpio_sysc,
};

/* GPIO1 */

static struct omap_hwmod_irq_info gpio1_mpu_irqs[] = {
	{ .name = "gpio_mpu_irq", .irq = INT_24XX_GPIO_BANK1 },
};

static struct omap_hwmod_ocp_if *omap2430_gpio1_slaves[] = {
	&omap2430_l4_wkup__gpio1,
};

static struct omap_hwmod omap2430_gpio1_hwmod = {
	.name		= "gpio1_hwmod",
	.mpu_irqs	= gpio1_mpu_irqs,
	.mpu_irqs_cnt	= ARRAY_SIZE(gpio1_mpu_irqs),
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.prcm_reg_id = 1,
			.module_bit = OMAP24XX_EN_GPIOS_SHIFT,
		},
	},
	.slaves		= omap2430_gpio1_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_gpio1_slaves),
	.class		= &gpio_class,
	.dev_attr	= &gpio_dev_attr,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

/* GPIO2 */

static struct omap_hwmod_irq_info gpio2_mpu_irqs[] = {
	{ .name = "gpio_mpu_irq", .irq = INT_24XX_GPIO_BANK2 },
};

static struct omap_hwmod_ocp_if *omap2430_gpio2_slaves[] = {
	&omap2430_l4_wkup__gpio2,
};

static struct omap_hwmod omap2430_gpio2_hwmod = {
	.name		= "gpio2_hwmod",
	.mpu_irqs	= gpio2_mpu_irqs,
	.mpu_irqs_cnt	= ARRAY_SIZE(gpio2_mpu_irqs),
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.prcm_reg_id = 1,
			.module_bit = OMAP24XX_EN_GPIOS_SHIFT,
		},
	},
	.slaves		= omap2430_gpio2_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_gpio2_slaves),
	.class		= &gpio_class,
	.dev_attr	= &gpio_dev_attr,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

/* GPIO3 */

static struct omap_hwmod_irq_info gpio3_mpu_irqs[] = {
	{ .name = "gpio_mpu_irq", .irq = INT_24XX_GPIO_BANK3 },
};

static struct omap_hwmod_ocp_if *omap2430_gpio3_slaves[] = {
	&omap2430_l4_wkup__gpio3,
};

static struct omap_hwmod omap2430_gpio3_hwmod = {
	.name		= "gpio3_hwmod",
	.mpu_irqs	= gpio3_mpu_irqs,
	.mpu_irqs_cnt	= ARRAY_SIZE(gpio3_mpu_irqs),
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.prcm_reg_id = 1,
			.module_bit = OMAP24XX_EN_GPIOS_SHIFT,
		},
	},
	.slaves		= omap2430_gpio3_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_gpio3_slaves),
	.class		= &gpio_class,
	.dev_attr	= &gpio_dev_attr,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

/* GPIO4 */

static struct omap_hwmod_irq_info gpio4_mpu_irqs[] = {
	{ .name = "gpio_mpu_irq", .irq = INT_24XX_GPIO_BANK4 },
};

static struct omap_hwmod_ocp_if *omap2430_gpio4_slaves[] = {
	&omap2430_l4_wkup__gpio4,
};

static struct omap_hwmod omap2430_gpio4_hwmod = {
	.name		= "gpio4_hwmod",
	.mpu_irqs	= gpio4_mpu_irqs,
	.mpu_irqs_cnt	= ARRAY_SIZE(gpio4_mpu_irqs),
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.prcm_reg_id = 1,
			.module_bit = OMAP24XX_EN_GPIOS_SHIFT,
		},
	},
	.slaves		= omap2430_gpio4_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_gpio4_slaves),
	.class		= &gpio_class,
	.dev_attr	= &gpio_dev_attr,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

/* GPIO5 */

static struct omap_hwmod_irq_info gpio5_mpu_irqs[] = {
	{ .name = "gpio_mpu_irq", .irq = INT_24XX_GPIO_BANK5 },
};

static struct omap_hwmod_ocp_if *omap2430_gpio5_slaves[] = {
	&omap2430_l4_core__gpio5,
};

static struct omap_hwmod omap2430_gpio5_hwmod = {
	.name		= "gpio5_hwmod",
	.mpu_irqs	= gpio5_mpu_irqs,
	.mpu_irqs_cnt	= ARRAY_SIZE(gpio5_mpu_irqs),
	.main_clk	= "gpio5_fck",
	.prcm		= {
		.omap2 = {
			.prcm_reg_id = 1,
			.module_bit = OMAP24XX_EN_GPIOS_SHIFT,
		},
	},
	.slaves		= omap2430_gpio5_slaves,
	.slaves_cnt	= ARRAY_SIZE(omap2430_gpio5_slaves),
	.class		= &gpio_class,
	.dev_attr	= &gpio_dev_attr,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

static __initdata struct omap_hwmod *omap2430_hwmods[] = {
	&omap2430_l3_hwmod,
	&omap2430_l4_core_hwmod,
	&omap2430_l4_wkup_hwmod,
	&omap2430_mpu_hwmod,
	&omap2430_gpio1_hwmod,
	&omap2430_gpio2_hwmod,
	&omap2430_gpio3_hwmod,
	&omap2430_gpio4_hwmod,
	&omap2430_gpio5_hwmod,
	NULL,
};

int __init omap2430_hwmod_init(void)
{
	return omap_hwmod_init(omap2430_hwmods);
}


