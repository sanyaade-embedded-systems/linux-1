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
#include <linux/uaccess.h> /* copy_to_user() */
#include "tiler.h"
#include "dmm_drv.h"
#include "dmm_prv.h"
#include "dmm_def.h"

#define __NEWCODE__
#ifdef __NEWCODE__
struct pat_area {
	int x0:8;
	int y0:8;
	int x1:8;
	int y1:8;
};

struct pat_ctrl {
	int start:4;
	int direction:4;
	int lut_id:8;
	int sync:12;
	int initiator:4;
};

struct pat_desc {
	struct pat_desc *next;
	struct pat_area area;
	struct pat_ctrl ctrl;
	unsigned long data;
};

void *dmm_base;

static void pat_area_set(struct pat_area *area, char id);
static void pat_data_set(unsigned long data, char id);
static void pat_ctrl_set(struct pat_ctrl *ctrl, char id);
static void pat_desc_set(struct pat_desc *desc, char id);
static void hwinfo_get();
static void pat_view_set();
static void pat_view_map_set();
static void pat_view_map_base_set();
static void tiler_or_set();

static void tiler_or_set() /* (struct tiler_or *or, char id) */
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set TILER_OR__0 register */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)TILER_OR__0);
	reg_val = __raw_readl(reg);
	regdump("TILER_OR__0", reg_val);

	bit_field = BITFIELD(31, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)0) << field_pos) & bit_field);
	__raw_writel(0x88888888, reg); /* __raw_writel(new_val, reg); */

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)TILER_OR__0);
	reg_val = __raw_readl(reg);
	regdump("TILER_OR__0", reg_val);

	/* set TILER_OR__1 register */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)TILER_OR__1);
	reg_val = __raw_readl(reg);
	regdump("TILER_OR__1", reg_val);

	bit_field = BITFIELD(31, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)0) << field_pos) & bit_field);
	__raw_writel(0x88888888, reg); /* __raw_writel(new_val, reg); */

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)TILER_OR__1);
	reg_val = __raw_readl(reg);
	regdump("TILER_OR__1", reg_val);
}
static void pat_view_set() /* (struct pat_view *view, char id) */
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set PAT_VIEW__0 register */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_VIEW__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW__0", reg_val);

	bit_field = BITFIELD(31, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)0) << field_pos) & bit_field);
	__raw_writel(0x88888888, reg); /* __raw_writel(new_val, reg); */


	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_VIEW__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW__0", reg_val);

	/* set PAT_VIEW__1 register */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_VIEW__1);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW__1", reg_val);

	bit_field = BITFIELD(31, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)0) << field_pos) & bit_field);
	__raw_writel(0x88888888, reg); /* __raw_writel(new_val, reg); */

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_VIEW__1);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW__1", reg_val);
}
static void pat_view_map_set() /* (struct pat_view_map *map, char id) */
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set PAT_VIEW_MAP__0 register */
	reg = (void __iomem *)(
		(unsigned long)dmm_base | (unsigned long)PAT_VIEW_MAP__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW_MAP__0", reg_val);

	bit_field = BITFIELD(31, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)0) << field_pos) & bit_field);
	__raw_writel(0x80808080, reg); /* __raw_writel(new_val, reg); */


	reg = (void __iomem *)(
		(unsigned long)dmm_base | (unsigned long)PAT_VIEW_MAP__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW_MAP__0", reg_val);
}

static void pat_view_map_base_set()/*(struct pat_view_map_base *base, char id)*/
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set PAT_VIEW_MAP_BASE register */
	reg = (void __iomem *)(
		(unsigned long)dmm_base | (unsigned long)PAT_VIEW_MAP_BASE);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW_MAP_BASE", reg_val);

	bit_field = BITFIELD(31, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)0) << field_pos) & bit_field);
	__raw_writel(0x80000000, reg); /* __raw_writel(new_val, reg); */


	reg = (void __iomem *)(
		(unsigned long)dmm_base | (unsigned long)PAT_VIEW_MAP_BASE);
	reg_val = __raw_readl(reg);
	regdump("PAT_VIEW_MAP_BASE", reg_val);
}

