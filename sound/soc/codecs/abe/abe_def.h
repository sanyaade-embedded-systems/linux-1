/*
 * ==========================================================================
 *               Texas Instruments OMAP(TM) Platform Firmware
 * (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this firmware is controlled by the terms and conditions found
 *  in the license agreement under which this firmware has been supplied.
 * ==========================================================================
 */

#ifndef _ABE_DEF_H_
#define _ABE_DEF_H_

/*
 * HARDWARE AND PERIPHERAL DEFINITIONS
 */

/* PMEM size: 1024 words of 64 bits
 * CMEM size: 2048 coefficients
 * SMEM size: 3072 stereo samples
 * DMEM size: 64 kB
 */
#define ABE_PMEM_SIZE		8192
#define ABE_CMEM_SIZE		8192
#define ABE_SMEM_SIZE		24576
#define ABE_DMEM_SIZE		65536L
#define ABE_ATC_DESC_SIZE	512

#define ABE_ABE_REVISION	0x00	/* revision number */
#define ABE_DUMMY_ADDRESS	ABE_ABE_REVISION /* used as dummy register for generic IO tasks */

#define ABE_MCU_IRQSTATUS_RAW	0x24	/* holds the MCU Irq signal */
#define ABE_DSP_IRQSTATUS_RAW	0x4C	/* holds the DSP Irq signal */
#define ABE_DMASTATUS_RAW	0x84	/* holds the DMA req lines to the sDMA  */

#define EVENT_GENERATOR_COUNTER	0x68
#define EVENT_GENERATOR_COUNTER_DEFAULT 2050 /* see computation in "abe_event_generator_switch" */

#define EVENT_GENERATOR_START	0x6C	/* start / stop the EVENT generator */
#define EVENT_GENERATOR_ON	1
#define EVENT_GENERATOR_OFF	0

#define EVENT_SOURCE_SELECTION	0x70	/* selection of the EVENT generator source */
#define EVENT_SOURCE_DMA	0
#define EVENT_SOURCE_COUNTER	1

#define AUDIO_ENGINE_SCHEDULER	0x74	/* selection of the ABE DMA req line from ATC */
#define ABE_ATC_DMIC_DMA_REQ	1
#define ABE_ATC_MCPDMDL_DMA_REQ	2
#define ABE_ATC_MCPDMUL_DMA_REQ	3

#define ABE_ATC_DIRECTION_IN	0	/* Direction=0 means input from ABE point of view */
#define ABE_ATC_DIRECTION_OUT	1	/* Direction=1 means output from ABE point of view */

#define ABE_DMAREQ_REGISTER(desc)	(abe_uint32 *)((desc/8) + CIRCULAR_BUFFER_PERIPHERAL_R__0)

//#define ABE_SEND_DMAREQ(dma)	(*((abe_uint32 *)(ABE_ATC_BASE_ADDRESS_MPU+ABE_DMASTATUS_RAW)) = (dma))

#define ABE_CBPR0_IDX		0	/* MM_DL */
#define ABE_CBPR1_IDX		1	/* VX_DL */
#define ABE_CBPR2_IDX		2	/* VX_UL */
#define ABE_CBPR3_IDX		3	/* MM_UL */
#define ABE_CBPR4_IDX		4	/* MM_UL2 */
#define ABE_CBPR5_IDX		5	/* TONES */
#define ABE_CBPR6_IDX		6	/* VIB */
#define ABE_CBPR7_IDX		7	/* DEBUG/CTL */

#define CIRCULAR_BUFFER_PERIPHERAL_R__0	(0x100 + ABE_CBPR0_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__1	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR1_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__2	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR2_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__3	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR3_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__4	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR4_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__5	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR5_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__6	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR6_IDX * 4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__7	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR7_IDX * 4)

/*
 * DMA requests
 */
