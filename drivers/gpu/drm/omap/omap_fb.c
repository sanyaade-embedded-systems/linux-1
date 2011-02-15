/*
 * linux/drivers/gpu/drm/omap/omap_fb.c
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

#include <linux/omap_gpu.h>
#include "omap_gpu_priv.h"

#include "drm_crtc.h"
#include "drm_crtc_helper.h"

/*
 * framebuffer funcs
 */

#define to_omap_framebuffer(x) container_of(x, struct omap_framebuffer, base)

struct omap_framebuffer {
	struct drm_framebuffer base;
};


static int omap_framebuffer_create_handle(struct drm_framebuffer *fb,
						struct drm_file *file_priv,
						unsigned int *handle)
{
	struct omap_framebuffer *omap_fb = to_omap_framebuffer(fb);
	DBG("framebuffer: get handle: %p", omap_fb);

	// TODO, I suppose this really should be some sort of GEM handle
	// to the framebuffer object, in case it needs to be mapped or
	// something.  Right now this will go-exist badly with PVR, who
	// implements the mmap() fxn.. need to think about how to handle
	// this..

	*handle = 42;

	return 0;
}

static void omap_framebuffer_destroy(struct drm_framebuffer *fb)
{
	struct omap_framebuffer *omap_fb = to_omap_framebuffer(fb);
	DBG("framebuffer: destroy: %p", omap_fb);
	drm_framebuffer_cleanup(fb);
	kfree(omap_fb);
}


static const struct drm_framebuffer_funcs omap_framebuffer_funcs = {
	.create_handle = omap_framebuffer_create_handle,
	.destroy = omap_framebuffer_destroy,
};

struct drm_framebuffer * omap_fb_create(struct drm_device *dev,
		struct drm_file *file, struct drm_mode_fb_cmd *mode_cmd)
{
	struct omap_framebuffer *omap_fb = NULL;
	struct drm_framebuffer *fb;
	int ret;

	DBG("create framebuffer: dev=%p, file=%p, mode_cmd=%p",
			dev, file, mode_cmd);

	omap_fb = kzalloc(sizeof(*omap_fb), GFP_KERNEL);
	if (!omap_fb) {
		dev_err(dev->dev, "could not allocate fb\n");
		goto fail;
	}

	fb = &omap_fb->base;
	ret = drm_framebuffer_init(dev, fb, &omap_framebuffer_funcs);
	if (ret) {
		dev_err(dev->dev, "framebuffer init failed: %d\n", ret);
		goto fail;
	}

	drm_helper_mode_fill_fb_struct(fb, mode_cmd);

	return fb;

fail:
	if (omap_fb) {
		kfree(omap_fb);
	}
	return NULL;
}

