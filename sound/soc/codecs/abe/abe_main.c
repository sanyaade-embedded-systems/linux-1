/*
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
 * @file  ABE_MAIN.C
 *
 * 'ABEMAIN.C' dummy main of the HAL
 *
 * @path 
 * @rev     01.00
 *! 
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */
 

#include "abe_main.h"

/*
 *  
 */

#if !PC_SIMULATION


void main (void)
{

        abe_dma_t dma_sink, dma_source;
        abe_data_format_t format;

        abe_reset_hal ();
        /*
            To be added here : 
            Device driver initialization:
                McPDM_DL : threshold=1, 6 slots activated
                DMIC : threshold=1, 6 microphones activated
                McPDM_UL : threshold=1, two microphones activated
        */

        format.f = 48000; format.samp_format = STEREO_MSB;
        abe_connect_cbpr_dmareq_port (MM_DL_PORT, &format, ABE_CBPR0_IDX,
								&dma_sink);
        /*
            Here : connect a DMA channel to MM_DL port 
        */

        format.f = 8000; format.samp_format = MONO_MSB;
        abe_connect_cbpr_dmareq_port (VX_DL_PORT, &format, ABE_CBPR0_IDX,
								&dma_sink);
        /*
            Here : connect a DMA channel to VX_DL port 
        */

        format.f = 8000; format.samp_format = MONO_MSB;
        abe_connect_cbpr_dmareq_port (VX_UL_PORT, &format, ABE_CBPR0_IDX,
								&dma_source);
        /*
            Here : connect a DMA channel to VX_UL port 
        */

        abe_enable_data_transfer (MM_DL_PORT );
        abe_enable_data_transfer (VX_DL_PORT );
        abe_enable_data_transfer (VX_UL_PORT );
        abe_enable_data_transfer (PDM_UL_PORT);
        abe_enable_data_transfer (PDM_DL1_PORT);

}

#endif
