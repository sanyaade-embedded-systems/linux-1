/*
 * PRCMAccInt.h
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _PRCM_ACC_INT_H
#define _PRCM_ACC_INT_H

/* Mappings of level 1 EASI function numbers to function names */
#ifdef OMAP_3430

#define EASIL1_PRCMPRCM_CLKCFG_CTRLValid_configWriteClk_valid32  \
						(PRCM_BASE_EASIL1 + 349)
#define EASIL1_PRCMCM_FCLKEN1_COREReadRegister32	(PRCM_BASE_EASIL1 + 743)
#define EASIL1_PRCMCM_FCLKEN1_COREEN_GPT8Write32	(PRCM_BASE_EASIL1 + 951)
#define EASIL1_PRCMCM_FCLKEN1_COREEN_GPT7Write32	(PRCM_BASE_EASIL1 + 961)
#define EASIL1_PRCMCM_ICLKEN1_COREReadRegister32	\
						(PRCM_BASE_EASIL1 + 1087)
#define EASIL1_PRCMCM_ICLKEN1_COREEN_MAILBOXESWrite32	\
						(PRCM_BASE_EASIL1 + 1105)
#define EASIL1_PRCMCM_ICLKEN1_COREEN_GPT8Write32	\
						(PRCM_BASE_EASIL1 + 1305)
#define EASIL1_PRCMCM_ICLKEN1_COREEN_GPT7Write32	\
						(PRCM_BASE_EASIL1 + 1315)
#define EASIL1_PRCMCM_CLKSEL1_CORECLKSEL_L3ReadIssel132	\
						(PRCM_BASE_EASIL1 + 2261)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT8Write32k32	\
						(PRCM_BASE_EASIL1 + 2364)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT8WriteSys32	\
						(PRCM_BASE_EASIL1 + 2365)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT8WriteExt32	\
						(PRCM_BASE_EASIL1 + 2366)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT7Write32k32	\
						(PRCM_BASE_EASIL1 + 2380)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT7WriteSys32	\
						(PRCM_BASE_EASIL1 + 2381)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT7WriteExt32	\
						(PRCM_BASE_EASIL1 + 2382)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT6WriteSys32	\
						(PRCM_BASE_EASIL1 + 2397)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT6WriteExt32	\
						(PRCM_BASE_EASIL1 + 2398)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT5WriteSys32	\
						(PRCM_BASE_EASIL1 + 2413)
#define EASIL1_PRCMCM_CLKSEL2_CORECLKSEL_GPT5WriteExt32	\
						(PRCM_BASE_EASIL1 + 2414)
#define EASIL1_PRCMCM_CLKSEL1_PLLAPLLs_ClkinRead32	\
						(PRCM_BASE_EASIL1 + 3747)
#define EASIL1_PRCMCM_FCLKEN_DSPEN_DSPWrite32	(PRCM_BASE_EASIL1 + 3834)
#define EASIL1_PRCMCM_ICLKEN_DSPEN_DSP_IPIWrite32	\
						(PRCM_BASE_EASIL1 + 3846)
#define EASIL1_PRCMCM_IDLEST_DSPReadRegister32	(PRCM_BASE_EASIL1 + 3850)
#define EASIL1_PRCMCM_IDLEST_DSPST_IPIRead32	(PRCM_BASE_EASIL1 + 3857)
#define EASIL1_PRCMCM_IDLEST_DSPST_DSPRead32	(PRCM_BASE_EASIL1 + 3863)
#define EASIL1_PRCMCM_AUTOIDLE_DSPAUTO_DSP_IPIWrite32	\
						(PRCM_BASE_EASIL1 + 3877)
#define EASIL1_PRCMCM_CLKSEL_DSPSYNC_DSPWrite32	(PRCM_BASE_EASIL1 + 3927)
#define EASIL1_PRCMCM_CLKSEL_DSPCLKSEL_DSP_IFWrite32	\
						(PRCM_BASE_EASIL1 + 3941)
#define EASIL1_PRCMCM_CLKSEL_DSPCLKSEL_DSPWrite32	\
						(PRCM_BASE_EASIL1 + 3965)
#define EASIL1_PRCMCM_CLKSTCTRL_DSPAutostate_DSPRead32	\
						(PRCM_BASE_EASIL1 + 3987)
