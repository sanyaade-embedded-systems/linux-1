/*
 * OMAP3-specific clock framework functions
 *
 * Copyright (C) 2007-2008 Texas Instruments, Inc.
 * Copyright (C) 2007-2009 Nokia Corporation
 *
 * Written by Paul Walmsley
 * Testing and integration fixes by Jouni Högander
 *
 * Parts of this code are based on code written by
 * Richard Woodruff, Tony Lindgren, Tuukka Tikkanen, Karthik Dasu
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
#include "prm-regbits-34xx.h"
#include "cm.h"
#include "cm-regbits-34xx.h"

static const struct clkops clkops_noncore_dpll_ops;

static void omap3430es2_clk_ssi_find_idlest(struct clk *clk,
					    void __iomem **idlest_reg,
					    u8 *idlest_bit);
static void omap3430es2_clk_hsotgusb_find_idlest(struct clk *clk,
					    void __iomem **idlest_reg,
					    u8 *idlest_bit);
static void omap3430es2_clk_dss_usbhost_find_idlest(struct clk *clk,
						    void __iomem **idlest_reg,
						    u8 *idlest_bit);

static const struct clkops clkops_omap3430es2_ssi_wait = {
	.enable		= omap2_dflt_clk_enable,
	.disable	= omap2_dflt_clk_disable,
	.find_idlest	= omap3430es2_clk_ssi_find_idlest,
	.find_companion = omap2_clk_dflt_find_companion,
};

static const struct clkops clkops_omap3430es2_hsotgusb_wait = {
	.enable		= omap2_dflt_clk_enable,
	.disable	= omap2_dflt_clk_disable,
	.find_idlest	= omap3430es2_clk_hsotgusb_find_idlest,
	.find_companion = omap2_clk_dflt_find_companion,
};

static const struct clkops clkops_omap3430es2_dss_usbhost_wait = {
	.enable		= omap2_dflt_clk_enable,
	.disable	= omap2_dflt_clk_disable,
	.find_idlest	= omap3430es2_clk_dss_usbhost_find_idlest,
	.find_companion = omap2_clk_dflt_find_companion,
};

#include "clock34xx.h"

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

#define CK_343X		(1 << 0)
#define CK_3430ES1	(1 << 1)
#define CK_3430ES2	(1 << 2)

static struct omap_clk omap34xx_clks[] = {
	CLK(NULL,	"omap_32k_fck",	&omap_32k_fck,	CK_343X),
	CLK(NULL,	"virt_12m_ck",	&virt_12m_ck,	CK_343X),
	CLK(NULL,	"virt_13m_ck",	&virt_13m_ck,	CK_343X),
	CLK(NULL,	"virt_16_8m_ck", &virt_16_8m_ck, CK_3430ES2),
	CLK(NULL,	"virt_19_2m_ck", &virt_19_2m_ck, CK_343X),
	CLK(NULL,	"virt_26m_ck",	&virt_26m_ck,	CK_343X),
	CLK(NULL,	"virt_38_4m_ck", &virt_38_4m_ck, CK_343X),
	CLK(NULL,	"osc_sys_ck",	&osc_sys_ck,	CK_343X),
	CLK(NULL,	"sys_ck",	&sys_ck,	CK_343X),
	CLK(NULL,	"sys_altclk",	&sys_altclk,	CK_343X),
	CLK(NULL,	"mcbsp_clks",	&mcbsp_clks,	CK_343X),
	CLK(NULL,	"sys_clkout1",	&sys_clkout1,	CK_343X),
	CLK(NULL,	"dpll1_ck",	&dpll1_ck,	CK_343X),
	CLK(NULL,	"dpll1_x2_ck",	&dpll1_x2_ck,	CK_343X),
	CLK(NULL,	"dpll1_x2m2_ck", &dpll1_x2m2_ck, CK_343X),
	CLK(NULL,	"dpll2_ck",	&dpll2_ck,	CK_343X),
	CLK(NULL,	"dpll2_m2_ck",	&dpll2_m2_ck,	CK_343X),
	CLK(NULL,	"dpll3_ck",	&dpll3_ck,	CK_343X),
	CLK(NULL,	"core_ck",	&core_ck,	CK_343X),
	CLK(NULL,	"dpll3_x2_ck",	&dpll3_x2_ck,	CK_343X),
	CLK(NULL,	"dpll3_m2_ck",	&dpll3_m2_ck,	CK_343X),
	CLK(NULL,	"dpll3_m2x2_ck", &dpll3_m2x2_ck, CK_343X),
	CLK(NULL,	"dpll3_m3_ck",	&dpll3_m3_ck,	CK_343X),
	CLK(NULL,	"dpll3_m3x2_ck", &dpll3_m3x2_ck, CK_343X),
	CLK(NULL,	"emu_core_alwon_ck", &emu_core_alwon_ck, CK_343X),
	CLK(NULL,	"dpll4_ck",	&dpll4_ck,	CK_343X),
	CLK(NULL,	"dpll4_x2_ck",	&dpll4_x2_ck,	CK_343X),
	CLK(NULL,	"omap_96m_alwon_fck", &omap_96m_alwon_fck, CK_343X),
	CLK(NULL,	"omap_96m_fck",	&omap_96m_fck,	CK_343X),
	CLK(NULL,	"cm_96m_fck",	&cm_96m_fck,	CK_343X),
	CLK(NULL,	"omap_54m_fck",	&omap_54m_fck,	CK_343X),
	CLK(NULL,	"omap_48m_fck",	&omap_48m_fck,	CK_343X),
	CLK(NULL,	"omap_12m_fck",	&omap_12m_fck,	CK_343X),
	CLK(NULL,	"dpll4_m2_ck",	&dpll4_m2_ck,	CK_343X),
	CLK(NULL,	"dpll4_m2x2_ck", &dpll4_m2x2_ck, CK_343X),
	CLK(NULL,	"dpll4_m3_ck",	&dpll4_m3_ck,	CK_343X),
	CLK(NULL,	"dpll4_m3x2_ck", &dpll4_m3x2_ck, CK_343X),
	CLK(NULL,	"dpll4_m4_ck",	&dpll4_m4_ck,	CK_343X),
	CLK(NULL,	"dpll4_m4x2_ck", &dpll4_m4x2_ck, CK_343X),
	CLK(NULL,	"dpll4_m5_ck",	&dpll4_m5_ck,	CK_343X),
	CLK(NULL,	"dpll4_m5x2_ck", &dpll4_m5x2_ck, CK_343X),
	CLK(NULL,	"dpll4_m6_ck",	&dpll4_m6_ck,	CK_343X),
	CLK(NULL,	"dpll4_m6x2_ck", &dpll4_m6x2_ck, CK_343X),
	CLK(NULL,	"emu_per_alwon_ck", &emu_per_alwon_ck, CK_343X),
	CLK(NULL,	"dpll5_ck",	&dpll5_ck,	CK_3430ES2),
	CLK(NULL,	"dpll5_m2_ck",	&dpll5_m2_ck,	CK_3430ES2),
	CLK(NULL,	"clkout2_src_ck", &clkout2_src_ck, CK_343X),
	CLK(NULL,	"sys_clkout2",	&sys_clkout2,	CK_343X),
	CLK(NULL,	"corex2_fck",	&corex2_fck,	CK_343X),
	CLK(NULL,	"dpll1_fck",	&dpll1_fck,	CK_343X),
	CLK(NULL,	"mpu_ck",	&mpu_ck,	CK_343X),
	CLK(NULL,	"arm_fck",	&arm_fck,	CK_343X),
	CLK(NULL,	"emu_mpu_alwon_ck", &emu_mpu_alwon_ck, CK_343X),
	CLK(NULL,	"dpll2_fck",	&dpll2_fck,	CK_343X),
	CLK(NULL,	"iva2_ck",	&iva2_ck,	CK_343X),
	CLK(NULL,	"l3_ick",	&l3_ick,	CK_343X),
	CLK(NULL,	"l4_ick",	&l4_ick,	CK_343X),
	CLK(NULL,	"rm_ick",	&rm_ick,	CK_343X),
	CLK(NULL,	"gfx_l3_ck",	&gfx_l3_ck,	CK_3430ES1),
	CLK(NULL,	"gfx_l3_fck",	&gfx_l3_fck,	CK_3430ES1),
	CLK(NULL,	"gfx_l3_ick",	&gfx_l3_ick,	CK_3430ES1),
	CLK(NULL,	"gfx_cg1_ck",	&gfx_cg1_ck,	CK_3430ES1),
	CLK(NULL,	"gfx_cg2_ck",	&gfx_cg2_ck,	CK_3430ES1),
	CLK(NULL,	"sgx_fck",	&sgx_fck,	CK_3430ES2),
	CLK(NULL,	"sgx_ick",	&sgx_ick,	CK_3430ES2),
	CLK(NULL,	"d2d_26m_fck",	&d2d_26m_fck,	CK_3430ES1),
	CLK(NULL,	"modem_fck",	&modem_fck,	CK_343X),
	CLK(NULL,	"sad2d_ick",	&sad2d_ick,	CK_343X),
	CLK(NULL,	"mad2d_ick",	&mad2d_ick,	CK_343X),
	CLK(NULL,	"gpt10_fck",	&gpt10_fck,	CK_343X),
	CLK(NULL,	"gpt11_fck",	&gpt11_fck,	CK_343X),
	CLK(NULL,	"cpefuse_fck",	&cpefuse_fck,	CK_3430ES2),
	CLK(NULL,	"ts_fck",	&ts_fck,	CK_3430ES2),
	CLK(NULL,	"usbtll_fck",	&usbtll_fck,	CK_3430ES2),
	CLK(NULL,	"core_96m_fck",	&core_96m_fck,	CK_343X),
	CLK("mmci-omap-hs.2",	"fck",	&mmchs3_fck,	CK_3430ES2),
	CLK("mmci-omap-hs.1",	"fck",	&mmchs2_fck,	CK_343X),
	CLK(NULL,	"mspro_fck",	&mspro_fck,	CK_343X),
	CLK("mmci-omap-hs.0",	"fck",	&mmchs1_fck,	CK_343X),
	CLK("i2c_omap.3", "fck",	&i2c3_fck,	CK_343X),
	CLK("i2c_omap.2", "fck",	&i2c2_fck,	CK_343X),
	CLK("i2c_omap.1", "fck",	&i2c1_fck,	CK_343X),
	CLK("omap-mcbsp.5", "fck",	&mcbsp5_fck,	CK_343X),
	CLK("omap-mcbsp.1", "fck",	&mcbsp1_fck,	CK_343X),
	CLK(NULL,	"core_48m_fck",	&core_48m_fck,	CK_343X),
	CLK("omap2_mcspi.4", "fck",	&mcspi4_fck,	CK_343X),
	CLK("omap2_mcspi.3", "fck",	&mcspi3_fck,	CK_343X),
	CLK("omap2_mcspi.2", "fck",	&mcspi2_fck,	CK_343X),
	CLK("omap2_mcspi.1", "fck",	&mcspi1_fck,	CK_343X),
	CLK(NULL,	"uart2_fck",	&uart2_fck,	CK_343X),
	CLK(NULL,	"uart1_fck",	&uart1_fck,	CK_343X),
	CLK(NULL,	"fshostusb_fck", &fshostusb_fck, CK_3430ES1),
	CLK(NULL,	"core_12m_fck",	&core_12m_fck,	CK_343X),
	CLK("omap_hdq.0", "fck",	&hdq_fck,	CK_343X),
	CLK(NULL,	"ssi_ssr_fck",	&ssi_ssr_fck_3430es1,	CK_3430ES1),
	CLK(NULL,	"ssi_ssr_fck",	&ssi_ssr_fck_3430es2,	CK_3430ES2),
	CLK(NULL,	"ssi_sst_fck",	&ssi_sst_fck_3430es1,	CK_3430ES1),
	CLK(NULL,	"ssi_sst_fck",	&ssi_sst_fck_3430es2,	CK_3430ES2),
	CLK(NULL,	"core_l3_ick",	&core_l3_ick,	CK_343X),
	CLK("musb_hdrc",	"ick",	&hsotgusb_ick_3430es1,	CK_3430ES1),
	CLK("musb_hdrc",	"ick",	&hsotgusb_ick_3430es2,	CK_3430ES2),
	CLK(NULL,	"sdrc_ick",	&sdrc_ick,	CK_343X),
	CLK(NULL,	"gpmc_fck",	&gpmc_fck,	CK_343X),
	CLK(NULL,	"security_l3_ick", &security_l3_ick, CK_343X),
	CLK(NULL,	"pka_ick",	&pka_ick,	CK_343X),
	CLK(NULL,	"core_l4_ick",	&core_l4_ick,	CK_343X),
	CLK(NULL,	"usbtll_ick",	&usbtll_ick,	CK_3430ES2),
	CLK("mmci-omap-hs.2",	"ick",	&mmchs3_ick,	CK_3430ES2),
	CLK(NULL,	"icr_ick",	&icr_ick,	CK_343X),
	CLK(NULL,	"aes2_ick",	&aes2_ick,	CK_343X),
	CLK(NULL,	"sha12_ick",	&sha12_ick,	CK_343X),
	CLK(NULL,	"des2_ick",	&des2_ick,	CK_343X),
	CLK("mmci-omap-hs.1",	"ick",	&mmchs2_ick,	CK_343X),
	CLK("mmci-omap-hs.0",	"ick",	&mmchs1_ick,	CK_343X),
	CLK(NULL,	"mspro_ick",	&mspro_ick,	CK_343X),
	CLK("omap_hdq.0", "ick",	&hdq_ick,	CK_343X),
	CLK("omap2_mcspi.4", "ick",	&mcspi4_ick,	CK_343X),
	CLK("omap2_mcspi.3", "ick",	&mcspi3_ick,	CK_343X),
	CLK("omap2_mcspi.2", "ick",	&mcspi2_ick,	CK_343X),
	CLK("omap2_mcspi.1", "ick",	&mcspi1_ick,	CK_343X),
	CLK("i2c_omap.3", "ick",	&i2c3_ick,	CK_343X),
	CLK("i2c_omap.2", "ick",	&i2c2_ick,	CK_343X),
	CLK("i2c_omap.1", "ick",	&i2c1_ick,	CK_343X),
	CLK(NULL,	"uart2_ick",	&uart2_ick,	CK_343X),
	CLK(NULL,	"uart1_ick",	&uart1_ick,	CK_343X),
	CLK(NULL,	"gpt11_ick",	&gpt11_ick,	CK_343X),
	CLK(NULL,	"gpt10_ick",	&gpt10_ick,	CK_343X),
	CLK("omap-mcbsp.5", "ick",	&mcbsp5_ick,	CK_343X),
	CLK("omap-mcbsp.1", "ick",	&mcbsp1_ick,	CK_343X),
	CLK(NULL,	"fac_ick",	&fac_ick,	CK_3430ES1),
	CLK(NULL,	"mailboxes_ick", &mailboxes_ick, CK_343X),
	CLK(NULL,	"omapctrl_ick",	&omapctrl_ick,	CK_343X),
	CLK(NULL,	"ssi_l4_ick",	&ssi_l4_ick,	CK_343X),
	CLK(NULL,	"ssi_ick",	&ssi_ick_3430es1,	CK_3430ES1),
	CLK(NULL,	"ssi_ick",	&ssi_ick_3430es2,	CK_3430ES2),
	CLK(NULL,	"usb_l4_ick",	&usb_l4_ick,	CK_3430ES1),
	CLK(NULL,	"security_l4_ick2", &security_l4_ick2, CK_343X),
	CLK(NULL,	"aes1_ick",	&aes1_ick,	CK_343X),
	CLK("omap_rng",	"ick",		&rng_ick,	CK_343X),
	CLK(NULL,	"sha11_ick",	&sha11_ick,	CK_343X),
	CLK(NULL,	"des1_ick",	&des1_ick,	CK_343X),
	CLK("omapfb",	"dss1_fck",	&dss1_alwon_fck_3430es1, CK_3430ES1),
	CLK("omapfb",	"dss1_fck",	&dss1_alwon_fck_3430es2, CK_3430ES2),
	CLK("omapfb",	"tv_fck",	&dss_tv_fck,	CK_343X),
	CLK("omapfb",	"video_fck",	&dss_96m_fck,	CK_343X),
	CLK("omapfb",	"dss2_fck",	&dss2_alwon_fck, CK_343X),
	CLK("omapfb",	"ick",		&dss_ick_3430es1,	CK_3430ES1),
	CLK("omapfb",	"ick",		&dss_ick_3430es2,	CK_3430ES2),
	CLK(NULL,	"cam_mclk",	&cam_mclk,	CK_343X),
	CLK(NULL,	"cam_ick",	&cam_ick,	CK_343X),
	CLK(NULL,	"csi2_96m_fck",	&csi2_96m_fck,	CK_343X),
	CLK(NULL,	"usbhost_120m_fck", &usbhost_120m_fck, CK_3430ES2),
	CLK(NULL,	"usbhost_48m_fck", &usbhost_48m_fck, CK_3430ES2),
	CLK(NULL,	"usbhost_ick",	&usbhost_ick,	CK_3430ES2),
	CLK(NULL,	"usim_fck",	&usim_fck,	CK_3430ES2),
	CLK(NULL,	"gpt1_fck",	&gpt1_fck,	CK_343X),
	CLK(NULL,	"wkup_32k_fck",	&wkup_32k_fck,	CK_343X),
	CLK(NULL,	"gpio1_dbck",	&gpio1_dbck,	CK_343X),
	CLK("omap_wdt",	"fck",		&wdt2_fck,	CK_343X),
	CLK(NULL,	"wkup_l4_ick",	&wkup_l4_ick,	CK_343X),
	CLK(NULL,	"usim_ick",	&usim_ick,	CK_3430ES2),
	CLK("omap_wdt",	"ick",		&wdt2_ick,	CK_343X),
	CLK(NULL,	"wdt1_ick",	&wdt1_ick,	CK_343X),
	CLK(NULL,	"gpio1_ick",	&gpio1_ick,	CK_343X),
	CLK(NULL,	"omap_32ksync_ick", &omap_32ksync_ick, CK_343X),
	CLK(NULL,	"gpt12_ick",	&gpt12_ick,	CK_343X),
	CLK(NULL,	"gpt1_ick",	&gpt1_ick,	CK_343X),
	CLK(NULL,	"per_96m_fck",	&per_96m_fck,	CK_343X),
	CLK(NULL,	"per_48m_fck",	&per_48m_fck,	CK_343X),
	CLK(NULL,	"uart3_fck",	&uart3_fck,	CK_343X),
	CLK(NULL,	"gpt2_fck",	&gpt2_fck,	CK_343X),
	CLK(NULL,	"gpt3_fck",	&gpt3_fck,	CK_343X),
	CLK(NULL,	"gpt4_fck",	&gpt4_fck,	CK_343X),
	CLK(NULL,	"gpt5_fck",	&gpt5_fck,	CK_343X),
	CLK(NULL,	"gpt6_fck",	&gpt6_fck,	CK_343X),
	CLK(NULL,	"gpt7_fck",	&gpt7_fck,	CK_343X),
	CLK(NULL,	"gpt8_fck",	&gpt8_fck,	CK_343X),
	CLK(NULL,	"gpt9_fck",	&gpt9_fck,	CK_343X),
	CLK(NULL,	"per_32k_alwon_fck", &per_32k_alwon_fck, CK_343X),
	CLK(NULL,	"gpio6_dbck",	&gpio6_dbck,	CK_343X),
	CLK(NULL,	"gpio5_dbck",	&gpio5_dbck,	CK_343X),
	CLK(NULL,	"gpio4_dbck",	&gpio4_dbck,	CK_343X),
	CLK(NULL,	"gpio3_dbck",	&gpio3_dbck,	CK_343X),
	CLK(NULL,	"gpio2_dbck",	&gpio2_dbck,	CK_343X),
	CLK(NULL,	"wdt3_fck",	&wdt3_fck,	CK_343X),
	CLK(NULL,	"per_l4_ick",	&per_l4_ick,	CK_343X),
	CLK(NULL,	"gpio6_ick",	&gpio6_ick,	CK_343X),
	CLK(NULL,	"gpio5_ick",	&gpio5_ick,	CK_343X),
	CLK(NULL,	"gpio4_ick",	&gpio4_ick,	CK_343X),
	CLK(NULL,	"gpio3_ick",	&gpio3_ick,	CK_343X),
	CLK(NULL,	"gpio2_ick",	&gpio2_ick,	CK_343X),
	CLK(NULL,	"wdt3_ick",	&wdt3_ick,	CK_343X),
	CLK(NULL,	"uart3_ick",	&uart3_ick,	CK_343X),
	CLK(NULL,	"gpt9_ick",	&gpt9_ick,	CK_343X),
	CLK(NULL,	"gpt8_ick",	&gpt8_ick,	CK_343X),
	CLK(NULL,	"gpt7_ick",	&gpt7_ick,	CK_343X),
	CLK(NULL,	"gpt6_ick",	&gpt6_ick,	CK_343X),
	CLK(NULL,	"gpt5_ick",	&gpt5_ick,	CK_343X),
	CLK(NULL,	"gpt4_ick",	&gpt4_ick,	CK_343X),
	CLK(NULL,	"gpt3_ick",	&gpt3_ick,	CK_343X),
	CLK(NULL,	"gpt2_ick",	&gpt2_ick,	CK_343X),
	CLK("omap-mcbsp.2", "ick",	&mcbsp2_ick,	CK_343X),
	CLK("omap-mcbsp.3", "ick",	&mcbsp3_ick,	CK_343X),
	CLK("omap-mcbsp.4", "ick",	&mcbsp4_ick,	CK_343X),
	CLK("omap-mcbsp.2", "fck",	&mcbsp2_fck,	CK_343X),
	CLK("omap-mcbsp.3", "fck",	&mcbsp3_fck,	CK_343X),
	CLK("omap-mcbsp.4", "fck",	&mcbsp4_fck,	CK_343X),
	CLK(NULL,	"emu_src_ck",	&emu_src_ck,	CK_343X),
	CLK(NULL,	"pclk_fck",	&pclk_fck,	CK_343X),
	CLK(NULL,	"pclkx2_fck",	&pclkx2_fck,	CK_343X),
	CLK(NULL,	"atclk_fck",	&atclk_fck,	CK_343X),
	CLK(NULL,	"traceclk_src_fck", &traceclk_src_fck, CK_343X),
	CLK(NULL,	"traceclk_fck",	&traceclk_fck,	CK_343X),
	CLK(NULL,	"sr1_fck",	&sr1_fck,	CK_343X),
	CLK(NULL,	"sr2_fck",	&sr2_fck,	CK_343X),
	CLK(NULL,	"sr_l4_ick",	&sr_l4_ick,	CK_343X),
	CLK(NULL,	"secure_32k_fck", &secure_32k_fck, CK_343X),
	CLK(NULL,	"gpt12_fck",	&gpt12_fck,	CK_343X),
	CLK(NULL,	"wdt1_fck",	&wdt1_fck,	CK_343X),
};

#define MIN_SDRC_DLL_LOCK_FREQ		83000000

#define CYCLES_PER_MHZ			1000000

/* Scale factor for fixed-point arith in omap3_core_dpll_m2_set_rate() */
#define SDRC_MPURATE_SCALE		8

