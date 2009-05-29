/*
 * OMAP4 clock framework
 *
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Written by Rajendra Nayak (rnayak@ti.com)
 *
 */

#ifndef __ARCH_ARM_MACH_OMAP2_CLOCK44XX_H
#define __ARCH_ARM_MACH_OMAP2_CLOCK44XX_H

#include <mach/control.h>

#include "clock.h"
#include "cm.h"
#include "cm1-regbits-44xx.h"
#include "cm2-regbits-44xx.h"
#include "prm.h"
#include "prm-regbits-44xx.h"

static unsigned long omap4_dpll_recalc(struct clk *clk);
static unsigned long omap4_clkoutx2_recalc(struct clk *clk);
static int omap4_noncore_dpll_set_rate(struct clk *clk, unsigned long rate);

/* PRM CLOCKS */

static struct clk omap_32k_fck = {
	.name		= "omap_32k_fck",
	.ops		= &clkops_null,
	.rate		= 32768,
	.flags		= RATE_FIXED,
};

static struct clk secure_32k_fck = {
	.name		= "secure_32k_fck",
	.ops		= &clkops_null,
	.rate		= 32768,
	.flags		= RATE_FIXED,
};

static struct clk virt_12m_ck = {
	.name		= "virt_12m_ck",
	.ops		= &clkops_null,
	.rate		= 12000000,
	.flags		= RATE_FIXED,
};

static struct clk virt_13m_ck = {
	.name		= "virt_13m_ck",
	.ops		= &clkops_null,
	.rate		= 13000000,
	.flags		= RATE_FIXED,
};

static struct clk virt_19_2m_ck = {
	.name		= "virt_19_2m_ck",
	.ops		= &clkops_null,
	.rate		= 19200000,
	.flags		= RATE_FIXED,
};

static struct clk virt_26m_ck = {
	.name		= "virt_26m_ck",
	.ops		= &clkops_null,
	.rate		= 26000000,
	.flags		= RATE_FIXED,
};

static struct clk virt_27m_ck = {
	.name		= "virt_27m_ck",
	.ops		= &clkops_null,
	.rate		= 27000000,
	.flags		= RATE_FIXED,
};

static struct clk virt_38_4m_ck = {
	.name		= "virt_38_4m_ck",
	.ops		= &clkops_null,
	.rate		= 38400000,
	.flags		= RATE_FIXED,
};

