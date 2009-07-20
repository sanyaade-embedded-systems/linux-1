/*
 * CPU frequency scaling for DaVinci
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Based on linux/arch/arm/plat-omap/cpu-omap.c. Original Copyright follows:
 *
 *  Copyright (C) 2005 Nokia Corporation
 *  Written by Tony Lindgren <tony@atomide.com>
 *
 *  Based on cpu-sa1110.c, Copyright (C) 2001 Russell King
 *
 * Copyright (C) 2007-2008 Texas Instruments, Inc.
 * Updated to support OMAP3
 * Rajendra Nayak <rnayak@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <asm/system.h>
#include <mach/clock.h>
#include <mach/common.h>

#include "clock.h"

static struct cpufreq_frequency_table *freq_table;
static struct clk *armclk;

static int davinci_verify_speed(struct cpufreq_policy *policy)
{
	if (freq_table)
		return cpufreq_frequency_table_verify(policy, freq_table);

	if (policy->cpu)
		return -EINVAL;

	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
				     policy->cpuinfo.max_freq);

	policy->min = clk_round_rate(armclk, policy->min * 1000) / 1000;
	policy->max = clk_round_rate(armclk, policy->max * 1000) / 1000;
	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
				     policy->cpuinfo.max_freq);
	return 0;
}

static unsigned int davinci_getspeed(unsigned int cpu)
{
	unsigned long rate;

	if (cpu)
		return 0;

	rate = clk_get_rate(armclk) / 1000;

	return rate;
}

static int davinci_target(struct cpufreq_policy *policy,
		       unsigned int target_freq,
		       unsigned int relation)
{
	struct cpufreq_freqs freqs;
	int ret = 0;

	/*
	 * Ensure desired rate is within allowed range.  Some govenors
	 * (ondemand) will just pass target_freq=0 to get the minimum.
	 */
	if (target_freq < policy->cpuinfo.min_freq)
		target_freq = policy->cpuinfo.min_freq;
	if (target_freq > policy->cpuinfo.max_freq)
		target_freq = policy->cpuinfo.max_freq;

	freqs.old = davinci_getspeed(0);
	freqs.new = clk_round_rate(armclk, target_freq * 1000) / 1000;
	freqs.cpu = 0;

	if (freqs.old == freqs.new)
		return ret;
	cpufreq_notify_transition(&freqs, CPUFREQ_PRECHANGE);
#ifdef CONFIG_CPU_FREQ_DEBUG
	printk(KERN_DEBUG "cpufreq-davinci: transition: %u --> %u\n",
	       freqs.old, freqs.new);
#endif
	ret = clk_set_rate(armclk, freqs.new * 1000);
	cpufreq_notify_transition(&freqs, CPUFREQ_POSTCHANGE);

	return ret;
}

static int __init davinci_cpu_init(struct cpufreq_policy *policy)
{
	int result = 0;

	armclk = clk_get(NULL, "arm");
	if (IS_ERR(armclk)) {
		printk(KERN_ERR "cpufreq-davinci: Unable to get ARM clock\n");
		return PTR_ERR(armclk);
	}

	if (policy->cpu != 0) {
		clk_put(armclk);
		return -EINVAL;
	}

	policy->cur = policy->min = policy->max = davinci_getspeed(0);

	davinci_soc_info.init_cpufreq_table(&freq_table);
	if (freq_table) {
		result = cpufreq_frequency_table_cpuinfo(policy, freq_table);
		if (!result)
			cpufreq_frequency_table_get_attr(freq_table,
							policy->cpu);
	} else {
		policy->cpuinfo.min_freq = policy->min;
		policy->cpuinfo.max_freq = policy->max;
	}

	clk_set_rate(armclk, policy->cpuinfo.max_freq * 1000);

	policy->min = policy->cpuinfo.min_freq;
	policy->max = policy->cpuinfo.max_freq;
	policy->cur = davinci_getspeed(0);

	/*
	 * Time measurement across the target() function yields
	 * ~500 us time taken with no drivers on notification list.
	 * Setting the latency to 700 us to accomodate addition of
	 * drivers to pre/post change notification list.
	 */
	policy->cpuinfo.transition_latency = 700 * 1000;
	return 0;
}

static int davinci_cpu_exit(struct cpufreq_policy *policy)
{
	clk_put(armclk);
	return 0;
}

static struct freq_attr *davinci_cpufreq_attr[] = {
	&cpufreq_freq_attr_scaling_available_freqs,
	NULL,
};

static struct cpufreq_driver davinci_driver = {
	.flags		= CPUFREQ_STICKY,
	.verify		= davinci_verify_speed,
	.target		= davinci_target,
	.get		= davinci_getspeed,
	.init		= davinci_cpu_init,
	.exit		= davinci_cpu_exit,
	.name		= "davinci",
	.attr		= davinci_cpufreq_attr,
};

static int __init davinci_cpufreq_init(void)
{
	return cpufreq_register_driver(&davinci_driver);
}

late_initcall(davinci_cpufreq_init);

/*
 * if ever we want to remove this, upon cleanup call:
 *
 * cpufreq_unregister_driver()
 * cpufreq_frequency_table_put_attr()
 */

