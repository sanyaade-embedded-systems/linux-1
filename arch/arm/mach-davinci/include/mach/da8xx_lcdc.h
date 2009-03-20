/*
 * Header file for TI DA830 LCD controller platform data.
 *
 * 2008 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#ifndef DA8XX_LCDC_PLAT_H
#define DA8XX_LCDC_PLAT_H

struct da8xx_lcdc_platform_data {
	const char *lcdc_clk_name;
};
int da830_lcd_hw_init(void);
int da850_lcd_hw_init(void);
#endif  /* ifndef DA830_LCDC_PLAT_H */

