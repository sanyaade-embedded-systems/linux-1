/*
 * linux/arch/arm/mach-omap2/usb-musb.c
 *
 * This file will contain the board specific details for the
 * MENTOR USB OTG controller on OMAP3430
 *
 * Copyright (C) 2007-2008 Texas Instruments
 * Copyright (C) 2008 Nokia Corporation
 * Author: Vikram Pandita
 *
 * Generalization by:
 * Felipe Balbi <felipe.balbi@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>

#include <asm/io.h>

#include <linux/usb/musb.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/pm.h>
#include <mach/mux.h>
#include <mach/usb.h>
#include <mach/netra.h>

#ifdef CONFIG_USB_MUSB_SOC
#define	USB_MUSB_NUM_CONTROLLERS	1

static int clk_on;

static int musb_set_clock(struct clk *clk, int state)
{
	if (state) {
		if (clk_on > 0)
			return -ENODEV;

		clk_enable(clk);
		clk_on = 1;
	} else {
		if (clk_on == 0)
			return -ENODEV;

		clk_disable(clk);
		clk_on = 0;
	}

	return 0;
}

static struct musb_hdrc_eps_bits musb_eps[] = {
	{	"ep1_tx", 10,	},
	{	"ep1_rx", 10,	},
	{	"ep2_tx", 9,	},
	{	"ep2_rx", 9,	},
	{	"ep3_tx", 3,	},
	{	"ep3_rx", 3,	},
	{	"ep4_tx", 3,	},
	{	"ep4_rx", 3,	},
	{	"ep5_tx", 3,	},
	{	"ep5_rx", 3,	},
	{	"ep6_tx", 3,	},
	{	"ep6_rx", 3,	},
	{	"ep7_tx", 3,	},
	{	"ep7_rx", 3,	},
	{	"ep8_tx", 2,	},
	{	"ep8_rx", 2,	},
	{	"ep9_tx", 2,	},
	{	"ep9_rx", 2,	},
	{	"ep10_tx", 2,	},
	{	"ep10_rx", 2,	},
	{	"ep11_tx", 2,	},
	{	"ep11_rx", 2,	},
	{	"ep12_tx", 2,	},
	{	"ep12_rx", 2,	},
	{	"ep13_tx", 2,	},
	{	"ep13_rx", 2,	},
	{	"ep14_tx", 2,	},
	{	"ep14_rx", 2,	},
	{	"ep15_tx", 2,	},
	{	"ep15_rx", 2,	},
};

static struct musb_hdrc_config musb_config = {
	.multipoint	= 0, /* netra IP does not support multipoint */
	.dyn_fifo	= 1,
	.soft_con	= 1,
	.dma		= 1,
	.num_eps	= 16,
	.dma_channels	= 7,
	.dma_req_chan	= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
	.ram_bits	= 12,
	.eps_bits	= musb_eps,
};

static struct musb_hdrc_platform_data musb_plat[] = {
	{
#ifdef CONFIG_USB_MUSB_OTG
	.mode		= MUSB_OTG,
#elif defined(CONFIG_USB_MUSB_HDRC_HCD)
	.mode		= MUSB_HOST,
#elif defined(CONFIG_USB_GADGET_MUSB_HDRC)
	.mode		= MUSB_PERIPHERAL,
#endif
	.clock		= cpu_is_omap34xx()
			? "hsotgusb_ick"
			: "usbhs_ick",
	.set_clock	= musb_set_clock,
	.config		= &musb_config,

