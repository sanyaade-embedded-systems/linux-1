/*
 * dmm.c
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>    /* struct cdev */
#include <linux/kdev_t.h>  /* MKDEV() */
#include <linux/fs.h>      /* register_chrdev_region() */
#include <linux/device.h>  /* struct class */
#include <linux/platform_device.h> /* platform_device() */
#include <linux/err.h>     /* IS_ERR() */
#include <linux/errno.h>
#include <linux/io.h>      /* ioremap() */
#include "dmm.h"
#include "dmm_drv.h"
#include "dmm_prv.h"
#include "dmm_def.h"

#define DMM_MAJOR 0
#define DMM_MINOR 0

#define DMM_IO_BASE_ADDR 0x4e000000

unsigned long *dmmvabase;

static int
dmm_open(struct inode *i, struct file *f);
static int
dmm_release(struct inode *i, struct file *f);
static int
dmm_ioctl(struct inode *i, struct file *f,
	  unsigned int c, unsigned long a);
static void
dmm_config();

static int dmm_major;
static int dmm_minor;

struct dmm_dev {
	struct cdev cdev;
};

static struct dmm_dev *dmm_device;
static struct class *dmmdev_class;
static const struct file_operations dmm_fops = {
	.open    = dmm_open,
	.ioctl   = dmm_ioctl,
	.release = dmm_release,
};

static struct platform_driver tiler_driver_ldm = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "tiler",
	},
	.probe = NULL,
	.shutdown = NULL,
	.remove = NULL,
};

struct dmmInstanceCtxT dmmInstanceCtxStatic = {
	0,
};

static int
__init dmm_init(void)
{
	dev_t dev  = 0;
	int retval = -1;

	if (dmm_major) {
		dev = MKDEV(dmm_major, dmm_minor);
		retval = register_chrdev_region(dev, 1, "tiler");
	} else {
		retval = alloc_chrdev_region(&dev, dmm_minor, 1, "tiler");
		dmm_major = MAJOR(dev);
	}

	dmm_device = kmalloc(sizeof(struct dmm_dev), GFP_KERNEL);
	if (!dmm_device) {
		retval = -ENOMEM;
		unregister_chrdev_region(dev, 1);
		printk(KERN_ERR "kmalloc():failed\n");
		goto EXIT;
	}
	memset(dmm_device, 0x0, sizeof(struct dmm_dev));
	cdev_init(&dmm_device->cdev, &dmm_fops);
	dmm_device->cdev.owner = THIS_MODULE;
	dmm_device->cdev.ops   = &dmm_fops;

	retval = cdev_add(&dmm_device->cdev, dev, 1);
	if (retval)
		printk(KERN_ERR "cdev_add():failed\n");

	dmmdev_class = class_create(THIS_MODULE, "tiler");

	if (IS_ERR(dmmdev_class)) {
		printk(KERN_ERR "class_create():failed\n");
		goto EXIT;
	}

	retval = device_create(dmmdev_class, NULL, dev, NULL, "tiler");

	retval = platform_driver_register(&tiler_driver_ldm);

	/* map the TILER i/o physical addr to krnl virt addr */
	dmmvabase = ioremap(DMM_IO_BASE_ADDR, 2048);

	/* config LISA/PAT */
	dmm_config();

EXIT:
	return retval;
}

static void
__exit dmm_exit(void)
{
	platform_driver_unregister(&tiler_driver_ldm);

	cdev_del(&dmm_device->cdev);
	kfree(dmm_device);

	device_destroy(dmmdev_class, MKDEV(dmm_major, dmm_minor));
	class_destroy(dmmdev_class);

	iounmap(dmmvabase);
}

static int
dmm_open(struct inode *ip, struct file *filp)
{
	int retval = -1;
	retval = 0;
	return retval;
}

static int
dmm_release(struct inode *ip, struct file *filp)
{
	int retval = -1;
	retval = 0;
	return retval;
}

