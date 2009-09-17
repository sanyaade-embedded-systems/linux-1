/*
 * dmm_ll_drv.c
 *
 * DMM driver support functions for TI OMAP processors.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/module.h>
#include <linux/io.h>
#include "dmm_def.h"
#include "dmm_2d_alloc.h"
#include "dmm_prv.h"
#include "dmm_reg.h"
MODULE_LICENSE("GPL v2");

#define tilerdump(x) /* printk(KERN_NOTICE "%s::%s():%d: %lx\n",
			__FILE__, __func__, __LINE__, (unsigned long)x); */

/* ========================================================================== */
/**
 *  dmm_tiler_alias_orientation_set()
 *
 * @brief  Set specific TILER alias orientation setting per initiator ID
 *  (alias view).
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 *  which alias view will be editted.
 *
 * @param viewOrientation - dmmViewOrientT - [in] New alias view orientation
 *  setting.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmViewOrientT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_tiler_alias_orientation_set(signed long initiatorID,
		struct dmmViewOrientT viewOrientation)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
		(0x220ul));
	__raw_writel(0x88888888, reg);

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
		(0x220ul+0x4));
	__raw_writel(0x88888888, reg);

	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_tiler_alias_orientation_get()
 *
 * @brief  Get specific TILER alias orientation setting per initiator ID
 *  (alias view).
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 *  which alias view will be editted.
 *
 * @param viewOrientation - dmmViewOrientT* - [out] Pointer to write alias view
 * orientation setting to.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmViewOrientT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_tiler_alias_orientation_get(signed long initiatorID,
		struct dmmViewOrientT *viewOrientation)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_peg_priority_set()
 *
 * @brief  Set specific PEG priority setting per initiator ID.
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 *  which priority will be editted.
 *
 * @param prio - unsigned long - [in] New priority setting.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_peg_priority_set(signed long initiatorID,
				     unsigned long prio)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_peg_priority_get()
 *
 * @brief  Get specific PEG priority setting per initiator ID.
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 *  which priority will be editted.
 *
 * @param prio - unsigned long* - [out] Poitner to write the priority setting.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_peg_priority_get(signed long initiatorID,
				     unsigned long *prio)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_area_refill()
 *
 * @brief  Initiate a PAT area refill (or terminate an ongoing - consult
 * documentation).
 *
 * @param patDesc - PATDescrT* - [in] Pointer to a PAT area descriptor that's
 * needed to extract settings from for the refill procedure initation.
 *
 * @param dmmPatAreaSel - signed long - [in] Selects which PAT area will be
 *  configured for a area refill procedure.
 *
 * @param refillType - dmmPATRefillMethodT - [in] Selects the refill method -
 * manual or automatic.
 *
 * @param forcedRefill - int - [in] Selects if forced refill should be used
 * effectively terminating any ongoing area refills related to the selected area
 *
 * @return errorCodeT
 *
 * @pre If forced mode is not used, no refills should be ongoing for the
 *  selected area - error status returned if this occurs.
 *
 * @post If non valid data is provided for patDesc and the refill engines fail
 * to perform the request, an error status is returned.
 *
 * @see errorCodeT,  PATDescrT, dmmPATRefillMethodT, int for further detail
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_area_refill(struct PATDescrT *patDesc,
				    signed long dmmPatAreaSel,
				    enum dmmPATRefillMethodT refillType,
				    int forcedRefill)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;
	unsigned long regval = 0xffffffff;
	unsigned long writeval = 0xffffffff;
	unsigned long f = 0xffffffff; /* field */
	unsigned long fp = 0xffffffff; /* field position */

	struct dmmPATStatusT areaStat;

	tilerdump(0);
	if (forcedRefill == 0) {
		eCode = dmm_pat_refill_area_status_get(
				dmmPatAreaSel, &areaStat);

		if (eCode == DMM_NO_ERROR) {
			if (areaStat.ready == 0 || areaStat.engineRunning) {
				printk(KERN_ALERT "hw not ready\n");
				eCode = DMM_HRDW_NOT_READY;
			}
		}
	}

	if (dmmPatAreaSel < 0 || dmmPatAreaSel > 3) {
		eCode = DMM_WRONG_PARAM;
	} else if (eCode == DMM_NO_ERROR) {
		if (refillType == AUTO) {
			reg = (void __iomem *)
				((unsigned long)dmm_virt_base_addr |
				(0x500ul + 0x0));
			regval = __raw_readl(reg);
			f  = BITFIELD(31, 4);
			fp = 4;
			writeval = (regval & (~(f))) |
				   ((((unsigned long)patDesc) << fp) & f);
			__raw_writel(writeval, reg);
		} else if (refillType == MANUAL) {
			reg = (void __iomem *)
				((unsigned long)dmm_virt_base_addr |
				(0x500ul + 0x0));
			regval = __raw_readl(reg);
			f  = BITFIELD(31, 4);
			fp = 4;
			writeval = (regval & (~(f))) |
				   ((((unsigned long)NULL) << fp) & f);
			__raw_writel(writeval, reg);
			reg = (void __iomem *)
				((unsigned long)dmm_virt_base_addr |
				(0x500ul + 0x4));
			regval = __raw_readl(reg);
			f  = BITFIELD(30, 24);
			fp = 24;
			writeval = (regval & (~(f))) |
				   ((((char)patDesc->area.y1) << fp) & f);
			__raw_writel(writeval, reg);

			regval = __raw_readl(reg);
			f  = BITFIELD(23, 16);
			fp = 16;
			writeval = (regval & (~(f))) |
				   ((((char)patDesc->area.x1) << fp) & f);
			__raw_writel(writeval, reg);

			regval = __raw_readl(reg);
			f  = BITFIELD(14, 8);
			fp = 8;
			writeval = (regval & (~(f))) |
				   ((((char)patDesc->area.y0) << fp) & f);
			__raw_writel(writeval, reg);

			regval = __raw_readl(reg);
			f  = BITFIELD(7, 0);
			fp = 0;
			writeval = (regval & (~(f))) |
				   ((((char)patDesc->area.x0) << fp) & f);
			__raw_writel(writeval, reg);
			/* Apply 4 bit lft shft to counter the 4 bit rt shft */
			reg = (void __iomem *)
			((unsigned long)dmm_virt_base_addr | (0x500ul + 0xc));
			regval = __raw_readl(reg);
			f  = BITFIELD(31, 4);
			fp = 4;
			writeval = (regval & (~(f))) |
				((((unsigned long)(patDesc->data >> 4)) <<
								fp) & f);
			__raw_writel(writeval, reg);
			reg = (void __iomem *)
				((unsigned long)dmm_virt_base_addr |
				(0x500ul + 0x8));
			regval = __raw_readl(reg);
			f  = BITFIELD(31, 28);
			fp = 28;
			writeval = (regval & (~(f))) |
				   (((patDesc->ctrl.initiator) << fp) & f);
			__raw_writel(writeval, reg);

			f  = BITFIELD(16, 16);
			fp = 16;
			writeval = (regval & (~(f))) |
				   (((patDesc->ctrl.sync) << fp) & f);
			__raw_writel(writeval, reg);

			f  = BITFIELD(6, 4);
			fp = 4;
			writeval = (regval & (~(f))) |
				   (((patDesc->ctrl.direction) << fp) & f);
			__raw_writel(writeval, reg);

			f  = BITFIELD(0, 0);
			fp = 0;
			writeval = (regval & (~(f))) |
				   (((patDesc->ctrl.start) << fp) & f);
			__raw_writel(writeval, reg);
		} else {
			eCode = DMM_WRONG_PARAM;
		}
		if (eCode == DMM_NO_ERROR) {
			eCode = dmm_pat_refill_area_status_get(dmmPatAreaSel,
							       &areaStat);

			if (eCode == DMM_NO_ERROR) {
				if (areaStat.validDescriptor == 0) {
					eCode = DMM_HRDW_CONFIG_FAILED;
					printk(KERN_ALERT "hw config fail\n");
				}
			}

			while (!areaStat.done && !areaStat.ready &&
					eCode == DMM_NO_ERROR) {
				eCode = dmm_pat_refill_area_status_get(
						dmmPatAreaSel,
						&areaStat);
			}

			if (areaStat.error) {
				eCode = DMM_HRDW_CONFIG_FAILED;
				printk(KERN_ALERT "hw config fail\n");
			}
		}
	}
	tilerdump(0);
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_refill_area_status_get()
 *
 * @brief  Gets the status for the selected PAT area.
 *
 * @param dmmPatAreaSel - signed long - [in] Selects which PAT area status will
 *  be queried.
 *
 * @param areaStatus - dmmPATStatusT* - [out] Structure containing the PAT area
 * status that will be filled by dmmPatRefillAreaStatusGet().
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post If the query fails the provided areaStatus structure is not updated
 *  at all!
 *
 * @see errorCodeT, dmmPATStatusT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_refill_area_status_get(signed long dmmPatAreaStatSel,
		struct dmmPATStatusT *areaStatus)
{
	enum errorCodeT eCode = DMM_NO_ERROR;

