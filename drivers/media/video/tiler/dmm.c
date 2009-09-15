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
#include <linux/mm.h>      /* mmaping */
#include <linux/mm_types.h>
#include <linux/sched.h>   /* current->mm */
#include "tiler.h"
#include "dmm_drv.h"
#include "dmm_prv.h"
#include "dmm_def.h"

#define DMM_MAJOR 0
#define DMM_MINOR 0
#define DMM_IO_BASE_ADDR 0x4e000000
#define tilerdump(x) /*printk(KERN_NOTICE "%s::%s():%d: %s=%p\n", \
					__FILE__, __func__, __LINE__, #x, x); */
unsigned long *dmm_virt_base_addr;
static struct dmmInstanceCtxT *ctxptr;

struct node {
	struct tiler_buf_info *ptr;
	unsigned long reserved;
	struct node *nextnode;
};

static struct node *lsthd;
static int id;

static int dmm_open(struct inode *i, struct file *f);
static int dmm_release(struct inode *i, struct file *f);
static int dmm_ioctl(struct inode *i, struct file *f,
		     unsigned int c, unsigned long a);
static int dmm_mmap(struct file *f, struct vm_area_struct *v);
static void dmm_vma_open(struct vm_area_struct *vma);
static void dmm_vma_close(struct vm_area_struct *vma);
static void dmm_config(void);
static int removenode(struct node *listhead, int offset);
static int tiler_destroy_buf_info_list(struct node *listhead);
static int addnode(struct node *listhead, struct tiler_buf_info *p);
static int createlist(struct node **listhead);
static int tiler_find_buf(unsigned long sysptr, struct tiler_block_info *blk);

static int
tiler_get_buf_info(struct node *listhead, struct tiler_buf_info **pp, int ofst);

static int dmm_major;
static int dmm_minor;

static struct vm_operations_struct dmm_remap_vm_ops = {
	.open =  dmm_vma_open,
	.close = dmm_vma_close,
};

struct dmm_dev {
	struct cdev cdev;
};

