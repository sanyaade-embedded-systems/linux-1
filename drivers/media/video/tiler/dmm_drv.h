/*
 * dmm_drv.h
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

#ifndef _DMM_DRV_H
#define _DMM_DRV_H

#include "dmm_def.h"

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
		struct dmmViewOrientT viewOrientation);

/* ========================================================================== */
/**
 *  dmm_tiler_alias_orientation_get()
 *
 * @brief  Set specific TILER alias orientation setting per initiator ID
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
		struct dmmViewOrientT *viewOrientation);

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
					unsigned long prio);

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
					unsigned long *prio);

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
 * effectively terminating any ongoing area refills related to the selected
 * area.
 *
 * @return errorCodeT
 *
 * @pre If forced mode is not used, no refills should be ongoing for the
 * selected
 * area - error status returned if this occurs.
 *
 * @post If non valid data is provided for patDesc and the refill engines fail
 * to perform the request, an error status is returned.
 *
 * @see errorCodeT,  PATDescrT, dmmPATRefillMethodT, int for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_area_refill(struct PATDescrT *patDesc,
				 signed long dmmPatAreaSel,
				 enum dmmPATRefillMethodT refillType,
				 int forcedRefill);

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
 * @post If the query fails the provided areaStatus structure is not updated at
 * all!
 *
 * @see errorCodeT, dmmPATStatusT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_refill_area_status_get(signed long dmmPatAreaStatSel,
		struct dmmPATStatusT *areaStatus);

/* ========================================================================== */
/**
 *  dmm_pat_refill_engine_config()
 *
 * @brief  Configure the selected PAT refill engine.
 *
 * @param dmmPatEngineSel - signed long - [in] Selects which PAT engine will
 *  be configured.
 *
 * @param engineMode - dmmPATEngineAccessT - [in] New engine mode.
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
		enum dmmPATEngineAccessT engineMode);

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
		enum dmmPATEngineAccessT *engineMode);

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
			      signed long patViewIndx);

/* ========================================================================== */
/**
 *  dmm_pat_view_get()
 *
 * @brief  Get specific PAT initiator view settings.
 *
 * @param initiatorID - signed long - [in] OCP id of DMM transfer initiator
 *  which PAT view will be editted.
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
enum errorCodeT dmm_pat_view_get(signed long initiatorID,
			      signed long *patViewIndx);

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
				    unsigned long dmmPATViewBase);

/* ========================================================================== */
/**
 *  dmm_pat_view_map_config_get()
 *
 * @brief  Get selected PAT view map configuration.
 *
 * @param patViewMapIndx - signed long - [in] Index of the selected PAT view
 *  map.
 *
 * @param memoryAccessMode - dmmMemoryAccessT - [int] Type of memory access
 * that configuration is queried.
 *
 * @param contX - unsigned long* - [out] Pointer to write the CONT_x register
 *  value.
 *
 * @param transType - dmmPATTranslationT* - [out] Pointer to write the
 * address translation schemes.
 *
 * @param dmmPATViewBase - unsigned long* - [out] Pointer to write the view
 *  map base address.
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
enum errorCodeT dmm_pat_view_map_config_get(signed long patViewMapIndx,
				       enum dmmMemoryAccessT memoryAccessMode,
				       unsigned long *contX,
				       enum dmmPATTranslationT *transType,
				       unsigned long *dmmPATViewBase);

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
				       unsigned long sdrcAddr);

/* ========================================================================== */
/**
 *  dmm_lisa_memory_map_config_get()
 *
 * @brief  Get selected LISA memory map configuration.
 *
 * @param lisaMemMapIndx - signed long - [in] Index of the selected LISA
 *  memory map.
 *
 * @param sysSize - dmmMemSectionSizeT* - [out] Pointer to write size of
 *  the memory section.
 *
 * @param sdrcIntl - dmmMemSdrcIntlModeT* - [out] Pointer to write SDRAM
 *  controller interleaving mode.
 *
 * @param sdrcAddrspc - unsigned long* - [out] Pointer to write SDRAM
 *  controller address space.
 *
 * @param sdrcMap - dmmMemSectionMappingT* - [out] Pointer to write SDRAM
 *  controller mapping.
 *
 * @param sdrcAddr - unsigned long* - [out] Pointer to write SDRAM
 *  controller address MSB.
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
enum errorCodeT dmm_lisa_memory_map_config_get(signed long lisaMemMapIndx,
		unsigned long *sysAddr,
		enum dmmMemSectionSizeT *sysSize,
		enum dmmMemSdrcIntlModeT *sdrcIntl,
		unsigned long *sdrcAddrspc,
		enum dmmMemSectionMappingT *sdrcMap,
		unsigned long *sdrcAddr);

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
enum errorCodeT dmm_engage_lisa_lock();

/* ========================================================================== */
/**
 *  dmm_sys_config_set()
 *
 * @brief  DMM clock management configuration.
 *
 * @param dmmIdleMode - unsigned long - [in] Idle mode.
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
enum errorCodeT dmm_sys_config_set(unsigned long dmmIdleMode);

/* ========================================================================== */
/**
 *  dmm_sys_config_get()
 *
 * @brief  DMM clock management configuration.
 *
 * @param dmmIdleMode - unsigned long* - [out] Idle mode status.
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
enum errorCodeT dmm_sys_config_get(unsigned long *dmmIdleMode);

/* ========================================================================== */
/**
 *  dmm_pat_irq_mode_set()
 *
 * @brief  Enable/disable PAT interrupt events.
 *
 * @param patIrqEvnt - dmmPATIrqEventsT* - [in] Interrupt events to set.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATIrqEventsT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_irq_mode_set(struct dmmPATIrqEventsT *patIrqEvnt);

/* ========================================================================== */
/**
 *  dmm_pat_irq_mode_get()
 *
 * @brief  Enable/disable PAT interrupt events.
 *
 * @param patIrqEvnt - dmmPATIrqEventsT* - [in/out] Get interrupt events masks.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATIrqEventsT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_irq_mode_get(struct dmmPATIrqEventsT *patIrqEvnt);

/* ========================================================================== */
/**
 *  dmm_pat_irq_status_get()
 *
 * @brief  Get the statuses of PAT interrupt events.
 *
 * @param patIrqEvnt - dmmPATIrqEventsT* - [in/out] Interrupt events statuses.
 *
 * @param clrEvents - int - [in] Celar events upon retrieval.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATIrqEventsT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_irq_status_get(struct dmmPATIrqEventsT *patIrqEvnt,
				   int clrEvents);

/* ========================================================================== */
/**
 *  dmm_module_config()
 *
 * @brief  Configure selected DMM modules.
 *
 * @param tilerConf - dmmTILERConfigLstT* - [in] A NULL termintated linked list
 * of all configurations for the specific DMM module or NULL if not to be
 * configured.
 *
 * @param pegConf - dmmPEGConfigLstT* - [in] A NULL termintated linked list
 * of all configurations for the specific DMM module or NULL if not to be
 * configured.
 *
 * @param lisaConf - dmmLISAConfigLstT* - [in] A NULL termintated linked list
 * of all configurations for the specific DMM module or NULL if not to be
 * configured.
 *
 * @param patEngineConf - dmmPATEngineConfigLstT* - [in] A NULL termintated
 * linked list of all configurations for the specific DMM module or NULL if
 * not to be configured.
 *
 * @param patViewCOnf - dmmPATViewConfigLstT* - [in] A NULL termintated
 * linked list of all configurations for the specific DMM module or NULL
 * if not to be configured.
 *
 * @param patViewMapConf - dmmPATViewMapConfigLstT* - [in] A NULL termintated
 * linked list of all configurations for the specific DMM module or NULL if
 * not to be configured.
 *
 * @param dmmSysConfig - unsigned long* - [in] Clock configuration of DMM,
 * NULL if not to be set.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmTILERConfigLstT, dmmPEGConfigLstT,
 * dmmLISAConfigLstT, dmmPATEngineConfigLstT, dmmPATViewConfigLstT,
 * dmmPATViewMapConfigLstT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_module_config(struct dmmTILERConfigLstT *tilerConf,
				struct dmmPEGConfigLstT *pegConf,
				struct dmmLISAConfigLstT *lisaConf,
				struct dmmPATEngineConfigLstT *patEngineConf,
				struct dmmPATViewConfigLstT *patViewCOnf,
				struct dmmPATViewMapConfigLstT *patViewMapConf,
				unsigned long *dmmSysConfig);

/* ========================================================================== */
/**
 *  dmm_module_get_config()
 *
 * @brief  Read the configuration of selected DMM modules.
 *
 * @param tilerConf - dmmTILERConfigLstT* - [out] A NULL termintated linked list
 * of all configurations for the specific DMM module to be read to.
 *
 * @param pegConf - dmmPEGConfigLstT* - [out] A NULL termintated linked list
 * of all configurations for the specific DMM module to be read to.
 *
 * @param lisaConf - dmmLISAConfigLstT* - [out] A NULL termintated linked list
 * of all configurations for the specific DMM module to be read to.
 *
 * @param patEngineConf - dmmPATEngineConfigLstT* - [out] A NULL termintated
 *  linked list of all configurations for the specific DMM module to be read to.
 *
 * @param patViewCOnf - dmmPATViewConfigLstT* - [out] A NULL termintated linked
 *  list of all configurations for the specific DMM module to be read to.
 *
 * @param patViewMapConf - dmmPATViewMapConfigLstT* - [out] A NULL termintated
 *  linked list of all configurations for the specific DMM module to be read to.
 *
 * @param patAreaStatus - dmmPATStatusLstT* - [out] A NULL termintated linked
 *  list of all PAT area status' to be read.
 *
 * @param dmmSysConfig - unsigned long* - [out] Clock configuration of DMM,
 *  NULL if not to be read.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmTILERConfigLstT, dmmPEGConfigLstT,
 * dmmLISAConfigLstT, dmmPATEngineConfigLstT, dmmPATViewConfigLstT,
 * dmmPATViewMapConfigLstT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_module_get_config(struct dmmTILERConfigLstT *tilerConf,
			struct dmmPEGConfigLstT *pegConf,
			struct dmmLISAConfigLstT *lisaConf,
			struct dmmPATEngineConfigLstT *patEngineConf,
			struct dmmPATViewConfigLstT *patViewCOnf,
			struct dmmPATViewMapConfigLstT *patViewMapConf,
			struct dmmPATStatusLstT *patAreaStatus,
			unsigned long *dmmSysConfig);

/* ========================================================================== */
/**
 *  dmm_pat_irq_config_set()
 *
 * @brief  Configures PAT interrupt masks.
 *
 * @param irqMaskConf - dmmPATIrqConfigLstT* - [in] A NULL termintated linked
 *  list of all interrupt masks.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATIrqConfigLstT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_irq_config_set(struct dmmPATIrqConfigLstT *irqMaskConf);

/* ========================================================================== */
/**
 *  dmm_pat_irq_config_get()
 *
 * @brief  Configures PAT interrupt masks.
 *
 * @param irqMaskConf - dmmPATIrqConfigLstT* - [in/out] A NULL termintated
 *  linked list of all interrupt masks.
 *
 * @param irqStatusConf - dmmPATIrqConfigLstT* - [in/out] A NULL termintated
 *  linked list of all interrupt statuses to read.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT, dmmPATIrqConfigLstT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_pat_irq_config_get(struct dmmPATIrqConfigLstT *irqMaskConf,
				   struct dmmPATIrqConfigLstT *irqStatusConf);

/* ========================================================================== */
/**
 *  dmm_instance_init()
 *
 * @brief  Initializes the Tiler cotnext.
 *
 * @param dmmInstanceCtxPtr - void * - [in] Tiler context instance.
 *
 * @param contXSize - signed long - [in] Tiler container width.
 *
 * @param contYSize - signed long - [in] Tiler container height.
 *
 * @param hMSP - MSP_HANDLE - [in] MSP handle related to this dmm_drv cotnext.
 *
 * @param usrAppData - void * - [in] Pointer to user specific data structure.
 *
 * @param usrCallback - MSP_usrCallback - [in] Pointer to callback supplied by
 * the user for notificiation events (interupts).
 *
 * @return int True if operation succeded.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see dmmTILERContCtxT for further detail.
 */