#define EASIL1_PRCMCM_CLKSTCTRL_DSPAutostate_DSPWrite32	\
						(PRCM_BASE_EASIL1 + 3993)
#define EASIL1_PRCMRM_RSTCTRL_DSPReadRegister32	(PRCM_BASE_EASIL1 + 3997)
#define EASIL1_PRCMRM_RSTCTRL_DSPRST1_DSPWrite32	\
						(PRCM_BASE_EASIL1 + 4025)
#define EASIL1_PRCMRM_RSTST_DSPReadRegister32	(PRCM_BASE_EASIL1 + 4029)
#define EASIL1_PRCMRM_RSTST_DSPWriteRegister32	(PRCM_BASE_EASIL1 + 4030)
#define EASIL1_PRCMPM_PWSTCTRL_DSPForceStateWrite32	\
						(PRCM_BASE_EASIL1 + 4165)
#define EASIL1_PRCMPM_PWSTCTRL_DSPPowerStateWriteRET32	\
						(PRCM_BASE_EASIL1 + 4193)
#define EASIL1_PRCMPM_PWSTST_DSPReadRegister32	(PRCM_BASE_EASIL1 + 4197)
#define EASIL1_PRCMPM_PWSTST_DSPInTransitionRead32	\
						(PRCM_BASE_EASIL1 + 4198)
#define EASIL1_PRCMPM_PWSTST_DSPPowerStateStGet32	\
						(PRCM_BASE_EASIL1 + 4235)
#define EASIL1_CM_FCLKEN_PER_GPT5WriteRegister32	\
						(PRCM_BASE_EASIL1 + 4368)
#define EASIL1_CM_ICLKEN_PER_GPT5WriteRegister32	\
						(PRCM_BASE_EASIL1 + 4370)
#define EASIL1_CM_CLKSEL_PER_GPT5Write32k32	(PRCM_BASE_EASIL1 + 4372)
#define EASIL1_CM_CLKSEL_PER_GPT6Write32k32	(PRCM_BASE_EASIL1 + 4373)
#define EASIL1_PRCMCM_CLKSTCTRL_IVA2WriteRegister32	\
						(PRCM_BASE_EASIL1 + 4374)
#define EASIL1_PRCMPM_PWSTCTRL_IVA2PowerStateWriteON32	\
						(PRCM_BASE_EASIL1 + 4375)
#define EASIL1_PRCMPM_PWSTCTRL_IVA2PowerStateWriteOFF32	\
						(PRCM_BASE_EASIL1 + 4376)
#define EASIL1_PRCMPM_PWSTST_IVA2InTransitionRead32	\
						(PRCM_BASE_EASIL1 + 4377)
#define EASIL1_PRCMPM_PWSTST_IVA2PowerStateStGet32	\
						(PRCM_BASE_EASIL1 + 4378)
#define EASIL1_PRCMPM_PWSTST_IVA2ReadRegister32	(PRCM_BASE_EASIL1 + 4379)

/* Register offset address definitions */

#define PRCM_PRCM_CLKCFG_CTRL_OFFSET        (u32)(0x80)
#define PRCM_CM_FCLKEN1_CORE_OFFSET          (u32)(0x200)
#define PRCM_CM_ICLKEN1_CORE_OFFSET          (u32)(0x210)
#define PRCM_CM_CLKSEL2_CORE_OFFSET          (u32)(0x244)
#define PRCM_CM_CLKSEL1_PLL_OFFSET           (u32)(0x540)
#define PRCM_CM_ICLKEN_DSP_OFFSET            (u32)(0x810)
#define PRCM_CM_IDLEST_DSP_OFFSET            (u32)(0x820)
#define PRCM_CM_AUTOIDLE_DSP_OFFSET          (u32)(0x830)
#define PRCM_CM_CLKSEL_DSP_OFFSET            (u32)(0x840)
#define PRCM_CM_CLKSTCTRL_DSP_OFFSET         (u32)(0x848)
#define PRCM_RM_RSTCTRL_DSP_OFFSET           (u32)(0x850)
#define PRCM_RM_RSTST_DSP_OFFSET             (u32)(0x858)
#define PRCM_PM_PWSTCTRL_DSP_OFFSET          (u32)(0x8e0)
#define PRCM_PM_PWSTST_DSP_OFFSET            (u32)(0x8e4)
#define PRCM_PM_PWSTST_IVA2_OFFSET            (u32)(0xE4)
#define PRCM_PM_PWSTCTRL_IVA2_OFFSET          (u32)(0xE0)
#define PRCM_CM_CLKSTCTRL_IVA2_OFFSET         (u32)(0x48)
#define CM_CLKSEL_PER_OFFSET                            (u32)(0x40)

