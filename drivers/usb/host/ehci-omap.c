/*
 * ehci-omap.c - driver for the EHCI controller  on OMAP 34xx chips
 *
 * Bus Glue for OMAP34xx 3-port EHCI controller
 * Tested on OMAP3430 SDP
 *
 * Copyright (C) 2007-2009 Texas Instruments, Inc.
 *	Author: Vikram Pandita <vikram.pandita@ti.com>
 *	Author: Anand Gadiyar <gadiyar@ti.com>
 *
 * Copyright (C) 2009 Nokia Corporation
 *	Contact: Felipe Balbi <felipe.balbi@nokia.com>
 *
 * Based on "ehci-fsl.c" and "ehci-au1xxx.c" ehci glue layers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * TODO (last updated Sep 21, 2009):
 *	- add kernel-doc
 *	- add suspend/resume workarounds
 *	- add aggressive power management support
 */

#include <linux/platform_device.h>
#include <linux/clk.h>
#include <mach/usb.h>

/*-------------------------------------------------------------------------*/

struct ehci_hcd_omap {
	struct usb_hcd		*hcd;

	struct clk		*usbhost_ick;
	struct clk		*usbhost2_120m_fck;
	struct clk		*usbhost1_48m_fck;
	struct clk		*usbtll_fck;
	struct clk		*usbtll_ick;

	void __iomem		*ehci_base;
};

/*-------------------------------------------------------------------------*/

static const struct hc_driver ehci_omap_hc_driver;

/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */

/**
 * ehci_hcd_omap_probe - initialize TI-based HCDs
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 */
static int ehci_hcd_omap_probe(struct platform_device *pdev)
{
	struct ehci_hcd_omap_platform_data *pdata = pdev->dev.platform_data;
	struct device *dev = &pdev->dev;
	struct ehci_hcd_omap *omap;
	struct resource *res;
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	int irq;
	int ret = -ENODEV;

	if (usb_disabled())
		return -ENODEV;

	dev_dbg(dev, "Initializing OMAP EHCI controller\n");

	if (!pdata) {
		dev_err(dev, "missing platform_data\n");
		return -EINVAL;
	}

	irq = platform_get_irq(pdev, 0);

	hcd = usb_create_hcd(&ehci_omap_hc_driver, dev, dev_name(dev));
	if (!hcd) {
		dev_dbg(&pdev->dev, "failed to create hcd with err %d\n", ret);
		ret = -ENOMEM;
		goto err_create_hcd;
	}

	omap = kzalloc(sizeof(*omap), GFP_KERNEL);
	if (!omap) {
		ret = -ENOMEM;
		goto err_alloc;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "No memory resource found. Check platform data\n");
		ret = -ENODEV;
		goto err_get_resource;
	}

	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		dev_dbg(dev, "controller already in use\n");
		ret = -EBUSY;
		goto err_request_mem;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_err(&pdev->dev, "error mapping memory\n");
		ret = -EFAULT;
		goto err_ioremap;
	}

	/* call platform specific init function */
	if (pdata ->init) {
		ret = pdata->init(pdev);
		if (ret) {
			dev_err(dev, "platform init failed\n");
			goto err_init;
		}
	}

	omap->hcd	= hcd;
	ehci		= hcd_to_ehci(hcd);
	ehci->caps	= hcd->regs;
	ehci->regs	= hcd->regs
		+ HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	dbg_hcs_params(ehci, "reset");
	dbg_hcc_params(ehci, "reset");

	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = readl(&ehci->caps->hcs_params);

	ehci->sbrn	= 0x20;
	platform_set_drvdata(pdev, omap);

	ret = usb_add_hcd(hcd, irq, IRQF_DISABLED | IRQF_SHARED);
	if (ret) {
		dev_dbg(&pdev->dev, "failed to add hcd with err %d\n", ret);
		goto err_add;
	}

	return 0;

err_add:
	if (pdata && pdata->exit)
		pdata->exit(pdev);
err_init:
	iounmap(hcd->regs);
err_ioremap:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err_request_mem:
err_get_resource:
	kfree(omap);
err_alloc:
	usb_put_hcd(hcd);
err_create_hcd:
	return ret;
}

/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * ehci_hcd_omap_remove - shutdown processing for EHCI HCDs
 * @pdev: USB Host Controller being removed
 *
 * Reverses the effect of usb_ehci_hcd_omap_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 */
static int ehci_hcd_omap_remove(struct platform_device *pdev)
{
	struct ehci_hcd_omap_platform_data *pdata = pdev->dev.platform_data;
	struct ehci_hcd_omap *omap = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = omap->hcd;

	if (pdata && pdata->exit)
		pdata->exit(pdev);

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	platform_set_drvdata(pdev, NULL);

	kfree(omap);

	return 0;
}

static void ehci_hcd_omap_shutdown(struct platform_device *pdev)
{
	struct ehci_hcd_omap *omap = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = omap->hcd;

	if (hcd->driver->shutdown)
		hcd->driver->shutdown(hcd);
}

static struct platform_driver ehci_hcd_omap_driver = {
	.probe			= ehci_hcd_omap_probe,
	.remove			= __exit_p(ehci_hcd_omap_remove),
	.shutdown		= ehci_hcd_omap_shutdown,
	.driver = {
		.name		= "ehci-omap",
	}
};

/*-------------------------------------------------------------------------*/

static const struct hc_driver ehci_omap_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "OMAP-EHCI Host Controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 */
	.reset			= ehci_init,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.endpoint_reset		= ehci_endpoint_reset,

	/*
	 * scheduling support
	 */
	.get_frame_number	= ehci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
	.bus_suspend		= ehci_bus_suspend,
	.bus_resume		= ehci_bus_resume,
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete = ehci_clear_tt_buffer_complete,
};

MODULE_ALIAS("platform:omap-ehci");
MODULE_AUTHOR("Texas Instruments, Inc.");

