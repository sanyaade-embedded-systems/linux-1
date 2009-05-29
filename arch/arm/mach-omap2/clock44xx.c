/*
 * OMAP4-specific clock framework functions
 *
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Written by Rajendra Nayak (rnayak@ti.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#undef DEBUG

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/limits.h>
#include <linux/bitops.h>

#include <mach/clock.h>
#include <mach/sram.h>
#include <asm/div64.h>
#include <asm/clkdev.h>

#include <mach/sdrc.h>
#include "clock.h"
#include "prm.h"
#include "prm-regbits-44xx.h"
#include "cm.h"
#include "cm1-regbits-44xx.h"
#include "cm2-regbits-44xx.h"

#include "clock44xx.h"

struct omap_clk {
	u32		cpu;
	struct clk_lookup lk;
};

#define CLK(dev, con, ck, cp) 		\
	{				\
		 .cpu = cp,		\
		.lk = {			\
			.dev_id = dev,	\
			.con_id = con,	\
			.clk = ck,	\
		},			\
	}

#define CK_443X		(1 << 0)

static struct omap_clk omap44xx_clks[] = {
	CLK(NULL,	"omap_32k_fck",	&omap_32k_fck,	CK_443X),
	CLK(NULL,	"virt_12m_ck",	&virt_12m_ck,	CK_443X),
	CLK(NULL,	"virt_13m_ck",	&virt_13m_ck,	CK_443X),
	CLK(NULL,	"virt_19_2m_ck", &virt_19_2m_ck, CK_443X),
	CLK(NULL,	"virt_26m_ck",	&virt_26m_ck,	CK_443X),
	CLK(NULL,	"virt_27m_ck",	&virt_27m_ck,	CK_443X),
	CLK(NULL,	"virt_38_4m_ck", &virt_38_4m_ck, CK_443X),
	CLK(NULL,	"sys_ck",	&sys_ck,	CK_443X),
	CLK(NULL,	"abe_dss_sys_ck", &abe_dss_sys_ck, CK_443X),
	CLK(NULL,	"dpll_sys_ref_ck", &dpll_sys_ref_ck, CK_443X),
	CLK(NULL,	"abe_dpll_alwon_ck", &abe_dpll_alwon_ck, CK_443X),
	CLK(NULL,	"gpt1_fck",	&gpt1_fck,	CK_443X),
	CLK(NULL,	"gpt2_fck",	&gpt2_fck,	CK_443X),
	CLK(NULL,	"gpt3_fck",	&gpt3_fck,	CK_443X),
	CLK(NULL,	"gpt4_fck",	&gpt4_fck,	CK_443X),
	CLK(NULL,	"gpt9_fck",	&gpt9_fck,	CK_443X),
	CLK(NULL,	"gpt10_fck",	&gpt10_fck,	CK_443X),
	CLK(NULL,	"gpt11_fck",	&gpt11_fck,	CK_443X),
	CLK(NULL,       "gpmc_fck",	&gpmc_fck,	CK_443X),
	CLK(NULL,       "dpll_abe_ck",	&dpll_abe_ck,	CK_443X),
	CLK(NULL,       "dpll_abe_x2_ck",	&dpll_abe_x2_ck, CK_443X),
	CLK(NULL,       "dpll_abe_m2_ck",	&dpll_abe_m2_ck, CK_443X),
	CLK(NULL,       "dpll_abe_x2m2_ck",	&dpll_abe_x2m2_ck, CK_443X),
	CLK(NULL,       "dpll_abe_x2m3_ck",	&dpll_abe_x2m3_ck, CK_443X),
	CLK(NULL,       "per_abe_x1_fck",	&per_abe_x1_fck, CK_443X),
	CLK(NULL,       "dpll_abe_x2_fck",	&dpll_abe_x2_fck, CK_443X),
	CLK(NULL,       "core_dpll_hs_ck",	&core_dpll_hs_ck, CK_443X),
	CLK(NULL,       "per_dpll_hs_ck",	&per_dpll_hs_ck, CK_443X),
	CLK(NULL,       "per_hsd_byp_ck",	&per_hsd_byp_ck, CK_443X),
	CLK(NULL,       "dpll_per_ck",	&dpll_per_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2_ck",	&dpll_per_x2_ck, CK_443X),
	CLK(NULL,       "dpll_per_m2_ck",	&dpll_per_m2_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2m2_ck",	&dpll_per_x2m2_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2m3_ck",	&dpll_per_x2m3_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2m4_ck",	&dpll_per_x2m4_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2m5_ck",	&dpll_per_x2m5_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2m6_ck",	&dpll_per_x2m6_ck, CK_443X),
	CLK(NULL,       "dpll_per_x2m7_ck",	&dpll_per_x2m7_ck, CK_443X),
	CLK(NULL,       "omap_96m_alwon_ck",	&omap_96m_alwon_ck, CK_443X),
	CLK(NULL,       "omap_192m_fck",	&omap_192m_fck, CK_443X),
	CLK(NULL,       "per_dpll_scrm_ck",	&per_dpll_scrm_ck, CK_443X),
	CLK(NULL,       "omap_128m_fck",	&omap_128m_fck, CK_443X),
	CLK(NULL,       "dss_fck",	&dss_fck, CK_443X),
	CLK(NULL,       "per_mpu_m3",	&per_mpu_m3, CK_443X),
	CLK(NULL,       "per_dpll_emu_ck",	&per_dpll_emu_ck, CK_443X),
	CLK(NULL,       "core_phy_hsd_byp_ck",	&core_phy_hsd_byp_ck, CK_443X),
	CLK(NULL,       "dpll_core_ck",		&dpll_core_ck, CK_443X),
	CLK(NULL,       "dpll_core_x2_ck",	&dpll_core_x2_ck, CK_443X),
	CLK(NULL,       "dpll_core_m2_ck",	&dpll_core_m2_ck, CK_443X),
	CLK(NULL,       "dpll_core_x2m3_ck",	&dpll_core_x2m3_ck, CK_443X),
	CLK(NULL,       "dpll_core_x2m4_ck",	&dpll_core_x2m4_ck, CK_443X),
	CLK(NULL,       "dpll_core_x2m5_ck",	&dpll_core_x2m5_ck, CK_443X),
	CLK(NULL,       "dpll_core_x2m6_ck",	&dpll_core_x2m6_ck, CK_443X),
	CLK(NULL,       "dpll_core_x2m7_ck",	&dpll_core_x2m7_ck, CK_443X),
	CLK(NULL,       "phy_root_ck",		&phy_root_ck, CK_443X),
	CLK(NULL,       "core_dpll_scrm_ck",	&core_dpll_scrm_ck, CK_443X),
	CLK(NULL,       "dpll_x2_ck",		&dpll_x2_ck, CK_443X),
	CLK(NULL,       "core_x2_ck",		&core_x2_ck, CK_443X),
	CLK(NULL,       "core_dpll_emu_ck",	&core_dpll_emu_ck, CK_443X),
	CLK(NULL,       "core_sgx_fck",		&core_sgx_fck, CK_443X),
	CLK(NULL,       "mpu_dpll_hs_ck",	&mpu_dpll_hs_ck, CK_443X),
	CLK(NULL,	"dpll_mpu_ck",		&dpll_mpu_ck, CK_443X),
	CLK(NULL,	"dpll_mpu_m2_ck",	&dpll_mpu_m2_ck, CK_443X),
	CLK(NULL,	"mpu_dpll_ck",		&mpu_dpll_ck, CK_443X),
	CLK(NULL,	"iva_dpll_hs_ck",	&iva_dpll_hs_ck, CK_443X),
	CLK(NULL,	"dpll_iva_ck",		&dpll_iva_ck, CK_443X),
	CLK(NULL,	"dpll_iva_x2_ck",	&dpll_iva_x2_ck, CK_443X),
	CLK(NULL,	"dpll_iva_x2m4_ck",	&dpll_iva_x2m4_ck, CK_443X),
	CLK(NULL,	"dpll_iva_x2m5_ck",	&dpll_iva_x2m5_ck, CK_443X),
	CLK(NULL,	"dsp_root_ck",		&dsp_root_ck, CK_443X),
	CLK(NULL,	"ivahd_ck",		&ivahd_ck, CK_443X),
	CLK(NULL,	"dpll_unipro_ck",	&dpll_unipro_ck, CK_443X),
	CLK(NULL,	"dpll_unipro_x2_ck",	&dpll_unipro_x2_ck, CK_443X),
	CLK(NULL,	"dpll_unipro_x2m2_ck",	&dpll_unipro_x2m2_ck, CK_443X),
	CLK(NULL,	"unipro1_phy_fck",	&unipro1_phy_fck, CK_443X),
};

static struct clk_functions omap2_clk_functions = {
	.clk_enable		= omap2_clk_enable,
	.clk_disable		= omap2_clk_disable,
	.clk_round_rate		= omap2_clk_round_rate,
	.clk_set_rate		= omap2_clk_set_rate,
	.clk_set_parent		= omap2_clk_set_parent,
	.clk_disable_unused	= omap2_clk_disable_unused,
};

void omap2_clk_prepare_for_reboot(void)
{
	return;
}

static int omap4_noncore_dpll_set_rate(struct clk *clk, unsigned long rate)
{
	return 0;
}

static unsigned long omap4_clkoutx2_recalc(struct clk *clk)
{
	return 0;
}

static unsigned long omap4_dpll_recalc(struct clk *clk)
{
	return 0;
}

static int __init omap2_clk_arch_init(void)
{
	if (!mpurate)
		return -EINVAL;

	recalculate_root_clocks();
	return 0;
}
arch_initcall(omap2_clk_arch_init);

int __init omap2_clk_init(void)
{
	/* struct prcm_config *prcm; */
	struct omap_clk *c;
	/* u32 clkrate; */
	u32 cpu_clkflg;

	if (cpu_is_omap44xx()) {
		cpu_mask = RATE_IN_443X;
		cpu_clkflg = CK_443X;
	}

	clk_init(&omap2_clk_functions);

	for (c = omap44xx_clks; c < omap44xx_clks + ARRAY_SIZE(omap44xx_clks); c++)
		clk_init_one(c->lk.clk);

	for (c = omap44xx_clks; c < omap44xx_clks + ARRAY_SIZE(omap44xx_clks); c++)
		if (c->cpu & cpu_clkflg) {
			clkdev_add(&c->lk);
			clk_register(c->lk.clk);
			/* TODO
			omap2_init_clk_clkdm(c->lk.clk);
			*/
		}

	recalculate_root_clocks();

	/*
	 * Only enable those clocks we will need, let the drivers
	 * enable other clocks as necessary
	 */
	clk_enable_init_clocks();

	return 0;
}