static void hwinfo_get()
{
	void __iomem *reg = NULL;

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)DMM_REVISION);
	regdump("DMM_REVISION", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)DMM_HWINFO);
	regdump("DMM_HWINFO", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)LISA_HWINFO);
	regdump("LISA_HWINFO", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)TILER_HWINFO);
	regdump("TILER_HWINFO", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_HWINFO);
	regdump("PAT_HWINFO", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_GEOMETRY);
	regdump("PAT_GEOMETRY", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_STATUS__0);
	regdump("PAT_STATUS__0", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_STATUS__1);
	regdump("PAT_STATUS__1", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_STATUS__2);
	regdump("PAT_STATUS__2", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_STATUS__3);
	regdump("PAT_STATUS__3", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PEG_HWINFO);
	regdump("PEG_HWINFO", __raw_readl(reg));
}
static void pat_desc_set(struct pat_desc *desc, char id)
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* write to pat registers */
	/* opt to individually set each reg, so set PAT_DESC register to NULL */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_DESCR__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_DESCR__0", reg_val);

	bit_field = BITFIELD(31, 4);
	field_pos = 4;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)desc) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_DESCR__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_DESCR__0", reg_val);
}
static void pat_ctrl_set(struct pat_ctrl *ctrl, char id)
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set PAT_CTRL register */
	/* TODO: casting as unsigned long */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_CTRL__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_CTRL__0", reg_val);

	bit_field = BITFIELD(31, 28);
	field_pos = 28;
	new_val = (reg_val & (~(bit_field))) |
		((((unsigned long)ctrl->initiator) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(16, 16);
	field_pos = 16;
	new_val = (reg_val & (~(bit_field))) |
		((((unsigned long)ctrl->sync) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(9, 8);
	field_pos = 8;
	new_val = (reg_val & (~(bit_field))) |
		((((unsigned long)ctrl->lut_id) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(6, 4);
	field_pos = 4;
	new_val = (reg_val & (~(bit_field))) |
		((((unsigned long)ctrl->direction) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(0, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
		((((unsigned long)ctrl->start) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_CTRL__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_CTRL__0", reg_val);
}
static void pat_data_set(unsigned long data, char id)
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set PAT_DATA register */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_DATA__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_DATA__0", reg_val);

	bit_field = BITFIELD(31, 4);
	field_pos = 4;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)data >> 4) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_DATA__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_DATA__0", reg_val);
}
static void pat_area_set(struct pat_area *area, char id)
{
	void __iomem *reg = NULL;
	unsigned long reg_val = 0x0;
	unsigned long new_val = 0x0;
	unsigned long bit_field = 0x0;
	unsigned long field_pos = 0x0;

	/* set PAT_AREA register */
	/* TODO: changed casting from char to unsigned long */
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_AREA__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_AREA__0", reg_val);

	bit_field = BITFIELD(30, 24);
	field_pos = 24;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)area->y1) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(23, 16);
	field_pos = 16;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)area->x1) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(14, 8);
	field_pos = 8;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)area->y0) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	bit_field = BITFIELD(7, 0);
	field_pos = 0;
	new_val = (reg_val & (~(bit_field))) |
			((((unsigned long)area->x0) << field_pos) & bit_field);
	__raw_writel(new_val, reg);

	reg_val = __raw_readl(reg);
	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)PAT_AREA__0);
	reg_val = __raw_readl(reg);
	regdump("PAT_AREA__0", reg_val);
}
#endif /* end: #ifdef __NEWCODE__ */

#define DMM_MAJOR 0
#define DMM_MINOR 0
#define DMM_IO_BASE_ADDR 0x4e000000

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

#ifndef __NEWCODE__
	/* config LISA/PAT */
	dmm_config();