	unsigned long stat = 0xffffffff;
	void __iomem *statreg = (void __iomem *)
				((unsigned long)dmm_virt_base_addr | 0x4c0ul);

	if (dmmPatAreaStatSel >= 0 && dmmPatAreaStatSel <= 3) {
		stat = __raw_readl(statreg);

		areaStatus->remainingLinesCounter = stat & BITFIELD(24, 16);
		areaStatus->error = (enum dmmPATStatusErrT)
				    (stat & BITFIELD(15, 10));
		areaStatus->linkedReconfig = stat & BITFIELD(4, 4);
		areaStatus->done = stat & BITFIELD(3, 3);
		areaStatus->engineRunning = stat & BITFIELD(2, 2);
		areaStatus->validDescriptor = stat & BITFIELD(1, 1);
		areaStatus->ready = stat & BITFIELD(0, 0);

	} else {
		eCode = DMM_WRONG_PARAM;
		printk(KERN_ALERT "wrong parameter\n");
	}

	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_refill_engine_config()
 *
 * @brief  Configure the selected PAT refill engine.
 *
 * @param dmmPatEngineSel - signed long - [in] Selects which PAT engine will be
 *  configured.
 *
 * @param engineMode - dmmPATEngineAccessT - [in] New engine mode.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATEngineAccessT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_refill_engine_config(signed long dmmPatEngineSel,
		enum dmmPATEngineAccessT engineMode)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
								(0x410ul));
	__raw_writel(0x00000000, reg);

	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_refill_engine_config_get()
 *
 * @brief  Get the selected PAT refill engine configuration.
 *
 * @param dmmPatEngineSel - signed long - [in] Selects which PAT engine will be
 *  configured.
 *
 * @param engineMode - dmmPATEngineAccessT* - [out] Pointer to write the engine
 *  mode.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATEngineAccessT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_refill_engine_config_get(signed long dmmPatEngineSel,
		enum dmmPATEngineAccessT *engineMode)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_view_set()
 *
 * @brief  Set specific PAT initiator view settings.
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 *  which PAT view will be editted.
 *
 * @param patViewIndx - signed long - [in] New view map setting.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_view_set(signed long initiatorID,
				 signed long patViewIndx)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;

	if (patViewIndx < 0 || patViewIndx > 3) {
		eCode = DMM_WRONG_PARAM;
	} else {
		reg = (void __iomem *)
			((unsigned long)dmm_virt_base_addr | (0x420ul));
		__raw_writel(0xDDDDDDDD, reg);

		reg = (void __iomem *)
			((unsigned long)dmm_virt_base_addr | (0x420ul+0x4));
		__raw_writel(0xDDDDDDDD, reg);
	}

	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_view_get()
 *
 * @brief  Get specific PAT initiator view settings.
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 * which PAT view will be editted.
 *
 * @param patViewIndx - signed long* - [out] Pointer to write the view map
 *  setting.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_view_get(
	signed long initiatorID, signed long *patViewIndx)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_pat_view_map_config()
 *
 * @brief  Configure selected PAT view map.
 *
 * @param patViewMapIndx - signed long - [in] Index of the selected PAT view
 *  map.
 *
 * @param memoryAccessMode - dmmMemoryAccessT - [in] Type of memory access to
 *  perform through this view.
 *
 * @param contX - unsigned long - [in] CONT_x register value.
 *
 * @param transType - dmmPATTranslationT - [in] Address translation schemes.
 *
 * @param dmmPATViewBase - unsigned long - [in] View map base address
 *  (31-bit only considered).
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmMemoryAccessT, dmmPATTranslationT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_view_map_config(signed long patViewMapIndx,
					enum dmmMemoryAccessT memoryAccessMode,
					unsigned long contX,
					enum dmmPATTranslationT transType,
					unsigned long dmmPATViewBase)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;
	unsigned long regval = 0xffffffff;

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
								(0x460ul));
	regval = __raw_readl(reg);
	__raw_writel(0xFFFFFFFF, reg);

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
								(0x440ul));
	regval = __raw_readl(reg);
	__raw_writel(0x80808080, reg);

	return eCode;
}

