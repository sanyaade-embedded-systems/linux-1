/*
 * include/linux/omap_gpu.h
 *
 * Copyright (C) 2011 Texas Instruments
 * Author: Rob Clark <rob@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __OMAP_GPU_H__
#define __OMAP_GPU_H__

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fb.h>
#include <drm/drmP.h>
#include <linux/fb.h>

void omap_gpu_hotplug_event(struct fb_info *fbi);
int omap_gpu_register_fbs(int num_fbs, struct fb_info **fbs);

/* interface that plug-in drivers (for now just PVR) can implement */
struct omap_gpu_plugin {
	const char *name;

	/* drm functions */
	int (*open)(struct drm_device *dev, struct drm_file *file);
	int (*load)(struct drm_device *dev, unsigned long flags);
	int (*unload)(struct drm_device *dev);

	/* file-ops */
	int (*release)(struct inode *inode, struct file *file);
	int (*mmap)(struct file *file, struct vm_area_struct *vma);

	struct drm_ioctl_desc *ioctls;
	int num_ioctls;
	int ioctl_start;

	struct list_head list;  /* note, this means struct can't be const.. */
};

int omap_gpu_register_plugin(struct omap_gpu_plugin *plugin);
int omap_gpu_unregister_plugin(struct omap_gpu_plugin *plugin);


#endif /* __OMAP_GPU_H__ */