/* ========================================================================== */
int dmm_instance_init(void *dmmInstanceCtxPtr,
		    signed long contXSize,
		    signed long contYSize,
		    void *hMSP,
		    void *usrAppData);

/* ========================================================================== */
/**
 *  dmm_instance_deinit()
 *
 * @brief  Deinitializes the Tiler cotnext.
 *
 * @param dmmInstanceCtxPtr - void * - [in] Tiler context instance.
 *
 * @return int True if operation succeded.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see dmmTILERContCtxT for further detail.
 */
/* ========================================================================== */
int dmm_instance_deinit(void *dmmInstanceCtxPtr);

/* ========================================================================== */
/**
 *  dmm_copy2tiler_alias_view()
 *
 * @brief  Auxiliary function for copying data to the Tiler alias view.
 *
 * @param destPtr - void * - [in] Destination pointer in Tiler alias view.
 *
 * @param srcPtr - void * - [in] Data source pointer.
 *
 * @param width - signed long - [in] Data width.
 *
 * @param height - signed long - [in] Data height.
 *
 * @param stride - signed long - [in] Data stride.
 *
 * @param accType - dmmMemoryAccessT - [in] Tiler memory view access type.
 *
 * @return errorCodeT error if event can't be signaled.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see dmmTILERContCtxT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_copy2tiler_alias_view(void *destPtr,
				       void *srcPtr,
				       signed long width,
				       signed long height,
				       signed long stride,
				       enum dmmMemoryAccessT accType);

/* ========================================================================== */
/**
 *  dmm_virtual_buffer_manipulations()
 *
 * @brief  Manipulates virtual buffers.
 *
 * @param dmmInstanceCtxPtr - void * - [in] Dmm context instance.
 *
 * @param sysPtr - void * - [in] Tiler system pointer to a 2D area.
 *
 * @param patOp - MSP_Dmm_Phy2VirtOpsT - [in] Refill operaion to perform.
 *
 * @param affectedArea - PATAreaT* - [in] Area that will be affected.
 *
 * @param destinationArea - PATAreaT* - [in] Destination coordinates.
 *
 * @return void * pointer to the area targeted by the operation.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see dmmTILERContPageAreaT for further detail.
 */
/* ========================================================================== */
void *dmm_virtual_buffer_manipulations(void *dmmInstanceCtxPtr,
				     void *sysPtr,
				     struct PATAreaT *affectedArea,
				     struct PATAreaT *destinationArea);

#endif /* _DMM_DRV_H */

