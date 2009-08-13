/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_EXT.H
 *
 * Constants definitions
 *
 * @path 
 * @rev     01.00
 */

#ifndef __ABEEXT_
#define __ABEEXT_

#include <linux/io.h>
#define PC_SIMULATION	0 /* Tuning is done on PC ? */

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


#endif /* ifndef ABEEXT */
