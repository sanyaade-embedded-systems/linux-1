/*
 * linux/arch/arm/mach-omap2/omap4-opp.h
 *
 * OMAP4 OPP Table Definitions
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Abhijit Pagare <abhijitpagare@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * History:
 *
 */

/* Note that these Values are to be updated once silicon
 * characteristic validated values are available.
 */

#ifndef __OMAP4_OPP_H_
#define __OMAP4_OPP_H_

#include <mach/omap-pm.h>

/* MPU speeds */
#define S600M   600000000
#define S550M   550000000
#define S500M   500000000
#define S250M   250000000
#define S125M   125000000

/* DSP speeds */
#define S430M   430000000
#define S400M   400000000
#define S360M   360000000
#define S180M   180000000
#define S90M    90000000

/* L3 speeds */
#define S83M    83000000
#define S166M   166000000

static struct omap_opp omap4_mpu_rate_table[] = {
	{0, 0, 0},
	/*OPP1*/
	{S125M, VDD1_OPP1, 0x1E},
	/*OPP2*/
	{S250M, VDD1_OPP2, 0x26},
	/*OPP3*/
	{S500M, VDD1_OPP3, 0x30},
	/*OPP4*/
	{S550M, VDD1_OPP4, 0x36},
	/*OPP5*/
	{S600M, VDD1_OPP5, 0x3C},
};

static struct omap_opp omap4_dsp_rate_table[] = {
	{0, 0, 0},
	/*OPP1*/
	{S90M, VDD2_OPP1, 0x1E},
	/*OPP2*/
	{S180M, VDD2_OPP2, 0x26},
	/*OPP3*/
	{S360M, VDD2_OPP3, 0x30},
	/*OPP4*/
	{S400M, VDD2_OPP4, 0x36},
	/*OPP5*/
	{S430M, VDD2_OPP5, 0x3C},
};

static struct omap_opp omap4_l3_rate_table[] = {
	{0, 0, 0},
	/*OPP1*/
	{0, VDD3_OPP1, 0x1E},
	/*OPP2*/
	{S83M, VDD3_OPP2, 0x24},
	/*OPP3*/
	{S166M, VDD3_OPP3, 0x2C},
};

#endif