#define External_DMA_0		0	/* Internal connection doesn't connect at ABE boundary */
#define DMIC_DMA_REQ		1	/* Transmit request digital microphone */
#define McPDM_DMA_DL		2	/* Multichannel PDM downlink */
#define McPDM_DMA_UP		3	/* Multichannel PDM uplink */
#define MCBSP1_DMA_TX		4	/* MCBSP module 1 - transmit request */
#define MCBSP1_DMA_RX		5	/* MCBSP module 1 - receive request */
#define MCBSP2_DMA_TX		6	/* MCBSP module 2 - transmit request */
#define MCBSP2_DMA_RX		7	/* MCBSP module 2 - receive request */
#define MCBSP3_DMA_TX		8	/* MCBSP module 3 - transmit request */
#define MCBSP3_DMA_RX		9	/* MCBSP module 3 - receive request */
#define SLIMBUS1_DMA_TX0	10	/* SLIMBUS module 1 - transmit request channel 0 */
#define SLIMBUS1_DMA_TX1	11	/* SLIMBUS module 1 - transmit request channel 1 */
#define SLIMBUS1_DMA_TX2	12	/* SLIMBUS module 1 - transmit request channel 2 */
#define SLIMBUS1_DMA_TX3	13	/* SLIMBUS module 1 - transmit request channel 3 */
#define SLIMBUS1_DMA_TX4	14	/* SLIMBUS module 1 - transmit request channel 4 */
#define SLIMBUS1_DMA_TX5	15	/* SLIMBUS module 1 - transmit request channel 5 */
#define SLIMBUS1_DMA_TX6	16	/* SLIMBUS module 1 - transmit request channel 6 */
#define SLIMBUS1_DMA_TX7	17	/* SLIMBUS module 1 - transmit request channel 7 */
#define SLIMBUS1_DMA_RX0	18	/* SLIMBUS module 1 - receive request channel 0 */
#define SLIMBUS1_DMA_RX1	19	/* SLIMBUS module 1 - receive request channel 1 */
#define SLIMBUS1_DMA_RX2	20	/* SLIMBUS module 1 - receive request channel 2 */
#define SLIMBUS1_DMA_RX3	21	/* SLIMBUS module 1 - receive request channel 3 */
#define SLIMBUS1_DMA_RX4	22	/* SLIMBUS module 1 - receive request channel 4 */
#define SLIMBUS1_DMA_RX5	23	/* SLIMBUS module 1 - receive request channel 5 */
#define SLIMBUS1_DMA_RX6	24	/* SLIMBUS module 1 - receive request channel 6 */
#define SLIMBUS1_DMA_RX7	25	/* SLIMBUS module 1 - receive request channel 7 */
#define McASP1_AXEVT		26	/* McASP - Data transmit DMA request line */
#define McASP1_AREVT		29	/* McASP - Data receive DMA request line */
#define CBPr_DMA_RTX0		32	/* DMA of the Circular buffer peripheral 0 */
#define CBPr_DMA_RTX1		33	/* DMA of the Circular buffer peripheral 1 */
#define CBPr_DMA_RTX2		34	/* DMA of the Circular buffer peripheral 2 */
#define CBPr_DMA_RTX3		35	/* DMA of the Circular buffer peripheral 3 */
#define CBPr_DMA_RTX4		36	/* DMA of the Circular buffer peripheral 4 */
#define CBPr_DMA_RTX5		37	/* DMA of the Circular buffer peripheral 5 */
#define CBPr_DMA_RTX6		38	/* DMA of the Circular buffer peripheral 6 */
#define CBPr_DMA_RTX7		39	/* DMA of the Circular buffer peripheral 7 */

/*
 * ATC DESCRIPTORS - DESTINATIONS
 */
#define DEST_DMEM access	0x00
#define DEST_MCBSP1_ TX		0x01
#define DEST_MCBSP2_ TX		0x02
#define DEST_MCBSP3_TX		0x03
#define DEST_SLIMBUS1_TX0	0x04
#define DEST_SLIMBUS1_TX1	0x05
#define DEST_SLIMBUS1_TX2	0x06
#define DEST_SLIMBUS1_TX3	0x07
#define DEST_SLIMBUS1_TX4	0x08
#define DEST_SLIMBUS1_TX5	0x09
#define DEST_SLIMBUS1_TX6	0x0A
#define DEST_SLIMBUS1_TX7	0x0B
#define DEST_MCPDM_DL		0x0C
#define DEST_MCASP_TX0		0x0D
#define DEST_MCASP_TX1		0x0E
#define DEST_MCASP_TX2		0x0F
#define DEST_MCASP_TX3		0x10
#define DEST_EXTPORT0		0x11
#define DEST_EXTPORT1		0x12
#define DEST_EXTPORT2		0x13
#define DEST_EXTPORT3		0x14
#define DEST_MCPDM_ON		0x15
#define DEST_CBP_CBPr		0x3F

