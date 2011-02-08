/*
 * linux/drivers/gpu/drm/omap/omap_gpu.c
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

#define DRIVER_NAME		"omap_gpu"
#define DRIVER_DESC		"OMAP GPU"
#define DRIVER_DATE		"20110201"
#define DRIVER_MAJOR		1
#define DRIVER_MINOR		0
#define DRIVER_PATCHLEVEL	0

struct drm_device *drm_device;

/* TODO: think about how to handle more than one plugin.. ie. some ops
 * me might want to stop on the first plugin that doesn't return an
 * error, etc..
 */
LIST_HEAD(plugin_list);

/* keep track of whether we are already loaded.. we may need to call
 * plugin's load() if they register after we are already loaded
 */
static bool loaded = false;

/* just needed for load() */
static int num_fbs;
static struct fb_info **fbs;

/*
 * mode config funcs
 */

/* Notes about mapping DSS and DRM entities:
 *    CRTC:        overlay.. currently just one per omapfb since we still
 *                 lean on omapfb for some of the setup and configuration
 *    encoder:     manager.. with some extension to allow one primary CRTC
 *                 and zero or more video CRTC's to be mapped to one encoder?
 *    connector:   dssdev.. manager can be attached/detatched from different
 *                 devices
 */

static void omap_fb_output_poll_changed(struct drm_device *dev)
{
	DBG("dev=%p", dev);
//	drm_i915_private_t *dev_priv = dev->dev_private;
//	drm_fb_helper_hotplug_event(&dev_priv->fbdev->helper);
}

static struct drm_mode_config_funcs omap_mode_config_funcs = {
	.fb_create = omap_fb_create,
	.output_poll_changed = omap_fb_output_poll_changed,
};

/* find the matching fb attached to the specified overlay */
static struct fb_info * find_fbi(struct omap_overlay *ovl)
{
	int i,  j;

	for (i = 0; i < num_fbs; i++) {
		struct omapfb_info *ofbi = FB2OFB(fbs[i]);
		for (j = 0; j < ofbi->num_overlays; j++) {
			if (ofbi->overlays[j] == ovl) {
				return fbs[i];
			}
		}
	}

	return NULL;
}

static int get_connector_type(enum omap_display_type type)
{
	switch (type) {
	case OMAP_DISPLAY_TYPE_HDMI:
		return DRM_MODE_CONNECTOR_HDMIA;
	default:
		return DRM_MODE_CONNECTOR_Unknown;
	}
}

static int omap_modeset_init(struct drm_device *dev)
{
	struct omap_gpu_private *priv = dev->dev_private;
	struct omap_dss_device *dssdev = NULL;
	int i, n;

	drm_mode_config_init(dev);

	for (i = 0, n = 0; i < omap_dss_get_num_overlays(); i++) {
		struct omap_overlay *ovl = omap_dss_get_overlay(i);

		/* for now, since we are re-using parts of omapfb, we need to
		 * keep the mapping between fb and ovl static, and create the
		 * CRTC with the corresponding ofbi:
		 */
		struct fb_info *fbi = find_fbi(ovl);

		if (fbi) {
			struct drm_crtc *crtc = omap_crtc_init(dev, ovl, fbi);
			if (!crtc) {
				dev_err(dev->dev, "could not create CRTC\n");
				goto fail;
			}
			priv->crtcs[n++] = crtc;
			break; // XXX for now, just use one display to avoid confusing pvr..
		}
	}

	for (i = 0; i < omap_dss_get_num_overlay_managers(); i++) {
		int j;
		struct omap_overlay_manager *mgr = omap_dss_get_overlay_manager(i);
		struct drm_encoder *encoder = omap_encoder_init(dev, mgr);

		if (!encoder) {
			dev_err(dev->dev, "could not create encoder\n");
			goto fail;
		}

		priv->encoders[i] = encoder;

		encoder->possible_crtcs = 0;

		/* for now, since we are re-using parts of omapfb, keep the
		 * mapping between encoders and CRTCs static:
		 */
		for (j = 0; priv->crtcs[j]; j++) {
			struct omap_overlay *ovl =
					omap_crtc_get_overlay(priv->crtcs[j]);
			if (ovl->manager == mgr) {
				encoder->possible_crtcs |= (1 << j);
			}
		}
	}

	i = 0;
	for_each_dss_dev(dssdev) {
		int j;
		struct drm_connector *connector = omap_connector_init(dev,
				get_connector_type(dssdev->type), dssdev);

		if (!connector) {
			dev_err(dev->dev, "could not create connector\n");
			goto fail;
		}

		priv->connectors[i++] = connector;

		for (j = 0; priv->encoders[j]; j++) {
			struct omap_overlay_manager *mgr =
					omap_encoder_get_manager(priv->encoders[j]);
			if (mgr->device == dssdev) {
				drm_mode_connector_attach_encoder(connector,
						priv->encoders[j]);
			}
		}
	}

	dev->mode_config.min_width = 640;
	dev->mode_config.min_height = 480;

	dev->mode_config.max_width = 2048;
	dev->mode_config.max_height = 2048;

	dev->mode_config.funcs = &omap_mode_config_funcs;

	return 0;

fail:
	/* TODO: cleanup what has been created so far */
	return -EINVAL;
}