/* 2^SDRC_MPURATE_BASE_SHIFT: MPU MHz that SDRC_MPURATE_LOOPS is defined for */
#define SDRC_MPURATE_BASE_SHIFT		9

/*
 * SDRC_MPURATE_LOOPS: Number of MPU loops to execute at
 * 2^MPURATE_BASE_SHIFT MHz for SDRC to stabilize
 */
#define SDRC_MPURATE_LOOPS		96

 * omap3430es2_clk_ssi_find_idlest - return CM_IDLEST info for SSI
 * @clk: struct clk * being enabled
 * @idlest_reg: void __iomem ** to store CM_IDLEST reg address into
 * @idlest_bit: pointer to a u8 to store the CM_IDLEST bit shift into
 *
 * The OMAP3430ES2 SSI target CM_IDLEST bit is at a different shift
 * from the CM_{I,F}CLKEN bit.  Pass back the correct info via
 * @idlest_reg and @idlest_bit.  No return value.
 */
static void omap3430es2_clk_ssi_find_idlest(struct clk *clk,
					    void __iomem **idlest_reg,
					    u8 *idlest_bit)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	*idlest_bit = OMAP3430ES2_ST_SSI_IDLE_SHIFT;
}

/**
 * omap3430es2_clk_dss_usbhost_find_idlest - CM_IDLEST info for DSS, USBHOST
 * @clk: struct clk * being enabled
 * @idlest_reg: void __iomem ** to store CM_IDLEST reg address into
 * @idlest_bit: pointer to a u8 to store the CM_IDLEST bit shift into
 *
 * Some OMAP modules on OMAP3 ES2+ chips have both initiator and
 * target IDLEST bits.  For our purposes, we are concerned with the
 * target IDLEST bits, which exist at a different bit position than
 * the *CLKEN bit position for these modules (DSS and USBHOST) (The
 * default find_idlest code assumes that they are at the same
 * position.)  No return value.
 */
