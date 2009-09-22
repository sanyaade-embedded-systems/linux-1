/*
 * linux/arch/arm/mach-omap2/resource.h
 *
 * OMAP3 and OMAP4 generic resource definitions
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Abhijit Pagare <abhijitpagare@ti.com>
 *
 * Based on file resource34xx.h developed by
 * Rajendra Nayak <rnayak@ti.com>
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

#ifndef __ARCH_ARM_MACH_OMAP2_RESOURCE_H
#define __ARCH_ARM_MACH_OMAP2_RESOURCE_H

#include <mach/resource.h>
#include <linux/clk.h>
#include <mach/clock.h>
#include <mach/powerdomain.h>
#include <mach/omap-pm.h>
#include <mach/omap34xx.h>


#ifdef CONFIG_ARCH_OMAP3430
#define MAX_VDD3_OPP		0
#define VDD3_OPP		0
#endif

#ifdef CONFIG_OMAP_SMARTREFLEX
extern int sr_voltagescale_vcbypass(u32 t_opp, u32 c_opp, u8 t_vsel, u8 c_vsel);
#endif

extern void lock_scratchpad_sem(void);
extern void unlock_scratchpad_sem(void);

/*
 * mpu_latency/core_latency are used to control the cpuidle C state.
 */
void init_latency(struct shared_resource *resp);
int set_latency(struct shared_resource *resp, u32 target_level);

static u8 mpu_qos_req_added;
static u8 core_qos_req_added;

static struct shared_resource_ops lat_res_ops = {
	.init 		= init_latency,
	.change_level   = set_latency,
};

static struct shared_resource mpu_latency = {
	.name 		= "mpu_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430 | CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data  = &mpu_qos_req_added,
	.ops 		= &lat_res_ops,
};

static struct shared_resource core_latency = {
	.name 		= "core_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430 | CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &core_qos_req_added,
	.ops 		= &lat_res_ops,
};

/*
 * Power domain Latencies are used to control the target Power
 * domain state once all clocks for the power domain
 * are released.
 */
void init_pd_latency(struct shared_resource *resp);
int set_pd_latency(struct shared_resource *resp, u32 target_level);

/* Power Domain Latency levels */
#define PD_LATENCY_OFF		0x0
#define PD_LATENCY_RET		0x1
#define PD_LATENCY_INACT	0x2
#define PD_LATENCY_ON		0x3

#define PD_LATENCY_MAXLEVEL 	0x4

struct pd_latency_db {
	char *pwrdm_name;
	struct powerdomain *pd;
	/* Latencies for each state transition, stored in us */
	unsigned long latency[PD_LATENCY_MAXLEVEL];
};

static struct shared_resource_ops pd_lat_res_ops = {
	.init		= init_pd_latency,
	.change_level 	= set_pd_latency,
};

void init_opp(struct shared_resource *resp);
int set_opp(struct shared_resource *resp, u32 target_level);
int validate_opp(struct shared_resource *resp, u32 target_level);
void init_freq(struct shared_resource *resp);
int set_freq(struct shared_resource *resp, u32 target_level);
int validate_freq(struct shared_resource *resp, u32 target_level);

struct bus_throughput_db {
	/* Throughput for each OPP/Freq of the bus */
	unsigned long throughput[3];
};

static struct shared_resource_ops opp_res_ops = {
	.init		= init_opp,
	.change_level	= set_opp,
	.validate_level	= validate_opp,
};

/* Throughput in KiB/s */
static struct bus_throughput_db l3_throughput_db = {
	.throughput[0] = 0,
	.throughput[1] = 2656000,
	.throughput[2] = 5312000,
};

static struct shared_resource_ops freq_res_ops = {
	.init		= init_freq,
	.change_level	= set_freq,
	.validate_level	= validate_freq,
};

static struct shared_resource vdd1_opp = {
	.name		= "vdd1_opp",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430 | CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_PERFORMANCE,
	.ops		= &opp_res_ops,
};

#include "resource34xx.h"
#include "resource44xx.h"

struct shared_resource *resources_omap[] __initdata = {
	&mpu_latency,
	&core_latency,

	/* Generic OPP/Frequency Resources */
	&vdd1_opp,
	&vdd2_opp,
	&mpu_freq,
	&dsp_freq,

#ifdef CONFIG_ARCH_OMAP34XX
	/* Power Domain Latency resources for OMAP3 */
	&core_pwrdm_latency,
	&iva2_pwrdm_latency,
	&gfx_pwrdm_latency,
	&sgx_pwrdm_latency,
	&dss_pwrdm_latency,
	&cam_pwrdm_latency,
	&per_pwrdm_latency,
	&neon_pwrdm_latency,
	&usbhost_pwrdm_latency,
	&emu_pwrdm_latency,
#endif

#ifdef CONFIG_ARCH_OMAP4
	/* Power Domain Latency resources for OMAP4 */
	&dsp_44xx_pwrdm_latency,
	&std_efuse_44xx_pwrdm_latency,
	&pd_l4_per_44xx_pwrdm_latency,
	&pd_l3_init_44xx_pwrdm_latency,
	&ivahd_44xx_pwrdm_latency,
	&pd_sgx_44xx_pwrdm_latency,
	&pd_emu_44xx_pwrdm_latency,
	&pd_dss_44xx_pwrdm_latency,
	&pd_cam_44xx_pwrdm_latency,
	&pd_audio_44xx_pwrdm_latency,

	/* OMAP4 specific OPP/Frequency resource */
	&vdd3_opp,
#endif

	NULL
};

#endif /* __ARCH_ARM_MACH_OMAP2_RESOURCE_H */
