/*
 * hw_prcm.h
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

/*
 *  ======== hw_prcm.h ========
 *  Description:
 *      PRCM types and API declarations
 *
 *! Revision History:
 *! ================
 *! 16 Feb 2003 sb: Initial version
 */

#ifndef __HW_PRCM_H
#define __HW_PRCM_H

/* HW_ClkModule:  Enumerated Type used to specify the clock domain */

enum HW_ClkModule_t {
#ifdef OMAP44XX
/*CM1 ABE*/
	HW_CLK_TESLA,
	HW_CLK_AESS,
	HW_CLK_PDM,
	HW_CLK_DMIC,
	HW_CLK_MCASP,
	HW_CLK_MCBSP1,
	HW_CLK_MCBSP2,
	HW_CLK_MCBSP3,
	HW_CLK_SLIMBUS,
	HW_CLK_TIMER5,
	HW_CLK_TIMER6,
	HW_CLK_TIMER7,
	HW_CLK_TIMER8,
	HW_CLK_WDT3,
/*CM2 L4PER*/
	HW_CLK_DMTIMER2,
	HW_CLK_DMTIMER3,
	HW_CLK_DMTIMER4,
	HW_CLK_DMTIMER9,
	HW_CLK_DMTIMER10,
	HW_CLK_DMTIMER11,
	HW_CLK_MCBSP4,
	HW_CLK_MCBSP5,

#else
	/* DSP Domain */
	HW_CLK_DSP_CPU,
	HW_CLK_DSP_IPI_MMU,
	HW_CLK_IVA_ARM,
	HW_CLK_IVA_COP,	/* IVA Coprocessor */

	/* Core Domain */
	HW_CLK_FN_WDT4,	/* Functional Clock */
	HW_CLK_FN_WDT3,
	HW_CLK_FN_UART2,
	HW_CLK_FN_UART1,
	HW_CLK_GPT5,
	HW_CLK_GPT6,
	HW_CLK_GPT7,
	HW_CLK_GPT8,

	HW_CLK_IF_WDT4,	/* Interface Clock */
	HW_CLK_IF_WDT3,
	HW_CLK_IF_UART2,
	HW_CLK_IF_UART1,
	HW_CLK_IF_MBOX
#endif
} ;

enum HW_ClkSubsys_t {
	HW_CLK_DSPSS,
	HW_CLK_IVASS
} ;

/* HW_GPtimers:  General purpose timers */
enum HW_GPtimer_t {
	HW_GPT5 = 5,
	HW_GPT6 = 6,
	HW_GPT7 = 7,
	HW_GPT8 = 8
} ;


/* GP timers Input clock type:  General purpose timers */
enum HW_Clocktype_t {
	HW_CLK_32KHz = 0,
	HW_CLK_SYS   = 1,
	HW_CLK_EXT   = 2
} ;

/* HW_ClkDiv:  Clock divisors */
enum HW_ClkDiv_t {
	HW_CLK_DIV_1 = 0x1,
	HW_CLK_DIV_2 = 0x2,
	HW_CLK_DIV_3 = 0x3,
	HW_CLK_DIV_4 = 0x4,
	HW_CLK_DIV_6 = 0x6,
	HW_CLK_DIV_8 = 0x8,
	HW_CLK_DIV_12 = 0xC
} ;

/* HW_RstModule:  Enumerated Type used to specify the module to be reset */
enum HW_RstModule_t {
#ifdef OMAP44XX
	HW_RST1_TESLA,	/*Reset the DSP*/
	HW_RST2_TESLA,	/* Reset MMU */
#else
	HW_RST1_IVA2,	/* Reset the DSP */
	HW_RST2_IVA2,	/* Reset MMU and LEON HWa */
	HW_RST3_IVA2	/* Reset LEON sequencer */
#endif
} ;

/* HW_PwrModule:  Enumerated Type used to specify the power domain */
enum HW_PwrModule_t {
/* Domains */
	HW_PWR_DOMAIN_CORE,
	HW_PWR_DOMAIN_MPU,
	HW_PWR_DOMAIN_WAKEUP,
#ifdef OMAP44XX
	HW_PWR_DOMAIN_TESLA,
	HW_PWR_DOMAIN_ABE,
	HW_PWR_DOMAIN_L4PER,
#else
	HW_PWR_DOMAIN_DSP,
#endif

/* Sub-domains */
	HW_PWR_DSP_IPI,	/* IPI = Intrusive Port Interface */
	HW_PWR_IVA_ISP	 /* ISP = Intrusive Slave Port */
} ;

enum HW_PwrState_t {
	HW_PWR_STATE_OFF,
	HW_PWR_STATE_RET,
	HW_PWR_STATE_INACT,
	HW_PWR_STATE_ON = 3
} ;

enum HW_ForceState_t {
	HW_FORCE_OFF,
	HW_FORCE_ON
} ;

enum HW_IdleState_t {
	HW_ACTIVE,
	HW_STANDBY
} ;

