/*
 * Header file for TI OMAPL1X7 LCD controller platform data.
 *
 * 2008 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#ifndef OMAPL1X7_LCDC_PLAT_H
#define OMAPL1X7_LCDC_PLAT_H

struct omapl1x7_lcdc_platform_data {
	const char *lcdc_clk_name;
};
int omapl1x7_lcd_hw_init(void);
#endif  /* ifndef OMAPL1X7_LCDC_PLAT_H */