static void omap3430es2_clk_dss_usbhost_find_idlest(struct clk *clk,
						    void __iomem **idlest_reg,
						    u8 *idlest_bit)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	/* USBHOST_IDLE has same shift */
	*idlest_bit = OMAP3430ES2_ST_DSS_IDLE_SHIFT;
}

/**
 * omap3430es2_clk_hsotgusb_find_idlest - return CM_IDLEST info for HSOTGUSB
 * @clk: struct clk * being enabled
 * @idlest_reg: void __iomem ** to store CM_IDLEST reg address into
 * @idlest_bit: pointer to a u8 to store the CM_IDLEST bit shift into
 *
 * The OMAP3430ES2 HSOTGUSB target CM_IDLEST bit is at a different
 * shift from the CM_{I,F}CLKEN bit.  Pass back the correct info via
 * @idlest_reg and @idlest_bit.  No return value.
 */
static void omap3430es2_clk_hsotgusb_find_idlest(struct clk *clk,
						 void __iomem **idlest_reg,
						 u8 *idlest_bit)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	*idlest_bit = OMAP3430ES2_ST_HSOTGUSB_IDLE_SHIFT;
}

/**
static int omap3_dpll4_set_rate(struct clk *clk, unsigned long rate)
{
	/*
	 * According to the 12-5 CDP code from TI, "Limitation 2.5"
	 * on 3430ES1 prevents us from changing DPLL multipliers or dividers
	 * on DPLL4.
	 */
	if (omap_rev() == OMAP3430_REV_ES1_0) {
		printk(KERN_ERR "clock: DPLL4 cannot change rate due to "
		       "silicon 'Limitation 2.5' on 3430ES1.\n");
		return -EINVAL;
	}
	return omap3_noncore_dpll_set_rate(clk, rate);
}

