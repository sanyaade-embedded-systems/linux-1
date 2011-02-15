/*
 * linux/drivers/gpu/drm/omap/omap_connector.c
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
 * connector funcs
 */

#define to_omap_connector(x) container_of(x, struct omap_connector, base)

struct omap_connector {
	struct drm_connector base;
	struct omap_dss_device *dssdev;
};


static inline void copy_timings_omap_to_drm(struct drm_display_mode *mode,
		struct omap_video_timings *timings)
{
	mode->clock = timings->pixel_clock;

	mode->hdisplay = timings->x_res;
	mode->hsync_start = mode->hdisplay + timings->hfp;
	mode->hsync_end = mode->hsync_start + timings->hsw;
	mode->htotal = mode->hsync_end + timings->hbp;

	mode->vdisplay = timings->y_res;
	mode->vsync_start = mode->vdisplay + timings->vfp;
	mode->vsync_end = mode->vsync_start + timings->vsw;
	mode->vtotal = mode->vsync_end + timings->vbp;

	/* note: whether or not it is interlaced, +/- h/vsync, etc,
	 * which should be set in the mode flags, is not exposed in
	 * the omap_video_timings struct.. but hdmi driver tracks
	 * those separately so all we have to have to set the mode
	 * is the way to recover these timings values, and the
	 * omap_dss_driver would do the rest.
	 */
}

static inline void copy_timings_drm_to_omap(struct omap_video_timings *timings,
		struct drm_display_mode *mode)
{
	timings->pixel_clock = mode->clock;

	timings->x_res = mode->hdisplay;
	timings->hfp = mode->hsync_start - mode->hdisplay;
	timings->hsw = mode->hsync_end - mode->hsync_start;
	timings->hbp = mode->htotal - mode->hsync_end;

	timings->y_res = mode->vdisplay;
	timings->vfp = mode->vsync_start - mode->vdisplay;
	timings->vsw = mode->vsync_end - mode->vsync_start;
	timings->vbp = mode->vtotal - mode->vsync_end;
}

static enum drm_connector_status
omap_connector_detect(struct drm_connector *connector)
{
	struct omap_connector *omap_connector = to_omap_connector(connector);
	struct omap_dss_device *dssdev = omap_connector->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;

	DBG("connector: detect: %p", omap_connector);

	if (dssdrv->smart_is_enabled) {
		if (dssdrv->smart_is_enabled(dssdev)) {
			return connector_status_connected;
		} else {
			return connector_status_disconnected;
		}
	}

	return connector_status_unknown;
}

static void omap_connector_destroy(struct drm_connector *connector)
{
	struct omap_connector *omap_connector = to_omap_connector(connector);
	DBG("connector: destroy: %p", omap_connector);
	drm_sysfs_connector_remove(connector);
	drm_connector_cleanup(connector);
	kfree(omap_connector);
}

static int omap_connector_get_modes(struct drm_connector *connector)
{
	struct omap_connector *omap_connector = to_omap_connector(connector);
	struct omap_dss_device *dssdev = omap_connector->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;
	int n = 0;

	DBG("connector: get modes: %p", omap_connector);

	/* if display exposes EDID, then we parse that in the normal way to
	 * build table of supported modes.. otherwise (ie. fixed resolution
	 * LCD panels) we just return a single mode corresponding to the
	 * currently configured timings:
	 */
	if (dssdrv->get_edid) {
		void *edid = dssdrv->get_edid(dssdev);

		drm_mode_connector_update_edid_property(connector, edid);
		n = drm_add_edid_modes(connector, edid);

		kfree(connector->display_info.raw_edid);
		connector->display_info.raw_edid = edid;

	} else {
		struct drm_display_mode *mode = drm_mode_create(connector->dev);
		struct omap_video_timings timings;

		dssdrv->get_timings(dssdev, &timings);

		copy_timings_omap_to_drm(mode, &timings);

		mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
		drm_mode_set_name(mode);
		drm_mode_probed_add(connector, mode);

		n = 1;
	}

	return n;
}

static int omap_connector_mode_valid(struct drm_connector *connector,
				 struct drm_display_mode *mode)
{
	struct omap_connector *omap_connector = to_omap_connector(connector);
	struct omap_dss_device *dssdev = omap_connector->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;
	struct omap_video_timings timings = {0};
	int ret = MODE_BAD;
	int n = 0;