enum HW_TransitionState_t {
	HW_AUTOTRANS_DIS,
	HW_SW_SUP_SLEEP,
	HW_SW_SUP_WAKEUP,
	HW_AUTOTRANS_EN
} ;

#ifdef OMAP44XX
enum HW_RetState_t {
	HW_RETSTATE_LOGIC,
	HW_RETSTATE_L1,
	HW_RETSTATE_L2
};

enum HW_WeakUpDep_t {
	HW_WKUP_IVA_MPU,
	HW_WKUP_IVA_DUCATI
};

enum HW_IvaDVFS_t {
	HW_DVFS_PERF_TESLA,
	HW_DVFS_PERF_IVAHD,
	HW_DVFS_PERF_ABE,
	HW_DVFS_RESULT,
	HW_DVFS_CURRENT
};

enum HW_IdleStatus_t {
	HW_TESLA_FULLFUNC = 0x0, /* Module is fully functional, including OCP*/
	HW_TESLA_TRANSITIONING, /* Transitioning (weakup, sleep or sleep abortion)*/
	HW_TESLA_IDLE,/* Idle mode(only OCP)*/
	HW_TESLA_DISABLED/*Module is disabled and cannot be accessed*/
};
#endif

extern HW_STATUS HW_RST_Reset(const void __iomem *baseAddress,
					enum HW_RstModule_t r);

extern HW_STATUS HW_RST_UnReset(const void __iomem *baseAddress,
					enum HW_RstModule_t r);

extern HW_STATUS HW_RSTCTRL_RegGet(const void __iomem *baseAddress,
					enum HW_RstModule_t p,
					u32 *value);
extern HW_STATUS HW_RSTST_RegGet(const void __iomem *baseAddress,
					enum HW_RstModule_t p, u32 *value);

extern HW_STATUS HW_PWR_PowerStateSet(const u32 baseAddress,
					enum HW_PwrModule_t p,
					enum HW_PwrState_t value);

extern HW_STATUS HW_CLK_SetInputClock(const u32 baseAddress,
					enum HW_GPtimer_t gpt,
					enum HW_Clocktype_t c);

#ifdef OMAP_3430
extern HW_STATUS HW_PWR_IVA2StateGet(const void __iomem *baseAddress,
					enum HW_PwrModule_t p,
					enum HW_PwrState_t *value);

extern HW_STATUS HW_PWRST_IVA2RegGet(const void __iomem *baseAddress,
						u32 *value);

extern HW_STATUS HW_PWR_IVA2PowerStateSet(const void __iomem *baseAddress,
					    enum HW_PwrModule_t p,
					    enum HW_PwrState_t value);

extern HW_STATUS HW_PWR_CLKCTRL_IVA2RegSet(const void __iomem *baseAddress,
					     enum HW_TransitionState_t val);
#endif

#ifdef OMAP44XX
extern HW_STATUS HW_CLK_Enable(const u32 baseAddress,
				enum HW_ClkModule_t c);

extern HW_STATUS HW_CLK_Disable(const u32 baseAddress,
				enum HW_ClkModule_t c);

extern HW_STATUS HW_CLK_AutoIdleEnable(const u32 baseAddress,
				enum HW_ClkModule_t c);

extern HW_STATUS HW_CLK_AutoIdleDisable(const u32 baseAddress,
				enum HW_ClkModule_t c);
#if 0
extern HW_STATUS HW_RSTCTRL_RegGet(const u32 baseAddress, u32 *value);

extern HW_STATUS HW_RSTST_RegGet(const u32 baseAddress, u32 *value);
#endif

extern HW_STATUS HW_RSTST_RegClear(const u32 baseAddress);

extern HW_STATUS HW_PWRSTCTRL_RegGet(const u32 baseAddress, u32 *value);

extern HW_STATUS HW_PWR_PowerStateGet(const u32 baseAddress,
				enum HW_PwrModule_t p, enum HW_PwrState_t *value);

extern HW_STATUS HW_PWR_PowerStateSet(const u32 baseAddress, enum HW_PwrModule_t p,
				enum HW_PwrState_t value);
#if 0

extern HW_STATUS HW_PWR_RetentionStateSet(const u32 baseAddress,
				enum HW_PwrModule_t p, enum HW_RetState_t src, enum HW_SetClear_t  value);
#endif

extern HW_STATUS HW_PWRST_RegGet(const u32 baseAddress, enum HW_PwrModule_t p, u32 *value);


extern HW_STATUS HW_PWR_ForceStateSet(const u32 baseAddress, enum HW_PwrModule_t p,
				enum HW_TransitionState_t value);

extern HW_STATUS HW_TESLA_RST_WriteVal(const u32 baseAddress, enum HW_RstModule_t p,
				enum HW_SetClear_t value);
#if 0
extern HW_STATUS HW_TESLA_CONTEXT_RegGet(const u32 baseAddress, u32 *value);

extern HW_STATUS HW_TESLA_CONTEXT_ClrSet(const u32 baseAddress);
#endif

#endif

#endif  /* __HW_PRCM_H */
