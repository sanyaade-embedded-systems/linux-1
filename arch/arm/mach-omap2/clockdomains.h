/*
 * OMAP2/3/4 clockdomains
 *
 * Copyright (C) 2008 Texas Instruments, Inc.
 * Copyright (C) 2008 Nokia Corporation
 *
 * Written by Paul Walmsley
 *
 * Updated with the OMAP4 specific Clock Domains by Abhijit Pagare
 */

#ifndef __ARCH_ARM_MACH_OMAP2_CLOCKDOMAINS_H
#define __ARCH_ARM_MACH_OMAP2_CLOCKDOMAINS_H

#include <mach/clockdomain.h>

#include "prcm-common.h"
#include "prm.h"
#include "prm-regbits-24xx.h"
#include "prm-regbits-34xx.h"
#include "prm-regbits-44xx.h"
#include "cm.h"
#include "cm-regbits-24xx.h"
#include "cm-regbits-34xx.h"
#include "cm1-regbits-44xx.h"
#include "cm2-regbits-44xx.h"

/*
 * OMAP2/3/4-common clockdomains
 *
 * Even though the 2420 has a single PRCM module from the
 * interconnect's perspective, internally it does appear to have
 * separate PRM and CM clockdomains.  The usual test case is
 * sys_clkout/sys_clkout2.
 */

