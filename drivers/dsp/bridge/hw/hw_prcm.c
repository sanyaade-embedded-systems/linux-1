/*
 * hw_prcm.c
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
 *  ======== hw_prcm.c ========
 *  Description:
 *      API definitions to configure PRCM (Power, Reset & Clocks Manager)
 *
 *! Revision History:
 *! ================
 *! 16 Feb 2003 sb: Initial version
 */

#include <GlobalTypes.h>
#include "PRCMRegAcM.h"
#include <hw_defs.h>
#include <hw_prcm.h>



static HW_STATUS HW_RST_WriteVal(const void __iomem *baseAddress,
				enum HW_RstModule_t r,
				enum HW_SetClear_t val);

HW_STATUS HW_RST_Reset(const void __iomem *baseAddress, enum HW_RstModule_t r)
{
	return HW_RST_WriteVal(baseAddress, r, HW_SET);
}

HW_STATUS HW_RST_UnReset(const void __iomem *baseAddress, enum HW_RstModule_t r)
{
	return HW_RST_WriteVal(baseAddress, r, HW_CLEAR);
}

static HW_STATUS HW_RST_WriteVal(const void __iomem *baseAddress,
				enum HW_RstModule_t r,
				enum HW_SetClear_t val)
{
	HW_STATUS status = RET_OK;

	switch (r) {
#ifdef OMAP44XX
	case HW_RST1_TESLA:
		PRM_TESLA_RSTCTRL_RST1_Write32(baseAddress, val);
		break;
	case HW_RST2_TESLA:
		PRM_TESLA_RSTCTRL_RST2_Write32(baseAddress, val);
		break;
#else
	case HW_RST1_IVA2:
		PRM_RSTCTRL_IVA2RST1_DSPWrite32(baseAddress, val);
		break;
	case HW_RST2_IVA2:
		PRM_RSTCTRL_IVA2RST2_DSPWrite32(baseAddress, val);
		break;
	case HW_RST3_IVA2:
		PRM_RSTCTRL_IVA2RST3_DSPWrite32(baseAddress, val);
		break;
#endif
	default:
		status = RET_FAIL;
		break;
	}
	return status;
}

#ifdef OMAP_3430
HW_STATUS HW_PWR_IVA2StateGet(const void __iomem *baseAddress,
		enum HW_PwrModule_t p, enum HW_PwrState_t *value)
{
	HW_STATUS status = RET_OK;
	u32 temp;

	switch (p) {
	case HW_PWR_DOMAIN_DSP:
		/* wait until Transition is complete */
		do {
			/* mdelay(1); */
			temp = PRCMPM_PWSTST_IVA2InTransitionRead32
				(baseAddress);

		} while (temp);
		temp = PRCMPM_PWSTST_IVA2ReadRegister32(baseAddress);
		*value = PRCMPM_PWSTST_IVA2PowerStateStGet32(temp);
		break;

	default:
		status = RET_FAIL;
		break;
	}
	return status;
}

HW_STATUS HW_PWRST_IVA2RegGet(const void __iomem *baseAddress, u32 *value)
{
	HW_STATUS status = RET_OK;

	*value = PRCMPM_PWSTST_IVA2ReadRegister32(baseAddress);

	return status;
}


HW_STATUS HW_PWR_IVA2PowerStateSet(const void __iomem *baseAddress,
				     enum HW_PwrModule_t p,
				     enum HW_PwrState_t value)
{
	HW_STATUS status = RET_OK;

	switch (p) {
	case HW_PWR_DOMAIN_DSP:
		switch (value) {
		case HW_PWR_STATE_ON:
			PRCMPM_PWSTCTRL_IVA2PowerStateWriteON32(baseAddress);
			break;
		case HW_PWR_STATE_RET:
			PRCMPM_PWSTCTRL_DSPPowerStateWriteRET32(baseAddress);
			break;
		case HW_PWR_STATE_OFF:
			PRCMPM_PWSTCTRL_IVA2PowerStateWriteOFF32(baseAddress);
			break;
		default:
			status = RET_FAIL;
			break;
		}
		break;

	default:
		status = RET_FAIL;
		break;
	}

	return status;
}

HW_STATUS HW_PWR_CLKCTRL_IVA2RegSet(const void __iomem *baseAddress,
				      enum HW_TransitionState_t val)
{
	HW_STATUS status = RET_OK;

	PRCMCM_CLKSTCTRL_IVA2WriteRegister32(baseAddress, val);

	return status;

}
#endif

HW_STATUS HW_RSTST_RegGet(const void __iomem *baseAddress,
		enum HW_RstModule_t m, u32 *value)
{
	HW_STATUS status = RET_OK;
#ifdef OMAP44XX
	*value = PRM_TESLA_RSTSTReadRegister32(baseAddress);

#else
	*value = PRCMRM_RSTST_DSPReadRegister32(baseAddress);
#endif

	return status;
}

HW_STATUS HW_RSTCTRL_RegGet(const void __iomem *baseAddress,
		enum HW_RstModule_t m, u32 *value)
{
	HW_STATUS status = RET_OK;
#ifdef OMAP44XX
	*value = PRM_TESLA_RSTCTRLReadRegister32(baseAddress);
#else
	*value = PRCMRM_RSTCTRL_DSPReadRegister32(baseAddress);
#endif


	return status;
}

#ifdef OMAP44XX

static HW_STATUS HW_CLK_WriteVal (const u32 baseAddress, enum HW_ClkModule_t c, enum HW_SetClear_t val);


HW_STATUS HW_CLK_Enable(const u32 baseAddress, enum HW_ClkModule_t c)
{
	return HW_CLK_WriteVal(baseAddress, c, HW_SET);
}

