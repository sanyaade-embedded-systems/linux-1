/*
 * Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 * Use of this software is controlled by the terms and conditions found 
 * in the license agreement under which this software has been supplied.
 * @file  ABE_API.C
 *
 * All the visible API for the audio drivers
 * @path 
 * @rev     01.00
*! 
*! Revision History
*! 27-Nov-2008 Original
*! 05-Jun-2009 V05 release
*/


#include "abe_main.h"

static abe_uint32 ABE_FW_PM [ABE_PMEM_SIZE / 4]  = {
#include "C_ABE_FW.PM"
};
static abe_uint32 ABE_FW_CM[ABE_CMEM_SIZE / 4]  = {
#include "C_ABE_FW.CM"
};
static abe_uint32 ABE_FW_DM[ABE_DMEM_SIZE / 4]  = {
#include "C_ABE_FW.lDM"
};
static abe_uint32 ABE_FW_SM[ABE_SMEM_SIZE / 4]  = {
#include "C_ABE_FW.SM32"
};

/*
 * @fn abe_reset_hal() 
 * Operations : reset the HAL by reloading the static variables and default AESS
 * registers. 
 * Called after a PRCM cold-start reset of ABE
 * @see BE_API.h
*/

void abe_reset_hal (void)
{
	abe_dbg_output = TERMINAL_OUTPUT;
/* reset features */
	abe_reset_all_features ();

/* load default port values */
	abe_reset_all_ports ();

/* load default sequence list */
	abe_reset_all_sequence ();

/* build the scheduler tables */
	abe_build_scheduler_table ();

/* init the buffer addresses */
	abe_reset_all_fifo ();

/* load firmware */
	abe_load_fw ();

/* init hardware components*/
	abe_hw_configuration ();

}


/*
 * @fn abe_load_fwl() 
 *
 *  Operations : 
 *     loads the Audio Engine firmware, generate a single pulse on the Event 
 *     generator to let execution start, read the version number returned from 
 *     this execution.
 *
 * @see        ABE_API.h
 */

void abe_load_fw (void)
{
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_PMEM, 0,
				(abe_uint32 *)ABE_FW_PM,  sizeof (ABE_FW_PM));
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_CMEM, 0,
				(abe_uint32 *)ABE_FW_CM,  sizeof (ABE_FW_CM));
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_SMEM, 0,
				(abe_uint32 *)ABE_FW_SM,  sizeof (ABE_FW_SM));
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM, 0,
				(abe_uint32 *)ABE_FW_DM,  sizeof (ABE_FW_DM));

}

/*
 *  ABE_DEFAULT_CONFIGURATION
 *
 * Parameter  : 
 * use-case-ID : "LP player", "voice-call" use-cases as defined in the paragraph 
 * "programming use-case sequences"
 * Operations : 
 * private API used during development. Loads all the necessary parameters and
 * data patterns to allow a stand-alone functional test without the need of. 
 * Return value : 
 * None.
 */

