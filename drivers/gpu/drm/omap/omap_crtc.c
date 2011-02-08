/*
 * linux/drivers/gpu/drm/omap/omap_crtc.c
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

#include "omap_gpu.h"
#include "omap_gpu_priv.h"

#include "drm_crtc.h"
#include "drm_crtc_helper.h"

#define to_omap_crtc(x) container_of(x, struct omap_crtc, base)

struct omap_crtc {
	struct drm_crtc base;
	struct fb_info *fbi;
	struct omap_overlay *ovl;
	struct omap_overlay_info info;

};

static void omap_crtc_gamma_set(struct drm_crtc *crtc,
		u16 *red, u16 *green, u16 *blue, uint32_t size)
{
//	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	DBG("crtc: set gamma");
	// XXX ignore?
}

static void omap_crtc_destroy(struct drm_crtc *crtc)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	DBG("crtc: destroy: %p", omap_crtc);
	drm_crtc_cleanup(crtc);
	kfree(omap_crtc);
}

static void omap_crtc_dpms(struct drm_crtc *crtc, int mode)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	DBG("crtc: dpms: %p, mode=%d", omap_crtc, mode);
}

static bool omap_crtc_mode_fixup(struct drm_crtc *crtc,
				  struct drm_display_mode *mode,
				  struct drm_display_mode *adjusted_mode)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	// XXX I guess we support anything?
	DBG("crtc: mode fixup: %p", omap_crtc);
	return true;
}

/* align x to next highest multiple of 2^n */
#define ALIGN2(x,n)   (((x) + ((1 << (n)) - 1)) & ~((1 << (n)) - 1))

static int omap_crtc_mode_set(struct drm_crtc *crtc,
			       struct drm_display_mode *mode,
			       struct drm_display_mode *adjusted_mode,
			       int x, int y,
			       struct drm_framebuffer *old_fb)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	int stride;

	/* PVR2D seems to need width to be multiple of 32 */
	stride = ALIGN2(mode->hdisplay, 5);

	/* just use adjusted mode */
	mode = adjusted_mode;

	omap_crtc->info.screen_width = stride;
	omap_crtc->info.width = mode->hdisplay;
	omap_crtc->info.height = mode->vdisplay;
	omap_crtc->info.color_mode = OMAP_DSS_COLOR_ARGB32;
	omap_crtc->info.rotation_type = OMAP_DSS_ROT_DMA;
	omap_crtc->info.rotation = OMAP_DSS_ROT_0;
	omap_crtc->info.mirror = 0;
	omap_crtc->info.pos_x = 0;
	omap_crtc->info.pos_y = 0;
	omap_crtc->info.out_width = mode->hdisplay;
	omap_crtc->info.out_height = mode->vdisplay;

	/* special hack to keep fb in sane state.. */
	{
		struct fb_var_screeninfo *var = &omap_crtc->fbi->var;
		struct fb_fix_screeninfo *fix = &omap_crtc->fbi->fix;

		var->xres_virtual = var->xres = mode->hdisplay;
		var->yres_virtual = var->yres = mode->vdisplay;
		fix->line_length = stride * (var->bits_per_pixel >> 3);
	}

	return 0;
}

static void omap_crtc_prepare (struct drm_crtc *crtc)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	struct drm_crtc_helper_funcs *crtc_funcs = crtc->helper_private;
	struct omap_overlay *ovl = omap_crtc->ovl;

	crtc_funcs->dpms(crtc, DRM_MODE_DPMS_OFF);

	ovl->get_overlay_info(ovl, &omap_crtc->info);
}

static void omap_crtc_commit (struct drm_crtc *crtc)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	struct drm_crtc_helper_funcs *crtc_funcs = crtc->helper_private;
	struct omap_overlay *ovl = omap_crtc->ovl;

	crtc_funcs->dpms(crtc, DRM_MODE_DPMS_OFF);

	ovl->set_overlay_info(ovl, &omap_crtc->info);

	crtc_funcs->dpms(crtc, DRM_MODE_DPMS_ON);
}

static int omap_crtc_pipe_set_base(struct drm_crtc *crtc, int x, int y,
		    struct drm_framebuffer *old_fb)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	// TODO set overlay position
	DBG("crtc: pipe set base: %p", omap_crtc);
	return 0;
}

void omap_crtc_load_lut(struct drm_crtc *crtc)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	DBG("crtc: load lut: %p", omap_crtc);
}

static const struct drm_crtc_funcs omap_crtc_funcs = {
	.gamma_set = omap_crtc_gamma_set,
	.set_config = drm_crtc_helper_set_config,
	.destroy = omap_crtc_destroy,
//TODO	.page_flip = omap_page_flip,
};

static const struct drm_crtc_helper_funcs omap_crtc_helper_funcs = {
	.dpms = omap_crtc_dpms,
	.mode_fixup = omap_crtc_mode_fixup,
	.mode_set = omap_crtc_mode_set,
	.prepare = omap_crtc_prepare,
	.commit = omap_crtc_commit,
	.mode_set_base = omap_crtc_pipe_set_base,
	.load_lut = omap_crtc_load_lut,
};

struct omap_overlay * omap_crtc_get_overlay(struct drm_crtc *crtc)
{
	struct omap_crtc *omap_crtc = to_omap_crtc(crtc);
	return omap_crtc->ovl;
}

/* initialize crtc
 */
struct drm_crtc * omap_crtc_init(struct drm_device *dev,
		struct omap_overlay *ovl, struct fb_info *fbi)
{
	struct drm_crtc *crtc = NULL;
	struct omap_crtc *omap_crtc =
			kzalloc(sizeof(*omap_crtc), GFP_KERNEL);

	if (!omap_crtc) {
		dev_err(dev->dev, "could not allocate CRTC\n");
		goto fail;
	}

	DBG("crtc: init crtc: %p", omap_crtc);

	omap_crtc->ovl = ovl;
	omap_crtc->fbi = fbi;
	crtc = &omap_crtc->base;
	drm_crtc_init(dev, crtc, &omap_crtc_funcs);
	drm_crtc_helper_add(crtc, &omap_crtc_helper_funcs);

	return crtc;

fail:
	if (crtc) {
		drm_crtc_cleanup(crtc);
		kfree(omap_crtc);
	}
	return NULL;
}