/* Bitfield mask and offset declarations */

#define PRCM_PRCM_CLKCFG_CTRL_Valid_config_MASK         (u32)(0x1)
#define PRCM_PRCM_CLKCFG_CTRL_Valid_config_OFFSET       (u32)(0)

#define PRCM_CM_FCLKEN1_CORE_EN_GPT8_MASK               (u32)(0x400)
#define PRCM_CM_FCLKEN1_CORE_EN_GPT8_OFFSET             (u32)(10)

#define PRCM_CM_FCLKEN1_CORE_EN_GPT7_MASK               (u32)(0x200)
#define PRCM_CM_FCLKEN1_CORE_EN_GPT7_OFFSET             (u32)(9)

#define PRCM_CM_ICLKEN1_CORE_EN_GPT8_MASK               (u32)(0x400)
#define PRCM_CM_ICLKEN1_CORE_EN_GPT8_OFFSET             (u32)(10)

#define PRCM_CM_ICLKEN1_CORE_EN_GPT7_MASK               (u32)(0x200)
#define PRCM_CM_ICLKEN1_CORE_EN_GPT7_OFFSET             (u32)(9)

#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT8_MASK           (u32)(0xc000)
#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT8_OFFSET         (u32)(14)

#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT7_MASK           (u32)(0x3000)
#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT7_OFFSET         (u32)(12)

#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT6_MASK           (u32)(0xc00)
#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT6_OFFSET         (u32)(10)

#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT5_MASK           (u32)(0x300)
#define PRCM_CM_CLKSEL2_CORE_CLKSEL_GPT5_OFFSET         (u32)(8)

#define PRCM_CM_CLKSEL1_PLL_APLLs_Clkin_MASK            (u32)(0x3800000)
#define PRCM_CM_CLKSEL1_PLL_APLLs_Clkin_OFFSET          (u32)(23)

#define PRCM_CM_ICLKEN_DSP_EN_DSP_IPI_MASK              (u32)(0x2)
#define PRCM_CM_ICLKEN_DSP_EN_DSP_IPI_OFFSET            (u32)(1)

#define PRCM_CM_IDLEST_DSP_ST_IPI_MASK                  (u32)(0x2)
#define PRCM_CM_IDLEST_DSP_ST_IPI_OFFSET                (u32)(1)

#define PRCM_CM_AUTOIDLE_DSP_AUTO_DSP_IPI_MASK          (u32)(0x2)
#define PRCM_CM_AUTOIDLE_DSP_AUTO_DSP_IPI_OFFSET        (u32)(1)

#define PRCM_CM_CLKSEL_DSP_SYNC_DSP_MASK                (u32)(0x80)
#define PRCM_CM_CLKSEL_DSP_SYNC_DSP_OFFSET              (u32)(7)

#define PRCM_CM_CLKSEL_DSP_CLKSEL_DSP_IF_MASK           (u32)(0x60)
#define PRCM_CM_CLKSEL_DSP_CLKSEL_DSP_IF_OFFSET         (u32)(5)

#define PRCM_CM_CLKSEL_DSP_CLKSEL_DSP_MASK              (u32)(0x1f)
#define PRCM_CM_CLKSEL_DSP_CLKSEL_DSP_OFFSET            (u32)(0)

#define PRCM_CM_CLKSTCTRL_DSP_Autostate_DSP_MASK        (u32)(0x1)
#define PRCM_CM_CLKSTCTRL_DSP_Autostate_DSP_OFFSET      (u32)(0)

#define PRCM_PM_PWSTCTRL_DSP_ForceState_MASK            (u32)(0x40000)
#define PRCM_PM_PWSTCTRL_DSP_ForceState_OFFSET          (u32)(18)

#define PRCM_PM_PWSTCTRL_DSP_PowerState_MASK            (u32)(0x3)
#define PRCM_PM_PWSTCTRL_DSP_PowerState_OFFSET          (u32)(0)