/*
 * CORE DPLL (DPLL3) rate programming functions
 *
 * These call into SRAM code to do the actual CM writes, since the SDRAM
 * is clocked from DPLL3.
 */

/**
 * omap3_core_dpll_m2_set_rate - set CORE DPLL M2 divider
 * @clk: struct clk * of DPLL to set
 * @rate: rounded target rate
 *
 * Program the DPLL M2 divider with the rounded target rate.  Returns
 * -EINVAL upon error, or 0 upon success.
 */
static int omap3_core_dpll_m2_set_rate(struct clk *clk, unsigned long rate)
{
	u32 new_div = 0;
	u32 unlock_dll = 0;
	u32 c;
	unsigned long validrate, sdrcrate, mpurate;
	struct omap_sdrc_params *sdrc_cs0;
	struct omap_sdrc_params *sdrc_cs1;
	int ret;

	if (!clk || !rate)
		return -EINVAL;

	if (clk != &dpll3_m2_ck)
		return -EINVAL;

	validrate = omap2_clksel_round_rate_div(clk, rate, &new_div);
	if (validrate != rate)
		return -EINVAL;

	sdrcrate = sdrc_ick.rate;
	if (rate > clk->rate)
		sdrcrate <<= ((rate / clk->rate) >> 1);
	else
		sdrcrate >>= ((clk->rate / rate) >> 1);

	ret = omap2_sdrc_get_params(sdrcrate, &sdrc_cs0, &sdrc_cs1);
	if (ret)
		return -EINVAL;

	if (sdrcrate < MIN_SDRC_DLL_LOCK_FREQ) {
		pr_debug("clock: will unlock SDRC DLL\n");
		unlock_dll = 1;
	}

	/*
	 * XXX This only needs to be done when the CPU frequency changes
	 */
	mpurate = arm_fck.rate / CYCLES_PER_MHZ;
	c = (mpurate << SDRC_MPURATE_SCALE) >> SDRC_MPURATE_BASE_SHIFT;
	c += 1;  /* for safety */
	c *= SDRC_MPURATE_LOOPS;
	c >>= SDRC_MPURATE_SCALE;
	if (c == 0)
		c = 1;

	pr_debug("clock: changing CORE DPLL rate from %lu to %lu\n", clk->rate,
		 validrate);
	pr_debug("clock: SDRC CS0 timing params used:"
		 " RFR %08x CTRLA %08x CTRLB %08x MR %08x\n",
		 sdrc_cs0->rfr_ctrl, sdrc_cs0->actim_ctrla,
		 sdrc_cs0->actim_ctrlb, sdrc_cs0->mr);
	if (sdrc_cs1)
		pr_debug("clock: SDRC CS1 timing params used: "
		 " RFR %08x CTRLA %08x CTRLB %08x MR %08x\n",
		 sdrc_cs1->rfr_ctrl, sdrc_cs1->actim_ctrla,
		 sdrc_cs1->actim_ctrlb, sdrc_cs1->mr);

	if (sdrc_cs1)
		omap3_configure_core_dpll(
				  new_div, unlock_dll, c, rate > clk->rate,
				  sdrc_cs0->rfr_ctrl, sdrc_cs0->actim_ctrla,
				  sdrc_cs0->actim_ctrlb, sdrc_cs0->mr,
				  sdrc_cs1->rfr_ctrl, sdrc_cs1->actim_ctrla,
				  sdrc_cs1->actim_ctrlb, sdrc_cs1->mr);
	else
		omap3_configure_core_dpll(
				  new_div, unlock_dll, c, rate > clk->rate,
				  sdrc_cs0->rfr_ctrl, sdrc_cs0->actim_ctrla,
				  sdrc_cs0->actim_ctrlb, sdrc_cs0->mr,
				  0, 0, 0, 0);

	return 0;
}


