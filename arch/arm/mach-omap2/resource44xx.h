/*
 * linux/arch/arm/mach-omap2/resource44xx.h
 *
 * OMAP4 resource definitions
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

/* OMAP4 Power Doamin Latency Resources */

#ifdef CONFIG_ARCH_OMAP4

static struct pd_latency_db dsp_44xx_pwrdm_lat_db = {
	.pwrdm_name = "dsp_pwrdm",
	/* Set proper values once CPUIdle is in effect */
	.latency[PD_LATENCY_OFF] = 1000,
	.latency[PD_LATENCY_RET] = 100,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource dsp_44xx_pwrdm_latency = {
	.name		= "dsp_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &dsp_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db std_efuse_44xx_pwrdm_lat_db = {
	.pwrdm_name = "std_effuse_pwrdm",
	/* Set proper values once CPUIdle is in effect */
	.latency[PD_LATENCY_OFF] = 1000,
	.latency[PD_LATENCY_RET] = 100,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource std_efuse_44xx_pwrdm_latency = {
	.name		= "std_efuse_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &std_efuse_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_l4_per_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_l4_per_pwrdm",
	.latency[PD_LATENCY_OFF] = 200,
	.latency[PD_LATENCY_RET] = 110,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_l4_per_44xx_pwrdm_latency = {
	.name		= "pd_l4_per_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_l4_per_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_l3_init_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_l3_init_pwrdm",
	/* Set proper values once CPUIdle is in effect */
	.latency[PD_LATENCY_OFF] = 1000,
	.latency[PD_LATENCY_RET] = 100,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_l3_init_44xx_pwrdm_latency = {
	.name		= "pd_l3_init_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_l3_init_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db ivahd_44xx_pwrdm_lat_db = {
	.pwrdm_name = "ivahd_pwrdm",
	.latency[PD_LATENCY_OFF] = 1100,
	.latency[PD_LATENCY_RET] = 350,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource ivahd_44xx_pwrdm_latency = {
	.name		= "ivahd_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &ivahd_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_sgx_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_sgx_pwrdm",
	.latency[PD_LATENCY_OFF] = 1000,
	.latency[PD_LATENCY_RET] = 100,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_sgx_44xx_pwrdm_latency = {
	.name		= "pd_sgx_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_sgx_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_emu_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_emu_pwrdm",
	.latency[PD_LATENCY_OFF] = 1000,
	.latency[PD_LATENCY_RET] = 100,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_emu_44xx_pwrdm_latency = {
	.name		= "pd_emu_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_emu_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_dss_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_dss_pwrdm",
	.latency[PD_LATENCY_OFF] = 70,
	.latency[PD_LATENCY_RET] = 20,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_dss_44xx_pwrdm_latency = {
	.name		= "pd_dss_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_dss_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_cam_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_cam_pwrdm",
	.latency[PD_LATENCY_OFF] = 850,
	.latency[PD_LATENCY_RET] = 35,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_cam_44xx_pwrdm_latency = {
	.name		= "pd_cam_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_cam_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};

static struct pd_latency_db pd_audio_44xx_pwrdm_lat_db = {
	.pwrdm_name = "pd_audio_pwrdm",
	/* Set proper values once CPUIdle is in effect */
	.latency[PD_LATENCY_OFF] = 1000,
	.latency[PD_LATENCY_RET] = 100,
	.latency[PD_LATENCY_INACT] = -1,
	.latency[PD_LATENCY_ON]  = 0
};

static struct shared_resource pd_audio_44xx_pwrdm_latency = {
	.name		= "pd_audio_44xx_pwrdm_latency",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_LATENCY,
	.resource_data	= &pd_audio_44xx_pwrdm_lat_db,
	.ops		= &pd_lat_res_ops,
};
/* End of Power Domain Resources */


/* OPP/Frequency Resources */

static struct shared_resource vdd2_opp = {
	.name		= "vdd2_opp",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_PERFORMANCE,
	.ops		= &opp_res_ops,
};

static struct shared_resource vdd3_opp = {
	.name		= "vdd3_opp",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_PERFORMANCE,
	.resource_data	= &l3_throughput_db,
	.ops		= &opp_res_ops,
};

static struct shared_resource mpu_freq = {
	.name		= "mpu_freq",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_PERFORMANCE,
	.ops		= &freq_res_ops,
};

static struct shared_resource dsp_freq = {
	.name		= "dsp_freq",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.flags		= RES_TYPE_PERFORMANCE,
	.ops		= &freq_res_ops,
};
/* End of OPP/Frequency Resources */

#endif