#if 0
/* something like this.. have look at intel driver for example */
static int omap_fbdev_init(struct drm_device *dev)
{
	struct intel_fbdev *ifbdev;
	drm_i915_private_t *dev_priv = dev->dev_private;
	int ret;

	ifbdev = kzalloc(sizeof(struct intel_fbdev), GFP_KERNEL);
	if (!ifbdev)
		return -ENOMEM;

	dev_priv->fbdev = ifbdev;
	ifbdev->helper.funcs = &intel_fb_helper_funcs;

	ret = drm_fb_helper_init(dev, &ifbdev->helper,
				 dev_priv->num_pipe,
				 INTELFB_CONN_LIMIT);
	if (ret) {
		kfree(ifbdev);
		return ret;
	}

	drm_fb_helper_single_add_all_connectors(&ifbdev->helper);
	drm_fb_helper_initial_config(&ifbdev->helper, 32);
	return 0;
}
#endif

/*
 * drm driver funcs
 */

/**
 * load - setup chip and create an initial config
 * @dev: DRM device
 * @flags: startup flags
 *
 * The driver load routine has to do several things:
 *   - initialize the memory manager
 *   - allocate initial config memory
 *   - setup the DRM framebuffer with the allocated memory
 */
static int dev_load(struct drm_device *dev, unsigned long flags)
{
	struct omap_gpu_private *priv;
	struct omap_gpu_plugin *plugin;
	int ret;

	DBG("load: dev=%p, num_fbs=%d, fbs=%p", dev, num_fbs, fbs);

	drm_device = dev;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		dev_err(dev->dev, "could not allocate priv\n");
		return -1;
	}

	dev->dev_private = priv;

	ret = omap_modeset_init(dev);
	if (ret) {
		// hmm
		//return ret;
	}
#if 0
	ret = omap_fbdev_init(dev);
	if (ret) {
		// hmm
		return ret;
	}
#endif

	loaded = true;

	list_for_each_entry(plugin, &plugin_list, list) {
		ret = plugin->load(dev, flags);
	}

	return 0;
}

static int dev_unload(struct drm_device *dev)
{
	struct omap_gpu_plugin *plugin;
	int ret;

	DBG("unload: dev=%p", dev);

	list_for_each_entry(plugin, &plugin_list, list) {
		ret = plugin->unload(dev);
	}

	loaded = false;

	return 0;
}

static int dev_open(struct drm_device *dev, struct drm_file *file)
{
	struct omap_gpu_plugin *plugin;
	bool found_pvr = false;
	int ret;

	DBG("open: dev=%p", dev);

	list_for_each_entry(plugin, &plugin_list, list) {
		if (!strcmp("omap_gpu_pvr", plugin->name)) {
			found_pvr = true;
			break;
		}
	}

	if (!found_pvr) {
		DBG("open: PVR submodule not loaded.. let's try now");
		request_module("omap_gpu_pvr");
	}

	list_for_each_entry(plugin, &plugin_list, list) {
		ret = plugin->open(dev, file);
	}

	return 0;
}

static int dev_firstopen(struct drm_device *dev)
{
	DBG("firstopen: dev=%p", dev);
	return 0;
}

/**
 * lastclose - clean up after all DRM clients have exited
 * @dev: DRM device
 *
 * Take care of cleaning up after all DRM clients have exited.  In the
 * mode setting case, we want to restore the kernel's initial mode (just
 * in case the last client left us in a bad state).
 *
 * Additionally, in the non-mode setting case, we'll tear down the AGP
 * and DMA structures, since the kernel won't be using them, and clean
 * up any GEM state.
 */
static void dev_lastclose(struct drm_device * dev)
{
	DBG("lastclose: dev=%p", dev);
}