static const struct clksel_rate sys_clk_12m_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_13m_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_19_2m_rates[] = {
	{ .div = 1, .val = 2, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_26m_rates[] = {
	{ .div = 1, .val = 3, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_27m_rates[] = {
	{ .div = 1, .val = 5, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_38_4m_rates[] = {
	{ .div = 1, .val = 4, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel sys_clk_clksel[] = {
	{ .parent = &virt_12m_ck,   .rates = sys_clk_12m_rates },
	{ .parent = &virt_13m_ck,   .rates = sys_clk_13m_rates },
	{ .parent = &virt_19_2m_ck, .rates = sys_clk_19_2m_rates },
	{ .parent = &virt_26m_ck,   .rates = sys_clk_26m_rates },
	{ .parent = &virt_27m_ck,   .rates = sys_clk_27m_rates },
	{ .parent = &virt_38_4m_ck, .rates = sys_clk_38_4m_rates },
	{ .parent = NULL },
};

/* sys_clk clock */
/* 12, 13, 19.2, 26, 27 or 38.4 MHz */
static struct clk sys_ck = {
	.name		= "sys_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.clksel_reg	= OMAP4430_CM_SYS_CLKSEL,
	.clksel_mask	= OMAP4430_SYS_CLKSEL_MASK,
	.clksel		= sys_clk_clksel,
	.flags		= RATE_FIXED,
	.recalc		= &omap2_clksel_recalc,
};

static const struct clksel_rate div2_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 2, .val = 1, .flags = RATE_IN_443X },
	{ .div = 0 }
};

static const struct clksel abe_dss_sys_ck_clksel[] = {
	{ .parent = &sys_ck, .rates = div2_rates },
	{ .parent = NULL }
};

static struct clk abe_dss_sys_ck = {
	.name 		= "abe_dss_sys_ck",
	.ops            = &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &sys_ck,
	.clksel_reg     = OMAP4430_CM_ABE_DSS_SYS_CLKSEL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel		= abe_dss_sys_ck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static const struct clksel dpll_sys_ref_ck_clksel[] = {
	{ .parent = &sys_ck, .rates = div2_rates },
	{ .parent = NULL }
};

static struct clk dpll_sys_ref_ck = {
	.name           = "dpll_sys_ref_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &sys_ck,
	.clksel_reg	= OMAP4430_CM_DPLL_SYS_REF_CLKSEL,
	.clksel_mask	= OMAP4430_CLKSEL_MASK,
	.clksel		= dpll_sys_ref_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel_rate dpll_sys_ref_ck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate omap_32k_fck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel abe_dpll_alwon_ck_clksel[] = {
	{ .parent = &dpll_sys_ref_ck, .rates = dpll_sys_ref_ck_rates },
	{ .parent = &omap_32k_fck, .rates = omap_32k_fck_rates },
	{ .parent = NULL }
};

static struct clk abe_dpll_alwon_ck = {
	.name           = "abe_dpll_alwon_ck",
	.ops            = &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &omap_32k_fck,
	.clksel_reg     = OMAP4430_CM_ABE_PLL_REF_CLKSEL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = abe_dpll_alwon_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel  omap443x_gpt_clksel[] = {
	{ .parent = &omap_32k_fck, .rates = gpt_32k_rates },
	{ .parent = &sys_ck,       .rates = gpt_sys_rates },
	{ .parent = NULL}
};

static struct clk gpt1_fck = {
	.name           = "gpt1_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_WKUP_GPTIMER1_CLKCTRL,
	.enable_bit	= OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_WKUP_GPTIMER1_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_MASK,
	.clksel		= omap443x_gpt_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk gpt2_fck = {
	.name           = "gpt2_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_L4PER_GPTIMER2_CLKCTRL,
	.enable_bit     = OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_L4PER_GPTIMER2_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = omap443x_gpt_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk gpt3_fck = {
	.name           = "gpt3_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_L4PER_GPTIMER3_CLKCTRL,
	.enable_bit     = OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_L4PER_GPTIMER3_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = omap443x_gpt_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk gpt4_fck = {
	.name           = "gpt4_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_L4PER_GPTIMER4_CLKCTRL,
	.enable_bit     = OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_L4PER_GPTIMER4_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = omap443x_gpt_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk gpt9_fck = {
	.name           = "gpt9_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_L4PER_GPTIMER9_CLKCTRL,
	.enable_bit     = OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_L4PER_GPTIMER9_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = omap443x_gpt_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk gpt10_fck = {
	.name           = "gpt10_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_L4PER_GPTIMER10_CLKCTRL,
	.enable_bit     = OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_L4PER_GPTIMER10_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = omap443x_gpt_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk gpt11_fck = {
	.name           = "gpt11_fck",
	.ops            = &clkops_omap2_dflt_wait,
	.parent         = &sys_ck,
	.init           = &omap2_init_clksel_parent,
	.enable_reg     = OMAP4430_CM_L4PER_GPTIMER11_CLKCTRL,
	.enable_bit     = OMAP4430_EN_GPT_SHIFT,
	.clksel_reg     = OMAP4430_CM_L4PER_GPTIMER11_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel         = omap443x_gpt_clksel,
	.recalc         = &omap2_clksel_recalc,
};

/* TODO:Dummy clock node still needed to boot on Omap4, remove later */
static struct clk gpmc_fck = {
	.name           = "gpmc_fck",
	.ops            = &clkops_null,
};

/* DPLL's */

#define DPLL_LOW_POWER_STOP	0x1
#define DPLL_LOW_POWER_BYPASS	0x5
#define	DPLL_LOCKED		0x7

static const struct clksel_rate div_mx_dpll_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 2, .val = 2, .flags = RATE_IN_443X },
	{ .div = 3, .val = 3, .flags = RATE_IN_443X },
	{ .div = 4, .val = 4, .flags = RATE_IN_443X },
	{ .div = 5, .val = 5, .flags = RATE_IN_443X },
	{ .div = 6, .val = 6, .flags = RATE_IN_443X },
	{ .div = 7, .val = 7, .flags = RATE_IN_443X },
	{ .div = 8, .val = 8, .flags = RATE_IN_443X },
	{ .div = 9, .val = 9, .flags = RATE_IN_443X },
	{ .div = 10, .val = 10, .flags = RATE_IN_443X },
	{ .div = 11, .val = 11, .flags = RATE_IN_443X },
	{ .div = 12, .val = 12, .flags = RATE_IN_443X },
	{ .div = 13, .val = 13, .flags = RATE_IN_443X },
	{ .div = 14, .val = 14, .flags = RATE_IN_443X },
	{ .div = 15, .val = 15, .flags = RATE_IN_443X },
	{ .div = 16, .val = 16, .flags = RATE_IN_443X },
	{ .div = 17, .val = 17, .flags = RATE_IN_443X },
	{ .div = 18, .val = 18, .flags = RATE_IN_443X },
	{ .div = 19, .val = 19, .flags = RATE_IN_443X },
	{ .div = 20, .val = 20, .flags = RATE_IN_443X },
	{ .div = 21, .val = 21, .flags = RATE_IN_443X },
	{ .div = 22, .val = 22, .flags = RATE_IN_443X },
	{ .div = 23, .val = 23, .flags = RATE_IN_443X },
	{ .div = 24, .val = 24, .flags = RATE_IN_443X },
	{ .div = 25, .val = 25, .flags = RATE_IN_443X },
	{ .div = 26, .val = 26, .flags = RATE_IN_443X },
	{ .div = 27, .val = 27, .flags = RATE_IN_443X },
	{ .div = 28, .val = 28, .flags = RATE_IN_443X },
	{ .div = 29, .val = 29, .flags = RATE_IN_443X },
	{ .div = 30, .val = 30, .flags = RATE_IN_443X },
	{ .div = 31, .val = 31, .flags = RATE_IN_443X },
	{ .div = 0 }
};

/* DPLL ABE */

static struct dpll_data dpll_abe_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_ABE,
	.mult_mask	= OMAP4430_CM2_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_DPLL_DIV_MASK,
	.clk_bypass	= &sys_ck,
	.clk_ref	= &abe_dpll_alwon_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_ABE,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_ABE,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_ABE,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_abe_ck = {
	.name           = "dpll_abe_ck",
	.ops            = &clkops_null,
	.parent         = &abe_dpll_alwon_ck,
	.dpll_data      = &dpll_abe_dd,
	.round_rate     = &omap2_dpll_round_rate,
	.set_rate       = &omap4_noncore_dpll_set_rate,
	.recalc         = &omap4_dpll_recalc,
};

static struct clk dpll_abe_x2_ck = {
	.name           = "dpll_abe_x2_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_abe_ck,
	.recalc         = &omap4_clkoutx2_recalc,
};

static const struct clksel dpll_abe_m2_clksel[] = {
	{ .parent = &dpll_abe_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_abe_m2_ck = {
	.name		= "dpll_abe_m2_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.clksel_reg	= OMAP4430_CM_DIV_M2_DPLL_ABE,
	.clksel_mask	= OMAP4430_DPLL_CLKOUT_DIV_MASK,
	.clksel		= dpll_abe_m2_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel dpll_abe_x2mx_clksel[] = {
	{ .parent = &dpll_abe_x2_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_abe_x2m2_ck = {
	.name		= "dpll_abe_x2m2_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M2_DPLL_ABE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUT_DIV_MASK,
	.clksel         = dpll_abe_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_abe_x2m3_ck = {
	.name		= "dpll_abe_x2m3_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M3_DPLL_ABE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_abe_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk per_abe_x1_fck = {
	.name		= "per_abe_x1_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_abe_m2_ck,
	.recalc         = &followparent_recalc,
};

static struct clk dpll_abe_x2_fck = {
	.name		= "dpll_abe_x2_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_abe_m2_ck,
	.recalc         = &followparent_recalc,
};

static struct clk core_dpll_hs_ck = {
	.name		= "core_dpll_hs_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_abe_m2_ck,
	.recalc         = &followparent_recalc,
};

static struct clk per_dpll_hs_ck = {
	.name 		= "per_dpll_hs_ck",
	.ops		= &clkops_null,
	.parent		= &core_dpll_hs_ck,
	.fixed_div      = 2,
	.recalc         = &omap2_fixed_divisor_recalc,
};

/* DPLL PER */

static const struct clksel_rate per_dpll_hs_ck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel per_hsd_byp_ck_clksel[] = {
	{ .parent = &dpll_sys_ref_ck, .rates = dpll_sys_ref_ck_rates },
	{ .parent = &per_dpll_hs_ck, .rates = per_dpll_hs_ck_rates },
	{ .parent = NULL }
};

static struct clk per_hsd_byp_ck = {
	.name 		= "per_hsd_byp_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &dpll_sys_ref_ck,
	.clksel_reg     = OMAP4430_CM_CLKSEL_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_BYP_CLKSEL_MASK,
	.clksel         = per_hsd_byp_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct dpll_data dpll_per_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_PER,
	.mult_mask	= OMAP4430_CM2_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_DPLL_DIV_MASK,
	.clk_bypass	= &per_dpll_hs_ck,
	.clk_ref	= &dpll_sys_ref_ck,
	.clk_hsd_bypass = &per_hsd_byp_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_PER,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_PER,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_PER,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_per_ck = {
	.name           = "dpll_per_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_sys_ref_ck,
	.dpll_data      = &dpll_per_dd,
	.round_rate     = &omap2_dpll_round_rate,
	.set_rate       = &omap4_noncore_dpll_set_rate,
	.recalc         = &omap4_dpll_recalc,
};

static struct clk dpll_per_x2_ck = {
	.name           = "dpll_per_x2_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_per_ck,
	.recalc         = &omap4_clkoutx2_recalc,
};

static const struct clksel dpll_per_m2_clksel[] = {
	{ .parent = &dpll_per_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_per_m2_ck = {
	.name		= "dpll_per_m2_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.clksel_reg	= OMAP4430_CM_DIV_M2_DPLL_PER,
	.clksel_mask	= OMAP4430_DPLL_CLKOUT_DIV_MASK,
	.clksel		= dpll_per_m2_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel dpll_per_x2mx_clksel[] = {
	{ .parent = &dpll_per_x2_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_per_x2m2_ck = {
	.name		= "dpll_per_x2m2_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M2_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_CLKOUT_DIV_MASK,
	.clksel         = dpll_per_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_per_x2m3_ck = {
	.name		= "dpll_per_x2m3_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M3_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_per_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_per_x2m4_ck = {
	.name		= "dpll_per_x2m4_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M4_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_per_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_per_x2m5_ck = {
	.name		= "dpll_per_x2m5_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M5_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_per_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_per_x2m6_ck = {
	.name		= "dpll_per_x2m6_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M6_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_per_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_per_x2m7_ck = {
	.name		= "dpll_per_x2m7_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M7_DPLL_PER,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_per_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk omap_96m_alwon_ck = {
	.name		= "omap_96m_alwon_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_per_m2_ck,
	.recalc         = &followparent_recalc,
};

static struct clk omap_192m_fck = {
	.name		= "omap_192m_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_per_x2m2_ck,
	.recalc         = &followparent_recalc,
};

static struct clk per_dpll_scrm_ck = {
	.name		= "per_dpll_scrm_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_per_x2m3_ck,
	.recalc         = &followparent_recalc,
};

static struct clk omap_128m_fck = {
	.name		= "omap_128m_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_per_x2m4_ck,
	.recalc		= &followparent_recalc,
};

static struct clk dss_fck = {
	.name 		= "dss_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_per_x2m5_ck,
	.recalc		= &followparent_recalc,
};

static struct clk per_mpu_m3 = {
	.name		= "per_mpu_m3",
	.ops		= &clkops_null,
	.parent		= &dpll_per_x2m6_ck,
	.recalc		= &followparent_recalc,
};

static struct clk per_dpll_emu_ck = {
	.name		= "per_dpll_emu_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_per_x2m7_ck,
	.recalc		= &followparent_recalc,
};

/* DPLL CORE */
static const struct clksel_rate core_dpll_hs_ck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel core_phy_hsd_byp_ck_clksel[] = {
	{ .parent = &dpll_sys_ref_ck, .rates = dpll_sys_ref_ck_rates },
	{ .parent = &core_dpll_hs_ck, .rates = core_dpll_hs_ck_rates },
	{ .parent = NULL }
};

static struct clk core_phy_hsd_byp_ck = {
	.name 		= "core_phy_hsd_byp_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &dpll_sys_ref_ck,
	.clksel_reg     = OMAP4430_CM_CLKSEL_DPLL_CORE,
	.clksel_mask    = OMAP4430_DPLL_BYP_CLKSEL_MASK,
	.clksel         = core_phy_hsd_byp_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct dpll_data dpll_core_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_CORE,
	.mult_mask	= OMAP4430_CM2_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_DPLL_DIV_MASK,
	.clk_bypass	= &core_dpll_hs_ck,
	.clk_ref	= &dpll_sys_ref_ck,
	.clk_hsd_bypass	= &core_phy_hsd_byp_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_CORE,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_CORE,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_CORE,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_core_ck = {
	.name           = "dpll_core_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_sys_ref_ck,
	.dpll_data      = &dpll_core_dd,
	.round_rate     = &omap2_dpll_round_rate,
	.set_rate       = &omap4_noncore_dpll_set_rate,
	.recalc         = &omap4_dpll_recalc,
};

static struct clk dpll_core_x2_ck = {
	.name           = "dpll_core_x2_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_core_ck,
	.recalc         = &omap4_clkoutx2_recalc,
};

static const struct clksel dpll_core_m2_clksel[] = {
	{ .parent = &dpll_core_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_core_m2_ck = {
	.name		= "dpll_core_m2_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.clksel_reg	= OMAP4430_CM_DIV_M2_DPLL_CORE,
	.clksel_mask	= OMAP4430_DPLL_CLKOUT_DIV_MASK,
	.clksel		= dpll_core_m2_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel dpll_core_x2mx_clksel[] = {
	{ .parent = &dpll_core_x2_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_core_x2m3_ck = {
	.name		= "dpll_core_x2m3_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M3_DPLL_CORE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_core_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_core_x2m4_ck = {
	.name		= "dpll_core_x2m4_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M4_DPLL_CORE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_core_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_core_x2m5_ck = {
	.name		= "dpll_core_x2m5_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M5_DPLL_CORE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_core_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_core_x2m6_ck = {
	.name		= "dpll_core_x2m6_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M6_DPLL_CORE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_core_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_core_x2m7_ck = {
	.name		= "dpll_core_x2m7_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M7_DPLL_CORE,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_core_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk phy_root_ck = {
	.name		= "phy_root_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_core_m2_ck,
	.recalc         = &followparent_recalc,
};

static struct clk core_dpll_scrm_ck = {
	.name		= "core_dpll_scrm_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_core_x2m3_ck,
	.recalc         = &followparent_recalc,
};

static struct clk dpll_x2_ck = {
	.name		= "dpll_x2_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_core_x2m4_ck,
	.recalc		= &followparent_recalc,
};

static struct clk core_x2_ck = {
	.name		= "core_x2_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_core_x2m5_ck,
	.recalc		= &followparent_recalc,
};

static struct clk core_dpll_emu_ck = {
	.name		= "core_dpll_emu_ck",
	.ops		= &clkops_null,
	.parent         = &dpll_core_x2m6_ck,
	.recalc		= &followparent_recalc,
};

static struct clk core_sgx_fck = {
	.name		= "core_sgx_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_core_x2m7_ck,
	.recalc		= &followparent_recalc,
};

/* MPU DPLL */

static const struct clksel_rate div4_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 2, .val = 1, .flags = RATE_IN_443X },
	{ .div = 4, .val = 2, .flags = RATE_IN_443X },
	{ .div = 8, .val = 3, .flags = RATE_IN_443X },
	{ .div = 0 }
};

static const struct clksel mpu_dpll_hs_ck_clksel[] = {
	{ .parent = &core_x2_ck, .rates = div4_rates },
	{ .parent = NULL }
};

static struct clk mpu_dpll_hs_ck = {
	.name 		= "mpu_dpll_hs_ck",
	.ops            = &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &core_x2_ck,
	.clksel_reg     = OMAP4430_CM_BYPCLK_DPLL_MPU,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel		= mpu_dpll_hs_ck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct dpll_data dpll_mpu_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_MPU,
	.mult_mask	= OMAP4430_CM2_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_DPLL_DIV_MASK,
	.clk_bypass	= &mpu_dpll_hs_ck,
	.clk_ref	= &dpll_sys_ref_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_MPU,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_MPU,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_MPU,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_mpu_ck = {
	.name           = "dpll_mpu_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_sys_ref_ck,
	.dpll_data      = &dpll_mpu_dd,
	.round_rate     = &omap2_dpll_round_rate,
	.set_rate       = &omap4_noncore_dpll_set_rate,
	.recalc         = &omap4_dpll_recalc,
};

static const struct clksel dpll_mpu_m2_clksel[] = {
	{ .parent = &dpll_mpu_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_mpu_m2_ck = {
	.name		= "dpll_mpu_m2_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.clksel_reg	= OMAP4430_CM_DIV_M2_DPLL_MPU,
	.clksel_mask	= OMAP4430_DPLL_CLKOUT_DIV_MASK,
	.clksel		= dpll_mpu_m2_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk mpu_dpll_ck = {
	.name		= "mpu_dpll_ck",
	.ops		= &clkops_null,
	.parent         = &dpll_mpu_m2_ck,
	.recalc		= &followparent_recalc,
};

/* IVA DPLL */

static const struct clksel iva_dpll_hs_ck_clksel[] = {
	{ .parent = &core_x2_ck, .rates = div4_rates },
	{ .parent = NULL }
};

static struct clk iva_dpll_hs_ck = {
	.name 		= "iva_dpll_hs_ck",
	.ops            = &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &core_x2_ck,
	.clksel_reg     = OMAP4430_CM_BYPCLK_DPLL_IVA,
	.clksel_mask    = OMAP4430_CLKSEL_MASK,
	.clksel		= iva_dpll_hs_ck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct dpll_data dpll_iva_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_IVA,
	.mult_mask	= OMAP4430_CM2_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_DPLL_DIV_MASK,
	.clk_bypass	= &iva_dpll_hs_ck,
	.clk_ref	= &dpll_sys_ref_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_IVA,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_IVA,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_IVA,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_iva_ck = {
	.name           = "dpll_iva_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_sys_ref_ck,
	.dpll_data      = &dpll_iva_dd,
	.round_rate     = &omap2_dpll_round_rate,
	.set_rate       = &omap4_noncore_dpll_set_rate,
	.recalc         = &omap4_dpll_recalc,
};

static struct clk dpll_iva_x2_ck = {
	.name           = "dpll_iva_x2_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_iva_ck,
	.recalc         = &omap4_clkoutx2_recalc,
};

static const struct clksel dpll_iva_x2mx_clksel[] = {
	{ .parent = &dpll_iva_x2_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_iva_x2m4_ck = {
	.name		= "dpll_iva_x2m4_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M4_DPLL_IVA,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_iva_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dpll_iva_x2m5_ck = {
	.name		= "dpll_iva_x2m5_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M5_DPLL_IVA,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_iva_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk dsp_root_ck = {
	.name		= "dsp_root_ck",
	.ops		= &clkops_null,
	.parent         = &dpll_iva_x2m4_ck,
	.recalc		= &followparent_recalc,
};

static struct clk ivahd_ck = {
	.name		= "ivahd_ck",
	.ops		= &clkops_null,
	.parent         = &dpll_iva_x2m5_ck,
	.recalc		= &followparent_recalc,
};

#endif