static struct dmm_dev *dmm_device;
static struct class *dmmdev_class;
static const struct file_operations dmm_fops = {
	.open    = dmm_open,
	.ioctl   = dmm_ioctl,
	.release = dmm_release,
	.mmap    = dmm_mmap,
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

static int
__init dmm_init(void)
{
	dev_t dev  = 0;
	int retval = -1;
	int error = -1;
	struct device *device = NULL;

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

	device = device_create(dmmdev_class, NULL, dev, NULL, "tiler");
	if (device == NULL)
		printk(KERN_ERR "device_create() fail\n");

	retval = platform_driver_register(&tiler_driver_ldm);

	/* map the TILER i/o physical addr to krnl virt addr */
	dmm_virt_base_addr = (unsigned long *)ioremap(DMM_IO_BASE_ADDR, 2048);

	ctxptr = kmalloc(sizeof(struct dmmInstanceCtxT), GFP_KERNEL);
	memset(ctxptr, 0x0, sizeof(struct dmmInstanceCtxT));
	error = dmm_instance_init((void *)ctxptr, TILER_WIDTH,
				  TILER_HEIGHT, NULL, NULL);
	if (error == 1) {
		retval = 0;
	} else {
		kfree(ctxptr);
		return retval;
	}

	/* config LISA/PAT */
	dmm_config();

	/* create buffer info list */
	createlist(&lsthd);
	id = 0xda7a000;

EXIT:
	return retval;
}

static void
__exit dmm_exit(void)
{
	kfree(ctxptr);
	tiler_destroy_buf_info_list(lsthd);

	platform_driver_unregister(&tiler_driver_ldm);

	cdev_del(&dmm_device->cdev);
	kfree(dmm_device);

	device_destroy(dmmdev_class, MKDEV(dmm_major, dmm_minor));
	class_destroy(dmmdev_class);

	iounmap(dmm_virt_base_addr);
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
	struct tiler_block_info *block = NULL;
	struct tiler_buf_info *bufinfo = NULL;
	/* struct dmmInstanceCtxT *ctxptr = NULL; */
	void *ptr = NULL;
	int retval = -1;
	int error = -1;
	int i = 0;
	pgd_t *pgd = NULL;
	pmd_t *pmd = NULL;
	pte_t *ptep = NULL, pte = 0x0;

	switch (cmd) {
	case TILIOC_OPEN:
		retval = 0;
		break;
	case TILIOC_CLOSE:
		retval = 0;
		break;
	case TILIOC_GBUF:
		block = (struct tiler_block_info *)arg;
		if (block->fmt == TILFMT_PAGE) {
			error = tiler_alloc_buf(block->fmt,
						block->dim.len,
						1,
						&ptr);
		} else {
			error = tiler_alloc_buf(block->fmt,
						block->dim.area.width,
						block->dim.area.height,
						&ptr);
		}
		if (error == 0) {
			retval = 0;
			block->ssptr = (unsigned long)ptr;
		} else {
			printk(KERN_ERR "%s::%s():%d\n",
			       __FILE__, __func__, __LINE__);
		}
		break;
	case TILIOC_FBUF:
		block = (struct tiler_block_info *)arg;
		error = tiler_free_buf(block->ssptr);
		if (error == 0)
			retval = 0;
		break;
	case TILIOC_GSSP:
		block = (struct tiler_block_info *)arg;
		pgd = pgd_offset(current->mm, (unsigned long)block->ptr);

		block->ssptr = 0;
		if (!(pgd_none(*pgd) || pgd_bad(*pgd))) {
			pmd = pmd_offset(pgd, (unsigned long)block->ptr);
			if (!(pmd_none(*pmd) || pmd_bad(*pmd))) {
				ptep = pte_offset_map(pmd,
						(unsigned long)block->ptr);
				if (ptep) {
					pte = *ptep;
					if (pte_present(pte)) {
						block->ssptr = (pte & PAGE_MASK)
						| (~PAGE_MASK &
						(unsigned long)block->ptr);
						tilerdump(pte);
						tilerdump(block->ssptr);
						retval = 0;
						break;
					}
				}
			}
		}
		break;
	case TILIOC_MBUF:
		retval = 0;
		break;
	case TILIOC_QBUF:
		error = tiler_get_buf_info(
		lsthd, &bufinfo, ((struct tiler_buf_info *)arg)->offset);
		if (error != 0) {
			printk(KERN_ERR "TILDRV: tiler_get_buf_info() fail\n");
			return retval;
		}
		tilerdump(((struct tiler_buf_info *)arg)->offset);
		tilerdump(bufinfo->num_blocks);
		for (i = 0; i < bufinfo->num_blocks; i++)
			tilerdump(bufinfo->blocks[i].ssptr);
		memcpy((struct tiler_buf_info *)arg,
					bufinfo, sizeof(struct tiler_buf_info));
		retval = 0;
		break;
	case TILIOC_RBUF:
		bufinfo = kmalloc(sizeof(struct tiler_buf_info), GFP_KERNEL);
		memset(bufinfo, 0x0, sizeof(struct tiler_buf_info));
		memcpy(bufinfo, (struct tiler_buf_info *)arg,
						sizeof(struct tiler_buf_info));
		for (i = 0; i < bufinfo->num_blocks; i++)
			tilerdump(bufinfo->blocks[i].ssptr);
		bufinfo->offset = id;
		id += 0x1000;
		((struct tiler_buf_info *)arg)->offset = bufinfo->offset;
		error = addnode(lsthd, bufinfo);
		if (error != 0) {
			printk(KERN_ERR "TILIOC_RBUF: addnode() fail\n");
			return retval;
		}
		retval = 0;
		break;
	case TILIOC_URBUF:
		if (0)
			tilerdump(((struct tiler_buf_info *)arg)->offset);
		error = tiler_get_buf_info(lsthd, &bufinfo,
			((struct tiler_buf_info *)arg)->offset);
		if (error != 0) {
			printk(KERN_ERR "TILDRV: tiler_get_buf_info() fail\n");
			return retval;
		}
		error = removenode(lsthd,
					((struct tiler_buf_info *)arg)->offset);
		if (error != 0) {
			printk(KERN_ERR "TILDRV: removenode() fail: offset ="
				"%d\n", bufinfo->offset);
			return retval;
		}
		kfree(bufinfo);
		retval = 0;
		break;
	case TILIOC_QUERY_BLK:
		block = (struct tiler_block_info *)arg;
		error = tiler_find_buf(block->ssptr, block);
		if (error == 0)
			retval = 0;
		break;
	}
	return retval;
}

static int
dmm_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret = -1;
	struct tiler_buf_info *b = NULL;
	int i = 0, j = 0, k = 0, m = 0, p = 0;
	int bpp = 1;

	ret = tiler_get_buf_info(lsthd, &b, vma->vm_pgoff << PAGE_SHIFT);
	if (ret != 0) {
		printk(KERN_ERR "%s::%s():%d: tiler_get_buf_info failed\n",
						__FILE__, __func__, __LINE__);
		return 0x0;
	}

	for (i = 0; i < b->num_blocks; i++) {
		if (b->blocks[i].fmt >= TILFMT_8BIT &&
					b->blocks[i].fmt <= TILFMT_32BIT) {
			/* get line width */
			bpp = (b->blocks[i].fmt == TILFMT_8BIT ? 1 :
			       b->blocks[i].fmt == TILFMT_16BIT ? 2 : 4);
			p = (b->blocks[i].dim.area.width * bpp +
				TILER_PAGESIZE - 1) & ~(TILER_PAGESIZE - 1);

			for (j = 0; j < b->blocks[i].dim.area.height; j++) {
				/* map each page of the line */
				if (0)
					printk(KERN_NOTICE
					"%s::%s():%d: vm_start+%d = 0x%lx,"
					"blk[%d].ssptr+%d = 0x%lx, w=0x%x\n",
					__FILE__, __func__, __LINE__,
					k, vma->vm_start + k, i, m,
					(b->blocks[i].ssptr + m), p);
				vma->vm_pgoff =
					(b->blocks[i].ssptr + m) >> PAGE_SHIFT;
				if (remap_pfn_range(vma, vma->vm_start + k,
					(b->blocks[i].ssptr + m) >> PAGE_SHIFT,
					p, vma->vm_page_prot))
					return -EAGAIN;
				k += p;
				if (b->blocks[i].fmt == TILFMT_8BIT)
					m += 64*TILER_WIDTH;
				else
					m += 2*64*TILER_WIDTH;
			}
			m = 0;
		} else if (b->blocks[i].fmt == TILFMT_PAGE) {
			vma->vm_pgoff = (b->blocks[i].ssptr) >> PAGE_SHIFT;
			p = (b->blocks[i].dim.len + TILER_PAGESIZE - 1) &
							~(TILER_PAGESIZE - 1);
			if (0)
				printk(KERN_NOTICE "%s::%s():%d:"
				"vm_start = 0x%lx, blk[%d].ssptr = 0x%lx,"
				"w=0x%x\n",
				__FILE__, __func__, __LINE__,
				vma->vm_start + k, i, (b->blocks[i].ssptr), p);
			if (remap_pfn_range(vma, vma->vm_start + k,
				(b->blocks[i].ssptr) >> PAGE_SHIFT, p,
				vma->vm_page_prot))
				return -EAGAIN;;
			k += p;
		}
	}
	vma->vm_ops = &dmm_remap_vm_ops;
	dmm_vma_open(vma);
	return 0;
}