static void dev_preclose(struct drm_device * dev, struct drm_file *file_priv)
{
	DBG("preclose: dev=%p", dev);
}

static void dev_postclose(struct drm_device *dev, struct drm_file *file_priv)
{
	DBG("postclose: dev=%p", dev);
}

/**
 * enable_vblank - enable vblank interrupt events
 * @dev: DRM device
 * @crtc: which irq to enable
 *
 * Enable vblank interrupts for @crtc.  If the device doesn't have
 * a hardware vblank counter, this routine should be a no-op, since
 * interrupts will have to stay on to keep the count accurate.
 *
 * RETURNS
 * Zero on success, appropriate errno if the given @crtc's vblank
 * interrupt cannot be enabled.
 */
static int dev_enable_vblank(struct drm_device *dev, int crtc)
{
	DBG("enable_vblank: dev=%p, crtc=%d", dev, crtc);
	return 0;
}

/**
 * disable_vblank - disable vblank interrupt events
 * @dev: DRM device
 * @crtc: which irq to enable
 *
 * Disable vblank interrupts for @crtc.  If the device doesn't have
 * a hardware vblank counter, this routine should be a no-op, since
 * interrupts will have to stay on to keep the count accurate.
 */
static void dev_disable_vblank(struct drm_device *dev, int crtc)
{
	DBG("disable_vblank: dev=%p, crtc=%d", dev, crtc);
}

/**
 * Called by \c drm_device_is_agp.  Typically used to determine if a
 * card is really attached to AGP or not.
 *
 * \param dev  DRM device handle
 *
 * \returns
 * One of three values is returned depending on whether or not the
 * card is absolutely \b not AGP (return of 0), absolutely \b is AGP
 * (return of 1), or may or may not be AGP (return of 2).
 */
static int dev_device_is_agp(struct drm_device *dev)
{
	return 0;
}

static irqreturn_t dev_irq_handler(DRM_IRQ_ARGS)
{
	return IRQ_HANDLED;
}

static void dev_irq_preinstall(struct drm_device *dev)
{
	DBG("irq_preinstall: dev=%p", dev);
}

static int dev_irq_postinstall(struct drm_device *dev)
{
	DBG("irq_postinstall: dev=%p", dev);
	return 0;
}

static void dev_irq_uninstall(struct drm_device *dev)
{
	DBG("irq_uninstall: dev=%p", dev);
}

static int fop_release(struct inode *inode, struct file *file)
{
	struct drm_device *dev = drm_device;
	struct omap_gpu_plugin *plugin;
	int ret;

	DBG("release: inode=%p, file=%p", inode, file);

	ret = drm_release(inode, file);
	if (ret) {
		dev_err(dev->dev, "drm_release failed: ret=%d\n", ret);
	}

	list_for_each_entry(plugin, &plugin_list, list) {
		ret = plugin->release(inode, file);
	}

	return ret;
}

static int fop_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct omap_gpu_plugin *plugin;
	int ret = 0;

	list_for_each_entry(plugin, &plugin_list, list) {
		ret = plugin->mmap(file, vma);
		if (!ret) {
			/* on first plugin that succeeds, bail out of iteration */
			return ret;
		}
	}

	return ret;
}

static struct drm_driver omap_gpu_driver;

static int pdev_suspend(struct platform_device *pDevice, pm_message_t state)
{
	DBG("pdev_suspend");
	return 0;
}

static int pdev_resume(struct platform_device *device)
{
	DBG("pdev_resume");
	return 0;
}

static void pdev_shutdown(struct platform_device *device)
{
	DBG("pdev_shutdown");
}

static int pdev_probe(struct platform_device *device)
{
	DBG("pdev_probe");
	return drm_get_platform_dev(device, &omap_gpu_driver);
}

static int pdev_remove(struct platform_device *device)
{
	DBG("pdev_remove");
	drm_put_dev(drm_device);
	return 0;
}

struct drm_ioctl_desc ioctls[DRM_COMMAND_END - DRM_COMMAND_BASE] = {{0}};

