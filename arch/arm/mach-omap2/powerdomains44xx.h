/*
 * OMAP44XX powerdomain definitions
 *
 * Copyright (C) 2008-2009 Texas Instruments, Inc.
 *
 * Written by Abhijit Pagare(abhijitpagare@ti.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef ARCH_ARM_MACH_OMAP2_POWERDOMAINS44XX
#define ARCH_ARM_MACH_OMAP2_POWERDOMAINS44XX

/*
 * N.B. If powerdomains are added or removed from this file, update
 * the array in mach-omap2/powerdomains.h.
 */


#include <mach/powerdomain.h>

#include "prcm-common.h"
#include "prm.h"
#include "prm-regbits-44xx.h"
#include "cm.h"
#include "cm1-regbits-44xx.h"
#include "cm2-regbits-44xx.h"

/*
 * 44XX-specific powerdomains, dependencies
 */

#ifdef CONFIG_ARCH_OMAP4

/* Wakeup dependency for 44xx-specific pwrdm modules (to be populated later) */

/* Sleep dependency for 44xx-specific pwrdm modules (to be populated later) */


/*
 * Powerdomains (without the dpendency parameter definitions)
 */

static struct powerdomain dsp_44xx_pwrdm = {
	.name			= "dsp_pwrdm",
	.prcm_offs		= OMAP4430_DSP_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret	= PWRSTS_OFF_RET,
	.banks			= 3,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_RET,
		[1] = PWRSTS_OFF_RET,
		[2] = PWRSTS_OFF_RET,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
		[1] = PWRDM_POWER_ON,
		[2] = PWRDM_POWER_ON,
	},
};

static struct powerdomain std_efuse_44xx_pwrdm = {
	.name			= "std_effuse_pwrdm",
	.prcm_offs		= OMAP4430_CEFUSE_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct powerdomain mpu_44xx_pwrdm = {
	.name			= "mpu_pwrdm",
	.prcm_offs		= OMAP4430_MPU_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret 	= PWRSTS_OFF_RET,
	.banks			= 3,
	.pwrsts_mem_ret		= {
		[0] = PWRSTS_OFF_RET,
		[1] = PWRSTS_OFF_RET,
		[2] = PWRDM_POWER_RET,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
		[1] = PWRDM_POWER_ON,
		[2] = PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_l4_per_44xx_pwrdm = {
	.name			= "pd_l4_per_pwrdm",
	.prcm_offs		= OMAP4430_L4PER_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret	= PWRSTS_OFF_RET,
	.banks			= 2,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_OFF,
		[1] = PWRDM_POWER_RET,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
		[1] = PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_l3_init_44xx_pwrdm = {
	.name			= "pd_l3_init_pwrdm",
	.prcm_offs		= OMAP4430_L3INIT_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret	= PWRSTS_OFF_RET,
	.banks			= 1,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_OFF,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
	},
};

static struct powerdomain ivahd_44xx_pwrdm = {
	.name			= "ivahd_pwrdm",
	.prcm_offs		= OMAP4430_IVAHD_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_ON,
	.banks			= 4,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_OFF,
		[1] = PWRSTS_OFF_RET,
		[2] = PWRSTS_OFF_RET,
		[3] = PWRSTS_OFF_RET,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
		[1] = PWRDM_POWER_ON,
		[2] = PWRDM_POWER_ON,
		[3] = PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_sgx_44xx_pwrdm = {
	.name			= "pd_sgx_pwrdm",
	.prcm_offs		= OMAP4430_GFX_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_ON,
	.banks			= 1,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_OFF,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_emu_44xx_pwrdm = {
	.name			= "pd_emu_pwrdm",
	.prcm_offs		= OMAP4430_EMU_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_ON,
	.banks			= 1,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_OFF,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_dss_44xx_pwrdm = {
	.name			= "pd_dss_pwrdm",
	.prcm_offs		= OMAP4430_DSS_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret	= PWRSTS_OFF_RET,
	.banks			= 1,
	.pwrsts_mem_ret		= {
		[0] = PWRDM_POWER_OFF,
	},
	.pwrsts_mem_on		= {
		[0] = PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_core_44xx_pwrdm = {
	.name			= "core_pwrdm",
	.prcm_offs		= OMAP4430_CORE_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret	= PWRSTS_OFF_RET,
	.banks			= 5,
	.pwrsts_mem_ret		= {
		[0]		= PWRDM_POWER_OFF,
		[1]		= PWRSTS_OFF_RET,
		[2]		= PWRDM_POWER_RET,
		[3]		= PWRSTS_OFF_RET,
		[4]		= PWRSTS_OFF_RET,
	},
	.pwrsts_mem_on		= {
		[0]		= PWRDM_POWER_ON,
		[1]		= PWRSTS_OFF_RET_ON,
		[2]		= PWRDM_POWER_ON,
		[3]		= PWRDM_POWER_ON,
		[4]		= PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_cam_44xx_pwrdm = {
	.name			= "pd_cam_pwrdm",
	.prcm_offs		= OMAP4430_CAM_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_ON,
	.banks			= 1,
	.pwrsts_mem_ret		= {
		[0]		= PWRDM_POWER_OFF,
	},
	.pwrsts_mem_on		= {
		[0]		= PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_audio_44xx_pwrdm = {
	.name			= "pd_audio_pwrdm",
	.prcm_offs		= OMAP4430_ABE_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
	.pwrsts			= PWRSTS_OFF_ON,
	.banks			= 2,
	.pwrsts_mem_ret		= {
		[0]		= PWRDM_POWER_RET,
		[1]		= PWRDM_POWER_ON,
	},
	.pwrsts_mem_on		= {
		[0]		= PWRDM_POWER_ON,
		[1]		= PWRDM_POWER_ON,
	},
};

static struct powerdomain pd_alwon_mpu_44xx_pwrdm = {
	.name			= "pd_alwon_mpu_pwrdm",
	.prcm_offs		= OMAP4430_ALWAYS_ON_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct powerdomain pd_alwon_dsp_44xx_pwrdm = {
	.name			= "pd_alwon_dsp_pwrdm",
	.prcm_offs		= OMAP4430_ALWAYS_ON_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

static struct powerdomain pd_alwon_core_44xx_pwrdm = {
	.name			= "pd_alwon_core_pwrdm",
	.prcm_offs		= OMAP4430_ALWAYS_ON_MOD,
	.omap_chip		= OMAP_CHIP_INIT(CHIP_IS_OMAP4430),
};

#endif    /* CONFIG_ARCH_OMAP4 */

#endif