#else
	void __iomem *reg = NULL;
	dmm_base = ioremap(DMM_BASE, 0x1000);

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)LISA_MAP__0);
	regdump("LISA_MAP__0", __raw_readl(reg));

	reg = (void __iomem *)(
			(unsigned long)dmm_base | (unsigned long)LISA_MAP__1);
	regdump("LISA_MAP__1", __raw_readl(reg));

	pat_view_set();
	pat_view_map_set();
	pat_view_map_base_set();
	tiler_or_set();
#endif

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
dmm_ioctl(struct inode *ip, struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	struct tiler_buf_info *bufinfo = NULL;
	void *ptr = NULL;
	int retval = -1;
	int error = -1;
	pgd_t *pgd = NULL;
	pmd_t *pmd = NULL;
	pte_t *ptep = NULL, pte = 0x0;
	unsigned long bytes = 0x1;
	int offset = 0x0;
	unsigned long ssptr = 0x0;

	struct tiler_buf_info buf_info = {0};
	struct tiler_block_info block_info = {0};

	switch (cmd) {
	case TILIOC_OPEN:
		debug(__LINE__);
		retval = 0;
		break;
	case TILIOC_CLOSE:
		debug(__LINE__);
		retval = 0;
		break;
	case TILIOC_GBUF:
		debug(__LINE__);

		bytes = copy_from_user((void *)(&block_info),
			(const void *)arg, sizeof(struct tiler_block_info));
		if (bytes != 0)
			return retval;

		if (block_info.fmt == TILFMT_PAGE) {
			error = tiler_alloc_buf(block_info.fmt,
						block_info.dim.len,
						1,
						&ptr);
		} else {
			error = tiler_alloc_buf(block_info.fmt,
						block_info.dim.area.width,
						block_info.dim.area.height,
						&ptr);
		}

		if (error != 0)
			return retval;

		block_info.ssptr = (unsigned long)ptr;
		bytes = copy_to_user((void *)arg, (const void *)(&block_info),
					sizeof(struct tiler_block_info));
		if (bytes != 0)
			return retval;

		retval = 0;
		break;
	case TILIOC_FBUF:
		debug(__LINE__);

		bytes = copy_from_user((void *)(&block_info),
			(const void *)arg, sizeof(struct tiler_block_info));
		if (bytes != 0)
			return retval;

		error = tiler_free_buf(block_info.ssptr);
		if (error == 0)
			retval = 0;
		break;
	case TILIOC_GSSP:
		debug(__LINE__);

		pgd = pgd_offset(current->mm, arg);
		if (!(pgd_none(*pgd) || pgd_bad(*pgd))) {
			pmd = pmd_offset(pgd, arg);
			if (!(pmd_none(*pmd) || pmd_bad(*pmd))) {
				ptep = pte_offset_map(pmd, arg);
				if (ptep) {
					pte = *ptep;
					if (pte_present(pte)) {
						ssptr = (pte & PAGE_MASK)
							| (~PAGE_MASK & arg);
						retval = ssptr;
						break;
					}
				}
			}
		}
		retval = 0x0; /* va not in page table */
		break;
	case TILIOC_MBUF:
		debug(__LINE__);
		retval = 0;
		break;
	case TILIOC_QBUF:
		debug(__LINE__);

		bytes = copy_from_user((void *)(&buf_info),
			(const void *)arg, sizeof(struct tiler_buf_info));
		if (bytes != 0)
			return retval;

		offset = buf_info.offset;
		error = tiler_get_buf_info(lsthd, &bufinfo, offset);
		if (error != 0)
			return retval;

		bytes = copy_to_user((void *)arg,
			(const void *)bufinfo, sizeof(struct tiler_buf_info));
		if (bytes != 0)
			return retval;

		retval = 0;
		break;
	case TILIOC_RBUF:
		debug(__LINE__);

		bufinfo = kmalloc(sizeof(struct tiler_buf_info), GFP_KERNEL);
		memset(bufinfo, 0x0, sizeof(struct tiler_buf_info));
		bytes = copy_from_user((void *)bufinfo,
			(const void *)arg, sizeof(struct tiler_buf_info));
		if (bytes != 0)
			return retval;

		bufinfo->offset = id;
		id += 0x1000;

		bytes = copy_to_user((void *)arg,
			(const void *)bufinfo, sizeof(struct tiler_buf_info));
		if (bytes != 0)
			return retval;

		error = addnode(lsthd, bufinfo);
		if (error != 0)
			return retval;
		retval = 0;
		break;
	case TILIOC_URBUF:
		debug(__LINE__);

		bytes = copy_from_user((void *)(&buf_info),
			(const void *)arg, sizeof(struct tiler_buf_info));
		if (bytes != 0)
			return retval;

		offset = buf_info.offset;
		error = removenode(lsthd, offset);
		if (error != 0)
			return retval;

		retval = 0;
		break;
	case TILIOC_QUERY_BLK:
		debug(__LINE__);

		bytes = copy_from_user((void *)(&block_info),
			(const void *)arg, sizeof(struct tiler_block_info));
		if (bytes != 0)
			return retval;

		error = tiler_find_buf(block_info.ssptr, &block_info);
		if (error != 0)
			return retval;

		bytes = copy_to_user((void *)arg,
			(const void *)(&block_info),
			sizeof(struct tiler_block_info));
		if (bytes != 0)
			return retval;

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

	tilerdump(__LINE__);
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
	/* printk(KERN_NOTICE "dmm VMA close.\n"); */
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

	tilerdump(__LINE__);
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
	tilerdump(__LINE__);
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

	tilerdump(__LINE__);

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
		/* printk(KERN_ERR "x(%u-%u=%u>%u) y(%u-%u=%u>%u)\n",
		  bufferMappedZone->x0, bufferMappedZone->x1,
		  bufferMappedZone->xPageCount, bufferMappedZone->xPageOfst,
		  bufferMappedZone->y0, bufferMappedZone->y1,
		  bufferMappedZone->yPageCount, bufferMappedZone->yPageOfst); */

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

	tilerdump(__LINE__);

	area = dmm_tiler_get_area_from_sysptr(dmmTilerCtx,
		(void *)(DMM_ALIAS_VIEW_CLEAR & sysptr));
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

	tilerdump(__LINE__);

	/* if (aliasViewPtr) {
		allocedPtr = (void *)((unsigned long)allocedPtr &
				      DMM_ALIAS_VIEW_CLEAR);
	} */

	areaToFree = dmm_tiler_get_area_from_sysptr(dmmTilerCtx,
			(void *)(DMM_ALIAS_VIEW_CLEAR & sysptr));
	if (areaToFree != NULL)
		eCode = dmm_tiler_container_unmap_area(dmmTilerCtx, areaToFree);
	else
		eCode = DMM_SYS_ERROR;

	return eCode;
}
EXPORT_SYMBOL(tiler_free_buf);



#define DMM_SHIFT_PER_X_8 0
#define DMM_SHIFT_PER_Y_8 0
#define DMM_SHIFT_PER_X_16 0
#define DMM_SHIFT_PER_Y_16 1
#define DMM_SHIFT_PER_X_32 1
#define DMM_SHIFT_PER_Y_32 1
#define DMM_SHIFT_PER_X_PAGE 6
#define DMM_SHIFT_PER_Y_PAGE 6

#define DMM_TILER_THE(NAME) (1 << DMM_TILER_##NAME##_BITS)
#define DMM_TILER_THE_(N, NAME) (1 << DMM_TILER_##NAME##_BITS_(N))

#define DMM_TILER_CONT_WIDTH_BITS  14
#define DMM_TILER_CONT_HEIGHT_BITS 13

#define DMM_SHIFT_PER_P_(N) (DMM_SHIFT_PER_X_##N + DMM_SHIFT_PER_Y_##N)

#define DMM_TILER_CONT_HEIGHT_BITS_(N) \
	(DMM_TILER_CONT_HEIGHT_BITS - DMM_SHIFT_PER_Y_##N)
#define DMM_TILER_CONT_WIDTH_BITS_(N) \
	(DMM_TILER_CONT_WIDTH_BITS - DMM_SHIFT_PER_X_##N)

#define DMM_TILER_MASK(bits) ((1 << (bits)) - 1)

#define DMM_TILER_GET_OFFSET_(N, var) \
	((((unsigned long) var) & DMM_TILER_MASK(DMM_TILER_CONT_WIDTH_BITS + \
	DMM_TILER_CONT_HEIGHT_BITS)) >> DMM_SHIFT_PER_P_(N))

#define DMM_TILER_GET_0_X_(N, var) \
	(DMM_TILER_GET_OFFSET_(N, var) & \
	DMM_TILER_MASK(DMM_TILER_CONT_WIDTH_BITS_(N)))
#define DMM_TILER_GET_0_Y_(N, var) \
	(DMM_TILER_GET_OFFSET_(N, var) >> DMM_TILER_CONT_WIDTH_BITS_(N))
#define DMM_TILER_GET_90_X_(N, var) \
	(DMM_TILER_GET_OFFSET_(N, var) & \
	DMM_TILER_MASK(DMM_TILER_CONT_HEIGHT_BITS_(N)))
#define DMM_TILER_GET_90_Y_(N, var) \
	(DMM_TILER_GET_OFFSET_(N, var) >> DMM_TILER_CONT_HEIGHT_BITS_(N))

void tiler_get_natural_xy(unsigned long tsptr, unsigned long *x,
			  unsigned long *y)
{
	unsigned long x_bits, y_bits, offset;

	switch (DMM_GET_ACC_MODE(tsptr)) {
	case MODE_8_BIT:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(8);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(8);
		offset = DMM_TILER_GET_OFFSET_(8, tsptr);
		break;
	case MODE_16_BIT:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(16);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(16);
		offset = DMM_TILER_GET_OFFSET_(16, tsptr);
		break;
	case MODE_32_BIT:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(32);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(32);
		offset = DMM_TILER_GET_OFFSET_(32, tsptr);
		break;
	case MODE_PAGE:
	default:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(PAGE);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(PAGE);
		offset = DMM_TILER_GET_OFFSET_(PAGE, tsptr);
		break;
	}

	if (DMM_GET_ROTATED(tsptr)) {
		*x = offset >> y_bits;
		*y = offset & DMM_TILER_MASK(y_bits);
	} else {
		*x = offset & DMM_TILER_MASK(x_bits);
		*y = offset >> x_bits;
	}

	if (DMM_GET_X_INVERTED(tsptr))
		*x ^= DMM_TILER_MASK(x_bits);
	if (DMM_GET_Y_INVERTED(tsptr))
		*y ^= DMM_TILER_MASK(y_bits);
}

unsigned long tiler_get_address(struct dmmViewOrientT orient,
				enum dmmMemoryAccessT accessModeM,
				unsigned long x, unsigned long y)
{
	unsigned long x_bits, y_bits, tmp, x_mask, y_mask, alignment;

	switch (accessModeM) {
	case MODE_8_BIT:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(8);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(8);
		alignment = DMM_SHIFT_PER_P_(8);
		break;
	case MODE_16_BIT:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(16);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(16);
		alignment = DMM_SHIFT_PER_P_(16);
		break;
	case MODE_32_BIT:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(32);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(32);
		alignment = DMM_SHIFT_PER_P_(32);
		break;
	case MODE_PAGE:
	default:
		x_bits = DMM_TILER_CONT_WIDTH_BITS_(PAGE);
		y_bits = DMM_TILER_CONT_HEIGHT_BITS_(PAGE);
		alignment = DMM_SHIFT_PER_P_(PAGE);
		break;
	}

	x_mask = DMM_TILER_MASK(x_bits);
	y_mask = DMM_TILER_MASK(y_bits);
	if (x < 0 || x > x_mask || y < 0 || y > y_mask)
		return 0;

	if (orient.dmmXInvert)
		x ^= x_mask;
	if (orient.dmmYInvert)
		y ^= y_mask;

	if (orient.dmm90Rotate)
		tmp = ((x << y_bits) + y);
	else
		tmp = ((y << x_bits) + x);

	return (unsigned long)
		DMM_COMPOSE_TILER_PTR((tmp << alignment),
				      (orient.dmm90Rotate ? 1 : 0),
				      (orient.dmmYInvert ? 1 : 0),
				      (orient.dmmXInvert ? 1 : 0),
				      accessModeM);
}

unsigned long
tiler_reorient_addr(unsigned long tsptr,
		struct dmmViewOrientT orient)
{
	unsigned long x, y;

	tilerdump(__LINE__);

	tiler_get_natural_xy(tsptr, &x, &y);
	return tiler_get_address(orient, DMM_GET_ACC_MODE(tsptr), x, y);
}
EXPORT_SYMBOL(tiler_reorient_addr);

unsigned long
tiler_get_natural_addr(void *sysPtr)
{
	return (unsigned long)sysPtr & DMM_ALIAS_VIEW_CLEAR;
}
EXPORT_SYMBOL(tiler_get_natural_addr);

unsigned long
tiler_reorient_topleft(unsigned long tsptr,
		       struct dmmViewOrientT orient,
		       unsigned int validDataWidth,
		       unsigned int validDataHeight)
{
	struct dmmTILERContPageAreaT *bufferMappedZone;
	struct dmmTILERContCtxT *dmmTilerCtx =
			&((struct dmmInstanceCtxT *)ctxptr)->dmmTilerCtx;
	enum dmmMemoryAccessT accessModeM;
	unsigned long x_pagew, y_pagew, x, y;

	tilerdump(__LINE__);

	accessModeM = DMM_GET_ACC_MODE(tsptr);

	bufferMappedZone = \
		dmm_tiler_get_area_from_sysptr(dmmTilerCtx, (void *)tsptr);

	if (bufferMappedZone == NULL)
		return 0x0;

	switch (accessModeM) {
	case MODE_8_BIT:
		x_pagew = DMM_PAGE_DIMM_X_MODE_8;
		y_pagew = DMM_PAGE_DIMM_Y_MODE_8;
		break;
	case MODE_16_BIT:
		x_pagew = DMM_PAGE_DIMM_X_MODE_16;
		y_pagew = DMM_PAGE_DIMM_Y_MODE_16;
		break;
	case MODE_32_BIT:
		x_pagew = DMM_PAGE_DIMM_X_MODE_32;
		y_pagew = DMM_PAGE_DIMM_Y_MODE_32;
		break;
	case MODE_PAGE:
	default:
		x_pagew = y_pagew = 1;
		break;
	}

	if (!validDataWidth)
		validDataWidth = (bufferMappedZone->x1 -
				  bufferMappedZone->x0 + 1) * x_pagew;

	if (!validDataHeight)
		validDataHeight = (bufferMappedZone->y1 -
				  bufferMappedZone->y0 + 1) * y_pagew;

	x = bufferMappedZone->x0 * x_pagew;
	y = bufferMappedZone->y0 * y_pagew;

	if (orient.dmmXInvert)
		x += validDataWidth - 1;
	if (orient.dmmYInvert)
		y += validDataHeight - 1;

	return tiler_get_address(orient, accessModeM, x, y);
}
EXPORT_SYMBOL(tiler_reorient_topleft);

void
tiler_rotate_view(struct dmmViewOrientT *orient, unsigned long rotation)
{
	rotation = (rotation / 90) & 3;

	if (rotation & 2) {
		orient->dmmXInvert = !orient->dmmXInvert;
		orient->dmmYInvert = !orient->dmmYInvert;
	}

	if (rotation & 1) {
		if (orient->dmm90Rotate)
			orient->dmmYInvert = !orient->dmmYInvert;
		else
			orient->dmmXInvert = !orient->dmmXInvert;
		orient->dmm90Rotate = !orient->dmm90Rotate;
	}
}
EXPORT_SYMBOL(tiler_rotate_view);

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
		/* printk(KERN_ERR "%s():%d: success!\n", __func__, __LINE__);*/
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
			kfree(tmpnode->ptr);
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


