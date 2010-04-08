/*
 * cfg.c
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * Implementation of platform specific config services.
 *
 * Copyright (C) 2005-2006 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*  ----------------------------------- DSP/BIOS Bridge */
#include <dspbridge/std.h>
#include <dspbridge/dbdefs.h>
#include <dspbridge/errbase.h>

/*  ----------------------------------- Trace & Debug */
#include <dspbridge/dbc.h>

/*  ----------------------------------- OS Adaptation Layer */
#include <dspbridge/reg.h>

/*  ----------------------------------- This */
#include <dspbridge/cfg.h>
#include <dspbridge/drv.h>

struct drv_ext {
	struct list_head link;
	char sz_string[MAXREGPATHLENGTH];
};

/*
 *  ======== cfg_exit ========
 *  Purpose:
 *      Discontinue usage of the CFG module.
 */
void cfg_exit(void)
{
	/* Do nothing */
}

/*
 *  ======== cfg_get_auto_start ========
 *  Purpose:
 *      Retreive the autostart mask, if any, for this board.
 */
dsp_status cfg_get_auto_start(struct cfg_devnode *dev_node_obj,
			      OUT u32 *pdwAutoStart)
{
	dsp_status status = DSP_SOK;
	u32 dw_buf_size;
	struct drv_data *drv_datap = dev_get_drvdata(bridge);

	dw_buf_size = sizeof(*pdwAutoStart);
	if (!dev_node_obj)
		status = CFG_E_INVALIDHDEVNODE;
	if (!pdwAutoStart || !drv_datap)
		status = CFG_E_INVALIDPOINTER;
	if (DSP_SUCCEEDED(status))
		*pdwAutoStart = (drv_datap->base_img) ? 1 : 0;

	DBC_ENSURE((status == DSP_SOK &&
		    (*pdwAutoStart == 0 || *pdwAutoStart == 1))
		   || status != DSP_SOK);
	return status;
}

/*
 *  ======== cfg_get_dev_object ========
 *  Purpose:
 *      Retrieve the Device Object handle for a given devnode.
 */
dsp_status cfg_get_dev_object(struct cfg_devnode *dev_node_obj,
			      OUT u32 *pdwValue)
{
	dsp_status status = DSP_SOK;
	u32 dw_buf_size;

	if (!dev_node_obj)
		status = CFG_E_INVALIDHDEVNODE;

	if (!pdwValue)
		status = CFG_E_INVALIDHDEVNODE;

	dw_buf_size = sizeof(pdwValue);
	if (DSP_SUCCEEDED(status)) {

		/* check the device string and then call the reg_set_value */
		if (!
		    (strcmp
		     ((char *)((struct drv_ext *)dev_node_obj)->sz_string,
		      "TIOMAP1510")))
			status =
			    reg_get_value("DEVICE_DSP", (u8 *) pdwValue,
					  &dw_buf_size);
	}
	if (DSP_FAILED(status))
		pr_err("%s: Failed, status 0x%x\n", __func__, status);
	return status;
}

/*
 *  ======== cfg_get_exec_file ========
 *  Purpose:
 *      Retreive the default executable, if any, for this board.
 */
dsp_status cfg_get_exec_file(struct cfg_devnode *dev_node_obj, u32 ul_buf_size,
			     OUT char *pstrExecFile)
{
	dsp_status status = DSP_SOK;
	struct drv_data *drv_datap = dev_get_drvdata(bridge);

	if (!dev_node_obj)
		status = CFG_E_INVALIDHDEVNODE;
	else if (!pstrExecFile || !drv_datap)
		status = CFG_E_INVALIDPOINTER;

	if (strlen(drv_datap->base_img) > ul_buf_size)
		status = DSP_ESIZE;

	if (DSP_SUCCEEDED(status) && drv_datap->base_img)
		strcpy(pstrExecFile, drv_datap->base_img);

	if (DSP_FAILED(status))
		pr_err("%s: Failed, status 0x%x\n", __func__, status);
	DBC_ENSURE(((status == DSP_SOK) &&
		    (strlen(pstrExecFile) <= ul_buf_size))
		   || (status != DSP_SOK));
	return status;
}

/*
 *  ======== cfg_get_object ========
 *  Purpose:
 *      Retrieve the Object handle from the Registry
 */
dsp_status cfg_get_object(OUT u32 *pdwValue, u32 dw_type)
{
	dsp_status status = DSP_EINVALIDARG;
	u32 dw_buf_size;
	DBC_REQUIRE(pdwValue != NULL);

	dw_buf_size = sizeof(pdwValue);
	switch (dw_type) {
	case (REG_DRV_OBJECT):
		status =
		    reg_get_value(DRVOBJECT, (u8 *) pdwValue, &dw_buf_size);
		if (DSP_FAILED(status))
			status = CFG_E_RESOURCENOTAVAIL;
		break;
	case (REG_MGR_OBJECT):
		status =
		    reg_get_value(MGROBJECT, (u8 *) pdwValue, &dw_buf_size);
		if (DSP_FAILED(status))
			status = CFG_E_RESOURCENOTAVAIL;
		break;
	default:
		break;
	}
	if (DSP_FAILED(status)) {
		*pdwValue = 0;
		pr_err("%s: Failed, status 0x%x\n", __func__, status);
	}
	DBC_ENSURE((DSP_SUCCEEDED(status) && *pdwValue != 0) ||
		   (DSP_FAILED(status) && *pdwValue == 0));
	return status;
}

/*
 *  ======== cfg_init ========
 *  Purpose:
 *      Initialize the CFG module's private state.
 */
bool cfg_init(void)
{
	return true;
}

/*
 *  ======== cfg_set_dev_object ========
 *  Purpose:
 *      Store the Device Object handle and dev_node pointer for a given devnode.
 */
dsp_status cfg_set_dev_object(struct cfg_devnode *dev_node_obj, u32 dwValue)
{
	dsp_status status = DSP_SOK;
	u32 dw_buff_size;

	if (!dev_node_obj)
		status = CFG_E_INVALIDHDEVNODE;

	dw_buff_size = sizeof(dwValue);
	if (DSP_SUCCEEDED(status)) {
		/* Store the WCD device object in the Registry */

		if (!(strcmp((char *)dev_node_obj, "TIOMAP1510"))) {
			status = reg_set_value("DEVICE_DSP", (u8 *) &dwValue,
					       dw_buff_size);
		}
	}
	if (DSP_FAILED(status))
		pr_err("%s: Failed, status 0x%x\n", __func__, status);

	return status;
}

/*
 *  ======== cfg_set_object ========
 *  Purpose:
 *      Store the Driver Object handle
 */
dsp_status cfg_set_object(u32 dwValue, u32 dw_type)
{
	dsp_status status = DSP_EINVALIDARG;
	u32 dw_buff_size;

	dw_buff_size = sizeof(dwValue);
	switch (dw_type) {
	case (REG_DRV_OBJECT):
		status =
		    reg_set_value(DRVOBJECT, (u8 *) &dwValue, dw_buff_size);
		break;
	case (REG_MGR_OBJECT):
		status =
		    reg_set_value(MGROBJECT, (u8 *) &dwValue, dw_buff_size);
		break;
	default:
		break;
	}
	if (DSP_FAILED(status))
		pr_err("%s: Failed, status 0x%x\n", __func__, status);
	return status;
}
