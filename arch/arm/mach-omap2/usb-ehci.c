/*
 * linux/arch/arm/mach-omap2/usb-ehci.c
 *
 * This file will contain the board specific details for the
 * Synopsys EHCI host controller on OMAP3430
 *
 * Copyright (C) 2007 Texas Instruments
 * Author: Vikram Pandita <vikram.pandita@ti.com>
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
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <mach/mux.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/usb.h>

#define OMAP_USBTLL_REVISION			(0x00)
#define OMAP_USBTLL_SYSCONFIG			(0x10)
#define OMAP_USBTLL_SYSCONFIG_CACTIVITY		BIT(8)
#define OMAP_USBTLL_SYSCONFIG_SIDLEMODE		BIT(3)
#define OMAP_USBTLL_SYSCONFIG_ENAWAKEUP		BIT(2)
#define OMAP_USBTLL_SYSCONFIG_SOFTRESET		BIT(1)
#define OMAP_USBTLL_SYSCONFIG_AUTOIDLE		BIT(0)

#define OMAP_USBTLL_SYSSTATUS			(0x14)
#define OMAP_USBTLL_SYSSTATUS_RESETDONE		BIT(0)

#define OMAP_USBTLL_IRQSTATUS			(0x18)
#define OMAP_USBTLL_IRQENABLE			(0x1c)

#define OMAP_TLL_SHARED_CONF			(0x30)
#define OMAP_TLL_SHARED_CONF_USB_90D_DDR_EN	BIT(6)
#define OMAP_TLL_SHARED_CONF_USB_180D_SDR_EN	BIT(5)
/* FIXME: This one is a bit-field */
#define OMAP_TLL_SHARED_CONF_USB_DIVRATION	BIT(2)
#define OMAP_TLL_SHARED_CONF_FCLK_REQ		BIT(1)
#define OMAP_TLL_SHARED_CONF_FCLK_IS_ON		BIT(0)

#define OMAP_TLL_CHANNEL_CONF(i)		(0x40 + (4 * i))
#define OMAP_TLL_CHANNEL_CONF_ULPINOBITSTUFF	BIT(11)
#define OMAP_TLL_CHANNEL_CONF_ULPIAUTOIDLE	BIT(10)
#define OMAP_TLL_CHANNEL_CONF_UTMIAUTOIDLE	BIT(9)
#define OMAP_TLL_CHANNEL_CONF_ULPIDDRMODE	BIT(8)
#define OMAP_TLL_CHANNEL_CONF_CHANEN		BIT(0)

/* UHH Registers */
#define OMAP_UHH_REVISION			(0x00)
#define OMAP_UHH_SYSCONFIG			(0x10)
#define OMAP_UHH_SYSCONFIG_MIDLEMODE		BIT(12)
#define OMAP_UHH_SYSCONFIG_CACTIVITY		BIT(8)
#define OMAP_UHH_SYSCONFIG_SIDLEMODE		BIT(3)
#define OMAP_UHH_SYSCONFIG_ENAWAKEUP		BIT(2)
#define OMAP_UHH_SYSCONFIG_SOFTRESET		BIT(1)
#define OMAP_UHH_SYSCONFIG_AUTOIDLE		BIT(0)

#define OMAP_UHH_SYSSTATUS			(0x14)
#define OMAP_UHH_HOSTCONFIG			(0x40)
#define OMAP_UHH_HOSTCONFIG_INCR4_BURST_EN	BIT(2)
#define OMAP_UHH_HOSTCONFIG_INCR8_BURST_EN	BIT(3)
#define OMAP_UHH_HOSTCONFIG_INCR16_BURST_EN	BIT(4)
#define OMAP_UHH_HOSTCONFIG_INCRX_ALIGN_EN	BIT(5)

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)