static int
dmm_ioctl(struct inode *ip, struct file *filp,
	  unsigned int cmd, unsigned long arg)
{
	struct dmm_data *d = NULL;
	void *SSPtr = NULL;
	int retval = -1;
	int error = -1;

	switch (cmd) {
	case IOCGALLOC:
		d = (struct dmm_data *)arg;

		error = dmm_tiler_buf_alloc(&dmmInstanceCtxStatic,
					    d->w,
					    d->h,
					    d->pixfmt,
					    &SSPtr,
					    NULL);
		if (error == 0) {
			retval = 0;
			d->ssptr = SSPtr;
		} else {
			printk(KERN_ERR "%s::%s():%d\n",
			       __FILE__, __func__, __LINE__);
		}
		break;
	case IOCSFREE:
		d = (struct dmm_data *)arg;

		error = dmm_tiler_buf_free(&dmmInstanceCtxStatic,
					   d->ssptr,
					   1);
		if (error == 0)
			retval = 0;
		break;
	case IOCGTSPTR:
		d = (struct dmm_data *)arg;
		struct dmmViewOrientT orient;

		orient.dmm90Rotate = d->syx >> 2 & 1;
		orient.dmmXInvert = d->syx >> 1 & 1;
		orient.dmmYInvert = d->syx >> 0 & 1;

		error = dmm_tiler_translate_sysptr(dmm_get_context_pointer(),
						   (void *)d->ssptr,
						   orient, 0, 0, 1, 0);

		if (error == 0)
			retval = 0;
		break;
	}

	return retval;
}

