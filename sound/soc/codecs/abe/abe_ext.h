/*
 * ==========================================================================
 *               Texas Instruments OMAP(TM) Platform Firmware
 * (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this firmware is controlled by the terms and conditions found
 *  in the license agreement under which this firmware has been supplied.
 * ==========================================================================
 */

#ifndef _ABE_EXT_H_
#define _ABE_EXT_H_

#include <linux/io.h>
#define PC_SIMULATION			0		/* Tuning is done on PC ? */

/*
 * OS DEPENDENT MMU CONFIGURATION
 */
#define ABE_ATC_BASE_ADDRESS_L3		L3_ABE_44XX_PHYS + 0xf1000
	/* base address used for L3/DMA access */
#define ABE_DMEM_BASE_ADDRESS_L3	L3_ABE_44XX_PHYS + 0x80000
	/* 64kB  as seen from DMA access */
#define ABE_PMEM_BASE_ADDRESS_MPU	IO_ADDRESS(L3_ABE_44XX_PHYS) + 0xe0000
	/*  8kB  as seen from MPU access */
#define ABE_CMEM_BASE_ADDRESS_MPU	IO_ADDRESS(L3_ABE_44XX_PHYS) + 0xa0000
	/* 8kB */
#define ABE_SMEM_BASE_ADDRESS_MPU	IO_ADDRESS(L3_ABE_44XX_PHYS) + 0xc0000
	/* 24kB */
#define ABE_DMEM_BASE_ADDRESS_MPU	IO_ADDRESS(L3_ABE_44XX_PHYS) + 0x80000
	/* 64kB */
#define ABE_ATC_BASE_ADDRESS_MPU	IO_ADDRESS(L3_ABE_44XX_PHYS) + 0xf1000

#endif	/* _ABE_EXT_H_ */