/*
 * ATC DESCRIPTORS - SOURCES
 */
#define SRC_DMEM access		0x0
#define SRC_MCBSP1_RX		0x01
#define SRC_MCBSP2_RX		0x02
#define SRC_MCBSP3_RX		0x03
#define SRC_SLIMBUS1_RX0	0x04
#define SRC_SLIMBUS1_RX1	0x05
#define SRC_SLIMBUS1_RX2	0x06
#define SRC_SLIMBUS1_RX3	0x07
#define SRC_SLIMBUS1_RX4	0x08
#define SRC_SLIMBUS1_RX5	0x09
#define SRC_SLIMBUS1_RX6	0x0A
#define SRC_SLIMBUS1_RX7	0x0B
#define SRC_DMIC_UP		0x0C
#define SRC_MCPDM_UP		0x0D
#define SRC_MCASP_RX0		0x0E
#define SRC_MCASP_RX1		0x0F
#define SRC_MCASP_RX2		0x10
#define SRC_MCASP_RX3		0x11
#define SRC_CBP_CBPr		0x3F

/*
 * cache-flush mechanism
 */
#define NB_BYTES_CACHELINE_SHFT	4
#define NB_BYTES_IN_CACHE_LINE	(1<<NB_BYTES_CACHELINE_SHFT) /* there are 16 bytes in each cache lines  */

/*
 * DEFINITIONS SHARED WITH VIRTAUDIO
 */

#define UC1_LP			1	/* MP3 low-power player use-case */
#define UC2_VOICE_CALL_AND_IHF_MMDL 2	/* enables voice ul/dl on earpiece + MM_DL on IHF */
#define UC3_VOICE_CALL		3	/* basic voice-call use-case */
#define UC4_EANC		4	/* EANC test */
#define UC5_PINGPONG_MMDL	5	/* Test MM_DL with Ping-Pong */
#define UC6_PINGPONG_MMDL_WITH_IRQ 6	/* ping-pong with IRQ instead of sDMA */

#define HAL_RESET_HAL		10	/* abe_reset_hal () */
#define HAL_WRITE_MIXER		11	/* abe_write_mixer () */

