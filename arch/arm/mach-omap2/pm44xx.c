/*
 * OMAP4 Power Management Routines
 *
 * Copyright (C) 2010 Texas Instruments, Inc.
 * Rajendra Nayak <rnayak@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/pm.h>
#include <linux/suspend.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <plat/control.h>
#include <plat/clockdomain.h>
#include <plat/powerdomain.h>
#include <mach/omap4-common.h>
#include "prm.h"
#include "pm.h"

struct power_state {
	struct powerdomain *pwrdm;
	u32 next_state;
#ifdef CONFIG_SUSPEND
	u32 saved_state;
#endif
	struct list_head node;
};

static LIST_HEAD(pwrst_list);

static struct powerdomain *cpu0_pwrdm, *cpu1_pwrdm, *mpu_pwrdm;

#ifdef CONFIG_SUSPEND
static int omap4_pm_prepare(void)
{
	disable_hlt();
	return 0;
}

static int omap4_pm_suspend(void)
{
	do_wfi();
	return 0;
}

static int omap4_pm_enter(suspend_state_t suspend_state)
{
	int ret = 0;

	switch (suspend_state) {
	case PM_SUSPEND_STANDBY:
	case PM_SUSPEND_MEM:
		ret = omap4_pm_suspend();
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static void omap4_pm_finish(void)
{
	enable_hlt();
	return;
}

static int omap4_pm_begin(suspend_state_t state)
{
	return 0;
}

static void omap4_pm_end(void)
{
	return;
}

static struct platform_suspend_ops omap_pm_ops = {
	.begin		= omap4_pm_begin,
	.end		= omap4_pm_end,
	.prepare	= omap4_pm_prepare,
	.enter		= omap4_pm_enter,
	.finish		= omap4_pm_finish,
	.valid		= suspend_valid_only_mem,
};
#endif /* CONFIG_SUSPEND */

static int __init pwrdms_setup(struct powerdomain *pwrdm, void *unused)
{
	struct power_state *pwrst;

	if (!pwrdm->pwrsts)
		return 0;

	pwrst = kmalloc(sizeof(struct power_state), GFP_ATOMIC);
	if (!pwrst)
		return -ENOMEM;
	pwrst->pwrdm = pwrdm;
	pwrst->next_state = PWRDM_POWER_ON;
	list_add(&pwrst->node, &pwrst_list);

	return pwrdm_set_next_pwrst(pwrst->pwrdm, pwrst->next_state);
}

/**
 * omap4_pm_init - Init routine for OMAP4 PM
 *
 * Initializes all powerdomain and clockdomain target states
 * and all PRCM settings.
 */
static int __init omap4_pm_init(void)
{
	int ret;

	if (!cpu_is_omap44xx())
		return -ENODEV;

	printk(KERN_ERR "Power Management for TI OMAP4.\n");

#ifdef CONFIG_PM
	ret = pwrdm_for_each(pwrdms_setup, NULL);
	if (ret) {
		printk(KERN_ERR "Failed to setup powerdomains\n");
		goto err2;
	}
#endif

	cpu0_pwrdm = pwrdm_lookup("cpu0_pwrdm");
	cpu1_pwrdm = pwrdm_lookup("cpu1_pwrdm");
	mpu_pwrdm = pwrdm_lookup("mpu_pwrdm");
	if (!cpu0_pwrdm || !cpu1_pwrdm || !mpu_pwrdm) {
		printk(KERN_ERR "Failed to get lookup for CPUx/MPU pwrdm's\n");
		goto err2;
	}

#ifdef CONFIG_SUSPEND
	suspend_set_ops(&omap_pm_ops);
#endif /* CONFIG_SUSPEND */

	omap4_idle_init();

err2:
	return ret;
}
late_initcall(omap4_pm_init);
