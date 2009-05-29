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
#endif