static const struct clkops clkops_noncore_dpll_ops = {
	.enable		= &omap3_noncore_dpll_enable,
	.disable	= &omap3_noncore_dpll_disable,
};

/* DPLL autoidle read/set code */


/* Clock control for DPLL outputs */

/* Common clock code */

/*
 * As it is structured now, this will prevent an OMAP2/3 multiboot
 * kernel from compiling.  This will need further attention.
 */
#if defined(CONFIG_ARCH_OMAP3)

static struct clk_functions omap2_clk_functions = {
	.clk_enable		= omap2_clk_enable,
	.clk_disable		= omap2_clk_disable,
	.clk_round_rate		= omap2_clk_round_rate,
	.clk_set_rate		= omap2_clk_set_rate,
	.clk_set_parent		= omap2_clk_set_parent,
	.clk_disable_unused	= omap2_clk_disable_unused,
};

/*
 * Set clocks for bypass mode for reboot to work.
 */
void omap2_clk_prepare_for_reboot(void)
{
	/* REVISIT: Not ready for 343x */
#if 0
	u32 rate;

	if (vclk == NULL || sclk == NULL)
		return;

	rate = clk_get_rate(sclk);
	clk_set_rate(vclk, rate);
#endif
}

/* REVISIT: Move this init stuff out into clock.c */