	/* REVISIT charge pump on TWL4030 can supply up to
	 * 100 mA ... but this value is board-specific, like
	 * "mode", and should be passed to usb_musb_init().
	 */
	.power		= 50,			/* up to 100 mA */
	},
	{
#ifdef CONFIG_USB_MUSB_OTG
	.mode           = MUSB_OTG,
#elif defined(CONFIG_USB_MUSB_HDRC_HCD)
	.mode           = MUSB_HOST,
#elif defined(CONFIG_USB_GADGET_MUSB_HDRC)
	.mode           = MUSB_PERIPHERAL,
#endif
	.clock          = cpu_is_omap34xx()
			? "hsotgusb_ick"
			: "usbhs_ick",
	.set_clock      = musb_set_clock,
	.config         = &musb_config,

	/* REVISIT charge pump on TWL4030 can supply up to
	 * 100 mA ... but this value is board-specific, like
	 * "mode", and should be passed to usb_musb_init().
	 */
	.power          = 50,                   /* up to 100 mA */
	}
};

static u64 musb_dmamask = DMA_32BIT_MASK;

static struct resource musb_resources[] = {
	[0] = { /* start and end set dynamically */
		.start  = NETRA_IPSS_USB0_BASE,
		.end    = 0x7FF,
		.flags  = IORESOURCE_MEM,
	},
	[1] = { /* DMA IRQ */
		.start  = NETRA_USBSINT0_IRQ,
		.flags  = IORESOURCE_IRQ,
	},
	[2] = { /* start and end set dynamically */
		.start  = NETRA_IPSS_USB1_BASE,
		.end    = 0x7FF,
		.flags  = IORESOURCE_MEM,
	},
	[3] = { /* DMA IRQ */
		.start  = NETRA_USBSINT1_IRQ,
		.flags  = IORESOURCE_IRQ,
	},
	[4] = { /* general IRQ */
		.start  = NETRA_USBSSINT_IRQ,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct resource musb_resources1[] = {
	{ /* start and end set dynamically */
		.start  = NETRA_IPSS_USB0_BASE,
		.end    = NETRA_IPSS_USB0_BASE + 0x7FF,
		.flags  = IORESOURCE_MEM,
	},
	{ /* DMA IRQ */
		.start  = NETRA_USBSINT0_IRQ,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct resource musb_resources2[] = {
	{ /* start and end set dynamically */
		.start  = NETRA_IPSS_USB1_BASE,
		.end    = NETRA_IPSS_USB1_BASE + 0x7FF,
		.flags  = IORESOURCE_MEM,
	},
	{ /* DMA IRQ */
		.start  = NETRA_USBSINT1_IRQ,
		.flags  = IORESOURCE_IRQ,
	},
	{ /* general IRQ */
		.start  = NETRA_USBSSINT_IRQ,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device musb_devices[] = {
	{
		.name		= "musb_hdrc",
		.id		= 0,
		.dev = {
			.dma_mask		= &musb_dmamask,
			.coherent_dma_mask	= DMA_32BIT_MASK,
			.platform_data		= &musb_plat[0],
		},
		.num_resources	= ARRAY_SIZE(musb_resources1),
		.resource	= musb_resources1,
	},
       {
		.name           = "musb_hdrc",
		.id             = 1,
		.dev = {
			.dma_mask               = &musb_dmamask,
			.coherent_dma_mask      = DMA_32BIT_MASK,
			.platform_data          = &musb_plat[1],
		},
		.num_resources  = ARRAY_SIZE(musb_resources2),
		.resource       = musb_resources2,
	},
};
#endif

void __init usb_musb_init(void)
{
	int i;

	musb_config.ram_bits = 13; /* 2^(13+2) = 32K */

#ifdef CONFIG_USB_MUSB_SOC
	for (i = 0; i < USB_MUSB_NUM_CONTROLLERS; i++) {
		musb_plat[i].clock = "usbotg_ck";
		musb_plat[i].power = 250;
		if (platform_device_register(&musb_devices[i]) < 0) {
			printk(KERN_ERR "Unable to register HS-USB (MUSB) device\n");
			return;
		}
		printk(KERN_INFO, "registering %s.%d controller\n",
			musb_devices[i].name, i);
	}
#endif
}
subsys_initcall(usb_musb_init);