#define PRCM_PM_PWSTCTRL_IVA2_PowerState_MASK            (u32)(0x3)
#define PRCM_PM_PWSTCTRL_IVA2_PowerState_OFFSET          (u32)(0)

#define PRCM_PM_PWSTST_DSP_InTransition_MASK            (u32)(0x100000)
#define PRCM_PM_PWSTST_DSP_InTransition_OFFSET          (u32)(20)

#define PRCM_PM_PWSTST_IVA2_InTransition_MASK            (u32)(0x100000)
#define PRCM_PM_PWSTST_IVA2_InTransition_OFFSET          (u32)(20)

#define PRCM_PM_PWSTST_DSP_PowerStateSt_MASK            (u32)(0x3)
#define PRCM_PM_PWSTST_DSP_PowerStateSt_OFFSET          (u32)(0)

#define PRCM_PM_PWSTST_IVA2_PowerStateSt_MASK            (u32)(0x3)
#define PRCM_PM_PWSTST_IVA2_PowerStateSt_OFFSET          (u32)(0)

#define CM_FCLKEN_PER_OFFSET		(u32)(0x0)
#define CM_FCLKEN_PER_GPT5_OFFSET         (u32)(6)
#define CM_FCLKEN_PER_GPT5_MASK     (u32)(0x40)

#define CM_FCLKEN_PER_GPT6_OFFSET   (u32)(7)
#define CM_FCLKEN_PER_GPT6_MASK      (u32)(0x80)

#define CM_ICLKEN_PER_OFFSET		(u32)(0x10)
#define CM_ICLKEN_PER_GPT5_OFFSET  (u32)(6)
#define CM_ICLKEN_PER_GPT5_MASK     (u32)(0x40)

#define CM_ICLKEN_PER_GPT6_OFFSET  (u32)(7)
#define CM_ICLKEN_PER_GPT6_MASK     (u32)(0x80)

#define CM_CLKSEL_PER_GPT5_OFFSET   (u32)(3)
#define CM_CLKSEL_PER_GPT5_MASK      (u32)(0x8)

#define CM_CLKSEL_PER_GPT6_OFFSET   (u32)(4)
#define CM_CLKSEL_PER_GPT6_MASK       (u32)(0x10)


#define CM_FCLKEN_IVA2_OFFSET		(u32)(0x0)
#define CM_FCLKEN_IVA2_EN_MASK	(u32)(0x1)
#define CM_FCLKEN_IVA2_EN_OFFSET	(u32)(0x0)

#define CM_IDLEST_IVA2_OFFSET 		(u32)(0x20)
#define CM_IDLEST_IVA2_ST_IVA2_MASK (u32) (0x01)
#define CM_IDLEST_IVA2_ST_IVA2_OFFSET (u32) (0x00)

#define CM_FCLKEN1_CORE_OFFSET 	(u32)(0xA00)

#define CM_ICLKEN1_CORE_OFFSET  	(u32)(0xA10)
#define CM_ICLKEN1_CORE_EN_MAILBOXES_MASK  (u32)(0x00000080)   /* bit 7 */
#define CM_ICLKEN1_CORE_EN_MAILBOXES_OFFSET	(u32)(7)

#define CM_CLKSTCTRL_IVA2_OFFSET (u32)(0x0)
#define CM_CLKSTCTRL_IVA2_MASK    (u32)(0x3)


#define PRM_RSTCTRL_IVA2_OFFSET  	(u32)(0x50)
#define PRM_RSTCTRL_IVA2_RST1_MASK	(u32)(0x1)
#define PRM_RSTCTRL_IVA2_RST1_OFFSET	(u32)(0x0)
#define PRM_RSTCTRL_IVA2_RST2_MASK	(u32)(0x2)
#define PRM_RSTCTRL_IVA2_RST2_OFFSET	(u32)(0x1)
#define PRM_RSTCTRL_IVA2_RST3_MASK	(u32)(0x4)
#define PRM_RSTCTRL_IVA2_RST3_OFFSET	(u32)(0x2)


/* The following represent the enumerated values for each bitfield */

enum PRCMPRCM_CLKCFG_CTRLValid_configE {
    PRCMPRCM_CLKCFG_CTRLValid_configUpdated = 0x0000,
    PRCMPRCM_CLKCFG_CTRLValid_configClk_valid = 0x0001
} ;

