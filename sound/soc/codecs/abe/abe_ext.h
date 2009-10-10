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

#define PC_SIMULATION			0		/* Tuning is done on PC ? */

/*
 * OS DEPENDENT MMU CONFIGURATION
 */

#define ABE_ATC_BASE_ADDRESS_L3		0x490F1000L	/* base address used for L3/DMA access */
#define ABE_DMEM_BASE_ADDRESS_L3	0x49080000L	/* 64kB  as seen from DMA access */

#define ABE_PMEM_BASE_ADDRESS_MPU	0x401E0000L	/*  8kB  as seen from MPU access */
#define ABE_CMEM_BASE_ADDRESS_MPU	0x401A0000L	/*  8kB  +++++++++++++++++++++++ */
#define ABE_SMEM_BASE_ADDRESS_MPU	0x401C0000L	/* 24kB                          */
#define ABE_DMEM_BASE_ADDRESS_MPU	0x40180000L	/* 64kB                          */
#define ABE_ATC_BASE_ADDRESS_MPU	0x401F1000L

#endif	/* _ABE_EXT_H_ */