void
dmm_vma_open(struct vm_area_struct *vma)
{
	if (0)
		printk(KERN_NOTICE "dmm VMA open, virt %lx, phys %lx\n",
			      vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

void
dmm_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "dmm VMA close.\n");
}

static void
dmm_config(void)
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
tiler_alloc_buf(enum tiler_fmt fmt,
		unsigned long width,
		unsigned long height,
		void **sysptr)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	struct dmmTILERContCtxT *dmmTilerCtx =
			&((struct dmmInstanceCtxT *)ctxptr)->dmmTilerCtx;
	struct dmmTILERContPageAreaT *bufferMappedZone;
	void *custmPagesPtr = NULL;
	enum dmmMemoryAccessT contMod;

	if (fmt == TILFMT_8BIT)
		contMod = MODE_8_BIT;
	else if (fmt == TILFMT_16BIT)
		contMod = MODE_16_BIT;
	else if (fmt == TILFMT_32BIT)
		contMod = MODE_32_BIT;
	else if (fmt == TILFMT_PAGE)
		contMod = MODE_PAGE;
	else
		return DMM_SYS_ERROR;

	if (eCode == DMM_NO_ERROR) {
		eCode = dmm_tiler_container_map_area(dmmTilerCtx,
						     width,
						     height,
						     contMod,
						     sysptr,
						     &bufferMappedZone);
	}

	if (eCode == DMM_NO_ERROR) {
		bufferMappedZone->xPageOfst = 0;
		bufferMappedZone->yPageOfst = 0;
		bufferMappedZone->xPageCount =
			bufferMappedZone->x1 - bufferMappedZone->x0 + 1;
		bufferMappedZone->yPageCount =
			bufferMappedZone->y1 - bufferMappedZone->y0 + 1;
		printk(KERN_ERR "x(%u-%u=%u>%u) y(%u-%u=%u>%u)\n",
		     bufferMappedZone->x0, bufferMappedZone->x1,
		     bufferMappedZone->xPageCount, bufferMappedZone->xPageOfst,
		     bufferMappedZone->y0, bufferMappedZone->y1,
		     bufferMappedZone->yPageCount, bufferMappedZone->yPageOfst);

		eCode = dmm_pat_phy2virt_mapping(bufferMappedZone,
						 custmPagesPtr);
	}

	if (eCode != DMM_NO_ERROR)
		*sysptr = NULL;

	return eCode;
}
EXPORT_SYMBOL(tiler_alloc_buf);