enum errorCodeT dmm_pat_view_map_config_get(signed long patViewMapIndx,
		enum dmmMemoryAccessT memoryAccessMode,
		unsigned long *contX,
		enum dmmPATTranslationT *transType,
		unsigned long *dmmPATViewBase)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_lisa_memory_map_config()
 *
 * @brief  Configure selected LISA memory map.
 *
 * @param lisaMemMapIndx - signed long - [in] Index of the selected LISA memory
 *  map.
 *
 * @param sysSize - dmmMemSectionSizeT - [in] Size of the memory section.
 *
 * @param sdrcIntl - dmmMemSdrcIntlModeT - [in] SDRAM controller interleaving
 *  mode
 *
 * @param sdrcAddrspc - unsigned long - [in] SDRAM controller address space.
 *
 * @param sdrcMap - dmmMemSectionMappingT - [in] SDRAM controller mapping.
 *
 * @param sdrcAddr - unsigned long - [in] SDRAM controller address MSB.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmMemSectionSizeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_lisa_memory_map_config(signed long lisaMemMapIndx,
		unsigned long sysAddr,
		enum dmmMemSectionSizeT sysSize,
		enum dmmMemSdrcIntlModeT sdrcIntl,
		unsigned long sdrcAddrspc,
		enum dmmMemSectionMappingT sdrcMap,
		unsigned long sdrcAddr)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;

	if (lisaMemMapIndx != 0)
		printk(KERN_WARNING "lisaMemMapIndx != 0\n");

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
					(0x40ul + (0x4*lisaMemMapIndx)));
	__raw_writel(0x80710100, reg);

	return eCode;
}