static void
dmm_config()
{
	struct dmmPATIrqConfigLstT patEvents;
	struct dmmPATEngineConfigLstT patEngineConf[2];
	struct dmmPATViewConfigLstT patViewConf[16];
	struct dmmPATViewMapConfigLstT patViewMapConf[4];
	struct dmmLISAConfigLstT lisaMemMapConf[1];
	struct dmmTILERConfigLstT dmmTilerAliasView[16];
	unsigned int retCode = 0x0;
	unsigned int i = 0x0;

	/* clear irq event registers */
	patEvents.nextConf = NULL;
	patEvents.irqConf.clrEvents = 1;
	patEvents.irqConf.irqEvnts.irqAreaSelect = 0;
	patEvents.irqConf.irqEvnts.lutMiss = 1;
	patEvents.irqConf.irqEvnts.updData = 1;
	patEvents.irqConf.irqEvnts.updCtrl = 1;
	patEvents.irqConf.irqEvnts.updArea = 1;
	patEvents.irqConf.irqEvnts.invData = 1;
	patEvents.irqConf.irqEvnts.invDsc = 1;
	patEvents.irqConf.irqEvnts.fillLst = 1;
	patEvents.irqConf.irqEvnts.fillDsc = 1;

	if (dmm_pat_irq_config_set((struct dmmPATIrqConfigLstT *)&patEvents) !=
			DMM_NO_ERROR) {
		printk(KERN_ERR "%s::%s():%d: ERROR!\n",
		       __FILE__, __func__, __LINE__);
		retCode = 0x2;
	}

	/* configure pat engines */
	patEngineConf[0].nextConf = &patEngineConf[1];
	patEngineConf[0].engineConf.dmmPatEngineSel = 0;
	patEngineConf[0].engineConf.engineMode = NORMAL_MODE;

	patEngineConf[1].nextConf = NULL;
	patEngineConf[1].engineConf.dmmPatEngineSel = 1;
	patEngineConf[1].engineConf.engineMode = NORMAL_MODE;

	if (dmm_module_config(NULL, NULL, NULL,
			    (struct dmmPATEngineConfigLstT *)patEngineConf,
			    NULL, NULL, NULL) !=
			DMM_NO_ERROR) {
		printk(KERN_ERR "%s::%s():%d: ERROR!\n",
		       __FILE__, __func__, __LINE__);
		retCode = 0x2;
	}

	/* configure LISA map 0 */
	lisaMemMapConf[0].nextConf = NULL;
	lisaMemMapConf[0].mapConf.lisaMemMapIndx = 0;
	lisaMemMapConf[0].mapConf.sysAddr = 0x80;
	lisaMemMapConf[0].mapConf.sysSize = 0x5;
	lisaMemMapConf[0].mapConf.sdrcIntl = 0x0  ;
	lisaMemMapConf[0].mapConf.sdrcAddrspc = 0x1;
	lisaMemMapConf[0].mapConf.sdrcMap = 0x1;
	lisaMemMapConf[0].mapConf.sdrcAddr = 0x00;

	if (dmm_module_config(NULL, NULL,
		(struct dmmLISAConfigLstT *)lisaMemMapConf,
		NULL, NULL, NULL, NULL) != DMM_NO_ERROR) {
		printk(KERN_ERR "%s::%s():%d: ERROR!\n",
		       __FILE__, __func__, __LINE__);
		retCode = 0x2;
	}

	/*  */
	for (i = 0; i < 16; i++) {
		patViewConf[i].nextConf = &patViewConf[i+1];
		patViewConf[i].aliasViewConf.initiatorId = i;
		patViewConf[i].aliasViewConf.viewIndex = 0;
	}
	patViewConf[15].nextConf = NULL;

	if (dmm_module_config(NULL, NULL, NULL, NULL,
			(struct dmmPATViewConfigLstT *)&patViewConf,
			NULL, NULL) !=
			DMM_NO_ERROR) {
		printk(KERN_ERR "%s::%s():%d: ERROR!\n",
		       __FILE__, __func__, __LINE__);
		retCode = 0x2;
	}

	/* configure pat view map 0 */
	patViewMapConf[0].nextConf = &patViewMapConf[1];
	patViewMapConf[0].viewConf.patViewMapIndx = 0;
	patViewMapConf[0].viewConf.memoryAccessMode = MODE_8_BIT;
	patViewMapConf[0].viewConf.contX = 0;
	patViewMapConf[0].viewConf.transType = INDIRECT;

	/* Setting only the MSB seems to have no effect whatsoever */
	patViewMapConf[0].viewConf.dmmPATViewBase = 0xFFFFFFFF;

	patViewMapConf[1].nextConf = &patViewMapConf[2];
	patViewMapConf[1].viewConf.patViewMapIndx = 0;
	patViewMapConf[1].viewConf.memoryAccessMode = MODE_16_BIT;
	patViewMapConf[1].viewConf.contX = 0;
	patViewMapConf[1].viewConf.transType = INDIRECT;

	/* Setting only the MSB seems to have no effect whatsoever */
	patViewMapConf[1].viewConf.dmmPATViewBase = 0xFFFFFFFF;

	patViewMapConf[2].nextConf = &patViewMapConf[3];
	patViewMapConf[2].viewConf.patViewMapIndx = 0;
	patViewMapConf[2].viewConf.memoryAccessMode = MODE_32_BIT;
	patViewMapConf[2].viewConf.contX = 0;
	patViewMapConf[2].viewConf.transType = INDIRECT;

	/* Setting only the MSB seems to have no effect whatsoever */
	patViewMapConf[2].viewConf.dmmPATViewBase = 0xFFFFFFFF;

	patViewMapConf[3].nextConf = NULL;
	patViewMapConf[3].viewConf.patViewMapIndx = 0;
	patViewMapConf[3].viewConf.memoryAccessMode = MODE_PAGE;
	patViewMapConf[3].viewConf.contX = 0;
	patViewMapConf[3].viewConf.transType = INDIRECT;

	/* Setting only the MSB seems to have no effect whatsoever */
	patViewMapConf[3].viewConf.dmmPATViewBase = 0xFFFFFFFF;

	if (dmm_module_config(NULL, NULL, NULL, NULL, NULL,
			    (struct dmmPATViewMapConfigLstT *)&patViewMapConf,
			    NULL) != DMM_NO_ERROR) {
		printk(KERN_ERR "%s::%s():%d: ERROR!\n",
		       __FILE__, __func__, __LINE__);
		retCode = 0x2;
	}

	/* set all initiators to zero degree natural */
	for (i = 0; i < 16; i++) {
		dmmTilerAliasView[i].nextConf = &dmmTilerAliasView[i + 1];
		dmmTilerAliasView[i].aliasConf.initiatorId = i;
		dmmTilerAliasView[i].aliasConf.orient.dmm90Rotate = 0;
		dmmTilerAliasView[i].aliasConf.orient.dmmXInvert = 0;
		dmmTilerAliasView[i].aliasConf.orient.dmmYInvert = 0;
	}
	dmmTilerAliasView[15].nextConf = NULL;

	if (dmm_module_config((struct dmmTILERConfigLstT *)dmmTilerAliasView,
			    NULL, NULL, NULL, NULL, NULL, NULL) !=
			DMM_NO_ERROR) {
		printk(KERN_ERR "%s::%s():%d: ERROR!\n",
		       __FILE__, __func__, __LINE__);
		retCode = 0x2;
	}
}