	copy_timings_drm_to_omap(&timings, mode);

	if (!dssdrv->check_timings(dssdev, &timings)) {
		ret = MODE_OK;
	}

#if 0
	DBG("connector: mode %s: "
			"%d:\"%s\" %d %d %d %d %d %d %d %d %d %d 0x%x 0x%x",
			(ret == MODE_OK) ? "valid" : "invalid",
			mode->base.id, mode->name, mode->vrefresh, mode->clock,
			mode->hdisplay, mode->hsync_start,
			mode->hsync_end, mode->htotal,
			mode->vdisplay, mode->vsync_start,
			mode->vsync_end, mode->vtotal, mode->type, mode->flags);
#endif

	return ret;
}

static struct drm_encoder *
omap_connector_attached_encoder (struct drm_connector *connector)
{
	int i;
	struct omap_connector *omap_connector = to_omap_connector(connector);

	// XXX get the one currently whose manager is connected to this dssdev

	DBG("connector: find attached encoder: %p", omap_connector);

	for (i = 0; i < DRM_CONNECTOR_MAX_ENCODER; i++) {
		struct drm_mode_object *obj;

		if (connector->encoder_ids[i] == 0)
			break;

		obj = drm_mode_object_find(connector->dev,
				connector->encoder_ids[i],
				DRM_MODE_OBJECT_ENCODER);

		if (obj)
			return obj_to_encoder(obj);
	}
	return NULL;
}

static const struct drm_connector_funcs omap_connector_funcs = {
	.dpms = drm_helper_connector_dpms,
	.detect = omap_connector_detect,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = omap_connector_destroy,
};

static const struct drm_connector_helper_funcs omap_connector_helper_funcs = {
	.get_modes = omap_connector_get_modes,
	.mode_valid = omap_connector_mode_valid,
	.best_encoder = omap_connector_attached_encoder,
};

/* called from encoder when mode is set, to propagate settings to the dssdev
 */
void omap_connector_mode_set(struct drm_connector *connector,
		struct drm_display_mode *mode)
{
	struct drm_device *dev = connector->dev;
	struct omap_connector *omap_connector = to_omap_connector(connector);
	struct omap_dss_device *dssdev = omap_connector->dssdev;
	struct omap_dss_driver *dssdrv = dssdev->driver;
	struct omap_video_timings timings;

	copy_timings_drm_to_omap(&timings, mode);

	if (dssdrv->check_timings(dssdev, &timings)) {
		dev_err(dev->dev, "could not set timings\n");
		return;
	}

	DBG("connector: set mode: "
			"%d:\"%s\" %d %d %d %d %d %d %d %d %d %d 0x%x 0x%x\n",
			mode->base.id, mode->name, mode->vrefresh, mode->clock,
			mode->hdisplay, mode->hsync_start,
			mode->hsync_end, mode->htotal,
			mode->vdisplay, mode->vsync_start,
			mode->vsync_end, mode->vtotal, mode->type, mode->flags);

	dssdrv->set_timings(dssdev, &timings);
}

/* initialize connector */
struct drm_connector * omap_connector_init(struct drm_device *dev,
		int connector_type, struct omap_dss_device *dssdev)
{
	struct drm_connector *connector = NULL;
	struct omap_connector *omap_connector;

	omap_connector = kzalloc(sizeof(struct omap_connector), GFP_KERNEL);
	if (!omap_connector) {
		dev_err(dev->dev, "could not allocate connector\n");
		goto fail;
	}

	DBG("connector: init: %p", omap_connector);

	omap_connector->dssdev = dssdev;
	connector = &omap_connector->base;

	drm_connector_init(dev, connector, &omap_connector_funcs, connector_type);
	drm_connector_helper_add(connector, &omap_connector_helper_funcs);

	connector->polled = DRM_CONNECTOR_POLL_HPD;
	connector->interlace_allowed = 1;
	connector->doublescan_allowed = 0;

	drm_sysfs_connector_add(connector);

	return connector;

fail:
	if (connector) {
		drm_connector_cleanup(connector);
		kfree(omap_connector);
	}

	return NULL;
}