enum PRCMCM_CLKSEL2_CORECLKSEL_GPT8E {
    PRCMCM_CLKSEL2_CORECLKSEL_GPT832k = 0x0000,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT8Sys = 0x0001,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT8Ext = 0x0002,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT8Reserved = 0x0003
} ;

enum PRCMCM_CLKSEL2_CORECLKSEL_GPT7E {
    PRCMCM_CLKSEL2_CORECLKSEL_GPT732k = 0x0000,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT7Sys = 0x0001,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT7Ext = 0x0002,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT7Reserved = 0x0003
} ;

enum PRCMCM_CLKSEL2_CORECLKSEL_GPT6E {
    PRCMCM_CLKSEL2_CORECLKSEL_GPT632k = 0x0000,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT6Sys = 0x0001,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT6Ext = 0x0002,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT6Reserved = 0x0003
} ;

enum PRCMCM_CLKSEL2_CORECLKSEL_GPT5E {
    PRCMCM_CLKSEL2_CORECLKSEL_GPT532k = 0x0000,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT5Sys = 0x0001,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT5Ext = 0x0002,
    PRCMCM_CLKSEL2_CORECLKSEL_GPT5Reserved = 0x0003
} ;

enum PRCMPM_PWSTCTRL_DSPPowerStateE {
    PRCMPM_PWSTCTRL_DSPPowerStateON = 0x0000,
    PRCMPM_PWSTCTRL_DSPPowerStateRET = 0x0001,
    PRCMPM_PWSTCTRL_DSPPowerStateReserved = 0x0002,
    PRCMPM_PWSTCTRL_DSPPowerStateOFF = 0x0003
} ;

enum PRCMPM_PWSTCTRL_IVA2PowerStateE {
    PRCMPM_PWSTCTRL_IVA2PowerStateON = 0x0003,
    PRCMPM_PWSTCTRL_IVA2PowerStateRET = 0x0001,
    PRCMPM_PWSTCTRL_IVA2PowerStateReserved = 0x0002,
    PRCMPM_PWSTCTRL_IVA2PowerStateOFF = 0x0000
} ;

#else

#define PRM_PRM_IRQSTATUS_TESLA_OFFSET 			 (u32)(0x030)
#define PRM_PRM_IRQENABLE_TESLA_OFFSET           (u32)(0x038)

/*TESLA_PRM*/
#define PRM_PM_TESLA_PWRSTCTRL_OFFSET            (u32)(0x400)
#define PRM_PM_TESLA_PWRSTST_OFFSET              (u32)(0x404)
#define PRM_RM_TESLA_RSTCTRL_OFFSET              (u32)(0x410)
#define PRM_RM_TESLA_RSTST_OFFSET                (u32)(0x414)
#define PRM_RM_TESLA_TESLA_CONTEXT_OFFSET        (u32)(0x424)

/*CORE_PRM*/
#define PRM_PM_CORE_PWRSTCTRL_OFFSET             (u32)(0x700)

/*ALWAYS_ON_PRM*/
#define PRM_PM_ALWON_SR_IVA_WKDEP_OFFSET         (u32)(0x630)
#define PRM_RM_ALWON_SR_IVA_CONTEXT_OFFSET       (u32)(0x634)


/*CM1*/
/*base address = 0x4A00_5000*/

/*CKGEN_CM1*/
#define CM1_CM_CLKMODE_DPLL_IVA_OFFSET           (u32)(0x1A0)
#define CM1_CM_IDLEST_DPLL_IVA_OFFSET            (u32)(0x1A4)
#define CM1_CM_AUTOIDLE_DPLL_IVA_OFFSET          (u32)(0x1A8)
#define CM1_CM_CLKSEL_DPLL_IVA_OFFSET            (u32)(0x1AC)
#define CM1_CM_DIV_M2_DPLL_IVA_OFFSET            (u32)(0x1B0)
#define CM1_CM_DIV_M4_DPLL_IVA_OFFSET            (u32)(0x1B8)
#define CM1_CM_DIV_M5_DPLL_IVA_OFFSET            (u32)(0x1BC)
#define CM1_CM_SSC_DELTAMSTEP_DPLL_IVA_OFFSET    (u32)(0x1C8)
#define CM1_CM_SSC_MODFREQDIV_DPLL_IVA_OFFSET    (u32)(0x1CC)
#define CM1_CM_BYPCLK_DPLL_IVA_OFFSET            (u32)(0x1DC)