int
dmm_tiler_buf_alloc(void *dmmInstanceCtxPtr,
		    unsigned short sizeWidth,
		    unsigned short sizeHeight,
		    int contMod,
		    void **allocedPtr,
		    void *custmPagesPtr)
{
	enum errorCodeT eCode = DMM_NO_ERROR;

	struct dmmTILERContCtxT *dmmTilerCtx =
		&((struct dmmInstanceCtxT *)dmmInstanceCtxPtr)->dmmTilerCtx;

	struct dmmTILERContPageAreaT *bufferMappedZone;

	if (eCode == DMM_NO_ERROR) {
		eCode = dmm_tiler_container_map_area(dmmTilerCtx,
						 sizeWidth,
						 sizeHeight,
						 contMod,
						 allocedPtr,
						 &bufferMappedZone);
	}

	if (eCode == DMM_NO_ERROR) {

		bufferMappedZone->xPageOfst = 0;
		bufferMappedZone->yPageOfst = 0;
		bufferMappedZone->xPageCount =
			bufferMappedZone->x1 - bufferMappedZone->x0 + 1;
		bufferMappedZone->yPageCount =
			bufferMappedZone->y1 - bufferMappedZone->y0 + 1;

		eCode = dmm_pat_phy2virt_mapping(bufferMappedZone,
						custmPagesPtr);
	}

	if (eCode != DMM_NO_ERROR)
		*allocedPtr = NULL;

	return eCode;
}
EXPORT_SYMBOL(dmm_tiler_buf_alloc);

int
dmm_tiler_buf_free(void *dmmInstanceCtxPtr,
		   void *allocedPtr,
		   int aliasViewPtr)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	struct dmmTILERContCtxT *dmmTilerCtx =
		&((struct dmmInstanceCtxT *)dmmInstanceCtxPtr)->dmmTilerCtx;

	struct dmmTILERContPageAreaT *areaToFree;

	if (aliasViewPtr) {
		allocedPtr = (void *)((unsigned long)allocedPtr &
				      DMM_ALIAS_VIEW_CLEAR);
	}

	areaToFree = dmm_tiler_get_area_from_sysptr(dmmTilerCtx, allocedPtr);

	if (areaToFree != NULL)
		eCode = dmm_tiler_container_unmap_area(dmmTilerCtx, areaToFree);
	else
		eCode = DMM_SYS_ERROR;

	return eCode;
}
EXPORT_SYMBOL(dmm_tiler_buf_free);

