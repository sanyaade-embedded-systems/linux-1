/*
 * LCD panel support for the TI OMAP3 Beagle board
 *
 * Author: Koen Kooi <koen@openembedded.org>
 *
 * Derived from drivers/video/omap/lcd-omap3evm.c
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c/twl4030.h>

#include <mach/gpio.h>
#include <mach/mux.h>
#include <mach/omapfb.h>
#include <asm/mach-types.h>

#define LCD_PANEL_ENABLE_GPIO       170

static int omap3beagle_panel_init(struct lcd_panel *panel,
				struct omapfb_device *fbdev)
{
	omap_request_gpio(LCD_PANEL_ENABLE_GPIO);
	return 0;
}

static void omap3beagle_panel_cleanup(struct lcd_panel *panel)
{
}

static int omap3beagle_panel_enable(struct lcd_panel *panel)
{
	omap_set_gpio_dataout(LCD_PANEL_ENABLE_GPIO, 1);
	return 0;
}

static void omap3beagle_panel_disable(struct lcd_panel *panel)
{
	omap_set_gpio_dataout(LCD_PANEL_ENABLE_GPIO, 0);
}

static unsigned long omap3beagle_panel_get_caps(struct lcd_panel *panel)
{
	return 0;
}

struct lcd_panel omap3beagle_panel = {
	.name		= "omap3beagle",
	.config		= OMAP_LCDC_PANEL_TFT,

	.bpp		= 16,
	.data_lines	= 24,

#if 0				/* 720p50 */
	.x_res		= 1280,
	.y_res		= 720,
	.hfp		= 440,		/* right_margin */
	.hsw		= 40,		/* hsync_len */
	.hbp		= 220,		/* left_margin */
	.vfp		= 5,		/* lower_margin */
	.vsw		= 5,		/* vsync_len */
	.vbp		= 20,		/* upper_margin */
	.pixel_clock	= 74250,
#elif 1				/* 720p60 */
	.x_res		= 1280,
	.y_res		= 720,
	.hfp		= 110,		/* right_margin */
	.hsw		= 40,		/* hsync_len */
	.hbp		= 220,		/* left_margin */
	.vfp		= 5,		/* lower_margin */
	.vsw		= 5,		/* vsync_len */
	.vbp		= 20,		/* upper_margin */
	.pixel_clock	= 74250,
#else
	.x_res		= 1360,
	.y_res		= 768,
	.hfp		= 64,		/* right_margin */
	.hsw		= 112,		/* hsync_len */
	.hbp		= 256,		/* left_margin */
	.vfp		= 3,		/* lower_margin */
	.vsw		= 6,		/* vsync_len */
	.vbp		= 18,		/* upper_margin */
	.pixel_clock	= 85500,
	.pcd		= 5,
#endif

	.acb		= 40,

	.init		= omap3beagle_panel_init,
	.cleanup	= omap3beagle_panel_cleanup,
	.enable		= omap3beagle_panel_enable,
	.disable	= omap3beagle_panel_disable,
	.get_caps	= omap3beagle_panel_get_caps,
};

static int omap3beagle_panel_probe(struct platform_device *pdev)
{
	omapfb_register_panel(&omap3beagle_panel);
	return 0;
}

static int omap3beagle_panel_remove(struct platform_device *pdev)
{
	return 0;
}

static int omap3beagle_panel_suspend(struct platform_device *pdev,
				   pm_message_t mesg)
{
	return 0;
}

static int omap3beagle_panel_resume(struct platform_device *pdev)
{
	return 0;
}

struct platform_driver omap3beagle_panel_driver = {
	.probe		= omap3beagle_panel_probe,
	.remove		= omap3beagle_panel_remove,
	.suspend	= omap3beagle_panel_suspend,
	.resume		= omap3beagle_panel_resume,
	.driver		= {
		.name	= "omap3beagle_lcd",
		.owner	= THIS_MODULE,
	},
};

static int __init omap3beagle_panel_drv_init(void)
{
	return platform_driver_register(&omap3beagle_panel_driver);
}

static void __exit omap3beagle_panel_drv_exit(void)
{
	platform_driver_unregister(&omap3beagle_panel_driver);
}

module_init(omap3beagle_panel_drv_init);
module_exit(omap3beagle_panel_drv_exit);