/* This is an implicit clockdomain - it is never defined as such in TRM */
static struct clockdomain wkup_clkdm = {
	.name		= "wkup_clkdm",
	.pwrdm		= { .name = "wkup_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX | CHIP_IS_OMAP3430 \
						| CHIP_IS_OMAP4430),
};

static struct clockdomain prm_clkdm = {
	.name		= "prm_clkdm",
	.pwrdm		= { .name = "wkup_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX | CHIP_IS_OMAP3430 \
						| CHIP_IS_OMAP4430),
};

/*
 * 2420-only clockdomains
 */

#if defined(CONFIG_ARCH_OMAP2420)

static struct clockdomain mpu_2420_clkdm = {
	.name		= "mpu_clkdm",
	.pwrdm		= { .name = "mpu_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_MPU_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2420),
};

static struct clockdomain iva1_2420_clkdm = {
	.name		= "iva1_clkdm",
	.pwrdm		= { .name = "dsp_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP2420_AUTOSTATE_IVA_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2420),
};

#endif  /* CONFIG_ARCH_OMAP2420 */


/*
 * 2430-only clockdomains
 */

#if defined(CONFIG_ARCH_OMAP2430)

static struct clockdomain mpu_2430_clkdm = {
	.name		= "mpu_clkdm",
	.pwrdm		= { .name = "mpu_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_MPU_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

static struct clockdomain mdm_clkdm = {
	.name		= "mdm_clkdm",
	.pwrdm		= { .name = "mdm_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP2430_AUTOSTATE_MDM_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP2430),
};

#endif    /* CONFIG_ARCH_OMAP2430 */


/*
 * 24XX-only clockdomains
 */

#if defined(CONFIG_ARCH_OMAP24XX)

static struct clockdomain cm_clkdm = {
	.name		= "cm_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX),
};

static struct clockdomain dsp_clkdm = {
	.name		= "dsp_clkdm",
	.pwrdm		= { .name = "dsp_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_DSP_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX),
};

static struct clockdomain gfx_24xx_clkdm = {
	.name		= "gfx_clkdm",
	.pwrdm		= { .name = "gfx_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_GFX_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX),
};

static struct clockdomain core_l3_24xx_clkdm = {
	.name		= "core_l3_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_L3_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX),
};

static struct clockdomain core_l4_24xx_clkdm = {
	.name		= "core_l4_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_L4_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX),
};

static struct clockdomain dss_24xx_clkdm = {
	.name		= "dss_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP,
	.clktrctrl_mask = OMAP24XX_AUTOSTATE_DSS_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP24XX),
};

#endif   /* CONFIG_ARCH_OMAP24XX */


/*
 * 34xx clockdomains
 */

#if defined(CONFIG_ARCH_OMAP34XX)

static struct clockdomain cm_clkdm = {
	.name		= "cm_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain mpu_34xx_clkdm = {
	.name		= "mpu_clkdm",
	.pwrdm		= { .name = "mpu_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP | CLKDM_CAN_FORCE_WAKEUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_MPU_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain neon_clkdm = {
	.name		= "neon_clkdm",
	.pwrdm		= { .name = "neon_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_NEON_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain iva2_clkdm = {
	.name		= "iva2_clkdm",
	.pwrdm		= { .name = "iva2_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_IVA2_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain gfx_3430es1_clkdm = {
	.name		= "gfx_clkdm",
	.pwrdm		= { .name = "gfx_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430ES1_CLKTRCTRL_GFX_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430ES1),
};

static struct clockdomain sgx_clkdm = {
	.name		= "sgx_clkdm",
	.pwrdm		= { .name = "sgx_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430ES2_CLKTRCTRL_SGX_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_GE_OMAP3430ES2),
};

/*
 * The die-to-die clockdomain was documented in the 34xx ES1 TRM, but
 * then that information was removed from the 34xx ES2+ TRM.  It is
 * unclear whether the core is still there, but the clockdomain logic
 * is there, and must be programmed to an appropriate state if the
 * CORE clockdomain is to become inactive.
 */
static struct clockdomain d2d_clkdm = {
	.name		= "d2d_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430ES1_CLKTRCTRL_D2D_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain core_l3_34xx_clkdm = {
	.name		= "core_l3_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_L3_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain core_l4_34xx_clkdm = {
	.name		= "core_l4_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_L4_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain dss_34xx_clkdm = {
	.name		= "dss_clkdm",
	.pwrdm		= { .name = "dss_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_DSS_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain cam_clkdm = {
	.name		= "cam_clkdm",
	.pwrdm		= { .name = "cam_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_CAM_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain usbhost_clkdm = {
	.name		= "usbhost_clkdm",
	.pwrdm		= { .name = "usbhost_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430ES2_CLKTRCTRL_USBHOST_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_GE_OMAP3430ES2),
};

static struct clockdomain per_clkdm = {
	.name		= "per_clkdm",
	.pwrdm		= { .name = "per_pwrdm" },
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_PER_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

/*
 * Disable hw supervised mode for emu_clkdm, because emu_pwrdm is
 * switched of even if sdti is in use
 */
static struct clockdomain emu_clkdm = {
	.name		= "emu_clkdm",
	.pwrdm		= { .name = "emu_pwrdm" },
	.flags		= /* CLKDM_CAN_ENABLE_AUTO |  */CLKDM_CAN_SWSUP,
	.clktrctrl_mask = OMAP3430_CLKTRCTRL_EMU_MASK,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain dpll1_clkdm = {
	.name		= "dpll1_clkdm",
	.pwrdm		= { .name = "dpll1_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain dpll2_clkdm = {
	.name		= "dpll2_clkdm",
	.pwrdm		= { .name = "dpll2_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain dpll3_clkdm = {
	.name		= "dpll3_clkdm",
	.pwrdm		= { .name = "dpll3_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain dpll4_clkdm = {
	.name		= "dpll4_clkdm",
	.pwrdm		= { .name = "dpll4_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
};

static struct clockdomain dpll5_clkdm = {
	.name		= "dpll5_clkdm",
	.pwrdm		= { .name = "dpll5_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_GE_OMAP3430ES2),
};

#endif   /* CONFIG_ARCH_OMAP34XX */

/*
 * 44xx clockdomains
 */

/* The Flags will be populated later */

#if defined(CONFIG_ARCH_OMAP4)

static struct clockdomain cm1_clkdm = {
	.name		= "cm1_clkdm",
	.pwrdm		= { .name = "pd_alwon_core_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cm2_clkdm = {
	.name		= "cm2_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_dsp_clkdm = {
	.name		= "cd_dsp_clkdm",
	.pwrdm		= { .name = "dsp_pwrdm" },
	.clktrctrl_mask	= OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_std_efuse_clkdm = {
	.name		= "cd_std_efuse_clkdm",
	.pwrdm		= { .name = "std_effuse_pwrdm" },
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_cortexa9_clkdm = {
	.name		= "cd_cortexa9_clkdm",
	.pwrdm		= { .name = "mpu_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l4_per_clkdm = {
	.name		= "cd_l4_per_clkdm",
	.pwrdm		= { .name = "pd_l4_per_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l3_init_clkdm = {
	.name		= "cd_l3_init_clkdm",
	.pwrdm		= { .name = "pd_l3_init_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_ivahd_clkdm = {
	.name		= "cd_ivahd_clkdm",
	.pwrdm		= { .name = "ivahd_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_sgx_clkdm = {
	.name		= "cd_sgx_clkdm",
	.pwrdm		= { .name = "pd_sgx_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_emu_clkdm = {
	.name		= "cd_emu_clkdm",
	.pwrdm		= { .name = "pd_emu_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_dss_clkdm = {
	.name		= "cd_dss_clkdm",
	.pwrdm		= { .name = "pd_dss_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l4_cfg_clkdm = {
	.name		= "cd_l4_cfg_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l3_instr_clkdm = {
	.name		= "cd_l3_instr_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l3_2_clkdm = {
	.name		= "cd_l3_2_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l3_1_clkdm = {
	.name		= "cd_l3_1_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_emif_clkdm = {
	.name		= "cd_emif_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_cortexm3_clkdm = {
	.name		= "cd_cortexm3_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_dma_clkdm = {
	.name		= "cd_dma_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_d2d_clkdm = {
	.name		= "cd_d2d_clkdm",
	.pwrdm		= { .name = "core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_cam_clkdm = {
	.name		= "cd_cam_clkdm",
	.pwrdm		= { .name = "pd_cam_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_abe_clkdm = {
	.name		= "cd_abe_clkdm",
	.pwrdm		= { .name = "pd_audio_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_HWSUP_SWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct clockdomain cd_l4_alwon_core_clkdm = {
	.name		= "cd_l4_alwon_core_clkdm",
	.pwrdm		= { .name = "pd_alwon_core_pwrdm" },
	.clktrctrl_mask = OMAP4430_CLKTRCTRL_MASK,
	.flags		= CLKDM_CAN_FORCE_WAKEUP | CLKDM_CAN_HWSUP,
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

#endif

/*
 * Clockdomain-powerdomain hwsup dependencies (34XX only)
 */

static struct clkdm_pwrdm_autodep clkdm_pwrdm_autodeps[] = {

#ifdef CONFIG_ARCH_OMAP34xx
	{
		.pwrdm	   = { .name = "mpu_pwrdm" },
		.omap_chip = OMAP_CHIP_INIT(CHIP_IS_OMAP3430)
	},
	{
		.pwrdm	   = { .name = "iva2_pwrdm" },
		.omap_chip = OMAP_CHIP_INIT(CHIP_IS_OMAP3430)
	},
	{
		.pwrdm	   = { .name = NULL },
	}
#endif

/* The dependencies for OMAP4 will be populated later */
};

/*
 *
 */

static struct clockdomain *clockdomains_omap[] = {

	&wkup_clkdm,
	&prm_clkdm,

#ifdef CONFIG_ARCH_OMAP2420
	&mpu_2420_clkdm,
	&iva1_2420_clkdm,
#endif

#ifdef CONFIG_ARCH_OMAP2430
	&mpu_2430_clkdm,
	&mdm_clkdm,
#endif

#ifdef CONFIG_ARCH_OMAP24XX
	&cm_clkdm,
	&dsp_clkdm,
	&gfx_24xx_clkdm,
	&core_l3_24xx_clkdm,
	&core_l4_24xx_clkdm,
	&dss_24xx_clkdm,
#endif

#ifdef CONFIG_ARCH_OMAP34XX
	&cm_clkdm,
	&mpu_34xx_clkdm,
	&neon_clkdm,
	&iva2_clkdm,
	&gfx_3430es1_clkdm,
	&sgx_clkdm,
	&d2d_clkdm,
	&core_l3_34xx_clkdm,
	&core_l4_34xx_clkdm,
	&dss_34xx_clkdm,
	&cam_clkdm,
	&usbhost_clkdm,
	&per_clkdm,
	&emu_clkdm,
	&dpll1_clkdm,
	&dpll2_clkdm,
	&dpll3_clkdm,
	&dpll4_clkdm,
	&dpll5_clkdm,
#endif

#ifdef CONFIG_ARCH_OMAP4
	&cm1_clkdm,
	&cm2_clkdm,
	&cd_dsp_clkdm,
	&cd_std_efuse_clkdm,
	&cd_cortexa9_clkdm,
	&cd_l4_per_clkdm,
	&cd_l3_init_clkdm,
	&cd_ivahd_clkdm,
	&cd_sgx_clkdm,
	&cd_emu_clkdm,
	&cd_dss_clkdm,
	&cd_l4_cfg_clkdm,
	&cd_l3_instr_clkdm,
	&cd_l3_2_clkdm,
	&cd_l3_1_clkdm,
	&cd_emif_clkdm,
	&cd_cortexm3_clkdm,
	&cd_dma_clkdm,
	&cd_d2d_clkdm,
	&cd_cam_clkdm,
	&cd_abe_clkdm,
	&cd_l4_alwon_core_clkdm,
#endif
	NULL,
};

#endif
