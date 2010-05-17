/*
 * gpio.c - OMAP2PLUS-specific gpio code
 *
 * Copyright (C) 2010 Texas Instruments, Inc.
 *
 * Author:
 *	Charulatha V <charu@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/slab.h>

#include <plat/omap_hwmod.h>
#include <plat/omap_device.h>

static struct omap_device_pm_latency omap_gpio_latency[] = {
	[0] = {
		.deactivate_func = omap_device_idle_hwmods,
		.activate_func   = omap_device_enable_hwmods,
		.flags		 = OMAP_DEVICE_LATENCY_AUTO_ADJUST,
	},
};

static int omap2_init_gpio(struct omap_hwmod *oh, void *user)
{
	struct omap_device *od;
	struct omap_gpio_platform_data *pdata;
	char *name = "omap-gpio";
	static int id;
	struct omap_gpio_dev_attr *gpio_dev_data;

	if (!oh)
		pr_err("Could not look up omap gpio %d\n", id + 1);

	pdata = kzalloc(sizeof(struct omap_gpio_platform_data),
					GFP_KERNEL);
	if (!pdata) {
		pr_err("Memory allocation failed gpio%d\n", id + 1);
		return -ENOMEM;
	}

	gpio_dev_data = (struct omap_gpio_dev_attr *)oh->dev_attr;
	pdata->gpio_attr = gpio_dev_data;
	pdata->method = (int)user;
	pdata->virtual_irq_start = IH_GPIO_BASE + 32 * id;

	od = omap_device_build(name, id, oh, pdata,
				sizeof(*pdata),	omap_gpio_latency,
				ARRAY_SIZE(omap_gpio_latency),
				false);
	WARN(IS_ERR(od), "Cant build omap_device for %s:%s.\n",
				name, oh->name);

	id++;
	return 0;
}

static int __init gpio_init(int method)
{
	return omap_hwmod_for_each_by_class("gpio", omap2_init_gpio,
						(void *)method);
}

/*
 * gpio_init needs to be done before
 * machine_init functions access gpio APIs.
 * Hence gpio_init is a postcore_initcall.
 */
#ifdef CONFIG_ARCH_OMAP2
static int __init omap242x_gpio_init(void)
{	if (!cpu_is_omap2420())
		return -EINVAL;

	return gpio_init(METHOD_GPIO_24XX);
}
postcore_initcall(omap242x_gpio_init);

static int __init omap243x_gpio_init(void)
{
	if (!cpu_is_omap2430())
		return -EINVAL;

	return gpio_init(METHOD_GPIO_24XX);
}
postcore_initcall(omap243x_gpio_init);
#endif

#ifdef CONFIG_ARCH_OMAP3
static int __init omap3xxx_gpio_init(void)
{
	if (!cpu_is_omap34xx())
		return -EINVAL;

	return gpio_init(METHOD_GPIO_24XX);
}
postcore_initcall(omap3xxx_gpio_init);
#endif

#ifdef CONFIG_ARCH_OMAP4
static int __init omap44xx_gpio_init(void)
{
	if (!cpu_is_omap44xx())
		return -EINVAL;

	return gpio_init(METHOD_GPIO_44XX);
}
postcore_initcall(omap44xx_gpio_init);
#endif