#if 0
#define HAL_WRITE_MIXER		12	/* void abe_irq_processing (void) */
#define HAL_WRITE_MIXER		13	/* void abe_event_generator_switch (abe_event_id e) */
#define HAL_WRITE_MIXER		14	/* abe_read_use_case_opp (abe_use_case_id *u, abe_opp_t *o) */
#define HAL_WRITE_MIXER		15	/* abe_write_mixer () */
#define HAL_WRITE_MIXER		17	/* abe_read_sys_clock (abe_micros_t *time); */
#define HAL_WRITE_MIXER		18	/* abe_fprintf (char *line); */
#define HAL_WRITE_MIXER		11	/* abe_reset_hal (void); */
#define HAL_WRITE_MIXER		11	/* abe_read_use_case_opp (abe_use_case_id *u, abe_opp_t *o); */
#define HAL_WRITE_MIXER		11	/* abe_load_fw (void); */
#define HAL_WRITE_MIXER		11	/* abe_read_port_address (abe_port_id port, abe_dma_t *dma); */
#define HAL_WRITE_MIXER		11	/* abe_default_configuration (abe_uint32 use_case, abe_uint32 param1, abe_uint32 param2, abe_uint32 param3, abe_uint32 param4); */
#define HAL_WRITE_MIXER		11	/* abe_irq_processing (void); */
#define HAL_WRITE_MIXER		11	/* abe_event_generator_switch (abe_event_id e); */
#define HAL_WRITE_MIXER		11	/* abe_read_lowest_opp (abe_opp_t *o); */
#define HAL_WRITE_MIXER		11	/* abe_set_opp_processing (abe_opp_t opp); */
#define HAL_WRITE_MIXER		11	/* abe_set_ping_pong_buffer (abe_port_id port, abe_uint32 n); */
#define HAL_WRITE_MIXER		11	/* abe_connect_irq_ping_pong_port (MM_DL, abe_data_format_t *f, abe_uint32 d, abe_uint32 s, abe_uint32 *p); */
#define HAL_WRITE_MIXER		11	/* abe_plug_subroutine (abe_uint32 *id, abe_subroutine2 f, abe_uint32 n); */
#define HAL_WRITE_MIXER		11	/* abe_plug_sequence (abe_uint32 *id, abe_sequence_t *s); */
#define HAL_WRITE_MIXER		11	/* abe_launch_sequence (abe_patch_rev patch, abe_uint32 n); */
#define HAL_WRITE_MIXER		11	/* abe_launch_sequence_param (abe_patch_rev patch, abe_uint32 n, abe_int32 *param1, abe_int32 *param2, abe_int32 *param3, abe_int32 *param4); */; */
#define HAL_WRITE_MIXER		11	/* abe_read_analog_gain_dl (abe_gain_t *a); */
#define HAL_WRITE_MIXER		11	/* abe_read_analog_gain_ul (abe_gain_t *a); */
#define HAL_WRITE_MIXER		11	/* abe_enable_dyn_ul_gain (void); */
#define HAL_WRITE_MIXER		11	/* abe_disable_dyn_ul_gain (void); */
#define HAL_WRITE_MIXER		11	/* abe_enable_dyn_extension (void); */
#define HAL_WRITE_MIXER		11	/* abe_disable_dyn_extension (void); */
#define HAL_WRITE_MIXER		11	/* abe_notify_analog_gain_changed (abe_ana_port_id Id, abe_gain_t *G); */
#define HAL_WRITE_MIXER		11	/* abe_reset_port (abe_port_id id); */
#define HAL_WRITE_MIXER		11	/* abe_read_remaining_data (abe_port_id port, abe_uint32 *n); */
#define HAL_WRITE_MIXER		11	/* abe_disable_data_transfer (abe_port_id *p); */
#define HAL_WRITE_MIXER		11	/* abe_enable_data_transfer (abe_port_id p); */
#define HAL_WRITE_MIXER		11	/* abe_read_global_counter (abe_time_stamp_t *t, abe_millis_t *m); */
#define HAL_WRITE_MIXER		11	/* abe_set_dmic_filter (abe_dmic_ratio_t d); */
#define HAL_WRITE_MIXER		11	/* abe_connect_cbpr_dmareq_port (abe_port_id id, abe_data_format_t *f, abe_uint32 d, abe_dma_t *a); */
#define HAL_WRITE_MIXER		11	/* abe_connect_dmareq_port (abe_port_id id, abe_data_format_t *f, abe_uint32 d, abe_dma_t *a); */
#define HAL_WRITE_MIXER		11	/* abe_connect_dmareq_ping_pong_port (abe_port_id id, abe_data_format_t *f, abe_uint32 d, abe_uint32 s, abe_dma_t *a); */
#define HAL_WRITE_MIXER		11	/* abe_connect_serial_port (abe_port_id id, abe_data_format_t *f, abe_uint32 i); */
#define HAL_WRITE_MIXER		11	/* abe_write_port_descriptor (abe_port_id id, abe_data_format_t *f, abe_port_protocol_t *p, abe_dma_t *dma); */
#define HAL_WRITE_MIXER		11	/* abe_read_port_descriptor (abe_port_id id, abe_data_format_t *f, abe_port_protocol_t *p); */
#define HAL_WRITE_MIXER		11	/* abe_read_aps_energy (abe_port_id *p, abe_gain_t *a); */
#define HAL_WRITE_MIXER		11	/* abe_write_port_gain (abe_port_id port, abe_gain_t gain, abe_micros_t ramp); */
#define HAL_WRITE_MIXER		11	/* abe_read_port_gain (abe_port_id port, abe_gain_t *gain, abe_micros_t *ramp); */
#define HAL_WRITE_MIXER		11	/* abe_read_gain_range (abe_port_id id, abe_gain_t *min, abe_gain_t *max, abe_gain_t *step); */
#define HAL_WRITE_MIXER		11	/* abe_write_equalizer (abe_equ_id id, abe_equ_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_write_asrc (abe_asrc_id id, abe_drift_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_write_aps (abe_aps_id id, abe_aps_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_write_mixer (abe_mixer_id id, abe_gain_t g, abe_micros_t ramp, abe_port_id p); */
#define HAL_WRITE_MIXER		11	/* abe_write_eanc (abe_eanc_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_write_router (abe_router_id id, abe_router_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_read_asrc (abe_asrc_id id, abe_drift_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_read_aps (abe_aps_id id, abe_aps_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_read_mixer (abe_mixer_id id, abe_gain_t *gain, abe_micros_t *ramp, abe_port_id p); */
#define HAL_WRITE_MIXER		11	/* abe_read_eanc  (abe_eanc_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_read_router (abe_router_id id, abe_router_t *param); */
#define HAL_WRITE_MIXER		11	/* abe_read_debug_trace (abe_uint32 *data, abe_uint32 *n); */
#define HAL_WRITE_MIXER		11	/* abe_set_debug_trace (abe_dbg_t debug); */
#define HAL_WRITE_MIXER		11	/* abe_set_debug_pins (abe_uint32 debug_pins); */
#endif

