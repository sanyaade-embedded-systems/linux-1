/*:
 * Address mappings and base address for OMAP4 interconnects
 * and peripherals.
 *
 * Copyright (C) 2009 Texas Instruments
 *
 * Author: Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Updated with VDD OPP support by Abhijit Pagare <abhijitpagare@ti.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_ARCH_OMAP44XX_H
#define __ASM_ARCH_OMAP44XX_H

/*
 * Please place only base defines here and put the rest in device
 * specific headers.
 */
#define L4_44XX_BASE			0x4a000000
#define L4_WK_44XX_BASE			0x4a300000
#define L4_PER_44XX_BASE		0x48000000
#define L4_EMU_44XX_BASE		0x54000000
#define L3_44XX_BASE			0x44000000
#define L3_ABE_44XX_BASE		0x49000000

#define OMAP4430_32KSYNCT_BASE		0x4a304000
#define OMAP4430_CM1_BASE		0x4A004000
#define OMAP4430_CM_BASE		OMAP4430_CM1_BASE
#define OMAP4430_CM2_BASE		0x4A008000
#define OMAP4430_PRM_BASE		0x4A306000
#define OMAP44XX_GPMC_BASE		0x50000000
#define OMAP443X_SCM_BASE		0x4a002000
#define OMAP443X_CTRL_BASE		0x4A100000
#define OMAP44XX_IC_BASE		0x48200000
#define OMAP44XX_IVA_INTC_BASE		0x40000000
#define IRQ_SIR_IRQ			0x0040
#define OMAP44XX_GIC_DIST_BASE		0x48241000
#define OMAP44XX_GIC_CPU_BASE		0x48240100
#define OMAP44XX_VA_GIC_CPU_BASE	IO_ADDRESS(OMAP44XX_GIC_CPU_BASE)
#define OMAP44XX_SCU_BASE		0x48240000
#define OMAP44XX_VA_SCU_BASE		IO_ADDRESS(OMAP44XX_SCU_BASE)
#define OMAP44XX_LOCAL_TWD_BASE		0x48240600
#define OMAP44XX_VA_LOCAL_TWD_BASE	IO_ADDRESS(OMAP44XX_LOCAL_TWD_BASE)
#define OMAP44XX_LOCAL_TWD_SIZE		0x00000100
#define OMAP44XX_WKUPGEN_BASE		0x48281000
#define OMAP44XX_VA_WKUPGEN_BASE	IO_ADDRESS(OMAP44XX_WKUPGEN_BASE)
#define OMAP44XX_L2CACHE_BASE		0x48242000
#define OMAP44XX_VA_L2CACHE_BASE	0x48242000

#define OMAP44xx_MAILBOX_BASE		(L4_44XX_BASE + 0xF4000)

#ifdef CONFIG_ARCH_OMAP4
/* VDD OPP identifiers */
#define VDD1_OPP		0x1
#define VDD2_OPP		0x2
#define VDD3_OPP		0x3

/* VDD1 OPPS */
#define VDD1_OPP1		0x1
#define VDD1_OPP2		0x2
#define VDD1_OPP3		0x3
#define VDD1_OPP4		0x4
#define VDD1_OPP5		0x5

/* VDD2 OPPS */
#define VDD2_OPP1		0x1
#define VDD2_OPP2		0x2
#define VDD2_OPP3		0x3
#define VDD2_OPP4		0x4
#define VDD2_OPP5		0x5

/* VDD3 OPPS */
#define VDD3_OPP1		0x1
#define VDD3_OPP2		0x2
#define VDD3_OPP3		0x3

#define MIN_VDD1_OPP		VDD1_OPP1
#define MAX_VDD1_OPP		VDD1_OPP5
#define MIN_VDD2_OPP		VDD2_OPP1
#define MAX_VDD2_OPP		VDD2_OPP5
#define MIN_VDD3_OPP		VDD2_OPP1
#define MAX_VDD3_OPP		VDD2_OPP3

#endif

#endif /* __ASM_ARCH_OMAP44XX_H */