static struct resource ehci_resources[] = {
	{
		.start	= OMAP34XX_EHCI_BASE,
		.end	= OMAP34XX_EHCI_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{         /* general IRQ */
		.start   = INT_34XX_EHCI_IRQ,
		.flags   = IORESOURCE_IRQ,
	}
};

static u64 ehci_dmamask = ~(u32)0;
static struct platform_device ehci_device = {
	.name           = "ehci-omap",
	.id             = 0,
	.dev = {
		.dma_mask               = &ehci_dmamask,
		.coherent_dma_mask      = 0xffffffff,
		.platform_data          = NULL,
	},
	.num_resources  = ARRAY_SIZE(ehci_resources),
	.resource       = ehci_resources,
};

/* MUX settings for EHCI pins */
/*
 * setup_ehci_io_mux - initialize IO pad mux for USBHOST
 */
static void setup_ehci_io_mux(enum ehci_hcd_omap_mode phy_mode)
{
	if (phy_mode == EHCI_HCD_OMAP_MODE_PHY) {
		/* PHY mode of operation for board: 750-2083-001
		 * ISP1504 connected to Port1 and Port2
		 * Do Func Mux setting for 12-pin ULPI PHY mode
		 */

		if (cpu_is_omap34xx()) {
			/* Port1 */
			omap_cfg_reg(Y9_3430_USB1HS_PHY_STP);
			omap_cfg_reg(Y8_3430_USB1HS_PHY_CLK);
			omap_cfg_reg(AA14_3430_USB1HS_PHY_DIR);
			omap_cfg_reg(AA11_3430_USB1HS_PHY_NXT);
			omap_cfg_reg(W13_3430_USB1HS_PHY_DATA0);
			omap_cfg_reg(W12_3430_USB1HS_PHY_DATA1);
			omap_cfg_reg(W11_3430_USB1HS_PHY_DATA2);
			omap_cfg_reg(Y11_3430_USB1HS_PHY_DATA3);
			omap_cfg_reg(W9_3430_USB1HS_PHY_DATA4);
			omap_cfg_reg(Y12_3430_USB1HS_PHY_DATA5);
			omap_cfg_reg(W8_3430_USB1HS_PHY_DATA6);
			omap_cfg_reg(Y13_3430_USB1HS_PHY_DATA7);

			/* Port2 */
			omap_cfg_reg(AA10_3430_USB2HS_PHY_STP);
			omap_cfg_reg(AA8_3430_USB2HS_PHY_CLK);
			omap_cfg_reg(AA9_3430_USB2HS_PHY_DIR);
			omap_cfg_reg(AB11_3430_USB2HS_PHY_NXT);
			omap_cfg_reg(AB10_3430_USB2HS_PHY_DATA0);
			omap_cfg_reg(AB9_3430_USB2HS_PHY_DATA1);
			omap_cfg_reg(W3_3430_USB2HS_PHY_DATA2);
			omap_cfg_reg(T4_3430_USB2HS_PHY_DATA3);
			omap_cfg_reg(T3_3430_USB2HS_PHY_DATA4);
			omap_cfg_reg(R3_3430_USB2HS_PHY_DATA5);
			omap_cfg_reg(R4_3430_USB2HS_PHY_DATA6);
			omap_cfg_reg(T2_3430_USB2HS_PHY_DATA7);
		} else if (cpu_is_omap44xx()) {
			/* Port 1 */
                        omap_cfg_reg(AE_4430_USB1HS_PHY_CLK);
                        omap_cfg_reg(AG_4430_USB1HS_PHY_STP);
                        omap_cfg_reg(AF_4430_USB1HS_PHY_DIR);
                        omap_cfg_reg(AE_4430_USB1HS_PHY_NXT);
                        omap_cfg_reg(AF_4430_USB1HS_PHY_D0);
                        omap_cfg_reg(AG_4430_USB1HS_PHY_D1);
                        omap_cfg_reg(AE_4430_USB1HS_PHY_D2);
                        omap_cfg_reg(AF_4430_USB1HS_PHY_D3);
                        omap_cfg_reg(AH_4430_USB1HS_PHY_D4);
                        omap_cfg_reg(AE_4430_USB1HS_PHY_D5);
                        omap_cfg_reg(AF_4430_USB1HS_PHY_D6);
                        omap_cfg_reg(AG_4430_USB1HS_PHY_D7);
		}
	} else {
		/* Set Func mux for :
		 * TLL mode of operation
		 * 12-pin ULPI SDR TLL mode for Port1/2/3
		 */

		/* Port1 */
		omap_cfg_reg(Y9_3430_USB1HS_TLL_STP);
		omap_cfg_reg(Y8_3430_USB1HS_TLL_CLK);
		omap_cfg_reg(AA14_3430_USB1HS_TLL_DIR);
		omap_cfg_reg(AA11_3430_USB1HS_TLL_NXT);
		omap_cfg_reg(W13_3430_USB1HS_TLL_DATA0);
		omap_cfg_reg(W12_3430_USB1HS_TLL_DATA1);
		omap_cfg_reg(W11_3430_USB1HS_TLL_DATA2);
		omap_cfg_reg(Y11_3430_USB1HS_TLL_DATA3);
		omap_cfg_reg(W9_3430_USB1HS_TLL_DATA4);
		omap_cfg_reg(Y12_3430_USB1HS_TLL_DATA5);
		omap_cfg_reg(W8_3430_USB1HS_TLL_DATA6);
		omap_cfg_reg(Y13_3430_USB1HS_TLL_DATA7);

		/* Port2 */
		omap_cfg_reg(AA10_3430_USB2HS_TLL_STP);
		omap_cfg_reg(AA8_3430_USB2HS_TLL_CLK);
		omap_cfg_reg(AA9_3430_USB2HS_TLL_DIR);
		omap_cfg_reg(AB11_3430_USB2HS_TLL_NXT);
		omap_cfg_reg(AB10_3430_USB2HS_TLL_DATA0);
		omap_cfg_reg(AB9_3430_USB2HS_TLL_DATA1);
		omap_cfg_reg(W3_3430_USB2HS_TLL_DATA2);
		omap_cfg_reg(T4_3430_USB2HS_TLL_DATA3);
		omap_cfg_reg(T3_3430_USB2HS_TLL_DATA4);
		omap_cfg_reg(R3_3430_USB2HS_TLL_DATA5);
		omap_cfg_reg(R4_3430_USB2HS_TLL_DATA6);
		omap_cfg_reg(T2_3430_USB2HS_TLL_DATA7);

		/* Port3 */
		omap_cfg_reg(AB3_3430_USB3HS_TLL_STP);
		omap_cfg_reg(AA6_3430_USB3HS_TLL_CLK);
		omap_cfg_reg(AA3_3430_USB3HS_TLL_DIR);
		omap_cfg_reg(Y3_3430_USB3HS_TLL_NXT);
		omap_cfg_reg(AA5_3430_USB3HS_TLL_DATA0);
		omap_cfg_reg(Y4_3430_USB3HS_TLL_DATA1);
		omap_cfg_reg(Y5_3430_USB3HS_TLL_DATA2);
		omap_cfg_reg(W5_3430_USB3HS_TLL_DATA3);
		omap_cfg_reg(AB12_3430_USB3HS_TLL_DATA4);
		omap_cfg_reg(AB13_3430_USB3HS_TLL_DATA5);
		omap_cfg_reg(AA13_3430_USB3HS_TLL_DATA6);
		omap_cfg_reg(AA12_3430_USB3HS_TLL_DATA7);
	}

	return;
}

static struct ehci_hcd_omap_platform_data pdata __initconst;

static int omap_usb_port_ulpi_bypass(enum omap_usbhost_port_mode mode)
{
	return mode == OMAP_USB_PORT_MODE_ULPI_PHY;
}

static int omap_usb_port_disabled(enum omap_usbhost_port_mode mode)
{
	return mode == OMAP_USB_PORT_MODE_DISABLED;
}

static int omap_usbtll_chanmode(enum omap_usbhost_port_mode mode)
{
	switch (mode) {
		/* UTMI-to-ULPI TLL mode */
	case OMAP_USB_PORT_MODE_ULPI_TLL_SDR:
	case OMAP_USB_PORT_MODE_ULPI_TLL_DDR:
		return 0;

		/* UTMI-to-serial modes */
	case OMAP_USB_PORT_MODE_UTMI_PHY_6PIN:
	case OMAP_USB_PORT_MODE_UTMI_PHY_6PIN_ALT:
	case OMAP_USB_PORT_MODE_UTMI_PHY_3PIN:
	case OMAP_USB_PORT_MODE_UTMI_PHY_4PIN:
	case OMAP_USB_PORT_MODE_UTMI_TLL_6PIN:
	case OMAP_USB_PORT_MODE_UTMI_TLL_6PIN_ALT:
	case OMAP_USB_PORT_MODE_UTMI_TLL_3PIN:
	case OMAP_USB_PORT_MODE_UTMI_TLL_4PIN:
	case OMAP_USB_PORT_MODE_UTMI_TLL_2PIN:
	case OMAP_USB_PORT_MODE_UTMI_TLL_2PIN_ALT:
		return 1;

		/* No mode selected */
	default:
		return 3;
	}
}

static unsigned omap_ulpi_bypass_bit_shift(unsigned i)
{
	switch (i) {
	case 0:
		return 0;
	case 1:
		return 11;
	case 2:
		return 12;
	default:
		return 0;
	}
}

static unsigned omap_usbtll_port_fslsmode(enum omap_usbhost_port_mode mode)
{
	switch (mode) {
		/* non serial modes */
	case OMAP_USB_PORT_MODE_ULPI_PHY:
	case OMAP_USB_PORT_MODE_ULPI_TLL_SDR:
	case OMAP_USB_PORT_MODE_ULPI_TLL_DDR:
		return 0x0;

		/* serial modes */
	case OMAP_USB_PORT_MODE_UTMI_PHY_6PIN:
		return 0x0;

	case OMAP_USB_PORT_MODE_UTMI_PHY_6PIN_ALT:
		return 0x1;

	case OMAP_USB_PORT_MODE_UTMI_PHY_3PIN:
		return 0x2;

	case OMAP_USB_PORT_MODE_UTMI_PHY_4PIN:
		return 0x3;

	case OMAP_USB_PORT_MODE_UTMI_TLL_6PIN:
		return 0x4;

	case OMAP_USB_PORT_MODE_UTMI_TLL_6PIN_ALT:
		return 0x5;

	case OMAP_USB_PORT_MODE_UTMI_TLL_3PIN:
		return 0x6;

	case OMAP_USB_PORT_MODE_UTMI_TLL_4PIN:
		return 0x7;

	case OMAP_USB_PORT_MODE_UTMI_TLL_2PIN:
		return 0xa;

	case OMAP_USB_PORT_MODE_UTMI_TLL_2PIN_ALT:
		return 0xb;

	default:
		return 0x0;
	}
}

/* FIXME: Remove these hacks */
#define USBTLL_SYSCONFIG	0x48062010
#define USBTLL_SHARED_CONF	0x48062030

#define UHH_SYSCONFIG		0x48064010
#define UHH_HOSTCONFIG		0x48064040

static void omap_usbtll_configure_ports(
		struct omap_usbhost_port_data const *port_data,
		unsigned n)
{
	unsigned i;
	u32 val;

	for (i = 0; i < n; ++i) {
		printk("\n Port%d->mode = %d(%x)\n",
			i+1, port_data[i].mode,
			omap_usbtll_port_fslsmode(port_data[i].mode));
	}

	for (i = 0; i < n; i++) {
		val = 0;

		if (omap_usb_port_disabled(port_data[i].mode))
			continue;

		if (omap_usb_port_ulpi_bypass(port_data[i].mode))
			continue;

#define OMAP_TLL_CHANNEL_CONF_CHANMODE	(1)
#define OMAP_TLL_CHANNEL_CONF_FSLSMODE	(24)
#define TLL_CHANNEL_CONF(i)	(0x48062040 + (4 * i))

		val |= OMAP_TLL_CHANNEL_CONF_UTMIAUTOIDLE |
			OMAP_TLL_CHANNEL_CONF_ULPIAUTOIDLE;
		val |= omap_usbtll_chanmode(port_data[i].mode)
				<< OMAP_TLL_CHANNEL_CONF_CHANMODE;

		val |= omap_usbtll_port_fslsmode(port_data[i].mode)
				<< OMAP_TLL_CHANNEL_CONF_FSLSMODE;

		val |= OMAP_TLL_CHANNEL_CONF_CHANEN;
		printk("\n USBTLLCHANNEL_CONF(%d) = 0x%x\n", i, val);
		omap_writel(val, TLL_CHANNEL_CONF(i));
	}

	val = 0;

	val |= OMAP_UHH_HOSTCONFIG_INCR4_BURST_EN
			| OMAP_UHH_HOSTCONFIG_INCR8_BURST_EN
			| OMAP_UHH_HOSTCONFIG_INCR16_BURST_EN;
	for (i = 0; i < n; i++) {
		if (!omap_usb_port_ulpi_bypass(port_data[i].mode)) {
			val |= 1 << omap_ulpi_bypass_bit_shift(i);
		}
	}
	printk("\n UHH_HOSTCONFIG = 0x%x\n", val);
	omap_writel(val, UHH_HOSTCONFIG);

}

void usb_host_and_tll_init(
		struct omap_usbhost_port_data const *port_data,
		unsigned n)
{
	clk_enable(clk_get(NULL, "usbtll_ick"));
	clk_enable(clk_get(NULL, "usbtll_fck"));
	clk_enable(clk_get(NULL, "usbhost_ick"));
	clk_enable(clk_get(NULL, "usbhost_48m_fck"));
	clk_enable(clk_get(NULL, "usbhost_120m_fck"));

	omap_writel(0x15, USBTLL_SYSCONFIG);
	omap_writel(omap_readl(USBTLL_SHARED_CONF) | 0x5,
					USBTLL_SHARED_CONF);

	omap_writel(0x0000110C, UHH_SYSCONFIG);
	omap_writel(0x101C, UHH_HOSTCONFIG);

	omap_usbtll_configure_ports(port_data, n);

}

void __init usb_ehci_init(enum ehci_hcd_omap_mode phy_mode,
		int chargepump, int phy_reset, int reset_gpio_port1,
		int reset_gpio_port2)
{
	if (cpu_is_omap44xx()) {
               ehci_resources[0].start = OMAP44XX_EHCI_BASE;
               ehci_resources[0].end   = OMAP44XX_EHCI_BASE + SZ_1K - 1;
               ehci_resources[1].start = INT_44XX_EHCI_IRQ;
       }

	platform_device_add_data(&ehci_device, &pdata, sizeof(pdata));

	/* Setup Pin IO MUX for EHCI */
	setup_ehci_io_mux(phy_mode);

#define USBHOST_PORT1_GPIO	57
#define USBHOST_PORT2_GPIO	61
	/* EHCI PHY Reset hacks */
	gpio_request(USBHOST_PORT1_GPIO, "USB1 PHY RESET");
	gpio_direction_output(USBHOST_PORT1_GPIO, 1);
	gpio_request(USBHOST_PORT2_GPIO, "USB2 PHY RESET");
	gpio_direction_output(USBHOST_PORT2_GPIO, 1);
	udelay(500);
	gpio_set_value(USBHOST_PORT1_GPIO, 0);
	gpio_set_value(USBHOST_PORT2_GPIO, 0);


	if (platform_device_register(&ehci_device) < 0) {
		printk(KERN_ERR "Unable to register HS-USB (EHCI) device\n");
		return;
	}
}

#else

void usb_host_and_tll_init(
		struct omap_usbhost_port_data const *port_data,
		unsigned n)
{
}

void __init usb_ehci_init(enum ehci_hcd_omap_mode phy_mode,
		int chargepump, int phy_reset, int reset_gpio_port1,
		int reset_gpio_port2)
{
}

#endif /* CONFIG_USB_EHCI_HCD */

