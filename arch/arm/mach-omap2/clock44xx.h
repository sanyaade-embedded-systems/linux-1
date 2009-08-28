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

static struct clk pad_cks = {
	.name		= "pad_cks",
	.ops		= &clkops_null,
	.rate		= 12000000,
	.flags		= RATE_FIXED,
};

static struct clk slimbus_ck = {
	.name		= "slimbus_ck",
	.ops		= &clkops_null,
	.rate		= 12000000,
	.flags		= RATE_FIXED,
};

static struct clk phy_clkout_ck = {
	.name		= "phy_clkout_ck",
	.ops		= &clkops_null,
	.rate		= 12000000,
	.flags		= RATE_FIXED,
};

static struct clk xclk_60m_otg_ck = {
	.name		= "xclk_60m_otg_ck",
	.ops		= &clkops_null,
	.rate		= 60000000,
	.flags		= RATE_FIXED,
};

static struct clk xclk_60m_hsp1_ck = {
	.name		= "xclk_60m_hsp1_ck",
	.ops		= &clkops_null,
	.rate		= 12000000,
	.flags		= RATE_FIXED,
};

static struct clk xclk_60m_hsp2_ck = {
	.name		= "xclk_60m_hsp2_ck",
	.ops		= &clkops_null,
	.rate		= 12000000,
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
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_13m_rates[] = {
	{ .div = 1, .val = 2, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_19_2m_rates[] = {
	{ .div = 1, .val = 3, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_26m_rates[] = {
	{ .div = 1, .val = 4, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_27m_rates[] = {
	{ .div = 1, .val = 5, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_clk_38_4m_rates[] = {
	{ .div = 1, .val = 6, .flags = RATE_IN_443X | DEFAULT_RATE },
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

static struct clk usb_dpll_hs_ck = {
	.name 		= "usb_dpll_hs_ck",
	.ops		= &clkops_null,
	.parent		= &core_dpll_hs_ck,
	.fixed_div	= 2,
	.recalc		= &omap2_fixed_divisor_recalc,
};

/* CM1_ABE clocks */

static const struct clksel_rate dpll_abe_x2_ck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 2, .val = 1, .flags = RATE_IN_443X },
	{ .div = 4, .val = 2, .flags = RATE_IN_443X },
	{ .div = 0 },
};

static const struct clksel abe_ck_clksel[] = {
	{ .parent = &dpll_abe_x2_ck, .rates = dpll_abe_x2_ck_rates },
	{ .parent = NULL },
};

static struct clk abe_ck = {
	.name		= "abe_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &dpll_abe_x2_ck,
	.clksel_reg	= OMAP4430_CM_CLKSEL_ABE,
	.clksel_mask	= OMAP4430_CLKSEL_OPP_MASK,
	.clksel		= abe_ck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static const struct clksel_rate abe_ck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 2, .val = 1, .flags = RATE_IN_443X },
	{ .div = 0 },
};

static const struct clksel aess_fck_clksel[] = {
	{ .parent = &abe_ck, .rates = abe_ck_rates },
	{ .parent = NULL },
};

static struct clk aess_fck = {
	.name		= "aess_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &abe_ck,
	.clksel_reg	= OMAP4430_CM1_ABE_AESS_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_AESS_FCLK_MASK,
	.clksel		= aess_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk abe_lp_ck = {
	.name 		= "abe_lp_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_abe_x2_ck,
	.fixed_div	= 16,
	.recalc	 = &omap2_fixed_divisor_recalc,
};

static struct clk abe_24m_fck = {
	.name 		= "abe_24m_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_abe_x2_ck,
	.fixed_div	= 16,
	.recalc	 = &omap2_fixed_divisor_recalc,
};

static struct clk abe_gpt5_fck = {
	.name		= "abe_gpt5_fck",
	.ops		= &clkops_omap2_dflt_wait,
	.parent		= &sys_ck,
	.init		= &omap2_init_clksel_parent,
	.enable_reg	= OMAP4430_CM1_ABE_TIMER5_CLKCTRL,
	.enable_bit	= OMAP4430_EN_GPT_SHIFT,
	.clksel_reg	= OMAP4430_CM1_ABE_TIMER5_CLKCTRL,
	.clksel_mask	= OMAP4430_GPTIMER_CLKSEL_MASK,
	.clksel		= omap443x_gpt_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk abe_gpt6_fck = {
	.name		= "abe_gpt6_fck",
	.ops		= &clkops_omap2_dflt_wait,
	.parent		= &sys_ck,
	.init		= &omap2_init_clksel_parent,
	.enable_reg	= OMAP4430_CM1_ABE_TIMER6_CLKCTRL,
	.enable_bit	= OMAP4430_EN_GPT_SHIFT,
	.clksel_reg	= OMAP4430_CM1_ABE_TIMER6_CLKCTRL,
	.clksel_mask	= OMAP4430_GPTIMER_CLKSEL_MASK,
	.clksel		= omap443x_gpt_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk abe_gpt7_fck = {
	.name		= "abe_gpt7_fck",
	.ops		= &clkops_omap2_dflt_wait,
	.parent		= &sys_ck,
	.init		= &omap2_init_clksel_parent,
	.enable_reg	= OMAP4430_CM1_ABE_TIMER7_CLKCTRL,
	.enable_bit	= OMAP4430_EN_GPT_SHIFT,
	.clksel_reg	= OMAP4430_CM1_ABE_TIMER7_CLKCTRL,
	.clksel_mask	= OMAP4430_GPTIMER_CLKSEL_MASK,
	.clksel		= omap443x_gpt_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk abe_gpt8_fck = {
	.name		= "abe_gpt8_fck",
	.ops		= &clkops_omap2_dflt_wait,
	.parent		= &sys_ck,
	.init		= &omap2_init_clksel_parent,
	.enable_reg	= OMAP4430_CM1_ABE_TIMER8_CLKCTRL,
	.enable_bit	= OMAP4430_EN_GPT_SHIFT,
	.clksel_reg	= OMAP4430_CM1_ABE_TIMER8_CLKCTRL,
	.clksel_mask	= OMAP4430_GPTIMER_CLKSEL_MASK,
	.clksel		= omap443x_gpt_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk per_24m_fck;

static const struct clksel_rate abe_24m_fck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate sys_ck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate per_24m_fck_rates[] = {
	{ .div = 1, .val = 2, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel mcbsp_int_fck_clksel[] = {
	{ .parent = &abe_24m_fck, .rates = abe_24m_fck_rates },
	{ .parent = &sys_ck, .rates = sys_ck_rates },
	{ .parent = &per_24m_fck, .rates = per_24m_fck_rates },
	{ .parent = NULL },
};

static const struct clksel mcasp_int_fck_clksel[] = {
	{ .parent = &abe_24m_fck, .rates = abe_24m_fck_rates },
	{ .parent = &sys_ck, .rates = sys_ck_rates },
	{ .parent = &per_24m_fck, .rates = per_24m_fck_rates },
	{ .parent = NULL },
};

static const struct clksel dmic_abe_int_fck_clksel[] = {
	{ .parent = &abe_24m_fck, .rates = abe_24m_fck_rates },
	{ .parent = &sys_ck, .rates = sys_ck_rates },
	{ .parent = &per_24m_fck, .rates = per_24m_fck_rates },
	{ .parent = NULL },
};

static struct clk mcbsp1_int_fck = {
	.name		= "mcbsp1_int_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &abe_24m_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCBSP1_CLKCTRL,
	.clksel_mask	= OMAP4430_ABE_CLKSEL_INTERNAL_SOURCE,
	.clksel		= mcbsp_int_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk mcbsp2_int_fck = {
	.name		= "mcbsp2_int_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &abe_24m_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCBSP2_CLKCTRL,
	.clksel_mask	= OMAP4430_ABE_CLKSEL_INTERNAL_SOURCE,
	.clksel		= mcbsp_int_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk mcbsp3_int_fck = {
	.name		= "mcbsp3_int_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &abe_24m_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCBSP3_CLKCTRL,
	.clksel_mask	= OMAP4430_ABE_CLKSEL_INTERNAL_SOURCE,
	.clksel		= mcbsp_int_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk mcasp1_int_fck = {
	.name		= "mcasp1_int_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &abe_24m_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCASP_CLKCTRL,
	.clksel_mask	= OMAP4430_ABE_CLKSEL_INTERNAL_SOURCE,
	.clksel		= mcasp_int_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk dmic_abe_int_fck = {
	.name		= "dmic_abe_int_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &abe_24m_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_DMIC_CLKCTRL,
	.clksel_mask	= OMAP4430_ABE_CLKSEL_INTERNAL_SOURCE,
	.clksel		= dmic_abe_int_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static const struct clksel_rate int_ck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate pad_cks_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate slimbus_ck_rates[] = {
	{ .div = 1, .val = 2, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel mcbsp1_fck_clksel[] = {
	{ .parent = &mcbsp1_int_fck, .rates = int_ck_rates },
	{ .parent = &pad_cks, .rates = pad_cks_rates },
	{ .parent = &slimbus_ck, .rates = slimbus_ck_rates },
	{ .parent = NULL },
};

static const struct clksel mcbsp2_fck_clksel[] = {
	{ .parent = &mcbsp2_int_fck, .rates = int_ck_rates },
	{ .parent = &pad_cks, .rates = pad_cks_rates },
	{ .parent = &slimbus_ck, .rates = slimbus_ck_rates },
	{ .parent = NULL },
};
static const struct clksel mcbsp3_fck_clksel[] = {
	{ .parent = &mcbsp3_int_fck, .rates = int_ck_rates },
	{ .parent = &pad_cks, .rates = pad_cks_rates },
	{ .parent = &slimbus_ck, .rates = slimbus_ck_rates },
	{ .parent = NULL },
};
static const struct clksel mcasp1_fck_clksel[] = {
	{ .parent = &mcasp1_int_fck, .rates = int_ck_rates },
	{ .parent = &pad_cks, .rates = pad_cks_rates },
	{ .parent = &slimbus_ck, .rates = slimbus_ck_rates },
	{ .parent = NULL },
};
static const struct clksel dmic_abe_fck_clksel[] = {
	{ .parent = &dmic_abe_int_fck, .rates = int_ck_rates },
	{ .parent = &pad_cks, .rates = pad_cks_rates },
	{ .parent = &slimbus_ck, .rates = slimbus_ck_rates },
	{ .parent = NULL },
};

static struct clk mcbsp1_fck = {
	.name		= "mcbsp1_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &mcbsp1_int_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCBSP1_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_SOURCE,
	.clksel		= mcbsp1_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk mcbsp2_fck = {
	.name		= "mcbsp2_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &mcbsp2_int_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCBSP2_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_SOURCE,
	.clksel		= mcbsp2_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk mcbsp3_fck = {
	.name		= "mcbsp3_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &mcbsp3_int_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCBSP3_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_SOURCE,
	.clksel		= mcbsp3_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk mcasp1_fck = {
	.name		= "mcasp1_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &mcasp1_int_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_MCASP_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_SOURCE,
	.clksel		= mcasp1_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk dmic_abe_fck = {
	.name		= "dmic_abe_fck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.parent		= &dmic_abe_int_fck,
	.clksel_reg	= OMAP4430_CM1_ABE_DMIC_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_SOURCE,
	.clksel		= dmic_abe_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
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

/* UNIPRO DPLL */

static struct dpll_data dpll_unipro_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_UNIPRO,
	.mult_mask	= OMAP4430_CM2_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_DPLL_DIV_MASK,
	.clk_ref	= &dpll_sys_ref_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_UNIPRO,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_UNIPRO,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_UNIPRO,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_unipro_ck = {
	.name           = "dpll_unipro_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_sys_ref_ck,
	.dpll_data      = &dpll_unipro_dd,
	.round_rate     = &omap2_dpll_round_rate,
	.set_rate       = &omap4_noncore_dpll_set_rate,
	.recalc         = &omap4_dpll_recalc,
};

static struct clk dpll_unipro_x2_ck = {
	.name           = "dpll_unipro_x2_ck",
	.ops            = &clkops_null,
	.parent         = &dpll_unipro_ck,
	.recalc         = &omap4_clkoutx2_recalc,
};

static const struct clksel dpll_unipro_x2mx_clksel[] = {
	{ .parent = &dpll_unipro_x2_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_unipro_x2m2_ck = {
	.name		= "dpll_unipro_x2m2_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_DIV_M2_DPLL_UNIPRO,
	.clksel_mask    = OMAP4430_DPLL_CLKOUTHIF_DIV_MASK,
	.clksel         = dpll_unipro_x2mx_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk unipro1_phy_fck = {
	.name		= "unipro1_phy_fck",
	.ops		= &clkops_null,
	.parent         = &dpll_unipro_x2m2_ck,
	.recalc		= &followparent_recalc,
};

/* USB DPLL */

static struct dpll_data dpll_usb_dd = {
	.mult_div1_reg	= OMAP4430_CM_CLKSEL_DPLL_USB,
	.mult_mask	= OMAP4430_CM2_USB_DPLL_MULT_MASK,
	.div1_mask	= OMAP4430_CM2_USB_DPLL_DIV_MASK,
	.clk_bypass	= &usb_dpll_hs_ck,
	.clk_ref	= &dpll_sys_ref_ck,
	.control_reg	= OMAP4430_CM_CLKMODE_DPLL_USB,
	.enable_mask	= OMAP4430_DPLL_EN_MASK,
	.modes		= (1 << DPLL_LOW_POWER_BYPASS) | (1 << DPLL_LOCKED),
	.autoidle_reg	= OMAP4430_CM_AUTOIDLE_DPLL_USB,
	.autoidle_mask	= OMAP4430_AUTO_DPLL_MODE_MASK,
	.idlest_reg	= OMAP4430_CM_IDLEST_DPLL_USB,
	.idlest_mask	= OMAP4430_ST_DPLL_CLK_MASK,
};

static struct clk dpll_usb_ck = {
	.name		= "dpll_usb_ck",
	.ops		= &clkops_null,
	.parent		= &dpll_sys_ref_ck,
	.dpll_data	= &dpll_usb_dd,
	.round_rate	= &omap2_dpll_round_rate,
	.set_rate	= &omap4_noncore_dpll_set_rate,
	.recalc		= &omap4_dpll_recalc,
};

static const struct clksel dpll_usb_m2_clksel[] = {
	{ .parent = &dpll_usb_ck, .rates = div_mx_dpll_rates },
	{ .parent = NULL }
};

static struct clk dpll_usb_m2_ck = {
	.name		= "dpll_usb_m2_ck",
	.ops		= &clkops_null,
	.init		= &omap2_init_clksel_parent,
	.clksel_reg	= OMAP4430_CM_DIV_M2_DPLL_USB,
	.clksel_mask	= OMAP4430_USB_DPLL_CLKOUT_DIV_MASK,
	.clksel		= dpll_usb_m2_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk init_480m_fck = {
	.name		= "init_480m_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_usb_m2_ck,
	.recalc		= &followparent_recalc,
};

static struct clk dpll_usb_clkdcoldo = {
	.name		= "dpll_usb_clkdcoldo",
	.ops		= &clkops_null,
	.parent		= &dpll_usb_ck,
	.recalc		= &followparent_recalc,
};

static struct clk init_960m_fck = {
	.name		= "init_960m_fck",
	.ops		= &clkops_null,
	.parent		= &dpll_usb_clkdcoldo,
	.recalc		= &followparent_recalc,
};

/* CM1_USB nodes */

static const struct clksel_rate init_480m_fck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X },
	{ .div = 8, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE},
	{ .div = 0 }
};

static const struct clksel init_60m_fck_clksel[] = {
	{ .parent = &init_480m_fck, .rates = init_480m_fck_rates },
	{ .parent = NULL }
};

static struct clk init_60m_fck = {
	.name		= "init_60m_fck",
	.ops		= &clkops_null,
	.parent	 	= &init_480m_fck,
	.clksel_reg	= OMAP4430_CM_CLKSEL_USB_60MHZ,
	.clksel_mask	= OMAP4430_USB_60MHZ_CLKSEL_MASK,
	.clksel	 	= init_60m_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static struct clk utmi_p3_fck = {
	.name		= "utmi_p3_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk init_60m_p1_fck = {
	.name		= "init_60m_p1_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk init_60m_p2_fck = {
	.name		= "init_60m_p2_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk utmi_root_gfck = {
	.name		= "utmi_root_gfck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk tll_ch0_fck = {
	.name		= "tll_ch0_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk tll_ch1_fck = {
	.name		= "tll_ch1_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk tll_ch2_fck = {
	.name		= "tll_ch2_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk hsic_p1_480m_fck = {
	.name		= "hsic_p1_480m_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static struct clk hsic_p2_480m_fck = {
	.name		= "hsic_p2_480m_fck",
	.ops		= &clkops_null,
	.parent		= &init_480m_fck,
	.recalc		= &followparent_recalc,
};

static const struct clksel_rate phy_clkout_ck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate xclk_60m_otg_ck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel otg_60m_fck_clksel[] = {
	{ .parent = &phy_clkout_ck, .rates = phy_clkout_ck_rates },
	{ .parent = &xclk_60m_otg_ck, .rates = xclk_60m_otg_ck_rates },
	{ .parent = NULL }
};

static struct clk otg_60m_fck = {
	.name		= "otg_60m_fck",
	.ops		= &clkops_null,
	.parent	 	= &phy_clkout_ck,
	.clksel_reg	= OMAP4430_CM_L3INIT_USB_OTG_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_60M_MASK,
	.clksel	 	= otg_60m_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static const struct clksel_rate init_60m_p1_fck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate xclk_60m_hsp1_ck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel utmi_p1_fck_clksel[] = {
	{ .parent = &init_60m_p1_fck, .rates = init_60m_p1_fck_rates },
	{ .parent = &xclk_60m_hsp1_ck, .rates = xclk_60m_hsp1_ck_rates },
	{ .parent = NULL }
};

static struct clk utmi_p1_fck = {
	.name		= "utmi_p1_fck",
	.ops		= &clkops_null,
	.parent	 	= &init_60m_p1_fck,
	.clksel_reg	= OMAP4430_CM_L3INIT_USB_HOST_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_UTMI_P1_MASK,
	.clksel	 	= utmi_p1_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

static const struct clksel_rate init_60m_p2_fck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate xclk_60m_hsp2_ck_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel utmi_p2_fck_clksel[] = {
	{ .parent = &init_60m_p2_fck, .rates = init_60m_p2_fck_rates },
	{ .parent = &xclk_60m_hsp2_ck, .rates = xclk_60m_hsp2_ck_rates },
	{ .parent = NULL }
};

static struct clk utmi_p2_fck = {
	.name		= "utmi_p2_fck",
	.ops		= &clkops_null,
	.parent	 	= &init_60m_p2_fck,
	.clksel_reg	= OMAP4430_CM_L3INIT_USB_HOST_CLKCTRL,
	.clksel_mask	= OMAP4430_CLKSEL_UTMI_P2_MASK,
	.clksel	 	= utmi_p2_fck_clksel,
	.recalc		= &omap2_clksel_recalc,
};

/* CM1 nodes */

static const struct clksel core_ck_clksel[] = {
	{ .parent = &core_x2_ck, .rates = div2_rates },
	{ .parent = NULL }
};

static struct clk core_ck = {
	.name		= "core_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_CLKSEL_CORE,
	.clksel_mask    = OMAP4430_CLKSEL_CORE_MASK,
	.clksel         = core_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel l3_ick_clksel[] = {
	{ .parent = &core_ck, .rates = div2_rates },
	{ .parent = NULL }
};

static struct clk l3_ick = {
	.name		= "l3_ick",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_CLKSEL_CORE,
	.clksel_mask    = OMAP4430_CM_CLKSEL_CORE_RESTORE_CLKSEL_L3_MASK,
	.clksel         = l3_ick_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel l4_root_ck_clksel[] = {
	{ .parent = &core_ck, .rates = div2_rates },
	{ .parent = NULL }
};

static struct clk l4_root_ck = {
	.name		= "l4_root_ck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_CLKSEL_CORE,
	.clksel_mask    = OMAP4430_CLKSEL_L4_MASK,
	.clksel         = l4_root_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static struct clk per_24m_fck = {
	.name 		= "per_24m_fck",
	.ops		= &clkops_null,
	.parent		= &omap_96m_alwon_ck,
	.fixed_div	= 4,
	.recalc		= &omap2_fixed_divisor_recalc,
};

/* CM2 nodes */

static const struct clksel_rate div124_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 2, .val = 1, .flags = RATE_IN_443X },
	{ .div = 4, .val = 2, .flags = RATE_IN_443X },
	{ .div = 0 }
};

static const struct clksel fdif_fck_clksel[] = {
	{ .parent = &omap_128m_fck, .rates = div124_rates },
	{ .parent = NULL }
};

static struct clk fdif_fck = {
	.name		= "fdif_fck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_CAM_FDIF_CLKCTRL,
	.clksel_mask    = OMAP4430_CLKSEL_FCLK_MASK,
	.clksel         = fdif_fck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel hsi_fck_clksel[] = {
	{ .parent = &omap_192m_fck, .rates = div124_rates },
	{ .parent = NULL }
};

static struct clk hsi_fck = {
	.name		= "hsi_fck",
	.ops		= &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.clksel_reg     = OMAP4430_CM_L3INIT_HSI_CLKCTRL,
	.clksel_mask    = OMAP4430_HSI_CLKSEL_MASK,
	.clksel         = hsi_fck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

static const struct clksel_rate core_ck_rates[] = {
	{ .div = 1, .val = 0, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel_rate per_mpu_m3_rates[] = {
	{ .div = 1, .val = 1, .flags = RATE_IN_443X | DEFAULT_RATE },
	{ .div = 0 }
};

static const struct clksel mpu_m3_iss_ck_clksel[] = {
	{ .parent = &core_ck, .rates = core_ck_rates },
	{ .parent = &per_mpu_m3, .rates = per_mpu_m3_rates },
	{ .parent = NULL }
};

static struct clk mpu_m3_iss_ck = {
	.name           = "mpu_m3_iss_ck",
	.ops            = &clkops_null,
	.init           = &omap2_init_clksel_parent,
	.parent         = &core_ck,
	.clksel_reg     = OMAP4430_CM_CLKSEL_DUCATI_ISS_ROOT,
	.clksel_mask    = OMAP4430_MPU_M3_ISS_ROOT_CLKSEL_MASK,
	.clksel         = mpu_m3_iss_ck_clksel,
	.recalc         = &omap2_clksel_recalc,
};

#endif