/*TESLA_CM1*/
#define CM1_CM_TESLA_CLKSTCTRL_OFFSET            (u32)(0x400)
#define CM1_CM_TESLA_STATICDEP_OFFSET            (u32)(0x404)
#define CM1_CM_TESLA_DYNAMICDEP_OFFSET           (u32)(0x408)
#define CM1_CM_TESLA_TESLA_CLKCTRL_OFFSET        (u32)(0x420)

/*ABE_CM1*/
#define CM1_CM1_ABE_CLKSTCTRL_OFFSET             (u32)(0x500)
#define CM1_CM1_ABE_MCBSP1_CLKCTRL_OFFSET        (u32)(0x548)
#define CM1_CM1_ABE_MCBSP2_CLKCTRL_OFFSET        (u32)(0x550)
#define CM1_CM1_ABE_MCBSP3_CLKCTRL_OFFSET        (u32)(0x558)
#define CM1_CM1_ABE_TIMER5_CLKCTRL_OFFSET        (u32)(0x568)
#define CM1_CM1_ABE_TIMER6_CLKCTRL_OFFSET        (u32)(0x570)
#define CM1_CM1_ABE_TIMER7_CLKCTRL_OFFSET        (u32)(0x578)
#define CM1_CM1_ABE_TIMER8_CLKCTRL_OFFSET        (u32)(0x580)

/*CM2*/
/*base address = 0x4A00_8000*/

/*CKGEN_CM2*/
#define CM2_CM_IVA_DVFS_PERF_TESLA_OFFSET        (u32)(0x128)
#define CM2_CM_IVA_DVFS_PERF_IVAHD_OFFSET        (u32)(0x12C)
#define CM2_CM_IVA_DVFS_PERF_ABE_OFFSET          (u32)(0x130)
#define CM2_CM_IVA_DVFS_RESULT_OFFSET            (u32)(0x134)
#define CM2_CM_IVA_DVFS_CURRENT_OFFSET           (u32)(0x138)

/*L4PER_CM2*/
#define CM2_CM_L4PER_CLKSTCTRL_OFFSET            (u32)(0x1400)
#define CM2_CM_L4PER_DMTIMER10_CLKCTRL_OFFSET    (u32)(0x1428)
#define CM2_CM_L4PER_DMTIMER11_CLKCTRL_OFFSET    (u32)(0x1430)
#define CM2_CM_L4PER_DMTIMER2_CLKCTRL_OFFSET     (u32)(0x1438)
#define CM2_CM_L4PER_DMTIMER3_CLKCTRL_OFFSET     (u32)(0x1440)
#define CM2_CM_L4PER_DMTIMER4_CLKCTRL_OFFSET     (u32)(0x1448)
#define CM2_CM_L4PER_DMTIMER9_CLKCTRL_OFFSET     (u32)(0x1450)
#define CM2_CM_L4PER_MCBSP4_CLKCTRL_OFFSET       (u32)(0x14E0)
#define CM2_CM_L4PER_MCBSP5_CLKCTRL_OFFSET       (u32)(0x14E8)


/*BITS MASKS & OFFSETS*/

/******************PM_TESLA_PWRSTCTRL***********/
#define PM_TESLA_PWRSTCTRL_PowerState_OFFSET            (u32)(0x0)
#define PM_TESLA_PWRSTCTRL_PowerState_MASK              (u32)(0x3)
#define PM_TESLA_PWRSTCTRL_LogicRetState_OFFSET         (u32)(0x2)
#define PM_TESLA_PWRSTCTRL_LogicRetState_MASK           (u32)(0x4)
#define PM_TESLA_PWRSTCTRL_L1RetState_OFFSET            (u32)(0x8)
#define PM_TESLA_PWRSTCTRL_L1RetState_MASK              (u32)(0x100)
#define PM_TESLA_PWRSTCTRL_L2RetState_OFFSET            (u32)(0x9)
#define PM_TESLA_PWRSTCTRL_L2RetState_MASK              (u32)(0x200)

/******************PRM_PM_TESLA_PWRSTST*********/
#define PM_TESLA_PWRSTST_PowerState_OFFSET              (u32)(0x0)
#define PM_TESLA_PWRSTST_PowerState_MASK                (u32)(0x3)

