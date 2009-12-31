/*
 * OMAP4-specific clock framework functions
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * Rajendra Nayak
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/cpufreq.h>
#include "opp4xxx.h"

#ifdef CONFIG_CPU_FREQ

extern struct opp_table omap4_vdd1_table[];

extern struct opp_table omap4_vdd1_table[];

static struct cpufreq_frequency_table freq_table[NO_OF_VDD1_OPP + 1];

void omap2_clk_init_cpufreq_table(struct cpufreq_frequency_table **table)
{
	int i = 0;

	for (i = 0; i < NO_OF_VDD1_OPP; i++) {
		freq_table[i].index = i;
		freq_table[i].frequency = omap4_vdd1_table[i].freq / 1000;
	}

	if (i == 0) {
		printk(KERN_WARNING "%s: failed to initialize frequency table\n",
								__func__);
		return;
	}

	freq_table[i].index = i;
	freq_table[i].frequency = CPUFREQ_TABLE_END;
	*table = &freq_table[0];
}
#endif
