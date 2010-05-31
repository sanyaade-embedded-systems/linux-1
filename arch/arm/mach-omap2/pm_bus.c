/*
 * Runtime PM support code for OMAP
 *
 * Author: Kevin Hilman, Deep Root Systems, LLC
 *
 * Copyright (C) 2010 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>

#include <plat/omap_device.h>
#include <plat/omap-pm.h>

#ifdef CONFIG_PM_RUNTIME
int platform_pm_runtime_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_device *odev = to_omap_device(pdev);
	int r, ret = 0;

	dev_dbg(dev, "%s\n", __func__);

	if (dev->driver->pm && dev->driver->pm->runtime_suspend)
		ret = dev->driver->pm->runtime_suspend(dev);
	if (!ret && omap_device_is_valid(odev)) {
		r = omap_device_idle(pdev);
		WARN_ON(r);
	}

	return ret;
};

int platform_pm_runtime_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_device *odev = to_omap_device(pdev);
	int r, ret = 0;

	dev_dbg(dev, "%s\n", __func__);

	if (omap_device_is_valid(odev)) {
		r = omap_device_enable(pdev);
		WARN_ON(r);
	}

	if (dev->driver->pm && dev->driver->pm->runtime_resume)
		ret = dev->driver->pm->runtime_resume(dev);

	return ret;
};

int platform_pm_runtime_idle(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_device *odev = to_omap_device(pdev);
	int ret;

	ret = pm_runtime_suspend(dev);
	dev_dbg(dev, "%s [%d]\n", __func__, ret);

	return 0;
};
#endif /* CONFIG_PM_RUNTIME */

#ifdef CONFIG_SUSPEND
int platform_pm_suspend_noirq(struct device *dev)
{
	struct device_driver *drv = dev->driver;
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_device *odev = to_omap_device(pdev);
	int ret = 0;

	if (!drv)
		return 0;

	if (drv->pm) {
		if (drv->pm->suspend_noirq)
			ret = drv->pm->suspend_noirq(dev);
	}

	if (omap_device_is_valid(odev)) {
		if (odev->flags & OMAP_DEVICE_NO_BUS_SUSPEND) {
			dev_dbg(dev, "no automatic bus-level system resume.\n");
			return 0;
		}

		dev_dbg(dev, "%s\n", __func__);
		omap_device_idle(pdev);
	} else {
		dev_dbg(dev, "not an omap_device\n");
	}

	return ret;
}

int platform_pm_resume_noirq(struct device *dev)
{
	struct device_driver *drv = dev->driver;
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_device *odev = to_omap_device(pdev);
	int ret = 0;

	if (omap_device_is_valid(odev)) {
		if (odev->flags & OMAP_DEVICE_NO_BUS_SUSPEND) {
			dev_dbg(dev, "no automatic bus-level system resume.\n");
			return 0;
		}

		dev_dbg(dev, "%s\n", __func__);
		omap_device_enable(pdev);
	} else {
		dev_dbg(dev, "not an omap_device\n");
	}

	if (!drv)
		return 0;

	if (drv->pm) {
		if (drv->pm->resume_noirq)
			ret = drv->pm->resume_noirq(dev);
	}

	return ret;
}
#endif /* CONFIG_SUSPEND */