void *
dmm_tiler_translate_sysptr(void *dmmInstanceCtxPtr,
			   void *sysPtr,
			   struct dmmViewOrientT orient,
			   unsigned int validDataWidth,
			   unsigned int validDataHeight,
			   int aliasViewPtr,
			   int ptrToaliasView)
{
	struct dmmTILERContPageAreaT *bufferMappedZone;

	struct dmmTILERContCtxT *dmmTilerCtx =
		&((struct dmmInstanceCtxT *)dmmInstanceCtxPtr)->dmmTilerCtx;

	signed long pageDimmensionX;
	signed long pageDimmensionY;
	unsigned long addrAlignment;
	enum dmmMemoryAccessT accessModeM;

	unsigned long areaX0;
	unsigned long areaY0;
	unsigned long areaX1;
	unsigned long areaY1;

	unsigned long contWidth;
	unsigned long contHeight;

	unsigned long newX;
	unsigned long newY;

	if (aliasViewPtr)
		sysPtr = (void *)((unsigned long)sysPtr & DMM_ALIAS_VIEW_CLEAR);

	accessModeM = DMM_GET_ACC_MODE(sysPtr);

	bufferMappedZone = dmm_tiler_get_area_from_sysptr(dmmTilerCtx, sysPtr);

	if (bufferMappedZone == NULL)
		return NULL;

	switch (accessModeM) {
	case MODE_8_BIT:
		if (orient.dmm90Rotate) {
			contWidth = DMM_TILER_CONT_HEIGHT_8;
			contHeight = DMM_TILER_CONT_WIDTH_8;
		} else {
			contWidth = DMM_TILER_CONT_WIDTH_8;
			contHeight = DMM_TILER_CONT_HEIGHT_8;
		}
		pageDimmensionX = DMM_PAGE_DIMM_X_MODE_8;
		pageDimmensionY = DMM_PAGE_DIMM_Y_MODE_8;
		addrAlignment = 0;
		areaX0 = bufferMappedZone->x0;
		areaY0 = bufferMappedZone->y0;
		areaX1 = bufferMappedZone->x1;
		areaY1 = bufferMappedZone->y1;
		break;
	case MODE_16_BIT:
		if (orient.dmm90Rotate) {
			contWidth = DMM_TILER_CONT_HEIGHT_16;
			contHeight = DMM_TILER_CONT_WIDTH_16;
		} else {
			contWidth = DMM_TILER_CONT_WIDTH_16;
			contHeight = DMM_TILER_CONT_HEIGHT_16;
		}
		pageDimmensionX = DMM_PAGE_DIMM_X_MODE_16;
		pageDimmensionY = DMM_PAGE_DIMM_Y_MODE_16;
		addrAlignment = 1;
		areaX0 = bufferMappedZone->x0;
		areaY0 = bufferMappedZone->y0;
		areaX1 = bufferMappedZone->x1;
		areaY1 = bufferMappedZone->y1;
		break;
	case MODE_32_BIT:
		if (orient.dmm90Rotate) {
			contWidth = DMM_TILER_CONT_HEIGHT_32;
			contHeight = DMM_TILER_CONT_WIDTH_32;
		} else {
			contWidth = DMM_TILER_CONT_WIDTH_32;
			contHeight = DMM_TILER_CONT_HEIGHT_32;
		}
		pageDimmensionX = DMM_PAGE_DIMM_X_MODE_32;
		pageDimmensionY = DMM_PAGE_DIMM_Y_MODE_32;
		addrAlignment = 2;
		areaX0 = bufferMappedZone->x0;
		areaY0 = bufferMappedZone->y0;
		areaX1 = bufferMappedZone->x1;
		areaY1 = bufferMappedZone->y1;
		break;
	case MODE_PAGE:
		if (orient.dmm90Rotate) {
			contWidth = DMM_TILER_CONT_HEIGHT_8;
			contHeight = DMM_TILER_CONT_WIDTH_8;
		} else {
			contWidth = DMM_TILER_CONT_WIDTH_8;
			contHeight = DMM_TILER_CONT_HEIGHT_8;
		}
		pageDimmensionX = DMM_PAGE_DIMM_X_MODE_8;
		pageDimmensionY = DMM_PAGE_DIMM_Y_MODE_8;
		addrAlignment = 0;
		areaX0 = bufferMappedZone->x0;
		areaY0 = bufferMappedZone->y0;
		areaX1 = bufferMappedZone->x1;
		areaY1 = bufferMappedZone->y1;
		break;
	default:
		return NULL;
	}

	if (orient.dmmXInvert) {
		if (validDataWidth > 0 && validDataHeight)
			newX = contWidth - validDataWidth;
		else
			newX = contWidth - (areaX1 + 1) * pageDimmensionX;
	} else {
		newX = areaX0 * pageDimmensionX;
	}

	if (orient.dmmYInvert) {
		if (validDataWidth > 0 && validDataHeight)
			newY = contHeight - validDataHeight;
		else
			newY = contHeight - (areaY1 + 1) * pageDimmensionY;
	} else {
		newY = areaY0 * pageDimmensionY;
	}

	if (orient.dmm90Rotate)
		sysPtr = (void *)((newX*contHeight+newY)<<
				  ((unsigned long)addrAlignment));
	else
		sysPtr = (void *)((newY*contWidth+newX)<<
				  ((unsigned long)addrAlignment));

	if (ptrToaliasView) {
		sysPtr = DMM_COMPOSE_TILER_ALIAS_PTR(sysPtr, accessModeM);
	} else {
		sysPtr = DMM_COMPOSE_TILER_PTR(sysPtr,
					       orient.dmm90Rotate,
					       orient.dmmYInvert,
					       orient.dmmXInvert,
					       accessModeM);
	}

	return sysPtr;
}
EXPORT_SYMBOL(dmm_tiler_translate_sysptr);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("davidsin@ti.com");
module_init(dmm_init);
module_exit(dmm_exit);


