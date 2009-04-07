/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2005 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/*
 * file   edma3_sample_app.c
 * brief  DaVinci DMA Controller
 *
 *   This file contains DaVinci DMA Controller Test  code.
 *
 *   NOTE: THIS FILE IS PROVIDED ONLY FOR INITIAL DEMO RELEASE AND MAY BE
 *         REMOVED AFTER THE DEMO OR THE CONTENTS OF THIS FILE ARE SUBJECT
 *         TO CHANGE.
 *
 *   (C) Copyright 2008, Texas Instruments, Inc
 *
 *   @author	Anuj Aggarwal
 *   @version	0.1 -
 *		Created on 30/09/05
 *		Assumption: Channel and ParamEntry has 1 to 1 mapping
 *		0.2 - Sudhakar Rajashekhara
 *		06/04/2009 - Modified to  use new EDMA APIs
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/sysctl.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>

#include <mach/memory.h>
#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/cpu.h>
#include <mach/edma.h>

// #undef EDMA3_DEBUG
#define EDMA3_DEBUG

#ifdef EDMA3_DEBUG
#define DMA_PRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
#define DMA_FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
#define DMA_FN_OUT printk(KERN_INFO "[%s]: end\n",__FUNCTION__)
#else
#define DMA_PRINTK( x... )
#define DMA_FN_IN
#define DMA_FN_OUT
#endif

#define MAX_DMA_TRANSFER_IN_BYTES   (32768)
#define STATIC_SHIFT                3
#define TCINTEN_SHIFT               20
#define ITCINTEN_SHIFT              21
#define TCCHEN_SHIFT                22
#define ITCCHEN_SHIFT               23

static volatile int irqraised1 = 0;
static volatile int irqraised2 = 0;

int edma3_memtomemcpytest_dma(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue);
int edma3_memtomemcpytest_dma_link(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue);

dma_addr_t dmaphyssrc1 = 0;
dma_addr_t dmaphyssrc2 = 0;
dma_addr_t dmaphysdest1 = 0;
dma_addr_t dmaphysdest2 = 0;

char *dmabufsrc1 = NULL;
char *dmabufsrc2 = NULL;
char *dmabufdest1 = NULL;
char *dmabufdest2 = NULL;

static int acnt = 512;
static int bcnt = 8;
static int ccnt = 8;

module_param(acnt, int, S_IRUGO);
module_param(bcnt, int, S_IRUGO);
module_param(ccnt, int, S_IRUGO);


static void callback1(unsigned lch, u16 ch_status, void *data)
{
	switch(ch_status) {
	case DMA_COMPLETE:
		irqraised1 = 1;
		/*DMA_PRINTK ("\n From Callback 1: Channel %d status is: %u\n",
				lch, ch_status);*/
		break;
	case DMA_CC_ERROR:
		irqraised1 = -1;
		DMA_PRINTK ("\nFrom Callback 1: DMA_CC_ERROR occured "
				"on Channel %d\n", lch);
		break;
	default:
		break;
	}
}

static int __init edma_test_init(void)
{
	int result = 0;
	int iterations = 0;
	int numTCs = 2;
	int modes = 2;
	int i,j;

	printk ("\nInitializing edma3_sample_app module\n");

	DMA_PRINTK ( "\nACNT=%d, BCNT=%d, CCNT=%d", acnt, bcnt, ccnt);

	/* allocate consistent memory for DMA
	 * dmaphyssrc1(handle)= device viewed address.
	 * dmabufsrc1 = CPU-viewed address
	 */

	dmabufsrc1 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,
					&dmaphyssrc1, 0);
	if (!dmabufsrc1) {
		DMA_PRINTK ("dma_alloc_coherent failed for dmaphyssrc1\n");
		return -ENOMEM;
	}

	dmabufdest1 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,
						&dmaphysdest1, 0);
	if (!dmabufdest1) {
		DMA_PRINTK("dma_alloc_coherent failed for dmaphysdest1\n");
		dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,
				dmaphyssrc1);
		return -ENOMEM;
	}

	dmabufsrc2 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,
					&dmaphyssrc2, 0);
	if (!dmabufsrc2) {
		DMA_PRINTK ("dma_alloc_coherent failed for dmaphyssrc2\n");

		dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,
				dmaphyssrc1);
		dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest1,
				dmaphysdest1);
		return -ENOMEM;
	}

	dmabufdest2 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,
					&dmaphysdest2, 0);
	if (!dmabufdest2) {
		DMA_PRINTK ("dma_alloc_coherent failed for dmaphysdest2\n");

		dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,
				dmaphyssrc1);
		dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest1,
				dmaphysdest1);
		dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc2,
				dmaphyssrc2);
		return -ENOMEM;
	}

	for (iterations = 0 ; iterations < 10 ; iterations++) {
		DMA_PRINTK ("Iteration = %d\n", iterations);

		for (j = 0 ; j < numTCs ; j++) { //TC
			DMA_PRINTK ("TC = %d\n", j);

			for (i = 0 ; i < modes ; i++) {	//sync_mode
				DMA_PRINTK ("Mode = %d\n", i);

				/* Run all EDMA3 test cases */
				DMA_PRINTK ("Starting edma3_memtomemcpytest_dma\n");
				result = edma3_memtomemcpytest_dma(acnt, bcnt, ccnt, i, j);
				if (0 == result) {
					printk("edma3_memtomemcpytest_dma passed\n");
				} else {
					printk("edma3_memtomemcpytest_dma failed\n");
				}

				if (0 == result) {
					DMA_PRINTK ("Starting edma3_memtomemcpytest_dma_link\n");
					result = edma3_memtomemcpytest_dma_link(acnt, bcnt, ccnt, i, j);
					if (0 == result) {
						printk("edma3_memtomemcpytest_dma_link passed\n");
					} else {
						printk("edma3_memtomemcpytest_dma_link failed\n");
					}
				}
			}
		}
	}

	return result;
}