void abe_default_configuration (abe_uint32 use_case)
{
	short D_multiFrame[PROCESSING_SLOTS][TASKS_IN_SLOT];
	abe_uint32 event;
	abe_satcdescriptor_aess desc;
	abe_uint32 atc_reg;
	abe_uint16 i;
	abe_uint8 *ptr; 
	abe_port_protocol_t *protocol;
	abe_data_format_t *format;

	#define SIZEOFIODESC 20

	/* clear local tables */
	for (ptr = (abe_uint8 *)&(D_multiFrame[0][0]), i=0;
		i < sizeof (D_multiFrame); i++)
	*ptr++ = 0;
	for (ptr = (abe_uint8 *)&desc, i=0; i < sizeof (desc); i++)  *ptr++ = 0;

	switch (use_case) {
		/* enables voice ul/dl on earpiece + MM_DL on IHF */
	case UC2_VOICE_CALL_AND_IHF_MMDL :
		abe_reset_port(PDM_UL_PORT);
		abe_reset_port(PDM_DL1_PORT);
		abe_reset_port(VX_DL_PORT);
		abe_reset_port(VX_UL_PORT);
		abe_reset_port(MM_DL_PORT);
		/* enables the DMAreq from AESS  AESS_DMAENABLE_SET = 255 */
		atc_reg = 0xFF;
		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x60,
								&atc_reg, 4);

		/* enables EVENT_GENERATOR_START=6C from McPDM */
		atc_reg = 0x01;
		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x6C,
								&atc_reg, 4);

		/* set McPDM_DL as EVENT_SOURCE_SELECTION */
		event = 0L;	/* source = DMAreq */
		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x70,
								&event, 4);

		event = 2L;	/* source = MCPDM_DL to AUDIO_ENGINE_SCHED */
		abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, 0x74,
								&event, 4);
		for (ptr = (abe_uint8 *)&(D_multiFrame[0][0]),
			i=0; i < sizeof (D_multiFrame); i++) *ptr++ = 0;
		
		/* Scheduling table for Voice UL/DL 8kHz + MM DL 48 kHz */
			D_multiFrame[0][0]  = D_tasksList_ADDR +
					sizeof(ABE_STask)*C_ABE_FW_TASK_IO_AMIC;
			D_multiFrame[0][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_VX_DL;

			D_multiFrame[1][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ASRC_VX_DL_8;
			D_multiFrame[1][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_DL_8_48_BP;
			D_multiFrame[1][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_DL_8_48_0SR;
			D_multiFrame[1][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_MM_DL;

			D_multiFrame[2][0]  = D_tasksList_ADDR +
				 sizeof(ABE_STask)*C_ABE_FW_TASK_VX_DL_8_48_LP;
			D_multiFrame[2][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ASRC_MM_DL;
			D_multiFrame[2][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1Mixer;
			D_multiFrame[2][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2Mixer;

			D_multiFrame[3][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EchoMixer;
			D_multiFrame[3][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_SideTone;
			D_multiFrame[3][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_SDTMixer;
			D_multiFrame[3][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_EQ;

			D_multiFrame[4][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_EQ;
			D_multiFrame[4][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_APS_EQ;
			D_multiFrame[4][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_GAIN;

			D_multiFrame[5][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VXRECMixer;
			D_multiFrame[5][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VXREC_SPLIT;

			D_multiFrame[8][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_APS_EQ;
			D_multiFrame[8][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_GAIN;

			D_multiFrame[9][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_LP0;
			D_multiFrame[9][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_0SR;
			D_multiFrame[9][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_LP1;
			D_multiFrame[9][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_EARP_48_96_LP1;

			D_multiFrame[10][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_LP0;
			D_multiFrame[10][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_0SR;
			D_multiFrame[10][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_LP1;
			D_multiFrame[10][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IHF_48_96_LP1;

			D_multiFrame[11][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_McPDM_DL;

			D_multiFrame[12][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_96_48_IIR;
			D_multiFrame[12][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_96_48_DEC;
			D_multiFrame[12][2] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_EQ;
			D_multiFrame[12][3] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_AMIC_SPLIT;

			D_multiFrame[14][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_APS_IIR;
			D_multiFrame[14][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL1_APS_CORE;

			D_multiFrame[15][0]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_L_APS_CORE;
			D_multiFrame[15][1]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_APS_IIR;
			D_multiFrame[15][2]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_APS_SPLIT;
			D_multiFrame[15][3]  = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_DL2_R_APS_CORE;

			D_multiFrame[21][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_ROUTING;
			D_multiFrame[21][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ULMixer;

			D_multiFrame[22][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_48_8_LP;
			D_multiFrame[22][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_48_8_DEC1;
			D_multiFrame[22][2] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_VX_UL_48_8_BP;
			D_multiFrame[22][3] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_ASRC_VX_UL_8;

			D_multiFrame[23][0] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_McPDM_DL;
			D_multiFrame[23][1] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_IO_VX_UL;
			D_multiFrame[23][2] = D_tasksList_ADDR +
				sizeof(ABE_STask)*C_ABE_FW_TASK_GAIN_UPDATE;

			abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM,
				D_multiFrame_ADDR, (abe_uint32*)D_multiFrame,
				sizeof (D_multiFrame)); 

			/* initializes the ATC descriptors in DMEM */
			protocol = &(abe_port[PDM_UL_PORT].protocol);
			format = &(abe_port[PDM_UL_PORT].format);
			abe_init_atc (PDM_UL_PORT);
			abe_init_io_tasks (PDM_UL_PORT, format, protocol);

			protocol = &(abe_port[PDM_DL1_PORT].protocol);
			format = &(abe_port[PDM_DL1_PORT].format);
			abe_init_atc (PDM_DL1_PORT);
			abe_init_io_tasks (PDM_DL1_PORT, format, protocol);

			protocol = &(abe_port[VX_DL_PORT].protocol);
			format = &(abe_port[VX_DL_PORT].format); 
			abe_init_atc (VX_DL_PORT);
			abe_init_io_tasks (VX_DL_PORT, format, protocol);

			protocol = &(abe_port[VX_UL_PORT].protocol);
			format = &(abe_port[VX_UL_PORT].format);
			abe_init_atc (VX_UL_PORT);
			abe_init_io_tasks (VX_UL_PORT, format, protocol);

			protocol = &(abe_port[MM_DL_PORT].protocol);
			format = &(abe_port[MM_DL_PORT].format);
			abe_init_atc (MM_DL_PORT);
			abe_init_io_tasks (MM_DL_PORT, format, protocol);

			/* Voice and multimedia audio router init */
			abe_block_copy (COPY_FROM_HOST_TO_ABE,
				ABE_DMEM, D_aUplinkRouting_ADDR + 12,
				(abe_uint32 *)abe_router_ul_table_init_1,
				sizeof (abe_router_ul_table_init_1));

	case UC4_EANC :
	default :
	break;
	}
}

/*
 *	ABE_IRQ_PROCESSING
 *
 *	Parameter  : 
 *		No parameter
 *
 *	Operations : 
 *		This subroutine will check the IRQ_FIFO from the AE and act 
 *		accordingly. 
 *		Some IRQ source are originated for the delivery of "end of time
 *		sequenced tasks" 
 *		notifications, some are originated from the Ping-Pong protocols,
 *		some are generated from  the embedded debugger when the firmware
 *		stops on programmable break-points, etc.
 *
 *	Return value : 
 *		None.
 */

void abe_irq_processing (void)
{
	abe_monitoring ();
}

/*
 *  ABE_EVENT_GENERATOR_SWITCH
 *
 *  Parameter  : 
 *      e: Event Generation Counter, McPDM, DMIC or default.
 *      p: pointer to the main port
 *
 *  Operations : 
 *      load the AESS event generator hardware source. Loads the firmware
 *      parameters accordingly. Indicates to the FW which data stream is the
 *      most important to preserve in case all the streams are asynchronous.
 *      If the parameter is "default", let the HAL decide which Event source is
 *      the best appropriate based on the opened ports.
 *
 *      When neither the DMIC and the McPDM are activated the AE will have its
 *      EVENT generator programmed with the EVENT_COUNTER. The event counter
 *      will be tuned in order to deliver a pulse frequency higher 
 *      than 96 kHz. The DPLL output at 50% OPP is (32768kHz x3000) = 98304kHz,
 *      then the closest ratio to 
 *      96kHz is 1023. The counter will be loaded with 2x1023=2046.
 *
 *  Return value : 
 *      None.
 */

void abe_event_generator_switch (abe_event_id e, abe_port_id *p)
{
	abe_uint32 event, selection, counter, start;
	counter = EVENT_GENERATOR_COUNTER_DEFAULT;

	switch (e) {
	case EVENT_MCPDM  : 
		selection = EVENT_SOURCE_DMA;
		start = EVENT_GENERATOR_ON; event = ABE_ATC_MCPDMDL_DMA_REQ; 
		break;
	case EVENT_DMIC   : 
		selection = EVENT_SOURCE_DMA;
		start = EVENT_GENERATOR_ON; event = ABE_ATC_DMIC_DMA_REQ;
		break;
	case EVENT_TIMER  :
		selection = EVENT_SOURCE_COUNTER;
		start = EVENT_GENERATOR_ON; event = 0;
		break;
	case EVENT_McBSP  :
		selection = EVENT_SOURCE_COUNTER;
		start = EVENT_GENERATOR_ON; event = 0;
		break;
	case EVENT_McASP  :
		selection = EVENT_SOURCE_COUNTER;
		start = EVENT_GENERATOR_ON; event = 0;
		break;
	case EVENT_SLIMBUS:
		selection = EVENT_SOURCE_COUNTER;
		start = EVENT_GENERATOR_ON; event = 0;
		break;
	case EVENT_DEFAULT:
		selection = EVENT_SOURCE_COUNTER;
		start = EVENT_GENERATOR_ON;
		event = 0;
		break;
	default:
		abe_dbg_param |= ERR_API;
		abe_dbg_error_log (ABE_BLOCK_COPY_ERR);
	}

	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, EVENT_GENERATOR_COUNTER,
								&counter, 4);
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, EVENT_SOURCE_SELECTION,
								&selection, 4);
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, EVENT_GENERATOR_START,
								&start, 4);
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_ATC, AUDIO_ENGINE_SCHEDULER,
								&event, 4);

	/* @@@ Tell the scheduler for Slot #24 in case of McBSP/McASP !!! */
}

/*
 *  ABE_READ_LOWEST_OPP
 *
 *  Parameter  : 
 *      Data pointer : returned data
 *
 *  Operations : 
 *      Returns the lowest possible OPP based on the current active ports
 *
 *  Return value : 
 *      None.
 */

void abe_read_lowest_opp (abe_opp_t *o)
{
	*o = OPP100;            /* @@@ RELEASE 5 allows only 100% OPP */
}

/*
 *  ABE_SET_OPP_PROCESSING
 *
 *  Parameter  : 
 *      New processing network and OPP: 
 *          0: Ultra Lowest power consumption audio player (no post-processing,
 *            no mixer)
 *          1: OPP 25% (simple multimedia features, including low-power player)
 *          2: OPP 50% (multimedia and voice calls)
 *          3: OPP100% (EANC, multimedia complex use-cases)      
 *
 *  Operations : 
 *      Rearranges the FW task network to the corresponding OPP list of features. 
 *      The corresponding AE ports are supposed to be set/reset accordingly 
 *       before this switch.  
 *
 *  Return value : 
 *      error code when the new OPP do not corresponds the list of activated
 *      features
 */

void abe_set_opp_processing (abe_opp_t opp)
{
#if 0           /*@@@ FW 05 supports only OPP100 */
	abe_uint32 scheduler_task_steps;

	switch (opp) {
	case OPP25: 
		scheduler_task_steps = TASKS_IN_SLOT/4;
		break;
	case OPP50:
		scheduler_task_steps = TASKS_IN_SLOT/2;
		break;
	case OPP100:
		scheduler_task_steps = TASKS_IN_SLOT/1;
		break;
	case OPP0: 
	default:
		abe_dbg_param |= ERR_API; 
		abe_dbg_error_log (ABE_BLOCK_COPY_ERR);
		break;
	}

	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM, D_taskStep_ADDR,
	&scheduler_task_steps, sizeof (scheduler_task_steps)); 
#endif
}

 
/*
 *  ABE_SET_PING_PONG_BUFFER
 *
 *  Parameter  : 
 *      Port_ID : 
 *      Pointer name : Read or Write pointer
 *      New data
 *
 *  Operations : 
 *      Updates the ping-pong read/write pointer with the input data.
 *
 *  Return value : 
 *      error code when the Port is not activated.
 */

void abe_set_ping_pong_buffer (abe_port_id port, abe_uint32 rw, abe_uint32 n)
{

}

 
/*
 *  ABE_PLUG_SUBROUTINE
 *
 *  Parameter :
 *  id: returned sequence index after plugging a new subroutine
 *  f : subroutine address to be inserted
 *
 *  Returned value : error code
 *
 *  Operations : register a list of subroutines for call-back purpose
 *
 */
void abe_plug_subroutine (abe_uint32 id, abe_subroutine2 *f)
{
}
 
/*
 *  ABE_UNPLUG_SUBROUTINE
 *
 *  Prototype : void abe_unplug_subroutine (uint32 id);
 *  
 *  Parameter :
 *  Id: index of the call-back to be removed
 *  
 *  Returned value : error code
 *  
 *  Operations : unregister a  subroutine used for call-back.
 *
 */ 
 
void abe_unplug_subroutine (abe_uint32 id)
{
}

 
 
/*
 *  ABE_PLUG_SEQUENCE
 *
 *  Parameter  : 
 *       Id: returned sequence index after pluging a new sequence (index in the 
 *        tables)
 *       s : sequence to be inserted
 *
 *  Operations : 
 *      Load a time-sequenced operations.
 *
 *  Return value : 
 *      None.
 */

void abe_plug_sequence (abe_uint32 *id, abe_seq_t *s)
{
	abe_seq_t *seq_ptr;
	abe_uint32 i;
	extern abe_uint32 sequence_write_pointer;
	extern abe_uint32 sequence_id_pointer;
	extern abe_seq_t all_sequence[MAXNBSEQUENCE]; 
	extern abe_uint32 sequence_id[MAXNBSEQUENCE]; 

	seq_ptr = s;

	for (i = 0; i < MAXLINESEQUENCE; i++) {
		/* sequence copied line by line */
		all_sequence [sequence_write_pointer++] = (*seq_ptr);   

		if ((*(abe_int32 *)seq_ptr) == (-1)) {
			sequence_id [sequence_id_pointer++] =
						sequence_write_pointer -1;
			*id = (sequence_id_pointer -1);
			/* return the index to pointer table */
		}
	}

	abe_dbg_error_log (ABE_SEQTOOLONG);
}


/*
 *  ABE_LAUNCH_SEQUENCE
 *
 *  Parameter  : 
 *      patch bit field used to guarantee the code compatibility without
 *      conditionnal compilation
 *      Sequence index
 *
 *  Operations : 
 *      Launch a list a time-sequenced operations.
 *
 *  Return value : 
 *      None.
 */

void abe_launch_sequence (abe_patch_rev patch, abe_uint32 n)
{

}


/*
 *  ABE_LAUNCH_SEQUENCE_PARAM
 *
 *  Parameter  : 
 *      patch bit field used to guarantee the code compatibility without
 *      conditionnal compilation
 *      Sequence index
 *      Parameters to the programmable sequence
 *
 *  Operations : 
 *      Launch a list a time-sequenced operations.
 *
 *  Return value : 
 *      None.
 */

void abe_launch_sequence_param (abe_patch_rev patch, abe_uint32 n,
    abe_int32 *param1, abe_int32 *param2, abe_int32 *param3, abe_int32 *param4)
{

}

  
 
/*
 *  ABE_SET_ANALOG_CONTROL
 *
 *  Parameter  : 
 *      Pointer to the list of control codes (similar to the I2C control bytes)
 *
 *  Operations : 
 *      sends the commands to the analog part. Detects if the command is a gain
 *      control 
 *      in order to assign the gain value between digital and analog processing.
 *
 *  Return value : 
 *      None.
 */

void abe_set_analog_control (abe_uint32 data)
{

}

 
/*
 *  ABE_READ_ANALOG_GAIN_DL
 *
 *  Parameter  : 
 *      gain value pointer
 *
 *  Operations : 
 *      returns to the data pointer the gain value of the phoenix headset in
 *      case the dynamic extension is activated.
 *
 *  Return value : 
 *      None.
 */

void abe_read_analog_gain_dl (abe_gain_t *a)
{

}

 
/*
 *  ABE_READ_ANALOG_GAIN_UL
 *
 *  Parameter  : 
 *      gain value pointer
 *
 *  Operations : 
 *      returns to the data pointer the gain value of the phoenix headset in
 *      case the 
 * 	dynamic extension is activated.
 *
 *  Return value : 
 *      None.
 */

void abe_read_analog_gain_ul (abe_gain_t *a)
{

}



/*
 *  ABE_ENABLE_DYN_UL_GAIN
 *
 *  Parameter  : 
 *      None.
 *
 *  Operations : 
 *      enables the audio engine capability to change dynamically the analog
 *      microphone amplitude based on the information of analog gain changes.
 *
 *  Return value : 
 *      None.
 */

void abe_enable_dyn_ul_gain (void)
{

}

 
/*
 *  ABE_DISABLE_DYN_UL_GAIN
 *
 *  Parameter  : 
 *      
 *
 *  Operations : 
 *      disables the audio engine capability to change dynamically the analog
 *      microphone amplitude based on the information of analog gain changes.
 *
 *  Return value : 
 *      None.
 */

void abe_disable_dyn_ul_gain (void)
{

}



 
/*
 *  ABE_ENABLE_DYN_EXTENSION
 *
 *  Parameter  : 
 *      None
 *
 *  Operations : 
 *      enables the audio engine capability to change the analog gain of Phoenix
 *      headset port. This feature enables dynamic range extension by increasing
 *      the digital gains and lowering the analog gains. This API is preferably 
 *      called at reset time.
 *
 *  Return value : 
 *      None.
 */

void abe_enable_dyn_extension (void)
{

}



/*
 *  ABE_DISABLE_DYN_EXTENSION
 *
 *  Parameter  : 
 *      None
 *
 *  Operations : 
 *      disables the audio engine capability to change the analog gain of
 *      Phoenix headset port.
 *
 *  Return value : 
 *      None.
 */

void abe_disable_dyn_extension (void)
{

}



/*
 *  ABE_NOTIFY_ANALOG_GAIN_CHANGED
 *
 *  Parameter  : 
 *      Id: name of the Phoenix (or other device) port for which a gain was
 *      changed
 *      G: pointer to the notified gain value
 *
 *  Operations : 
 *      The upper layer tells the HAL a new gain was programmed in the analog
 *      renderer. 
 *      This will help the tuning of the APS parameters. 
 *
 *  Return value : 
 *      None.
 */
void abe_notify_analog_gain_changed (abe_ana_port_id Id, abe_gain_t *G)
{

}



/*
 *  ABE_RESET_PORT
 *
 *  Parameters : 
 *  id: port name
 *
 *  Returned value : error code 
 *
 *  Operations : stop the port activity and reload default parameters on the
 *  associated processing features.
 *       Clears the internal AE buffers.
 *
 */
void abe_reset_port (abe_port_id id)
{
	abe_port [id] = ((abe_port_t *) abe_port_init) [id];
}


 
/*
 *  ABE_READ_REMAINING_DATA
 *
 *  Parameter  : 
 *      Port_ID : 
 *      size : pointer to the remaining number of 32bits words
 *
 *  Operations : 
 *      computes the remaining amount of data in the buffer.
 *
 *  Return value : 
 *      error code 
 */

void abe_read_remaining_data (abe_port_id port, abe_uint32 *n)
{
	/* @@@  FW 05.xx do not support ping-pong protocols */
}



/*
 *  ABE_DISABLE_DATA_TRANSFER
 *
 *  Parameter  : 
 *      p: port indentifier
 *
 *  Operations : 
 *      disables the ATC descriptor and stop IO/port activities
 *
 *  Return value : 
 *      None.
 */

void abe_disable_data_transfer (abe_port_id *p)
{
	abe_port_protocol_t *protocol;
	abe_data_format_t *format;

	protocol = &(abe_port[PDM_DL1_PORT].protocol);
	format = &(abe_port[PDM_UL_PORT].format);

	abe_init_io_tasks (PDM_UL_PORT, format, protocol); 
	abe_init_atc (*p);
	abe_port[PDM_DL1_PORT].status = IDLE_P;
}


/*
 *  ABE_ENABLE_DATA_TRANSFER
 *
 *  Parameter  : 
 *      p: port indentifier
 *
 *  Operations : 
 *      enables the ATC descriptor
 *
 *  Return value : 
 *      None.
 */

void abe_enable_data_transfer (abe_port_id id)
{
	abe_port_protocol_t *protocol;
	abe_data_format_t *format;

	protocol = &(abe_port[id].protocol);  
	format = &(abe_port[id].format);
	/* AESS/ATC descriptor disabled, enabled in the IO task */
	abe_init_atc (id);
	/* install a new IO task descriptor */
	abe_init_io_tasks (id, format, protocol);

	/* local host variable status= "port is running" */
	abe_port[PDM_DL1_PORT].status = RUN_P;
}


 
/*
 *  ABE_READ_GLOBAL_COUNTER
 *
 *  Parameter  : 
 *      data pointer to the counter value
 *      data pointer to the translated milliseconds
 *
 *  Operations : 
 *      returns the value of the 32bits counter incremented on each firmware
 *      scheduling task 
 *      loops (250us / 272us with respectively 48kHz / 44.1kHz on Phoenix).
 *      Translates this data 
 *      in milli seconds format. 
 *
 *  Return value : 
 *      None.
 */

void abe_read_global_counter (abe_time_stamp_t *t, abe_millis_t *m)
{

}


 
/*
 *  ABE_SET_DMIC_FILTER
 *
 *  Parameter  : 
 *      DMIC decimation ratio : 16/25/32/40
 *
 *  Operations : 
 *      Loads in CMEM a specific list of coefficients depending on the DMIC
 *       sampling 
 *      frequency (2.4MHz or 3.84MHz). This table compensates the DMIC decimator
 *       roll-off at 20kHz. 
 *      The default table is loaded with the DMIC 2.4MHz recommended config.
 *
 *  Return value : 
 *      None.
 */

void abe_set_dmic_filter (abe_dmic_ratio_t d)
{

}


/*
* @fn abe_connect_cbpr_dmareq_port() 
*
*  Operations : enables the data echange between a DMA and the ABE through the 
*       CBPr registers of AESS. 
*
*   Parameters : 
*   id: port name
*   f : desired data format
*   d : desired dma_request line (0..7)
*   a : returned pointer to the base address of the CBPr register and number of 
*       samples to exchange during a DMA_request.
*
* @see        ABE_API.h
*/

void abe_connect_cbpr_dmareq_port (abe_port_id id, abe_data_format_t *f,
					abe_uint32 d, abe_dma_t *returned_dma_t)
{
	abe_port [id] = ((abe_port_t *) abe_port_init) [id];

	(abe_port [id]).format = (*f);
	(abe_port [id]).protocol.protocol_switch = DMAREQ_PORT_PROT;
	(abe_port [id]).protocol.p.prot_dmareq.iter = 
						abe_dma_port_iter_factor(f);
	(abe_port [id]).protocol.p.prot_dmareq.dma_addr  = ABE_DMASTATUS_RAW;
	(abe_port [id]).protocol.p.prot_dmareq.dma_data  = (1 << d);

	abe_port [id].status = RUN_P;

	/* load the micro-task parameters */
	abe_init_io_tasks (id, &((abe_port [id]).format),
						&((abe_port [id]).protocol));
	/* load the dma_t with physical information from AE memory mapping */
	abe_init_dma_t (id, &((abe_port [id]).format),
					&((abe_port [id]).protocol));
	/* load the ATC descriptors - disabled */
	abe_init_atc (id);
	/* return the dma pointer address */
	abe_read_port_address (id, returned_dma_t);
}

   

/*
* @fn abe_connect_dmareq_port() 
*
*  Operations : enables the data echange between a DMA and the ABE through the 
*       CBPr registers of AESS. 
*
*   Parameters : 
*   id: port name
*   f : desired data format
*   d : desired dma_request line (0..7)
*   a : returned pointer to the base address of the ping-pong buffer and number 
*       of samples to exchange during a DMA_request..
*
* @see        ABE_API.h
*/

void abe_connect_dmareq_port (abe_port_id id, abe_data_format_t *f,
						abe_uint32 d, abe_dma_t *a)
{
}


/*
* @fn abe_connect_dmareq_ping_pong_port() 
*
*  Operations : enables the data echanges between a DMA and a direct access to 
*  	the DMEM memory of ABE. On each dma_request activation the DMA will 
*  	exchange "s" 
*       bytes and switch to the "pong" buffer for a new buffer exchange.ABE
*
*    Parameters : 
*    id: port name
*    f : desired data format
*    d : desired dma_request line (0..7)
*    s : half-buffer (ping) size 
*
*    a : returned pointer to the base address of the ping-pong buffer and number
*     of samples to exchange during a DMA_request.
*
* @see        ABE_API.h
*/

void abe_connect_dmareq_ping_pong_port (abe_port_id id, abe_data_format_t *f,
				 abe_uint32 d, abe_uint32 s, abe_dma_t *a)
{
}

/**
* @fn abe_connect_mcbsp_port() 
*
*  Operations : enables the data echanges between a McBSP and an ATC buffer in 
*   DMEM. This API is used connect 48kHz McBSP streams to MM_DL and 8/16kHz 
*   voice streams to VX_UL, VX_DL, BT_VX_UL, BT_VX_DL. It abstracts the 
*   abe_write_port API.
*
*   Parameters : 
*    id: port name
*    f : data format
*    i : peripheral ID (McBSP #1, #2, #3)
*
* @see        ABE_API.h
*/

void abe_connect_mcbsp_port (abe_port_id id, abe_data_format_t *f, abe_uint32 i)
{
}

/*
 *  ABE_WRITE_PORT_DESCRIPTOR
 *
 *  Parameter  : 
 *      id: port name
 *      f : input pointer to the data format
 *      p : input pointer to the protocol description
 *      dma : output pointer to the DMA iteration and data destination pointer :
 *
 *  Operations : 
 *      writes the corresponding port descriptor in the AE memory spaces. 
 *      The ATC DMEM descriptors 
 *      are initialized. 
 *      - translates the data format to AE I/O task format
 *      - copy to DMEM
 *      - load ATC descriptor - disabled
 *
 *  Return value : 
 *      None.
 */

void abe_write_port_descriptor (abe_port_id id, abe_data_format_t *format,
				      abe_port_protocol_t *prot, abe_dma_t *dma)
{
 #if 0
	abe_dbg_log(id_write_port_descriptor);

	if (abe_port [id].status)
		abe_dbg_log(ABE_PORT_REPROGRAMMING);


	/* load internal HAL data */
	abe_port [id].status = RUN_P;
	abe_port [id].format = (*format);
	abe_port [id].drift = 0;
	abe_port [id].callback = NOCALLBACK;
	abe_port [id].protocol = (*prot);

	/* load AE memory space */
	abe_init_io_tasks (id, format, prot);
	abe_init_dma_t (id, format, prot);
	abe_init_atc (id);
	abe_read_port_address (id, dma); 

#endif
}


/*
 *  ABE_READ_PORT_DESCRIPTOR
 *
 *  Parameter  : 
 *      id: port name
 *      f : input pointer to the data format
 *      p : input pointer to the protocol description
 *      dma : output pointer to the DMA iteration and data destination pointer :    
 *
 *  Operations : 
 *      returns the port parameters from the HAL internal buffer.
 *
 *  Return value : 
 *      error code in case the Port_id is not compatible with the current OPP 
 *      value
 */
void abe_read_port_descriptor (abe_port_id port, abe_data_format_t *f,
							abe_port_protocol_t *p)
{
	(*f) = (abe_port[port]).format;
	(*p) = (abe_port[port]).protocol;
}

/*
 *  ABE_READ_APS_ENERGY
 *
 *  Parameter  : 
 *      Port_ID : port ID supporting APS
 *      APS data struct pointer
 *
 *  Operations : 
 *      Returns the estimated amount of energy 
 *
 *  Return value : 
 *      error code when the Port is not activated.
 */

void abe_read_aps_energy (abe_port_id *p, abe_gain_t *a)
{

}

/*
 *  ABE_READ_PORT_ADDRESS
 *
 *  Parameter  : 
 *      dma : output pointer to the DMA iteration and data destination pointer 
  *
 *  Operations : 
 *      This API returns the address of the DMA register used on this audio port.
 *      Depending on the protocol being used, adds the base address offset L3 
 *      (DMA) or MPU (ARM)
 *
 *  Return value : 
 */

void abe_read_port_address (abe_port_id port, abe_dma_t *dma2)
{
	abe_dma_t_offset dma1;
	abe_uint32 protocol_switch;

	dma1  = (abe_port[port]).dma;
	protocol_switch = abe_port[port].protocol.protocol_switch;

	switch (protocol_switch) {
	case PINGPONG_PORT_PROT:
		dma1.data = (abe_uint32) (dma1.data + ABE_DMEM_BASE_ADDRESS_MPU);
		break;
	case DMAREQ_PORT_PROT  :
		if (MM_UL_PORT == port)
			dma1.data =  dma1.data + ABE_DMEM_BASE_ADDRESS_L3;
		else
			dma1.data =  dma1.data + ABE_ATC_BASE_ADDRESS_L3;
		break;
	case CIRCULAR_PORT_PROT:
		dma1.data =  dma1.data + ABE_ATC_BASE_ADDRESS_L3;
		break;

	case SLIMBUS_PORT_PROT :
	case SERIAL_PORT_PROT :
	case DMIC_PORT_PROT :
	case MCPDMDL_PORT_PROT :
	case MCPDMUL_PORT_PROT :
	default :
	break;
	}

	(*dma2).data  = (void *)(dma1.data);
	(*dma2).iter  = (dma1.iter);

}




/*
 *  ABE_WRITE_PORT_GAIN 
 *
 *  Parameter  : 
 *      port : name of the port (VX_DL_PORT, MM_DL_PORT, MM_EXT_DL_PORT,
 *      TONES_DL_PORT, etc)
 *      dig_gain_port pointer to returned port gain and time constant
 *
 *  Operations : 
 *      saves the gain data in the local HAL-L0 table of gains in native format. 
 *      Translate the gain to the AE-FW format and load it in CMEM
 *
 *  Return value : 
 *      error code in case the gain_id is not compatible with the current OPP 
 *      value.
 */

void abe_write_port_gain (abe_port_id port, abe_gain_port_t *dig_gain_port)
{
	_log(id_write_port_gain); 
}


/*
 *  ABE_READ_GAIN 
 *
 *  Parameter  : 
 *      port : name of the port (VX_DL_PORT, MM_DL_PORT, MM_EXT_DL_PORT,
 *      TONES_DL_PORT, etc)
 *      dig_gain_port pointer to returned port gain and time constant
 *
 *  Operations : 
 *      returns the real-time value of the gain from CMEM. 
 *
 *  Return value : 
 *      error code in case the gain_id is not compatible with the current
 *      OPP value.
 */

void abe_read_port_gain (abe_port_id port, abe_gain_port_t *dig_gain_port)
{


}



/*
 *  ABE_READ_GAIN_RANGES
 *
 *  Parameter  : 
 *      Id  : name of the AE port
 *      Gain data pointer to the minimum, maximum gain, 
 *        gain steps for the overall digital and/or analog hardware.
 *
 *  Operations : 
 *      returns the gain steps accuracy and range. If the API abe_enable_dyn_ext
 *      was called, the ports connected to Phoenix McPDM will also incorporate 
 *      the gains 
 *      of the analog part, else only the digital part is taken into account.
 *
 *  Return value : 
 *      None.
 */


void abe_read_gain_range (abe_port_id id, abe_gain_t *min, abe_gain_t *max,
							abe_gain_t *step)
{

}


/*
 *  ABE_WRITE_EQUALIZER
 *
 *  Parameter  : 
 *      Id  : name of the equalizer
 *      Param : equalizer coefficients
 *
 *  Operations : 
 *      Load the coefficients in CMEM. This API can be called when the
 *      corresponding equalizer is not activated. After reloading the firmware
 *      the default coefficients corresponds to "no equalizer feature". 
 *      Loading all coefficients with zeroes disables the feature.
 *
 *  Return value : 
 *      None.
 */

void abe_write_equalizer (abe_equ_id id, abe_equ_t *param)
{

}


/*
 * ABE_WRITE_ASRC
 *
 *  Parameter  : 
 *      Id  : name of the asrc
 *      param : drift value t compensate
 *
 *  Operations : 
 *      Load the drift coefficients in FW memory. This API can be called when
 *      the corresponding ASRC is not activated. After reloading the firmware
 *      the default coefficients corresponds to "no ASRC activated". Loading 
 *      the drift value with zero disables the feature.
 *
 *  Return value : 
 *      None.
 */


void abe_write_asrc (abe_asrc_id id, abe_drift_t *param)
{
#if 0
	int	dTempValue;
	int	aTempValue32[8];

/*
 * x = ppm
 *
 *   - 1000000/x must be multiple of 16
 *   - DeltaAlpha = round(2^20*x*16/1000000)=round(2^18/5^6*x) on 22 bits.
 *    Then shifted by 2
 *   - MinusDeltaAlpha
 *   - OneMinusEpsilon = 1-DeltaAlpha/2.
 *   
 *   PPM = 250
 * - 1000000/250=4000
 *   - DeltaAlpha = 4194.3 ~ 4195 => 0x00418C
 */    
	if (dPpm != 0) {
		/* Default value for -6250 ppm */
		aTempValue32[0] = 4;          /* D_ConstAlmost0 */
		aTempValue32[1] = -1;          /* D_DriftSign */
		aTempValue32[2] = 15;         /* D_Subblock */
		aTempValue32[3] = 0x00066668; /* D_DeltaAlpha */
		aTempValue32[4] = 0xFFF99998; /* D_MinusDeltaAlpha */
		aTempValue32[5] = 0x003CCCCC; /* D_OneMinusEpsilon */
		aTempValue32[6] = 0x00000000; /* D_AlphaZero */
		aTempValue32[7] = 0x00400000; /* D_BetaOne */
		/* compute new value for the PPM */
		if (dPpm > 0)
			aTempValue32[1] = 1;           /* D_DriftSign */
		else
			aTempValue32[1] = -1;          /* D_DriftSign */
		dTempValue = (int)(ceil(262144.0/15625.0*abs(dPpm)));
		aTempValue32[3] = dTempValue<<2;
		aTempValue32[4] = (-dTempValue)<<2;
		aTempValue32[5] = (0x00100000-(dTempValue/2))<<2;

		switch (dPath) {
		case 0x0000:
			/* Voice Uplink */
			/* Init CMem */
			dTempValue = 0x00000000;
			setCM(C_AlphaCurrent_UL_VX_ADDR,
				C_AlphaCurrent_UL_VX_ADDR,&dTempValue, Sth);
			dTempValue = 0x00400000;
			setCM(C_BetaCurrent_UL_VX_ADDR,
				C_BetaCurrent_UL_VX_ADDR, &dTempValue, Sth);

			/* Init DMem */
			setlDM(D_AsrcVars_UL_VX_ADDR, D_AsrcVars_UL_VX_ADDR+28,
			&aTempValue32[0], Sth);
			break;
		case 0x0001:
			/* Voice Downlink */
			/* Init CMem */
			dTempValue = 0x00000000;
			setCM(C_AlphaCurrent_DL_VX_ADDR,
				C_AlphaCurrent_DL_VX_ADDR, &dTempValue, Sth);
			dTempValue = 0x00400000;
			setCM(C_BetaCurrent_DL_VX_ADDR,
				C_BetaCurrent_DL_VX_ADDR, &dTempValue, Sth);

		/* Init DMem */
			setlDM(D_AsrcVars_DL_VX_ADDR,  D_AsrcVars_DL_VX_ADDR+28,
					 &aTempValue32[0], Sth);
			break;
		case 0x0002:
			/* Multimedia Downlink */
			/* Init CMem */
			dTempValue = 0x00000000;
			setCM(C_AlphaCurrent_DL_MM_ADDR,
				C_AlphaCurrent_DL_MM_ADDR, &dTempValue, Sth);
			dTempValue = 0x00400000;
			setCM(C_BetaCurrent_DL_MM_ADDR,
				C_BetaCurrent_DL_MM_ADDR, &dTempValue, Sth);

			/* Init DMem */
			setlDM(D_AsrcVars_DL_MM_ADDR, D_AsrcVars_DL_MM_ADDR+28,
							&aTempValue32[0], Sth);
			break;
		}
#endif

}


/*
 *  ABE_WRITE_APS
 *
 *  Parameter  : 
 *      Id  : name of the aps filter
 *      param : table of filter coefficients 
 *
 *  Operations : 
 *      Load the filters and thresholds coefficients in FW memory. This API can 
 *      be called when the corresponding APS is not activated. After reloading 
 *      the firmware the default coefficients corresponds to "no APS activated".
 *       Loading all the coefficients value with zero disables the feature.
 *
 *  Return value : 
 *      None.
 */

void abe_write_aps (abe_aps_id id, abe_aps_t *param)
{

}



/*
 *  ABE_WRITE_MIXER
 *
 *  Parameter  : 
 *      Id  : name of the mixer
 *      param : list of input and output gains of the mixer
 *
 *  Operations : 
 *      Load the gain coefficients in FW memory. This API can be called when the
 *      corresponding MIXER is not activated. After reloading the firmware the
 *      default coefficients corresponds to "all input and output mixer's gain
 *      in mute state". A mixer is disabled with a network 
 *      reconfiguration corresponding to an OPP value.
 *
 *  Return value : 
 *      None.
 */


void abe_write_mixer (abe_mixer_id id, abe_mixer_t *param)
{
#if 0
	Audio_fInitMixerDl1
	setCM(C_Gains_DL1M_ADDR, C_Gains_DL1M_ADDR+3, pGain, Sth);
	setCM(C_Gains_DL2M_ADDR, C_Gains_DL2M_ADDR+3, pGain, Sth);
	setCM(C_Gains_EchoM_ADDR, C_Gains_EchoM_ADDR+1, pGain, Sth)
	setCM(C_Gains_SDTM_ADDR, C_Gains_SDTM_ADDR+1, pGain, Sth);
	setCM(C_Gains_ULM_ADDR, C_Gains_ULM_ADDR+3, pGain, Sth);

	setCM(C_Gains_VxRecM_ADDR, C_Gains_VxRecM_ADDR+3, pGain, Sth); @MIXER_UL


	__int64 dValue;

	dValue = (__int64)(pGain[1] & 0x00FFFFFFL) |
						(((__int64)(pGain[0]))<<32);

DMIC1
	setSM(S_GTarget_ADDR+0, S_GTarget_ADDR+0, &dValue, Sth)
DMIC2
	setSM(S_GTarget_ADDR+1, S_GTarget_ADDR+1, &dValue, Sth);
DMIC3
	setSM(S_GTarget_ADDR+2, S_GTarget_ADDR+2, &dValue, Sth);
AMIC
	setSM(S_GTarget_ADDR+3, S_GTarget_ADDR+3, &dValue, Sth);
#endif

}



/*
 *  ABE_WRITE_EANC
 *
 *  Parameter  : 
 *      param : table of coefficients
 *
 *  Operations : 
 *     Load the EANC coefficients in FW memory. This API can be called when EANC
 *  	is not activated. After reloading the firmware the default coefficients 
 *  	corresponds to "no EANC activated". Loading the coefficients value with 
 *  	zero disables the feature.
 *
 *  Return value : 
 *      None.
 */


void abe_write_eanc (abe_eanc_t *param)
{

}



/*
 *  ABE_WRITE_ROUTER
 *
 *  Parameter  : 
 *      Id  : name of the router
 *      param : list of output index of the route
 *
 *  Operations : 
 *      The uplink router takes its input from DMIC (6 samples), AMIC 
 *      (2 samples) and  PORT1/2 (2 stereo ports). Each sample will be 
 *      individually stored in an intermediate table of 10 elements.
 *      The intermediate table is used to route the samples to 
 *      three directions : REC1 mixer, 2 EANC DMIC source of filtering
 *      and MM recording audio path. 
 *      For example, a use case consisting in AMIC used for uplink voice 
 *      communication, DMIC 0,1,2,3 used for multimedia recording,
 *       DMIC 5 used for EANC filter, DMIC 4 used for the feedback channel, 
 *      will be implemented with the following routing table index list : 
 *      [3, 2 , 1, 0, 0, 0 (two dummy indexes to data that will not be on 
 *      MM_UL), 4, 5, 7, 6]
 *
 *  Return value : 
 *      None.
 */
void abe_write_router (abe_router_id id, abe_router_t *param)
{
#if 0
VOICE ROUTER

	char aUplinkMuxing[16];
	unsigned addr;

	addr = D_aUplinkRouting_ADDR;

	/* Read routing table */
	getlDM(addr, addr+sizeof(aUplinkMuxing)-4, (int*)&aUplinkMuxing, Sth);

	/* Update routing table */
	aUplinkMuxing[12] = pVoiceRouting[0];
	aUplinkMuxing[13] = pVoiceRouting[1];

	/* Write routing table */
	setlDM(addr, addr+sizeof(aUplinkMuxing)-4, (int*)&aUplinkMuxing, Sth);
 
MULTIMEDIA ROUTER

	char aUplinkMuxing[16];
	unsigned  addr;
	int dIndex;

	addr = D_aUplinkRouting_ADDR;

	/* Read routing table */
	getlDM(addr, addr+sizeof(aUplinkMuxing)-4, (int*)&aUplinkMuxing, Sth);
	
	/* Update routing table */
	for (dIndex=0;dIndex<10; dIndex++) {
		aUplinkMuxing[dIndex] = pMmRouting[dIndex];
	}

	/* Write routing table */
	setlDM(addr, addr+sizeof(aUplinkMuxing)-4, (int*)&aUplinkMuxing, Sth);
 
MM_UL2 ROUTER
	char      aUplinkMuxing[16];
	unsigned  addr;

	addr = D_aUplinkRouting_ADDR;

	/* Read routing table */
	getlDM(addr, addr+sizeof(aUplinkMuxing)-4, (int*)&aUplinkMuxing, Sth);

	/* Update routing table */
	aUplinkMuxing[10] = pMmRouting[0];
	aUplinkMuxing[11] = pMmRouting[1];

	/* Write routing table */
	setlDM(addr, addr+sizeof(aUplinkMuxing)-4, (int*)&aUplinkMuxing, Sth);
	abe_block_copy (COPY_FROM_HOST_TO_ABE, ABE_DMEM +
		D_aUplinkRouting_ADDR+12, 0,
			(abe_uint32 *)abe_router_ul_table_init_1,
				sizeof (abe_router_ul_table_init_1));
#endif

}


/*
 *  ABE_READ_ASRC
 *
 *  Parameter  : 
 *      Id  : name of the asrc
 *      param : drift value to compensate
 *      
 *
 *  Operations : 
 *      read drift ppm value
 *
 *  Return value : 
 *      None.
 */
void abe_read_asrc (abe_asrc_id id, abe_drift_t *param)
{

}


/*
 *  ABE_READ_APS
 *
 *  Parameter  : 
 *      Id  : name of the aps filter
 *      param : table of filter coefficients 
 *
 *  Operations : 
 *      Read APS coefficients
 *
 *  Return value : 
 *      None.
 */
void abe_read_aps (abe_aps_id id, abe_aps_t *param)
{

}


/*
 *  ABE_READ_MIXER
 *
 *  Parameter  : 
 *      Id  : name of the mixer
 *      param : table of gains
 *
 *
 *  Return value : 
 *      None.
 */
void abe_read_mixer (abe_mixer_id id, abe_mixer_t *param)
{
}


/*
 *  ABE_READ_EANC
 *
 *  Parameter  : 
 *      param : table of coefficients
 *
 *  Operations : 
 *      read eanc coefficients
 *
 *  Return value : 
 *      None.
 */
void abe_read_eanc  (abe_eanc_t *param)
{

}


/*
 *  ABE_READ_ROUTER
 *
 *  Parameter  : 
 *      Id  : name of the router
 *      param : list of output index of the route
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      None.
 */
void abe_read_router (abe_router_id id, abe_router_t *param)
{

}



/*
 *  ABE_READ_DEBUG_TRACE
 *
 *  Parameter  : 
 *      data destination pointer 
 *      max number of data read
 *
 *  Operations : 
 *      reads the AE circular data pointer holding pairs of debug data + 
 *      timestamps, and store the pairs in linear addressing to the parameter
 *      pointer. Stops the copy when the max parameter is reached or when the 
 *      FIFO is empty.
 *
 *  Return value : 
 *      None.
 */

void abe_read_debug_trace (abe_uint32 *data, abe_uint32 *n)
{

}



 
/*
 *  ABE_SET_DEBUG_TRACE
 *
 *  Parameter  : 
 *      debug ID from a list to be defined 
 *
 *  Operations : 
 *      load a mask which filters the debug trace to dedicated types of data
 *
 *  Return value : 
 *      None.
 */

void abe_set_debug_trace (abe_dbg_t debug)
{

}





/*
 *  ABE_SET_DEBUG_PINS
 *
 *  Parameter  : 
 *      debugger commands to the AESS register in charge of the debug pins
 *
 *  Operations : 
 *      interpret the debugger commands: activate the AESS debug pins and 
 *      allocate those pins to the AE outputs.
 *
 *  Return value : 
 *      None.
 */

void abe_set_debug_pins (abe_uint32 debug_pins)
{

}
