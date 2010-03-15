/*
 * netra.h
 *
 * This file contains the processor specific definitions of the TI Netra SoC.
 *
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ASM_ARCH_NETRA_H
#define __ASM_ARCH_NETRA_H

#define L4_34XX_BASE		0x08600000
#define L4_WK_34XX_BASE		0x48300000
#define L4_WK_OMAP_BASE		L4_WK_34XX_BASE
#define L4_PER_34XX_BASE	0x49000000
#define L4_PER_OMAP_BASE	L4_PER_34XX_BASE
#define L4_EMU_34XX_BASE	0x54000000
#define L4_EMU_BASE		L4_EMU_34XX_BASE
#define L3_34XX_BASE		0x68000000
#define L3_OMAP_BASE		L3_34XX_BASE

#define OMAP3430_32KSYNCT_BASE	0x48320000
#define OMAP3430_CM_BASE	0x48004800
#define OMAP3430_PRM_BASE	0x48306800
#define OMAP343X_SMS_BASE	0x6C000000
#define OMAP343X_SDRC_BASE	0x6D000000
#define OMAP34XX_GPMC_BASE	0x6E000000
#define OMAP343X_SCM_BASE	0x48002000
#define OMAP343X_CTRL_BASE	OMAP343X_SCM_BASE

#define NETRA_INTC_PHYS		0x50000000

#define OMAP2_32KSYNCT_BASE		OMAP3430_32KSYNCT_BASE
#define OMAP2_CM_BASE			OMAP3430_CM_BASE
#define OMAP2_PRM_BASE			OMAP3430_PRM_BASE
#define OMAP2_VA_IC_BASE		IO_ADDRESS(NETRA_INTC_PHYS)

#define OMAP34XX_DSP_BASE	0x58000000
#define OMAP34XX_DSP_MEM_BASE	(OMAP34XX_DSP_BASE + 0x0)
#define OMAP34XX_DSP_IPI_BASE	(OMAP34XX_DSP_BASE + 0x1000000)
#define OMAP34XX_DSP_MMU_BASE	(OMAP34XX_DSP_BASE + 0x2000000)

#define NETRA_IPSS_USBSS_BASE	0x47400000
#define NETRA_IPSS_USBSS_LEN	0xFFF
#define NETRA_IPSS_USB0_BASE	0x47401000
#define NETRA_IPSS_USB1_BASE	0x47401800
#define NETRA_USB_CPPIDMA_BASE	0x47402000
#define NETRA_USB_CPPIDMA_LEN	0x5FFF

#define NETRA_PRE_SILICON_HAPS	/* define only for netra bringup */

#ifdef NETRA_PRE_SILICON_HAPS
/* only for HAPS54 platform */
#define NETRA_USBSSINT_IRQ	10 /* usb subsystem interrupt*/
#define NETRA_USBSINT0_IRQ	11 /* usb controller0 intrpt */
#define NETRA_USBSINT1_IRQ	12 /* usb controller1 intrpt */
#else
#define NETRA_USBSSINT_IRQ	49 /* usb subsystem interrupt*/
#define NETRA_USBSINT0_IRQ	50 /* usb controller0 intrpt */
#define NETRA_USBSINT1_IRQ	51 /* usb controller1 intrpt */
#endif
#endif /* __ASM_ARCH_NETRA_H */