void edma_test_exit(void)
{
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1, dmaphyssrc1);
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest1,
			dmaphysdest1);

	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc2, dmaphyssrc2);
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest2,
			dmaphysdest2);

	printk ("\nExiting edma3_sample_app module\n");
}



/* DMA Channel, Mem-2-Mem Copy, ASYNC Mode, INCR Mode */

int edma3_memtomemcpytest_dma (int acnt, int bcnt, int ccnt, int sync_mode, int event_queue)
{
	int result = 0;
	unsigned int dma_ch = 0;
	int i;
	int count = 0;
	unsigned int Istestpassed = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;
	struct edmacc_param param_set;

	/* Initalize source and destination buffers */
	for (count = 0u; count < (acnt*bcnt*ccnt); count++) {
		dmabufsrc1[count] = 'A' + (count % 26);
		dmabufdest1[count] = 0;
	}

	/* Set B count reload as B count. */
	BRCnt = bcnt;

	/* Setting up the SRC/DES Index */
	srcbidx = acnt;
	desbidx = acnt;

	/* A Sync Transfer Mode */
	srccidx = acnt;
	descidx = acnt;

	result = edma_alloc_channel (0, EDMA_CHANNEL_ANY, callback1, NULL, event_queue);

	if (result < 0) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma::edma_alloc_channel failed for dma_ch, error:%d\n", result);
		return result;
	}

	dma_ch = result;
	edma_set_src (0, dma_ch, (unsigned long)(dmaphyssrc1), INCR, W8BIT);

	edma_set_dest (0, dma_ch, (unsigned long)(dmaphysdest1), INCR, W8BIT);

	edma_set_src_index (0, dma_ch, srcbidx, srccidx);

	edma_set_dest_index (0, dma_ch, desbidx, descidx);

	/* A Sync Transfer Mode */
	edma_set_transfer_params (0, dma_ch, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 1 */
	edma_read_slot (0, dma_ch, &param_set);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	edma_write_slot (0, dma_ch, &param_set);

	numenabled = bcnt * ccnt;

	for (i = 0; i < numenabled; i++) {
		irqraised1 = 0;

		/*
		 * Now enable the transfer as many times as calculated above.
		 */
		result = edma_start(0, dma_ch);
		if (result != 0) {
			DMA_PRINTK ("edma3_memtomemcpytest_dma: davinci_start_dma failed \n");
			break;
		}

		/* Wait for the Completion ISR. */
		while (irqraised1 == 0u);

		/* Check the status of the completed transfer */
		if (irqraised1 < 0) {
			/* Some error occured, break from the FOR loop. */
			DMA_PRINTK ("edma3_memtomemcpytest_dma: Event Miss Occured!!!\n");
			break;
		}
	}

	if (0 == result) {
		for (i = 0; i < (acnt*bcnt*ccnt); i++) {
			if (dmabufsrc1[i] != dmabufdest1[i]) {
				DMA_PRINTK ("\n edma3_memtomemcpytest_dma: Data write-read matching failed at = %u\n",i);
				Istestpassed = 0u;
				break;
			}
		}
		if (i == (acnt*bcnt*ccnt))
			Istestpassed = 1u;

		edma_stop(0, dma_ch);
		edma_free_channel(0, dma_ch);
	}

	if (Istestpassed == 1u) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma: EDMA Data Transfer Successfull \n");
	} else {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma: EDMA Data Transfer Failed \n");
	}

	return result;
}

/* 2 DMA Channels Linked, Mem-2-Mem Copy, ASYNC Mode, INCR Mode */