HW_STATUS HW_CLK_Disable(const u32 baseAddress, enum HW_ClkModule_t c)
{
	return HW_CLK_WriteVal(baseAddress, c, HW_CLEAR);
}

static HW_STATUS HW_CLK_WriteVal (const u32 baseAddress, enum HW_ClkModule_t c, enum HW_SetClear_t val)
{
	HW_STATUS status = RET_OK;
	u32 val_clk = HW_CLEAR;

	if (val == HW_SET)
		val_clk = 0x2;

	switch (c) {
	case HW_CLK_TESLA:
	CM_TESLA_TESLA_CLKCTRLWriteRegister32(baseAddress, val);
	break;
	case HW_CLK_MCBSP1:
	CM_ABEEN_MCBSP1Write32(baseAddress, val_clk);
	break;
	case HW_CLK_MCBSP2:
	CM_ABEEN_MCBSP2Write32(baseAddress, val_clk);
	break;
	case HW_CLK_MCBSP3:
	CM_ABEEN_MCBSP3Write32(baseAddress, val_clk);
	break;
	case HW_CLK_MCBSP4:
	CM_L4PEREN_MCBSP4Write32(baseAddress, val_clk);
	break;
	case HW_CLK_MCBSP5:
	CM_L4PEREN_MCBSP5Write32(baseAddress, val_clk);
	break;
	case HW_CLK_TIMER5:
	CM_ABEEN_TIMER5Write32(baseAddress, val_clk);
	break;
	case HW_CLK_TIMER6:
	CM_ABEEN_TIMER6Write32(baseAddress, val_clk);
	break;
	case HW_CLK_TIMER7:
	CM_ABEEN_TIMER7Write32(baseAddress, val_clk);
	break;
	case HW_CLK_TIMER8:
	CM_ABEEN_TIMER8Write32(baseAddress, val_clk);
	break;
	case HW_CLK_DMTIMER2:
	CM_L4PEREN_DMTIMER2Write32(baseAddress, val_clk);
	break;
	case HW_CLK_DMTIMER3:
	CM_L4PEREN_DMTIMER3Write32(baseAddress, val_clk);
	break;
	case HW_CLK_DMTIMER4:
	CM_L4PEREN_DMTIMER4Write32(baseAddress, val_clk);
	break;
	case HW_CLK_DMTIMER9:
	CM_L4PEREN_DMTIMER9Write32(baseAddress, val_clk);
	break;
	case HW_CLK_DMTIMER10:
	CM_L4PEREN_DMTIMER10Write32(baseAddress, val_clk);
	break;
	case HW_CLK_DMTIMER11:
	CM_L4PEREN_DMTIMER11Write32(baseAddress, val_clk);
	break;
	default:
	status = RET_FAIL;
	break;
	}

	return status;
}


HW_STATUS HW_PWRSTCTRL_RegGet(const u32 baseAddress, u32 *value)
{
	HW_STATUS status = RET_OK;

	*value = PRM_TESLA_PWRSTCTRLReadRegister32(baseAddress);

	return status;
}


HW_STATUS HW_PWR_PowerStateGet(const u32 baseAddress,
	enum HW_PwrModule_t p, enum  HW_PwrState_t *value)
{
	HW_STATUS status = RET_OK;
	switch (p) {
	case HW_PWR_DOMAIN_TESLA:
	*value = (enum HW_PwrState_t)PRM_TESLA_PWRSTSTGet32(baseAddress);
	break;

	default:
	status = RET_FAIL;
	break;
	}

	return status;
}

HW_STATUS HW_PWR_PowerStateSet(const u32 baseAddress,
	enum HW_PwrModule_t p, enum HW_PwrState_t value)
{
	HW_STATUS status = RET_OK;

	switch (p) {
	case HW_PWR_DOMAIN_TESLA:
		switch (value) {
		case HW_PWR_STATE_ON:
		PRM_TESLA_PWRSTCTRLWriteON32(baseAddress);
		break;
		case HW_PWR_STATE_INACT:
		PRM_TESLA_PWRSTCTRLWriteINACTIVE32(baseAddress);
		break;
		case HW_PWR_STATE_RET:
		PRM_TESLA_PWRSTCTRLWriteRET32(baseAddress);
		break;
		case HW_PWR_STATE_OFF:
		PRM_TESLA_PWRSTCTRLWriteOFF32(baseAddress);
		break;
		default:
		status = RET_FAIL;
		break;
		}
	break;
	case HW_PWR_DOMAIN_CORE:
		switch (value) {
		case HW_PWR_STATE_ON:
		case HW_PWR_STATE_INACT:
		case HW_PWR_STATE_RET:
		case HW_PWR_STATE_OFF:
		PRM_CORE_PWRSTCTRLWrite32(baseAddress, value);
		break;
		default:
		status = RET_FAIL;
		break;
		}
	break;
	default:
	status = RET_FAIL;
	break;
	}

	return status;
}

HW_STATUS HW_PWR_ForceStateSet(const u32 baseAddress, enum HW_PwrModule_t p,
	enum HW_TransitionState_t value)
{
	HW_STATUS status = RET_OK;

	switch (p) {
	case HW_PWR_DOMAIN_TESLA:
	CM_CLKSTCTRL_TESLAWriteRegister32(baseAddress, value);
	break;
	case HW_PWR_DOMAIN_ABE:
	CM_CLKSTCTRL_ABEWriteRegister32(baseAddress, value);
	break;
	case HW_PWR_DOMAIN_L4PER:
	CM_CLKSTCTRL_L4PERWriteRegister32(baseAddress, value);
	break;
	default:
	status = RET_FAIL;
	break;
	}

	return status;
}


#endif

