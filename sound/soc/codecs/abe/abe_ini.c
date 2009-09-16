/*
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
 * @file  ABE_INI.C
 *
 * All the visible API for the audio drivers
 *
 * @path 
 * @rev     01.00
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */

 
 
#include "abe_main.h"

#include "abe_dat.h"    /* data declaration */


/*
 *  initialize the default values for call-backs to subroutines
 *      - FIFO IRQ call-backs for sequenced tasks
 *      - FIFO IRQ call-backs for audio player/recorders (ping-pong protocols)
 *      - Remote debugger interface
 *      - Error monitoring
 *      - Activity Tracing 
 */
 


/*
 *  ABE_HW_CONFIGURATION
 *
 *  Parameter  : 
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void    abe_hw_configuration (void)
{
	    abe_uint32 event;
	    abe_uint32 atc_reg;

        /* enables the DMAreq from AESS  AESS_DMAENABLE_SET = 255 */
        atc_reg = 0xFF;
        abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x60, &atc_reg, 4);


        /* enables EVENT_GENERATOR_START=6C from McPDM */
        atc_reg = 0x01;
        abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x6C, &atc_reg, 4);


        /* set McPDM_DL as EVENT_SOURCE_SELECTION */
        event = 0L;
        abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x70, &event, 4);

        event = 2L;
        abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x74, &event, 4);


}
 