/* :TODO: Currently we do not track enough information from alloc to get back
   the actual width and height of the container, so we must make a guess.  We
   do not even have enough information to get the virtual stride of the buffer,
   which is the real reason for this ioctl */
int
tiler_find_buf(unsigned long sysptr, struct tiler_block_info *blk)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	struct dmmTILERContCtxT *dmmTilerCtx =
			&((struct dmmInstanceCtxT *)ctxptr)->dmmTilerCtx;

	struct dmmTILERContPageAreaT *area;

	area = dmm_tiler_get_area_from_sysptr(dmmTilerCtx, (void *)sysptr);
	blk->ptr = NULL;
	if (area != NULL) {
		int accMode = DMM_GET_ACC_MODE(sysptr);
		blk->fmt = (accMode + 1);
		if (blk->fmt == TILFMT_PAGE) {
			blk->dim.len = area->xPageCount *
					area->yPageCount * TILER_PAGESIZE;
			blk->stride = 0;
			blk->ssptr =
				(unsigned long)
				DMM_COMPOSE_TILER_ALIAS_PTR(
				((area->x0 | (area->y0 << 8)) << 12), accMode);
		} else {
			blk->stride = blk->dim.area.width =
					area->xPageCount * TILER_BLOCK_WIDTH;
			blk->dim.area.height =
					area->yPageCount * TILER_BLOCK_HEIGHT;
			if (blk->fmt == TILFMT_8BIT) {
				blk->ssptr =
					(unsigned long)
					DMM_COMPOSE_TILER_ALIAS_PTR(
					((area->x0 << 6) | (area->y0 << 20)),
					accMode);
			} else {
				blk->ssptr =
					(unsigned long)
					DMM_COMPOSE_TILER_ALIAS_PTR(
					((area->x0 << 7) | (area->y0 << 20)),
					accMode);
				blk->stride <<= 1;
				blk->dim.area.height >>= 1;
				if (blk->fmt == TILFMT_32BIT)
					blk->dim.area.width >>= 1;
			}
			blk->stride = (blk->stride + TILER_PAGESIZE - 1) &
							~(TILER_PAGESIZE - 1);
		}
	} else {
		blk->fmt = TILFMT_INVALID;
		blk->dim.len = blk->stride = blk->ssptr = 0;
		eCode = DMM_WRONG_PARAM;
	}

	return eCode;
}