#define COPY_FROM_ABE_TO_HOST	1	/* ID used for LIB memory copy subroutines */
#define COPY_FROM_HOST_TO_ABE	2

/*
 * INTERNAL DEFINITIONS
 */

#define CC_M1			0xFF	/* unsigned version of (-1) */
#define CS_M1			0xFFFF	/* unsigned version of (-1) */
#define CL_M1			0xFFFFFFFFL /* unsigned version of (-1) */

#define NBEANC1			20	/* 20 Q6.26 coef for the FIR    */
#define NBEANC2			16	/* 16 Q6.26 coef for the IIR    */

#define NBEQ1			25	/* 24 Q6.26 coefficients	*/
#define NBEQ2			13	/* 2x12 Q6.26 coefficients       */

#define NBAPS1			10	/* TBD APS first set of parameters      */
#define NBAPS2			10	/* TBD APS second set of parameters      */

#define NBMIX_AUDIO_UL		2	/* Mixer used for sending tones to the uplink voice path */
#define NBMIX_DL1		4	/* Main downlink mixer */
#define NBMIX_DL2		4	/* Handsfree downlink mixer */
#define NBMIX_SDT		2	/* Side-tone mixer */
#define NBMIX_ECHO		2	/* Echo reference mixer */
#define NBMIX_VXREC		4	/* Voice record mixer */
	    /*
	    Mixer ID		Input port ID	    Comments
	    DL1_MIXER		0		   TONES_DL path
				1		   VX_DL path
				2		   MM_DL path
				3		   MM_UL2 path

	    DL2_MIXER		0		   TONES_DL path
				1		   VX_DL path
				2		   MM_DL path
				3		   MM_UL2 path

	    SDT_MIXER		0		   Uplink path
				1		   Downlink path

	    ECHO_MIXER		0		   DL1_MIXER  path
				1		   DL2_MIXER path

	    AUDUL_MIXER		0		   TONES_DL path
				1		   Uplink path
				2		   MM_DL path

	    VXREC_MIXER		0		   TONES_DL path
				1		   VX_DL path
				2		   MM_DL path
				3		   VX_UL path
	    */
#define MIX_DL1_INPUT_TONES	0
#define MIX_DL1_INPUT_VX_DL	1
#define MIX_DL1_INPUT_MM_DL	2
#define MIX_DL1_INPUT_MM_UL2	3

#define MIX_DL2_INPUT_TONES	0
#define MIX_DL2_INPUT_VX_DL	1
#define MIX_DL2_INPUT_MM_DL	2
#define MIX_DL2_INPUT_MM_UL2	3