enum errorCodeT dmm_lisa_memory_map_config_get(signed long lisaMemMapIndx,
		unsigned long *sysAddr,
		enum dmmMemSectionSizeT *sysSize,
		enum dmmMemSdrcIntlModeT *sdrcIntl,
		unsigned long *sdrcAddrspc,
		enum dmmMemSectionMappingT *sdrcMap,
		unsigned long *sdrcAddr)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

/* ========================================================================== */
/**
 *  dmm_engage_lisa_lock()
 *
 * @brief  Sets the LISA lock register preventing further writting to the
 * LISA memory map registers.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post Only a software reset of the DMM module can clear the lock bit.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_engage_lisa_lock()
{
	void __iomem *reg = NULL;
	unsigned long regval = 0xffffffff;
	unsigned long writeval = 0xffffffff;
	unsigned long f = 0xffffffff; /* field */
	unsigned long fp = 0xffffffff; /* field position */

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
								(0x1cul));
	regval = __raw_readl(reg);

	f  = BITFIELD(0, 0);
	fp = 0;
	writeval = (regval & (~(f))) | (((0x1ul) << fp) & f);
	__raw_writel(writeval, reg);

	return DMM_NO_ERROR;
}

enum errorCodeT dmm_sys_config_set(unsigned long dmmIdleMode)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

enum errorCodeT dmm_sys_config_get(unsigned long *dmmIdleMode)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

enum errorCodeT dmm_pat_irq_mode_set(struct dmmPATIrqEventsT *patIrqEvnt)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	void __iomem *reg = NULL;

	reg = (void __iomem *)((unsigned long)dmm_virt_base_addr |
								(0x4a0ul));

	__raw_writel(0x0000FFFF, reg); /* clr all irq registers */
	return eCode;
}

enum errorCodeT dmm_pat_irq_mode_get(struct dmmPATIrqEventsT *patIrqEvnt)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}

enum errorCodeT dmm_pat_irq_status_get(struct dmmPATIrqEventsT *patIrqEvnt,
				       int clrEvents)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	return eCode;
}