int edma3_memtomemcpytest_dma_link(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue)
{
	int result = 0;
	unsigned int dma_ch1 = 0;
	unsigned int dma_ch2 = 0;
	int i;
	int count = 0;
	unsigned int Istestpassed1 = 0u;
	unsigned int Istestpassed2 = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;
	struct edmacc_param param_set;

	/* Initalize source and destination buffers */
	for (count = 0u; count < (acnt*bcnt*ccnt); count++) {
		dmabufsrc1[count] = 'A' + (count % 26);
		dmabufdest1[count] = 0;

		dmabufsrc2[count] = 'A' + (count % 26);
		dmabufdest2[count] = 0;
	}

	/* Set B count reload as B count. */
	BRCnt = bcnt;

	/* Setting up the SRC/DES Index */
	srcbidx = acnt;
	desbidx = acnt;

	/* A Sync Transfer Mode */
	srccidx = acnt;
	descidx = acnt;

	result = edma_alloc_channel (0, EDMA_CHANNEL_ANY, callback1, NULL, event_queue);

	if (result < 0) {
		DMA_PRINTK ("edma3_memtomemcpytest_dma_link::edma_alloc_channel "
				"failed for dma_ch1, error:%d\n", result);
		return result;
	}

	dma_ch1 = result;
	edma_set_src (0, dma_ch1, (unsigned long)(dmaphyssrc1), INCR, W8BIT);

	edma_set_dest (0, dma_ch1, (unsigned long)(dmaphysdest1), INCR, W8BIT);

	edma_set_src_index (0, dma_ch1, srcbidx, srccidx);

	edma_set_dest_index (0, dma_ch1, desbidx, descidx);

	edma_set_transfer_params (0, dma_ch1, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 1 */
	edma_read_slot (0, dma_ch1, &param_set);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	edma_write_slot(0, dma_ch1, &param_set);

	/* Request a Link Channel */
	result = edma_alloc_slot (0, EDMA_SLOT_ANY);

	if (result < 0) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link::edma_alloc_slot "
				"failed for dma_ch2, error:%d\n", result);
		return result;
	}

	dma_ch2 = result;
	edma_set_src (0, dma_ch2, (unsigned long)(dmaphyssrc2), INCR, W8BIT);

	edma_set_dest (0, dma_ch2, (unsigned long)(dmaphysdest2), INCR, W8BIT);

	edma_set_src_index (0, dma_ch2, srcbidx, srccidx);

	edma_set_dest_index (0, dma_ch2, desbidx, descidx);

	edma_set_transfer_params (0, dma_ch2, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 2 */
	edma_read_slot (0, dma_ch2, &param_set);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	edma_write_slot(0, dma_ch2, &param_set);

	/* Link both the channels */
	edma_link(0, dma_ch1, dma_ch2);

	numenabled = bcnt * ccnt;

	for (i = 0; i < numenabled; i++) {
		irqraised1 = 0;

		/*
		 * Now enable the transfer as many times as calculated above.
		 */
		result = edma_start(0, dma_ch1);
		if (result != 0) {
			DMA_PRINTK ("edma3_memtomemcpytest_dma_link: davinci_start_dma failed \n");
			break;
		}

		/* Wait for the Completion ISR. */
		while (irqraised1 == 0u);

		/* Check the status of the completed transfer */
		if (irqraised1 < 0) {
			/* Some error occured, break from the FOR loop. */
			DMA_PRINTK ("edma3_memtomemcpytest_dma_link: "
					"Event Miss Occured!!!\n");
			break;
		}
	}

	if (result == 0) {
		for (i = 0; i < numenabled; i++) {
			irqraised1 = 0;

			/*
			 * Now enable the transfer as many times as calculated above
			 * on the LINK channel.
			 */
			result = edma_start(0, dma_ch1);
			if (result != 0) {
				DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link: davinci_start_dma failed \n");
				break;
			}

			/* Wait for the Completion ISR. */
			while (irqraised1 == 0u);

			/* Check the status of the completed transfer */
			if (irqraised1 < 0) {
				/* Some error occured, break from the FOR loop. */
				DMA_PRINTK ("edma3_memtomemcpytest_dma_link: "
						"Event Miss Occured!!!\n");
				break;
			}
		}
	}

	if (0 == result) {
		for (i = 0; i < (acnt*bcnt*ccnt); i++) {
			if (dmabufsrc1[i] != dmabufdest1[i]) {
				DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link(1): Data "
						"write-read matching failed at = %u\n",i);
				Istestpassed1 = 0u;
				break;
			}
		}
		if (i == (acnt*bcnt*ccnt)) {
			Istestpassed1 = 1u;
		}

		for (i = 0; i < (acnt*bcnt*ccnt); i++) {
			if (dmabufsrc2[i] != dmabufdest2[i]) {
				DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link(2): Data "
						"write-read matching failed at = %u\n",i);
				Istestpassed2 = 0u;
				break;
			}
		}
		if (i == (acnt*bcnt*ccnt)) {
			Istestpassed2 = 1u;
		}

		edma_stop(0, dma_ch1);
		edma_free_channel(0, dma_ch1);

		edma_stop(0, dma_ch2);
		edma_free_slot(0, dma_ch2);
		edma_free_channel(0, dma_ch2);
	}

	if ((Istestpassed1 == 1u) && (Istestpassed2 == 1u)) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link: EDMA Data Transfer Successfull\n");
	} else {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link: EDMA Data Transfer Failed\n");
	}

	return result;
}

module_init(edma_test_init);
module_exit(edma_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");