#define MIX_SDT_INPUT_DL1_MIXER	0
#define MIX_SDT_INPUT_DL2_MIXER	1

#define MIX_AUDUL_INPUT_TONES	0
#define MIX_AUDUL_INPUT_UPLINK	1
#define MIX_AUDUL_INPUT_MM_DL	2

#define MIX_VXREC_INPUT_TONES	0
#define MIX_VXREC_INPUT_VX_DL	1
#define MIX_VXREC_INPUT_MM_DL	2
#define MIX_VXREC_INPUT_VX_UL	3

#define NBROUTE_UL		16	/* nb of samples to route */
#define NBROUTE_CONFIG_MAX	10	/* 10 routing tables max */

#define NBROUTE_CONFIG		4	/* 4 pre-computed routing tables */
#define UPROUTE_CONFIG_AMIC	0	/* AMIC on VX_UL */
#define UPROUTE_CONFIG_DMIC1	1	/* DMIC first pair on VX_UL */
#define UPROUTE_CONFIG_DMIC2	2	/* DMIC second pair on VX_UL */
#define UPROUTE_CONFIG_DMIC3	3	/* DMIC last pair on VX_UL */

#define ABE_PMEM		1
#define ABE_CMEM		2
#define ABE_SMEM		3
#define ABE_DMEM		4
#define ABE_ATC			5

#define MAXCALLBACK		100	/* call-back indexes */
#define MAXNBSUBROUTINE		100	/* subroutines */

#define MAXNBSEQUENCE		20	/* time controlled sequenced */
#define MAXACTIVESEQUENCE	20	/* maximum simultaneous active sequences */
#define MAXSEQUENCESTEPS	2	/* max number of steps in the sequences */
#define MAXFEATUREPORT		8	/* max number of feature associated to a port */
#define SUB_0_PARAM		0
#define SUB_1_PARAM		1	/* number of parameters per sequence calls */
#define SUB_2_PARAM		2
#define SUB_3_PARAM		3
#define SUB_4_PARAM		4

#define FREE_LINE		0	/* active sequence mask = 0 means the line is free */
#define NOMASK			(1 << 0) /* no ask for collision protection */
#define MASK_PDM_OFF		(1 << 1) /* do not allow a PDM OFF during the execution of this sequence */
#define MASK_PDM_ON		(1 << 2) /* do not allow a PDM ON during the execution of this sequence */

#define NBCHARFEATURENAME	16	/* explicit name of the feature */
#define NBCHARPORTNAME		16	/* explicit name of the port */
#define MAXNBABEPORTS		LAST_PORT_ID /* number  of sink+source ports of the ABE */
#define MAX_MAP_DMEM		LAST_PORT_ID

#define SNK_P			ABE_ATC_DIRECTION_IN	/* sink / input port */
#define SRC_P			ABE_ATC_DIRECTION_OUT	/* source / ouptut port */

#define NODRIFT			0	/* no ASRC applied */
#define GAIN_MAXIMUM		30L
#define GAIN_24dB		24L
#define GAIN_18dB		18L
#define GAIN_12dB		12L
#define GAIN_6dB		6L
#define GAIN_0dB		0	/* default gain = 1 */
#define GAIN_M6dB		-6L
#define GAIN_M12dB		-12L
#define GAIN_M18dB		-18L
#define GAIN_M24dB		-24L
#define GAIN_M30dB		-30L
#define GAIN_M40dB		-40L
#define GAIN_M50dB		-50L
#define MUTE_GAIN		-140L	/* muted gain = -140 decibels */

#define RAMP_0MS		0	/* ramp_t is in milli- seconds */
#define RAMP_1MS		1L
#define RAMP_2MS		2L
#define RAMP_5MS		5L
#define RAMP_10MS		10L
#define RAMP_20MS		20L
#define RAMP_50MS		50L
#define RAMP_100MS		100L
#define RAMP_200MS		200L
#define RAMP_500MS		500L
#define RAMP_1000MS		1000L
#define RAMP_MAXLENGTH		10000L