/*
 *  ABE_BUILD_SCHEDULER_TABLE
 *
 *  Parameter  : 
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void abe_build_scheduler_table ()
{
	short VirtAudio_aMultiFrame[PROCESSING_SLOTS][TASKS_IN_SLOT];
	abe_uint16 i, n;
	abe_uint8 *ptr; 
	char  aUplinkMuxing[16];

	for (ptr = (abe_uint8 *)&(VirtAudio_aMultiFrame[0][0]),
			i=0; i < sizeof (VirtAudio_aMultiFrame); i++)
		*ptr++ = 0;

	/* Scheduling table for Voice UL/DL 8kHz */
	VirtAudio_aMultiFrame[0][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_AMIC;
	VirtAudio_aMultiFrame[0][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_VX_DL;

	VirtAudio_aMultiFrame[1][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ASRC_VX_DL_8;
	VirtAudio_aMultiFrame[1][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_DL_8_48_BP;
	VirtAudio_aMultiFrame[1][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_DL_8_48_0SR;
	VirtAudio_aMultiFrame[1][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_MM_DL;

	VirtAudio_aMultiFrame[2][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_DL_8_48_LP;
	VirtAudio_aMultiFrame[2][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ASRC_MM_DL;
	VirtAudio_aMultiFrame[2][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1Mixer;
	VirtAudio_aMultiFrame[2][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2Mixer;

	VirtAudio_aMultiFrame[3][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EchoMixer;
	VirtAudio_aMultiFrame[3][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_SideTone;
	VirtAudio_aMultiFrame[3][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_SDTMixer;
	VirtAudio_aMultiFrame[3][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_EQ;

	VirtAudio_aMultiFrame[4][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_EQ;
	VirtAudio_aMultiFrame[4][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_APS_EQ;
	VirtAudio_aMultiFrame[4][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_GAIN;

	VirtAudio_aMultiFrame[5][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VXRECMixer;
	VirtAudio_aMultiFrame[5][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VXREC_SPLIT;

	VirtAudio_aMultiFrame[8][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_APS_EQ;
	VirtAudio_aMultiFrame[8][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_GAIN;

	VirtAudio_aMultiFrame[9][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_LP0;
	VirtAudio_aMultiFrame[9][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_0SR;
	VirtAudio_aMultiFrame[9][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_LP1;
	VirtAudio_aMultiFrame[9][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_LP1;

	VirtAudio_aMultiFrame[10][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_LP0;
	VirtAudio_aMultiFrame[10][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_0SR;
	VirtAudio_aMultiFrame[10][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_LP1;
	VirtAudio_aMultiFrame[10][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_LP1;

	VirtAudio_aMultiFrame[11][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_McPDM_DL;

	VirtAudio_aMultiFrame[12][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_96_48_IIR;
	VirtAudio_aMultiFrame[12][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_96_48_DEC;
	VirtAudio_aMultiFrame[12][2] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_EQ;
	VirtAudio_aMultiFrame[12][3] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_SPLIT;

	VirtAudio_aMultiFrame[14][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_APS_IIR;
	VirtAudio_aMultiFrame[14][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_APS_CORE;

	VirtAudio_aMultiFrame[15][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_L_APS_CORE;
	VirtAudio_aMultiFrame[15][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_APS_IIR;
	VirtAudio_aMultiFrame[15][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_APS_SPLIT;
	VirtAudio_aMultiFrame[15][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_R_APS_CORE;

	VirtAudio_aMultiFrame[21][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_ROUTING;
	VirtAudio_aMultiFrame[21][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ULMixer;

	VirtAudio_aMultiFrame[22][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_48_8_LP;
	VirtAudio_aMultiFrame[22][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_48_8_DEC1;
	VirtAudio_aMultiFrame[22][2] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_48_8_BP;
	VirtAudio_aMultiFrame[22][3] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ASRC_VX_UL_8;

	VirtAudio_aMultiFrame[23][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_McPDM_DL;
	VirtAudio_aMultiFrame[23][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_VX_UL;
	VirtAudio_aMultiFrame[23][2] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_GAIN_UPDATE;

	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
			D_multiFrame_ADDR, (abe_uint32*)VirtAudio_aMultiFrame,
			sizeof (VirtAudio_aMultiFrame)); 

	n = D_aUplinkRouting_ADDR_END - D_aUplinkRouting_ADDR + 1;

	for(i = 0; i < n; i++)
		aUplinkMuxing[i] = ZERO_labelID;

	/* VX- UL routing table */
	aUplinkMuxing[12] = AMIC_L_labelID;
	aUplinkMuxing[13] = AMIC_R_labelID;

	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM, D_aUplinkRouting_ADDR,
			(abe_uint32 *)aUplinkMuxing, sizeof (aUplinkMuxing)); 
}
 
/*
 *  ABE_INIT_ATC
 *
 *  Parameter  : 
 *      prot : protocol being used 
 *  Operations : 
 *      load the DMEM ATC/AESS descriptors
 *  Return value : 
 */
void  abe_init_atc (abe_port_id id)
{
	abe_satcdescriptor_aess desc;
	abe_uint8 thr, thr1, thr2, iter;
	abe_int32 iterfactor;

	
	/* load default values of the descriptor */
	desc.rdpt = desc.wrpt = desc.irqdest = desc.cberr = desc.desen =0;
	desc.reserved0 = desc.reserved1 = desc.reserved2 = 0;
	desc.srcid = desc.destid = desc.badd = desc.iter = desc.cbsize = 0;

	switch ((abe_port[id]).protocol.protocol_switch) {
	case SLIMBUS_PORT_PROT :
		desc.cbdir = (abe_port[id]).protocol.direction;	
		desc.cbsize = (abe_port[id]).protocol.p.prot_slimbus.buf_size;	
		desc.badd =
			((abe_port[id]).protocol.p.prot_slimbus.buf_addr1) >> 4;
		desc.iter = (abe_port[id]).protocol.p.prot_slimbus.iter;
		desc.srcid =
		abe_atc_srcid [(abe_port[id]).protocol.p.prot_slimbus.desc_addr1 >> 3];
		desc.nw = 1;

		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
			(abe_port[id]).protocol.p.prot_slimbus.desc_addr1,
					(abe_uint32*)&desc, sizeof (desc)); 

		desc.badd = (abe_port[id]).protocol.p.prot_slimbus.buf_addr2;		    
		desc.srcid = 
		abe_atc_srcid [(abe_port[id]).protocol.p.prot_slimbus.desc_addr2 >> 3];

		abe_block_copy(COPY_FROM_HOST_TO_ABE, ABE_DMEM,
			(abe_port[id]).protocol.p.prot_slimbus.desc_addr2,
					(abe_uint32*)&desc, sizeof (desc)); 
		break;

	case SERIAL_PORT_PROT  :

		desc.cbdir = (abe_port[id]).protocol.direction;	
		desc.cbsize = (abe_port[id]).protocol.p.prot_serial.buf_size;
		desc.badd = ((abe_port[id]).protocol.p.prot_serial.buf_addr) >> 4;
		desc.iter = (abe_port[id]).protocol.p.prot_serial.iter;
		desc.srcid =
		abe_atc_srcid [(abe_port[id]).protocol.p.prot_serial.desc_addr >> 3];
		desc.nw = 1;

		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
			(abe_port[id]).protocol.p.prot_serial.desc_addr,
					(abe_uint32*)&desc, sizeof (desc)); 
		break;

	case DMIC_PORT_PROT    :

		desc.cbdir = ABE_ATC_DIRECTION_IN;	
		desc.cbsize = (abe_port[id]).protocol.p.prot_dmic.buf_size;
		desc.badd = ((abe_port[id]).protocol.p.prot_dmic.buf_addr) >> 4;
		desc.iter = DMIC_ITER;
		desc.srcid = abe_atc_srcid [ABE_ATC_DMIC_DMA_REQ];
		desc.nw = 1;

		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
			ABE_ATC_DMIC_DMA_REQ * ATC_SIZE, 
			(abe_uint32*)&desc, sizeof (desc)); 
		break;

	case MCPDMDL_PORT_PROT :

		abe_global_mcpdm_control = abe_port[id].protocol.p.prot_mcpdmdl.control;
		desc.cbdir = ABE_ATC_DIRECTION_OUT;
		desc.cbsize = (abe_port[id]).protocol.p.prot_mcpdmdl.buf_size;
		desc.badd = ((abe_port[id]).protocol.p.prot_mcpdmdl.buf_addr) >> 4;
		desc.iter = MCPDM_DL_ITER;
		desc.destid = abe_atc_dstid [ABE_ATC_MCPDMDL_DMA_REQ];
		desc.nw = 0;
		desc.wrpt = desc.iter;
		desc.desen = 1;

		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
			ABE_ATC_MCPDMDL_DMA_REQ * ATC_SIZE,
			(abe_uint32*)&desc, sizeof (desc)); 
		break;

	case MCPDMUL_PORT_PROT :

		desc.cbdir = ABE_ATC_DIRECTION_IN;
		desc.cbsize = (abe_port[id]).protocol.p.prot_mcpdmul.buf_size;
		desc.badd = ((abe_port[id]).protocol.p.prot_mcpdmul.buf_addr) >> 4;
		desc.iter = MCPDM_UL_ITER;
		desc.srcid = abe_atc_srcid [ABE_ATC_MCPDMUL_DMA_REQ];
		desc.wrpt = MCPDM_UL_ITER;
		desc.nw = 1;
		desc.desen = 1;

		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
				ABE_ATC_MCPDMUL_DMA_REQ * ATC_SIZE,
				(abe_uint32*)&desc, sizeof (desc)); 
		break;

	case PINGPONG_PORT_PROT:
		break;

	case DMAREQ_PORT_PROT  :

		desc.cbdir = (abe_port[id]).protocol.direction;	
		desc.cbsize = (abe_port[id]).protocol.p.prot_dmareq.buf_size;
		desc.badd = ((abe_port[id]).protocol.p.prot_dmareq.buf_addr) >> 4;
		desc.iter = 1;
		desc.nw = 0;


		thr = (abe_int8) abe_port[id].protocol.p.prot_dmareq.thr_flow;
		iterfactor = abe_dma_port_iter_factor (&((abe_port[id]).format));
		thr = (abe_uint8) (thr  * iterfactor);
		iter =
		(abe_uint8) abe_dma_port_iteration (&((abe_port[id]).format));

		if (abe_port[id].protocol.direction == ABE_ATC_DIRECTION_IN) {
			thr2  = (abe_uint8)(iter - thr);
			thr1  = (abe_uint8)(iter + thr);
		} else {
			thr2  = 0;
			thr1  = (abe_uint8)(thr * 2);
		}

		desc.wrpt =  (thr1 + thr2) >> 1;

		if (desc.cbdir == ABE_ATC_DIRECTION_OUT)
			desc.destid =
				abe_atc_dstid [(abe_port[id]).protocol.p.prot_dmareq.desc_addr >> 3];
		else
			desc.srcid =
				abe_atc_srcid [(abe_port[id]).protocol.p.prot_dmareq.desc_addr >> 3];

		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
				(abe_port[id]).protocol.p.prot_dmareq.desc_addr,
				(abe_uint32*)&desc, sizeof (desc));
		break;
	default:
	break;
	}
}

/*
 *  ABE_INIT_DMA_T
 *
 *  Parameter  : 
 *      prot : protocol being used 
 *  Operations : 
 *      load the dma_t with physical information from AE memory mapping
 *  Return value : 
 */
void abe_init_dma_t (abe_port_id id, abe_data_format_t *format,
						    abe_port_protocol_t *prot)
{
	abe_dma_t_offset dma;

	dma.data = 0;
	dma.iter = 0;

	switch (prot->protocol_switch) {
	case PINGPONG_PORT_PROT:
		dma.data = (prot->p).prot_pingpong.buf_addr;
		dma.iter = abe_dma_port_iteration (format);
		break;

	case DMAREQ_PORT_PROT :
		dma.data = (prot->p).prot_dmareq.buf_addr;
		dma.iter = (prot->p).prot_dmareq.iter;
		break;
 
	case CIRCULAR_PORT_PROT:
		dma.data = (prot->p).prot_dmareq.buf_addr; 
		dma.iter = (prot->p).prot_dmareq.iter;
		break;

	case SLIMBUS_PORT_PROT:
	case SERIAL_PORT_PROT:
	case DMIC_PORT_PROT:
	case MCPDMDL_PORT_PROT:
	case MCPDMUL_PORT_PROT:
	default:
		break;
	}

	abe_port [id].dma = dma;
}

/*
 *   ABE_INIT_IO_TASKS
 *
 *  Parameter  : 
 *      prot : protocol being used 
 *
 *  Operations : 
 * load the micro-task parameters doing to DMEM <==> SMEM data moves
 * I/O descriptors input parameters :
 * 	For Read from DMEM usually THR1/THR2 = X+1/X-1  
 *      For Write to DMEM usually  THR1/THR2 = 2/0      
 *      UP_1/2 =X+1/X-1
 *
 *  Return value : 
 *      
 */
void abe_init_io_tasks (abe_port_id id, abe_data_format_t *format,
						    abe_port_protocol_t *prot)
{
	ABE_SIODescriptor desc;
	abe_uint8 thr, thr1, thr2, iter, direction, data_shift,
							iter_samples, up1, up2;
	abe_int32 iterfactor, dmareq_addr, dmareq_field;
	abe_int32 atc_desc_address, sio_desc_address, sio_id;


	dmareq_addr = ABE_DUMMY_ADDRESS; 
	dmareq_field = 0;
	thr = DEFAULT_THR_READ;
	atc_desc_address = 0;

	switch (prot->protocol_switch) {
	case DMIC_PORT_PROT:
		atc_desc_address = ABE_ATC_DMIC_DMA_REQ;
		break;

	case MCPDMDL_PORT_PROT :
		atc_desc_address = ABE_ATC_MCPDMDL_DMA_REQ;
		break;

	case MCPDMUL_PORT_PROT :
		atc_desc_address = ABE_ATC_MCPDMUL_DMA_REQ;
		break;

	case PINGPONG_PORT_PROT:
	break;

	case SLIMBUS_PORT_PROT :
		thr = (abe_int8) abe_port[id].protocol.p.prot_dmareq.thr_flow;
		atc_desc_address = abe_port[id].protocol.p.prot_dmareq.desc_addr;
		break;

	case SERIAL_PORT_PROT:	/* McBSP/McASP */
		thr = (abe_int8) abe_port[id].protocol.p.prot_serial.thr_flow;
		atc_desc_address =
			(abe_int16) abe_port[id].protocol.p.prot_serial.desc_addr;
		break;

	case DMAREQ_PORT_PROT  :/* DMA w/wo CBPr */
		thr = (abe_int8) abe_port[id].protocol.p.prot_dmareq.thr_flow;
		dmareq_addr  = abe_port[id].protocol.p.prot_dmareq.dma_addr;
		dmareq_field = abe_port[id].protocol.p.prot_dmareq.dma_data;
		atc_desc_address = abe_port[id].protocol.p.prot_dmareq.desc_addr;
		break;
	default:
	break;
}

	iterfactor = abe_dma_port_iter_factor (format);
	data_shift = (abe_uint8)((iterfactor > 1)? 1:0);

	iter = (abe_uint8) abe_dma_port_iteration (format);
	iter_samples = (abe_uint8) (iter / iterfactor);

	if (abe_port[id].protocol.direction == ABE_ATC_DIRECTION_IN) {
		thr2 = (abe_uint8)(iter - thr);
		thr1 = (abe_uint8)(iter + thr);
		direction = 0; 
	} else {
		thr2  = 0;
		thr1  = (abe_uint8)(thr * 2);
		direction = 3;
	}

	up1 = (abe_uint8) (iter + thr);
	up2 = (abe_uint8) (MAXIMUM(0, (abe_int8)iter - (abe_int8)thr));

	desc.drift_ASRC = 0;
	desc.drift_io = 0;
	desc.hw_ctrl_addr = (abe_uint8) dmareq_addr;
	desc.copy_func_index =
		(abe_uint8) abe_dma_port_copy_subroutine_id(format,
					abe_port[id].protocol.direction);
	desc.x_io = iter_samples;
	desc.threshold_1 = thr1;
	desc.threshold_2 = thr2;
	desc.update_1 = up1;
	desc.update_2 = up2;
	desc.data_size = data_shift;
	desc.smem_addr = (abe_uint8) abe_port[id].smem_buffer1;
	desc.flow_counter = 0;
	desc.atc_irq_data = (abe_uint8) dmareq_field;
	desc.atc_address = (abe_uint8) (atc_desc_address >> 2);
	desc.direction_rw = direction;
	desc.padding16[0] = 0;
	desc.padding16[1] = 0;

	sio_id = sio_task_index [id];
	sio_desc_address = dmem_port_descriptors +
				(sio_id * sizeof(ABE_SIODescriptor));
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM, sio_desc_address,
					(abe_uint32*)&desc, sizeof (desc)); 
}

/*
 *  ABE_INIT_DMIC
 *
 *  Parameter  : 
 *      x : d
 *  Operations : 
 *  Return value : 
 *
 */
void    abe_init_dmic (abe_uint32 x)
{
}

/*
 *  ABE_INIT_MCPDM
 *
 *  Parameter  : 
 * x : d
 *  Operations : 
 *  Return value : 
 */
void    abe_init_mcpdm (abe_uint32 x)
{
}


/*
 *  ABE_RESET_FEATURE
 *
 *  Parameter  : 
 *      x : index of the feature to be initialized
 *  Operations : 
 *      reload the configuration 
 *  Return value : 
 */
void abe_reset_one_feature (abe_uint32 x)
{
	all_feature [x] = all_feature_init [x];
	abe_call_subroutine ((all_feature[x]).disable_feature, NOPARAMETER,
								NOPARAMETER);
}

/*
 *  ABE_RESET_ALL_FEATURE
 *
 *  Parameter  : 
 *      none
 *
 * Operations : 
 * load default configuration for all features
 * struct {
 * uint16 load_default_data;
 * uint16 read_parameter;
 * uint16 write_parameter;
 * uint16 running_status;
 * uint16 fw_input_buffer_address;
 * uint16 fw_output_buffer_address;
 * uint16 fw_scheduler_slot_position;
 * uint16 fw_scheduler_subslot_position;
 * uint16 min_opp;
 * char name[NBCHARFEATURENAME];
 * } abe_feature_t;
 *
 *  Return value : 
 *      
 */
void abe_reset_all_features (void)
{
	abe_uint16 i;
	for (i = 0; i < MAXNBFEATURE; i++)
		abe_reset_one_feature (i);
}


/*  ABE_RESET_ONE_PORT
 *
 *  Parameter  : 
 *      none
 *  Operations : 
 *      load default configuration for one port
 *  Return value : 
 *      
 */
void abe_reset_one_port (abe_uint32 x)
{
	(abe_port[x]) = (abe_port_init[x]);
}

/*
 *  ABE_RESET_ALL_PORTS
 *
 *  Parameter  : 
 *      none
 *  Operations : 
 *      load default configuration for all features
 *  Return value : 
 *      
 */
void abe_reset_all_ports (void)
{
	abe_uint16 i;

	for (i = 0; i < MAXNBABEPORTS; i++)
		abe_reset_one_port (i);
}

/*
 *  ABE_RESET_ALL_FIFO
 *
 *  Parameter  : 
 *      none
 *  Operations : 
 *      load default configuration for the buffer addresses 
 *  Return value : 
 *      
 */
void abe_reset_all_fifo (void)
{

}