/******************RM_TESLA_RSTCTRL*************/
#define RM_TESLA_RSTCTRL_RST1_MASK                      (u32)(0x1)
#define RM_TESLA_RSTCTRL_RST1_OFFSET                    (u32)(0x0)
#define RM_TESLA_RSTCTRL_RST2_MASK                      (u32)(0x2)
#define RM_TESLA_RSTCTRL_RST2_OFFSET                    (u32)(0x1)

/******************RM_TESLA_RSTST***************/

#define RM_TESLA_RSTST_Clear_MASK                       (u32)(0x0F)

/******************RM_TESLA_TESLA_CONTEXT*******/

#define RM_TESLA_TESLA_CONTEXT_Clear_MASK               (u32)(0x701)

/******************PM_CORE_PWRSTCTRL************/

#define PRM_PM_CORE_PWRSTCTRL_PowerControl_OFFSET       (u32)(0x0)
#define PRM_PM_CORE_PWRSTCTRL_PowerControl_MASK         (u32)(0x3)

/******************PM_ALWON_SR_IVA_WKDEP********/

#define PM_ALWON_SR_IVA_WKDEP_MPU_OFFSET                (u32)(0x0)
#define PM_ALWON_SR_IVA_WKDEP_MPU_MASK                  (u32)(0x1)
#define PM_ALWON_SR_IVA_WKDEP_DUCATI_OFFSET             (u32)(0x1)
#define PM_ALWON_SR_IVA_WKDEP_DUCATI_MASK               (u32)(0x2)

/******************CM_TESLA_CLKSTCTRL***********/

#define CM_TESLA_CLKSTCTRL_Transition_OFFSET            (u32)(0x0)
#define CM_TESLA_CLKSTCTRL_Transition_MASK              (u32)(0x3)

/******************CM_TESLA_TESLA_CLKCTRL*******/
#define CM_TESLA_TESLA_CLKCTRL_STBY_MASK                (u32)(0x40000)
#define CM_TESLA_TESLA_CLKCTRL_STBY_OFFSET              (u32)(0x18)
#define CM_TESLA_TESLA_CLKCTRL_IDLE_MASK                (u32)(0x30000)
#define CM_TESLA_TESLA_CLKCTRL_IDLE_OFFSET              (u32)(0x16)
#define CM_TESLA_TESLA_CLKCTRL_MODMODE_OFFSET           (u32)(0x0)
#define CM_TESLA_TESLA_CLKCTRL_MODMODE_MASK             (u32)(0x03)

/******************CM1_ABE_CLKSTCTRL************/

#define CM1_ABE_CLKSTCTRL_Transition_OFFSET             (u32)(0x0)
#define CM1_ABE_CLKSTCTRL_Transition_MASK               (u32)(0x3)

/******************CM1_ABE_MCBSPX&TIMERX_CLKCTRL*/
#define CM1_ABE_CLKCTRL_OFFSET                          (u32)(0x0)
#define CM1_ABE_CLKCTRL_MASK                            (u32)(0x3)

/******************CM1_L4PER_CLKSTCTRL************/

#define CM1_L4PER_CLKSTCTRL_Transition_OFFSET             (u32)(0x0)
#define CM1_L4PER_CLKSTCTRL_Transition_MASK               (u32)(0x3)

/******************CM1_L4PER_MCBSPX&DMTIMERX_CLKCTRL*/
#define CM1_L4PER_CLKCTRL_OFFSET                          (u32)(0x0)
#define CM1_L4PER_CLKCTRL_MASK                            (u32)(0x3)

/******************CM_IVA_DVFS_PERF*************/
#define CM_IVA_DVFS_PERF_OFFSET                         (u32)(0x0)
#define CM_IVA_DVFS_PERF_MASK                           (u32)(0xFF)

/*****************************************************************************
* EXPORTED TYPES
******************************************************************************
*/

/* The following type definitions represent the 
* enumerated values for each bitfield 
*/

enum PRM_PM_TESLA_PWRSTCTRLE {
	PM_TESLA_PWRSTCTRLPowerStateOFF           = 0x0000,
	PM_TESLA_PWRSTCTRLPowerStateRET           = 0x0001,
	PM_TESLA_PWRSTCTRLPowerStateINACTIVE      = 0x0002,
	PM_TESLA_PWRSTCTRLPowerStateON            = 0x0003
};

#endif
#endif