#define LINABE_TO_DECIBELS	1	/* for abe_translate_gain_format */
#define DECIBELS_TO_LINABE	2
#define IIRABE_TO_MICROS	1	/* for abe_translate_ramp_format */
#define MICROS_TO_IIABE		2

#define IDLE_P			1	/* port idled */
#define RUN_P			2	/* port running */
#define NOCALLBACK		0
#define NOPARAMETER		0

#define MCPDM_UL_ITER		2	/* number of ATC access upon AMIC DMArequests, all the FIFOs are enabled */
#define MCPDM_DL_ITER		6	/* All the McPDM FIFOs are enabled simultaneously */
#define DMIC_ITER		6	/* All the DMIC FIFOs are enabled simultaneously */

#define DEFAULT_THR_READ	1	/* port / flow management */
#define DEFAULT_THR_WRITE	1	/* port / flow management */

#define DEFAULT_CONTROL_MCPDMDL 1	/* allows control on the PDM line */

#define MAX_PINGPONG_BUFFERS	2	/* TBD later if needed */

/*
 * Indexes to the subroutines
 */
#define SUB_WRITE_MIXER		1
#define SUB_WRITE_PORT_GAIN	2

/* OLD WAY */
#define c_feat_init_eq		1
#define c_feat_read_eq1		2
#define c_write_eq1		3
#define c_feat_read_eq2		4
#define c_write_eq2		5
#define c_feat_read_eq3		6
#define c_write_eq3		7

/*
 * MACROS
 */

#define LOAD_ABEREG(reg,data)	{abe_uint32 *ocp_addr = (abe_uint32 *)((reg)+ABE_ATC_BASE_ADDRESS_MPU); *ocp_addr= (data);}

#define maximum(a,b)		(((a)<(b))?(b):(a))
#define absolute(a)		( ((a)>0) ? (a):((-1)*(a)) )

// Gives 1% errors
//#define abe_power_of_two(x) (abe_float)(1 + x*(0.69315 + x*(0.24022 + x*(0.056614 + x*(0.00975 )))))   /* for x = [-1..+1] */
//#define abe_log_of_two(i)   (abe_float)(-2.4983 + i*(4.0321 + i*(-2.0843 + i*(0.63 + i*(-0.0793)))))   /* for i = [+1..+2[ */
// Gives 0.1% errors
//#define abe_power_of_two(xx) (abe_float)(1 + xx*(0.69314718055995 + xx*(0.24022650695909 + xx*(0.05661419083812 + xx*(0.0096258236109 )))))   /* for x = [-1..+1] */
//#define abe_log_of_two(i)   (abe_float)(-3.02985297173966 + i*(6.07170945221999 + i*(-5.27332161514862 + i*(3.22638187067771 + i*(-1.23767101624897 + i*(0.26766043958616 + i*(-0.02490211314987)))))))   /* for i = [+1..+2[ */

#define abe_power_of_two(xx) (abe_float)(0.9999999924494 + xx*(0.69314847688495 + xx*(0.24022677604481 + xx*(0.05549256818679 + xx*(0.00961666477618 + xx*(0.0013584351075 + xx*(0.00015654359307)))))))	/* for x = [-1..+1] */
#define abe_log_of_two(xx)   (abe_float)(-3.02985297175803 + xx*(6.07170945229365 + xx*(-5.27332161527062 + xx*(3.22638187078450 + xx*(-1.23767101630110 + xx*(0.26766043959961 + xx*(-0.02490211315130)))))))  /* for x = [+1..+2] */
#define abe_sine(xx)	 (abe_float)(-0.000000389441 + xx*(6.283360925789 + xx*(-0.011140658372 + xx*(-41.073653348384 + xx*(-3.121196875959 + xx*(100.619954580736 + xx*( -59.133359355846)))))))	  /* for x = [0 .. pi/2] */
#define abe_sqrt(xx)	 (abe_float)(0.32298238417665 + xx*(0.93621865220393 + xx*(-0.36276443369703 + xx*(0.13008602653101+ xx*(-0.03017833169073 + xx*(0.00393731964847 + xx*-0.00021858629159 ))))))     /* for x = [1 .. 4] */

#endif	/* _ABE_DEF_H_ */
