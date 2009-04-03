/*
 * ALSA PCM interface for the TI DAVINCI processor
 *
 * Author:      Vladimir Barinov, <vbarinov@embeddedalley.com>
 * Copyright:   (C) 2007 MontaVista Software, Inc., <source@mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _DAVINCI_PCM_H
#define _DAVINCI_PCM_H

#include <mach/edma.h>

struct davinci_pcm_dma_params {
	u8 cc_inst;
	char *name;			/* stream identifier */
	int channel;			/* sync dma channel ID */
	dma_addr_t dma_addr;		/* device physical address for DMA */
	unsigned int data_type;		/* xfer data type */
	unsigned int acnt;
	enum dma_event_q eventq_no;	/* event queue number */
};

struct evm_snd_platform_data {
	char *clk_name;
	int tx_dma_ch;
	int rx_dma_ch;
	u8 cc_inst;
	u32 tx_dma_offset;
	u32 rx_dma_offset;
	enum dma_event_q eventq_no;	/* event queue number */
	unsigned int codec_fmt;

	/* McASP specific fields */
	int tdm_slots;
	u8 op_mode;
	u8 num_serializer;
	u8 *serial_dir;

	u8 version;

	/* McASP FIFO related - 0 to disable FIFO*/
	u8 txnumevt;
	u8 rxnumevt;
};

extern struct snd_soc_platform davinci_soc_platform;

#endif
