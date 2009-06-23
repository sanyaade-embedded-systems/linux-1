/*
 * LCD panel support for the TI 4430SDP board
 *
 * Copyright (C) 2007 MontaVista
 * Author: Hunyue Yau <hyau@mvista.com>
 *
 * Derived from drivers/video/omap/lcd-apollon.c
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

#include <mach/mux.h>
#include <mach/omapfb.h>
#include <asm/mach-types.h>

#define LCD_PIXCLOCK_MAX                5400 /* freq 5.4 MHz */

static int sdp4430_panel_init(struct lcd_panel *panel,
				struct omapfb_device *fbdev)
{
	return 0;
}

static void sdp4430_panel_cleanup(struct lcd_panel *panel)
{
	return;
}

static int sdp4430_panel_enable(struct lcd_panel *panel)
{
	return 0;
}

static void sdp4430_panel_disable(struct lcd_panel *panel)
{
	return;
}

static unsigned long sdp4430_panel_get_caps(struct lcd_panel *panel)
{
	return 0;
}

struct lcd_panel sdp4430_panel = {
	.name		= "sdp4430",
	.config		= OMAP_LCDC_PANEL_TFT | OMAP_LCDC_INV_VSYNC |
			OMAP_LCDC_INV_HSYNC,
	.bpp		= 16,
	.data_lines	= 16,
	.x_res		= 240,
	.y_res		= 320,
	.hsw		= 3,	/* hsync_len (4) - 1 */
	.hfp		= 3,	/* right_margin (4) - 1 */
	.hbp		= 39,	/* left_margin (40) - 1 */
	.vsw		= 1,	/* vsync_len (2) - 1 */
	.vfp		= 2,	/* lower_margin */
	.vbp		= 7,	/* upper_margin (8) - 1 */

	.pixel_clock	= LCD_PIXCLOCK_MAX,

	.init		= sdp4430_panel_init,
	.cleanup	= sdp4430_panel_cleanup,
	.enable		= sdp4430_panel_enable,
	.disable	= sdp4430_panel_disable,
	.get_caps	= sdp4430_panel_get_caps,
};

static int sdp4430_panel_probe(struct platform_device *pdev)
{
	omapfb_register_panel(&sdp4430_panel);
	return 0;
}

static int sdp4430_panel_remove(struct platform_device *pdev)
{
	return 0;
}
static int sdp4430_panel_suspend(struct platform_device *pdev,
					pm_message_t mesg)
{
	return 0;
}

static int sdp4430_panel_resume(struct platform_device *pdev)
{
	return 0;
}

struct platform_driver sdp4430_panel_driver = {
	.probe		=	sdp4430_panel_probe,
	.remove		=	sdp4430_panel_remove,
	.suspend	=	sdp4430_panel_suspend,
	.resume		=	sdp4430_panel_resume,

	.driver		=	{
	.name		=	"sdp4430_lcd",
	.owner		=	THIS_MODULE,
				},
};

static int __init sdp4430_panel_drv_init(void)
{
	return platform_driver_register(&sdp4430_panel_driver);
}

static void __exit sdp4430_panel_drv_exit(void)
{
	platform_driver_unregister(&sdp4430_panel_driver);
	return;
}

module_init(sdp4430_panel_drv_init);
module_exit(sdp4430_panel_drv_exit);