int
tiler_free_buf(unsigned long sysptr)
{
	enum errorCodeT eCode = DMM_NO_ERROR;
	struct dmmTILERContCtxT *dmmTilerCtx =
			&((struct dmmInstanceCtxT *)ctxptr)->dmmTilerCtx;

	struct dmmTILERContPageAreaT *areaToFree;

	/* if (aliasViewPtr) {
		allocedPtr = (void *)((unsigned long)allocedPtr &
				      DMM_ALIAS_VIEW_CLEAR);
	} */

	areaToFree = dmm_tiler_get_area_from_sysptr(dmmTilerCtx,
							(void *)sysptr);
	if (areaToFree != NULL)
		eCode = dmm_tiler_container_unmap_area(dmmTilerCtx, areaToFree);
	else
		eCode = DMM_SYS_ERROR;

	return eCode;
}
EXPORT_SYMBOL(tiler_free_buf);

unsigned long
tiler_get_tiler_address(void *sysPtr,
			struct dmmViewOrientT orient,
			unsigned int validDataWidth,
			unsigned int validDataHeight,
			int aliasViewPtr,
			int ptrToaliasView)
{
	struct dmmTILERContPageAreaT *bufferMappedZone;
	struct dmmTILERContCtxT *dmmTilerCtx =
			&((struct dmmInstanceCtxT *)ctxptr)->dmmTilerCtx;

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
		return 0x0;

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
		return 0x0;
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

	return (unsigned long)sysPtr;
}
EXPORT_SYMBOL(tiler_get_tiler_address);

static int createlist(struct node **listhead)
{
	int error = -1;
	void *ret = NULL;

	*listhead = kmalloc(sizeof(struct node), GFP_KERNEL);
	if (*listhead == NULL) {
		printk(KERN_ERR "%s():%d: ERROR!\n", __func__, __LINE__);
		return error;
	}
	ret = memset(*listhead, 0x0, sizeof(struct node));
	if (ret != *listhead) {
		printk(KERN_ERR "%s():%d: ERROR!\n", __func__, __LINE__);
		return error;
	} else {
		printk(KERN_ERR "%s():%d: success!\n", __func__, __LINE__);
	}
	return 0;
}

static int addnode(struct node *listhead, struct tiler_buf_info *ptr)
{
	int error = -1;
	struct node *tmpnode = NULL;
	struct node *newnode = NULL;
	void *ret = NULL;

	/* assert(listhead != NULL); */
	newnode = kmalloc(sizeof(struct node), GFP_KERNEL);
	if (newnode == NULL) {
		printk(KERN_ERR "%s():%d: ERROR!\n", __func__, __LINE__);
		return error;
	}
	ret = memset(newnode, 0x0, sizeof(struct node));
	if (ret != newnode) {
		printk(KERN_ERR "%s():%d: ERROR!\n", __func__, __LINE__);
		return error;
	}
	newnode->ptr = ptr;
	tmpnode = listhead;

	while (tmpnode->nextnode != NULL)
		tmpnode = tmpnode->nextnode;
	tmpnode->nextnode = newnode;

	return 0;
}

static int
removenode(struct node *listhead, int offset)
{
	struct node *node = NULL;
	struct node *tmpnode = NULL;

	node = listhead;

	while (node->nextnode != NULL) {
		if (node->nextnode->ptr->offset == offset) {
			tmpnode = node->nextnode;
			node->nextnode = tmpnode->nextnode;
			kfree(tmpnode);
			tmpnode = NULL;
			return 0;
		}
		node = node->nextnode;
	}
	return -1;
}

static int
tiler_destroy_buf_info_list(struct node *listhead)
{
	struct node *tmpnode = NULL;
	struct node *node = NULL;

	node = listhead;

	while (node->nextnode != NULL) {
		tmpnode = node->nextnode;
		node->nextnode = tmpnode->nextnode;
		kfree(tmpnode);
		tmpnode = NULL;
	}
	kfree(listhead);
	return 0;
}

static int
tiler_get_buf_info(struct node *listhead, struct tiler_buf_info **pp, int offst)
{
	struct node *node = NULL;

	node = listhead;

	while (node->nextnode != NULL) {
		if (node->nextnode->ptr->offset == offst) {
			*pp = node->nextnode->ptr;
			return 0;
		}
		node = node->nextnode;
	}
	return -1;
}

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("davidsin@ti.com");
module_init(dmm_init);
module_exit(dmm_exit);