static struct drm_driver omap_gpu_driver = {
		.driver_features = DRIVER_HAVE_IRQ | DRIVER_USE_PLATFORM_DEVICE | DRIVER_MODESET,
		.load = dev_load,
		.unload = dev_unload,
		.open = dev_open,
		.firstopen = dev_firstopen,
		.lastclose = dev_lastclose,
		.preclose = dev_preclose,
		.postclose = dev_postclose,
		.get_vblank_counter = drm_vblank_count,
		.enable_vblank = dev_enable_vblank,
		.disable_vblank = dev_disable_vblank,
		.device_is_agp = dev_device_is_agp,
		.irq_preinstall = dev_irq_preinstall,
		.irq_postinstall = dev_irq_postinstall,
		.irq_uninstall = dev_irq_uninstall,
		.irq_handler = dev_irq_handler,
		.reclaim_buffers = drm_core_reclaim_buffers,
		.get_map_ofs = drm_core_get_map_ofs,
		.get_reg_ofs = drm_core_get_reg_ofs,
		.ioctls = ioctls,
		.num_ioctls = 0,
#if 0
#if defined(CONFIG_DEBUG_FS)
		.debugfs_init = debugfs_init,
		.debugfs_cleanup = debugfs_cleanup,
#endif
#endif
		.fops = {
				.owner = THIS_MODULE,
				.open = drm_open,
				.unlocked_ioctl = drm_ioctl,
				.release = fop_release,
				.mmap = fop_mmap,
#if 0
				/* TODO: how to integrate multiple plugins different
				 * memory management under GEM interface
				 */
				.release = drm_release,
				.mmap = drm_gem_mmap,
#endif
				.poll = drm_poll,
				.fasync = drm_fasync,
				.read = drm_read,
		},
		.platform_driver = {
			.driver = {
				.name = DRIVER_NAME,
			},
			.probe = pdev_probe,
			.remove = pdev_remove,
			.suspend = pdev_suspend,
			.resume = pdev_resume,
			.shutdown = pdev_shutdown,
		},
		.name = DRIVER_NAME,
		.desc = DRIVER_DESC,
		.date = DRIVER_DATE,
		.major = DRIVER_MAJOR,
		.minor = DRIVER_MINOR,
		.patchlevel = DRIVER_PATCHLEVEL,
};

void omap_gpu_hotplug_event(struct fb_info *fbi)
{
	struct drm_device *dev = drm_device;
	DBG("omap_gpu_hotplug_event: dev=%p, fbi=%p", dev, fbi);
	if (dev) {
		drm_sysfs_hotplug_event(dev);
	}
}
EXPORT_SYMBOL(omap_gpu_hotplug_event);

int omap_gpu_register_fbs(int _num_fbs, struct fb_info **_fbs)
{
	num_fbs = _num_fbs;
	fbs = _fbs;
	DBG("num_fbs=%d, fbs=%p", num_fbs, fbs);
	return 0;
}
EXPORT_SYMBOL(omap_gpu_register_fbs);

int omap_gpu_register_plugin(struct omap_gpu_plugin *plugin)
{
	struct drm_device *dev = drm_device;
	int i;

	DBG("register plugin: %p (%s)", plugin, plugin->name);

	// XXX note, PVR code uses drm_file->driver_priv... need to come up with some sane way to handle this.

	list_add_tail(&plugin->list, &plugin_list);

	/* register the plugin's ioctl's */
	for (i = 0; i < plugin->num_ioctls; i++) {
		int nr = i + plugin->ioctl_start;

		/* check for out of bounds ioctl nr or already registered ioctl */
		if (nr > ARRAY_SIZE(ioctls) || ioctls[nr].func) {
			dev_err(dev->dev, "invalid ioctl: %d (nr=%d)\n", i, nr);
			return -EINVAL;
		}

		DBG("register ioctl: %d %08x", nr, plugin->ioctls[i].cmd);

		ioctls[nr] = plugin->ioctls[i];

		if (nr >= omap_gpu_driver.num_ioctls) {
			omap_gpu_driver.num_ioctls = nr + 1;
		}
	}

	if (loaded) {
		plugin->load(dev, 0);
	}

	return 0;
}
EXPORT_SYMBOL(omap_gpu_register_plugin);

int omap_gpu_unregister_plugin(struct omap_gpu_plugin *plugin)
{
	list_del(&plugin->list);
	return 0;
}
EXPORT_SYMBOL(omap_gpu_unregister_plugin);

static int __init omap_gpu_init(void)
{
	DBG("init");
	return drm_init(&omap_gpu_driver);
}

static void __exit omap_gpu_fini(void)
{
	DBG("fini");
	drm_exit(&omap_gpu_driver);
}

/* TODO: change to module_init() when we no longer depend on omapfb...
 * until then we need to ensure that omapfb's probe, and call to
 * omap_gpu_register_fbs() happens before omap_gpu_init()..
 */
late_initcall(omap_gpu_init);
module_exit(omap_gpu_fini);
