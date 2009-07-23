/*
 * lcd4430_sdp panel support
 *
 * Copyright (C) 2009 Texas Instruments
 * Author: Mayuresh Janorkar <mayur@ti.com>
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

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/i2c/twl.h>
#include <linux/spi/spi.h>

#include <mach/gpio.h>
#include <mach/gpio.h>
#include <mach/mux.h>
#include <asm/mach-types.h>
#include <mach/control.h>

#include <mach/display.h>

#define LCD_XRES		800
#define LCD_YRES		480
#define LCD_PIXCLOCK_MAX	21625

/*NEC NL8048HL11-01B  Manual
* defines HFB, HSW, HBP, VFP, VSW, VBP as shown below
*/

static struct omap_video_timings lcd_4430_sdp_panel_timings = {
	/* 800 x 480 @ 60 Hz  Reduced blanking VESA CVT 0.31M3-R */
	.x_res          = LCD_XRES,
	.y_res          = LCD_YRES,
	.pixel_clock    = LCD_PIXCLOCK_MAX,
	.hfp            = 6,
	.hsw            = 1,
	.hbp            = 4,
	.vfp            = 3,
	.vsw            = 1,
	.vbp            = 4,
};

static int lcd_4430_sdp_panel_probe(struct omap_dss_device *dssdev)
{
	dssdev->panel.config = OMAP_DSS_LCD_TFT | OMAP_DSS_LCD_IVS |
				OMAP_DSS_LCD_IHS;
	dssdev->panel.timings = lcd_4430_sdp_panel_timings;

	return 0;
}

static void lcd_4430_sdp_panel_remove(struct omap_dss_device *dssdev)
{
}

static int lcd_4430_sdp_panel_enable(struct omap_dss_device *dssdev)
{
	int r = 0;
	mdelay(4);
	if (dssdev->platform_enable)
		r = dssdev->platform_enable(dssdev);
	/* Enable LCD-Enable bit in DISPC_CONTROL */
	omap_writel((omap_readl(0x48043040) | 0x1), 0x48043040);
	return r;
}

static void lcd_4430_sdp_panel_disable(struct omap_dss_device *dssdev)
{
	if (dssdev->platform_disable)
		dssdev->platform_disable(dssdev);
	mdelay(4);
}

static int lcd_4430_sdp_panel_suspend(struct omap_dss_device *dssdev)
{
	lcd_4430_sdp_panel_disable(dssdev);
	return 0;
}

static int lcd_4430_sdp_panel_resume(struct omap_dss_device *dssdev)
{
	return lcd_4430_sdp_panel_enable(dssdev);
}

static struct omap_dss_driver lcd_4430_sdp_driver = {
	.probe          = lcd_4430_sdp_panel_probe,
	.remove         = lcd_4430_sdp_panel_remove,

	.enable         = lcd_4430_sdp_panel_enable,
	.disable        = lcd_4430_sdp_panel_disable,
	.suspend        = lcd_4430_sdp_panel_suspend,
	.resume         = lcd_4430_sdp_panel_resume,

	.driver		= {
		.name   = "sdp4430_panel",
		.owner 	= THIS_MODULE,
	},
};

static int __init lcd_4430_sdp_lcd_init(void)
{

	return omap_dss_register_driver(&lcd_4430_sdp_driver);
}

static void __exit lcd_4430_sdp_lcd_exit(void)
{
	return omap_dss_unregister_driver(&lcd_4430_sdp_driver);
}

module_init(lcd_4430_sdp_lcd_init);
module_exit(lcd_4430_sdp_lcd_exit);
MODULE_LICENSE("GPL");