/*
 * Switch the MPU rate if specified on cmdline.
 * We cannot do this early until cmdline is parsed.
 */
static int __init omap2_clk_arch_init(void)
{
	if (!mpurate)
		return -EINVAL;

	/* REVISIT: not yet ready for 343x */
#if 0
	if (clk_set_rate(&virt_prcm_set, mpurate))
		printk(KERN_ERR "Could not find matching MPU rate\n");
#endif

	recalculate_root_clocks();

	printk(KERN_INFO "Switched to new clocking rate (Crystal/DPLL3/MPU): "
	       "%ld.%01ld/%ld/%ld MHz\n",
	       (osc_sys_ck.rate / 1000000), (osc_sys_ck.rate / 100000) % 10,
	       (core_ck.rate / 1000000), (dpll1_fck.rate / 1000000)) ;

	return 0;
}
arch_initcall(omap2_clk_arch_init);

int __init omap2_clk_init(void)
{
	/* struct prcm_config *prcm; */
	struct omap_clk *c;
	/* u32 clkrate; */
	u32 cpu_clkflg;

	if (cpu_is_omap34xx()) {
		cpu_mask = RATE_IN_343X;
		cpu_clkflg = CK_343X;

		/*
		 * Update this if there are further clock changes between ES2
		 * and production parts
		 */
		if (omap_rev() == OMAP3430_REV_ES1_0) {
			/* No 3430ES1-only rates exist, so no RATE_IN_3430ES1 */
			cpu_clkflg |= CK_3430ES1;
		} else {
			cpu_mask |= RATE_IN_3430ES2;
			cpu_clkflg |= CK_3430ES2;
		}
	}

	clk_init(&omap2_clk_functions);

	for (c = omap34xx_clks; c < omap34xx_clks + ARRAY_SIZE(omap34xx_clks); c++)
		clk_preinit(c->lk.clk);

	for (c = omap34xx_clks; c < omap34xx_clks + ARRAY_SIZE(omap34xx_clks); c++)
		if (c->cpu & cpu_clkflg) {
			clkdev_add(&c->lk);
			clk_register(c->lk.clk);
			omap2_init_clk_clkdm(c->lk.clk);
		}

	/* REVISIT: Not yet ready for OMAP3 */
#if 0
	/* Check the MPU rate set by bootloader */
	clkrate = omap2_get_dpll_rate_24xx(&dpll_ck);
	for (prcm = rate_table; prcm->mpu_speed; prcm++) {
		if (!(prcm->flags & cpu_mask))
			continue;
		if (prcm->xtal_speed != sys_ck.rate)
			continue;
		if (prcm->dpll_speed <= clkrate)
			 break;
	}
	curr_prcm_set = prcm;
#endif

	recalculate_root_clocks();

	printk(KERN_INFO "Clocking rate (Crystal/DPLL/ARM core): "
	       "%ld.%01ld/%ld/%ld MHz\n",
	       (osc_sys_ck.rate / 1000000), (osc_sys_ck.rate / 100000) % 10,
	       (core_ck.rate / 1000000), (arm_fck.rate / 1000000));

	/*
	 * Only enable those clocks we will need, let the drivers
	 * enable other clocks as necessary
	 */
	clk_enable_init_clocks();

	/* Avoid sleeping during omap2_clk_prepare_for_reboot() */
	/* REVISIT: not yet ready for 343x */
#if 0
	vclk = clk_get(NULL, "virt_prcm_set");
	sclk = clk_get(NULL, "sys_ck");
#endif
	return 0;
}

#endif
