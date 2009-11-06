/*
 * ==========================================================================
 *               Texas Instruments OMAP(TM) Platform Firmware
 * (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this firmware is controlled by the terms and conditions found
 *  in the license agreement under which this firmware has been supplied.
 * ==========================================================================
 */

#ifndef _ABE_DAT_H_
#define _ABE_DAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Callbacks
 */
abe_subroutine2 callbacks[MAXCALLBACK];	/* 2 parameters subroutine pointers */

abe_port_t abe_port[MAXNBABEPORTS];	/* list of ABE ports */

const abe_port_t abe_port_init[MAXNBABEPORTS] = {
/* status, data format, drift, callback, io-task buffer 1, io-task buffer 2,
 * protocol, dma offset, features, name
 * - Features reseted at start
 */

	/* DMIC1 */
	{
		IDLE_P,
		{96000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_dmic1,
		0,
		{
			SNK_P,
			DMIC_PORT_PROT,
			{{
				dmem_dmic,
				dmem_dmic_size,
				DMIC_ITER
			}},
		},
		{0, 0},
		{EQDMIC1, 0},
		"DMIC_UL",
	},
	/* DMIC2 */
	{
		IDLE_P,
		{96000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_dmic2,
		0,
		{
			SNK_P,
			DMIC_PORT_PROT,
			{{
				dmem_dmic,
				dmem_dmic_size,
				DMIC_ITER
			}},
		},
		{0, 0},
		{EQDMIC2, 0},
		"DMIC_UL",
	},
	/* DMIC3 */
	{
		IDLE_P,
		{96000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_dmic3,
		0,
		{
			SNK_P,
			DMIC_PORT_PROT,
			{{
				dmem_dmic,
				dmem_dmic_size,
				DMIC_ITER
			}},
		},
		{0, 0},
		{EQDMIC3, 0},
		"DMIC_UL",
	},
	/* PDM_UL */
	{
		IDLE_P,
		{96000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_amic,
		0,
		{
			SNK_P,
			MCPDMUL_PORT_PROT,
			{{
				dmem_amic,
				dmem_amic_size,
				MCPDM_UL_ITER,
			}},
		},
		{0, 0},
		{EQAMIC, 0},
		"AMIC_UL",
	},
	/* BT_VX_UL */
	{
		IDLE_P,
		{8000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_bt_vx_ul,
		0,
		{
			SNK_P,
			SERIAL_PORT_PROT,
			{{
				MCBSP1_DMA_TX * ATC_SIZE,
				dmem_bt_vx_ul,
				dmem_bt_vx_ul_size,
				1 * SCHED_LOOP_8kHz,
				DEFAULT_THR_WRITE,
			}},
		},
		{0, 0},
		{0},
		"BT_VX_UL",
	},
	/* MM_UL */
	{
		IDLE_P,
		{48000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_mm_ul,
		0,
		{
			SRC_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX3 * ATC_SIZE,
				dmem_mm_ul,
				dmem_mm_ul_size,
				10 * SCHED_LOOP_48kHz,
				DEFAULT_THR_WRITE,
				ABE_DMASTATUS_RAW,
				1 << 3,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__3, 120},
		{UPROUTE, 0},
		"MM_UL",
	},
	/* MM_UL2 */
	{
		IDLE_P,
		{48000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_mm_ul2,
		0,
		{
			SRC_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX4 * ATC_SIZE,
				dmem_mm_ul2,
				dmem_mm_ul2_size,
				2 * SCHED_LOOP_48kHz,
				DEFAULT_THR_WRITE,
				ABE_DMASTATUS_RAW,
				1 << 4,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__4, 24},
		{UPROUTE, 0},
		"MM_UL2",
	},
	/* VX_UL */
	{
		IDLE_P,
		{8000, MONO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_vx_ul,
		0,
		{
			SRC_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX2*ATC_SIZE,
				dmem_vx_ul,
				dmem_vx_ul_size / 2,
				1 * SCHED_LOOP_8kHz,
				DEFAULT_THR_WRITE,
				ABE_DMASTATUS_RAW,
				1 << 2,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__2, 2},
		{ASRC2, 0},
		"VX_UL",
	},
	/* MM_DL */
	{
		IDLE_P,
		{48000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_mm_dl,
		0,
		{
			SNK_P,
			PINGPONG_PORT_PROT,
			{{
				CBPr_DMA_RTX0 * ATC_SIZE,
				dmem_mm_dl,
				dmem_mm_dl_size,
				2 * SCHED_LOOP_48kHz,
				DEFAULT_THR_READ,
				ABE_DMASTATUS_RAW,
				1 << 0,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__0, 24},
		{ASRC3, 0},
		"MM_DL",
	},
	/* VX_DL */
	{
		IDLE_P,
		{8000, MONO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_vx_dl,
		0,
		{
			SNK_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX1 * ATC_SIZE,
				dmem_vx_dl,
				dmem_vx_dl_size,
				1 * SCHED_LOOP_8kHz,
				DEFAULT_THR_READ,
				ABE_DMASTATUS_RAW,
				1 << 1,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__1, 2},
		{ASRC1, 0},
		"VX_DL",
	},
	/* TONES_DL */
	{
		IDLE_P,
		{48000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_tones_dl,
		0,
		{
			SNK_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX5 * ATC_SIZE,
				dmem_tones_dl,
				dmem_tones_dl_size,
				2 * SCHED_LOOP_48kHz,
				DEFAULT_THR_READ,
				ABE_DMASTATUS_RAW,
				1 << 5,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__5, 24},
		{0},
		"TONES_DL",
	},
	/* VIB_DL */
	{
		IDLE_P,
		{24000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_vib,
		0,
		{
			SNK_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX6 * ATC_SIZE,
				dmem_vib_dl,
				dmem_vib_dl_size,
				2 * SCHED_LOOP_24kHz,
				DEFAULT_THR_READ,
				ABE_DMASTATUS_RAW,
				1 << 6,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__6, 12},
		{0},
		"VIB_DL",
	},
	/* BT_VX_DL */
	{
		IDLE_P,
		{8000, MONO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_bt_vx_dl,
		0,
		{
			SRC_P,
			SERIAL_PORT_PROT,
			{{
				MCBSP1_DMA_RX * ATC_SIZE,
				dmem_bt_vx_dl,
				dmem_bt_vx_dl_size,
				1 * SCHED_LOOP_8kHz,
				DEFAULT_THR_WRITE,
			}},
		},
		{0, 0},
		{0},
		"BT_VX_DL",
	},
	/* PDM_DL1 */
	{
		IDLE_P,
		{96000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		0,
		0,
		{
			SRC_P,
			MCPDMDL_PORT_PROT,
			{{
				dmem_mcpdm,
				dmem_mcpdm_size,
			}},
		},
		{0, 0},
		{MIXDL1, EQ1, APS1, 0},
		"PDM_DL1",
	},
	/* MM_EXT_OUT */
	{
		IDLE_P,
		{48000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_mm_ext_out,
		0,
		{
			SRC_P,
			SERIAL_PORT_PROT,
			{{
				MCBSP1_DMA_RX * ATC_SIZE,
				dmem_mm_ext_out,
				dmem_mm_ext_out_size,
				2 * SCHED_LOOP_48kHz,
				DEFAULT_THR_WRITE,
			}},
		},
		{0, 0},
		{0},
		"MM_EXT_OUT",
	},
	/* PDM_DL2 */
	{
		IDLE_P,
		{96000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		0,
		0,
		{
			SRC_P,
			MCPDMDL_PORT_PROT,
			{{0, 0, 0, 0, 0, 0, 0}},
		},
		{0, 0},
		{MIXDL2, EQ2L, APS2L, EQ2R, APS2R, 0},
		"PDM_DL2",
	},
	/* PDM_VIB */
	{
		IDLE_P,
		{24000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		0,
		0,
		{
			SRC_P,
			MCPDMDL_PORT_PROT,
			{{0, 0, 0, 0, 0, 0, 0}},
		},
		{0, 0},
		{0},
		"PDM_VIB",
	},
	/* SCHD_DBG_PORT */
	{
		IDLE_P,
		{48000, STEREO_MSB},
		NODRIFT,
		NOCALLBACK,
		smem_mm_trace,
		0,
		{
			SRC_P,
			DMAREQ_PORT_PROT,
			{{
				CBPr_DMA_RTX7 * ATC_SIZE,
				dmem_mm_trace,
				dmem_mm_trace_size,
				2 * SCHED_LOOP_48kHz,
				DEFAULT_THR_WRITE,
				ABE_DMASTATUS_RAW,
				1 << 4,
			}},
		},
		{CIRCULAR_BUFFER_PERIPHERAL_R__7, 24},
		{SEQUENCE, CONTROL, GAINS, 0},
		"SCHD_DBG",
	},
};

const abe_port_info_t abe_port_info[MAXNBABEPORTS] = {
	/* DMIC1 */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* DMIC2 */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT2, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* DMIC3 */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT3, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* PDM_UL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* BT_VX_UL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* MM_UL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* MM_UL2 */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* VX_UL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* MM_DL */
	{
		ABE_OPP50,
		{SUB_WRITE_MIXER, {MM_DL_PORT, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* VX_DL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* TONES_DL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* VIB_DL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* BT_VX_DL */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* PDM_DL1 */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* MM_EXT_OUT */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* PDM_DL2 */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* PDM_VIB */
	{
		ABE_OPP50,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
	/* SCHD_DBG_PORT */
	{
		ABE_OPP25,
		{SUB_WRITE_PORT_GAIN, {DMIC_PORT1, MUTE_GAIN, 0, 0}},
		{0, {0, 0, 0, 0}},
	},
};

/*
 * Firmware features
 */
abe_feature_t all_feature[MAXNBFEATURE];

const abe_feature_t all_feature_init[] = {
/* on_reset, off, read, write, status, input, output, slots, opp, name */
	/* EQ1: equalizer downlink path headset + earphone */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq1,
		c_write_eq1,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP25,
		"  DLEQ1",
	},
	/* EQ2L: equalizer downlink path integrated handsfree left */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq2,
		c_write_eq2,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  DLEQ2L",
	},
	/* EQ2R: equalizer downlink path integrated handsfree right */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  DLEQ2R",
	},
	/* EQSDT: equalizer downlink path side-tone */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  EQSDT",
	},
	/* EQDMIC1: SRC+equalizer uplink DMIC 1st pair */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  EQDMIC1",
	},
	/* EQDMIC2: SRC+equalizer uplink DMIC 2nd pair */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  EQDMIC2",
	},
	/* EQDMIC3: SRC+equalizer uplink DMIC 3rd pair */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  EQDMIC3",
	},
	/* EQAMIC: SRC+equalizer uplink AMIC */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  EQAMIC",
	},
	/* APS1: Acoustic protection for headset */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP25,
		"  APS1",
	},
	/* APS2: acoustic protection high-pass filter for handsfree left */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  APS2",
	},
	/* APS3: acoustic protection high-pass filter for handsfree right */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  APS3",
	},
	/* ASRC1: asynchronous sample-rate-converter for the downlink voice path */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  ASRC_VXDL"
	},
	/* ASRC2: asynchronous sample-rate-converter for the uplink voice path */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  ASRC_VXUL",
	},
	/* ASRC3: asynchronous sample-rate-converter for the multimedia player */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  ASRC_MMDL",
	},
	/* ASRC4: asynchronous sample-rate-converter for the echo reference */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  ASRC_ECHO",
	},
	/* MXDL1: mixer of the headset and earphone path */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP25,
		"  MIX_DL1",
	},
	/* MXDL2: mixer of the hands-free path */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  MIX_DL2",
	},
	/* MXAUDUL: mixer for uplink tone mixer */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  MXSAUDUL",
	},
	/* MXVXREC: mixer for voice recording */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  MXVXREC",
	},
	/* MXSDT: mixer for side-tone */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  MIX_SDT",
	},
	/* MXECHO: mixer for echo reference */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  MIX_ECHO",
	},
	/* UPROUTE: router of the uplink path */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP50,
		"  DLEQ3",
	},
	/* GAINS: all gains */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP25,
		"  DLEQ3",
	},
	/* EANC: active noise canceller */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP100,
		"  DLEQ3",
	},
	/* SEQ: sequencing queue of micro tasks */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP25,
		"  DLEQ3",
	},
	/* CTL: Phoenix control queue through McPDM */
	{
		c_feat_init_eq,
		c_feat_init_eq,
		c_feat_read_eq3,
		c_write_eq3,
		0,
		0x1000,
		0x1010,
		2,
		0,
		ABE_OPP25,
		"  DLEQ3",
	},
};

/*
 * Table indexed by the PORT_ID and returning the index
 * to the D_IODescr table of structures
 */
const abe_uint32 sio_task_index[LAST_PORT_ID] =
{
	/* AE sink ports - Uplink */
	7,	/* DMIC_PORT  - specific protocol */
	7,	/* DMIC_PORT  - specific protocol */
	7,	/* DMIC_PORT  - specific protocol */
	8,	/* PDM_UL_PORT - specific protocol */
	14,	/* BT_VX_UL_PORT BT uplink */

	/* AE source ports - Uplink */
	3,	/* MM_UL_PORT up to 5 stereo channels */
	4,	/* MM_UL2_PORT stereo FM record path */
	2,	/* VX_UL_PORT stereo FM record path */

	/* AE sink ports - Downlink */
	0,	/* MM_DL_PORT multimedia player audio path */
	1,	/* VX_DL_PORT */
	5,	/* TONES_DL_PORT */
	6,	/* VIB_DL_PORT */

	/* AE source ports - Downlink */
	13,	/* BT_VX_DL_PORT */
	9,	/* PDM_DL1_PORT */
	12,	/* MM_EXT_OUT_PORT */
	10,	/* PDM_DL2_PORT */
	11,	/* PDM_VIB_PORT */

	/* dummy port used to declare the other tasks of the scheduler */
	SCHD_DBG_PORT,
};

/*
 * Memory mapping of DMEM FIFOs
 */
abe_uint32 abe_map_dmem[LAST_PORT_ID];			/* DMEM port map */
abe_uint32 abe_map_dmem_secondary[LAST_PORT_ID];
abe_uint32 abe_map_dmem_size[LAST_PORT_ID];		/* DMEM port buffer sizes */

/*
 * AESS/ATC destination and source address translation
 * (except McASPs) from the original 64bits words address
 */
const abe_uint32 abe_atc_dstid[ABE_ATC_DESC_SIZE >> 3] = {
     /* DMA_0   DMIC    PDM_DL  PDM_UL  McB1TX  McB1RX  McB2TX  McB2RX	 0-7 */
	0,      0,      12,     0,      1,      0,      2,      0,
     /* McB3TX  McB3RX  SLIMT0  SLIMT1  SLIMT2  SLIMT3  SLIMT4  SLIMT5	 8-15 */
	3,      0,      4,      5,      6,      7,      8,      9,
     /* SLIMT6  SLIMT7  SLIMR0  SLIMR1  SLIMR2  SLIMR3  SLIMR4  SLIMR5	16-23 */
	10,     11,     0,      0,      0,      0,      0,      0,
     /* SLIMR6  SLIMR7  McASP1X ------  ------  McASP1R -----   ------  24-31 */
	0,      0,      14,     0,      0,      0,      0,      0,
     /* CBPrT0  CBPrT1  CBPrT2  CBPrT3  CBPrT4  CBPrT5  CBPrT6  CBPrT7  32-39 */
	63,     63,     63,     63,     63,     63,     63,     63,
     /* CBP_T0  CBP_T1  CBP_T2  CBP_T3  CBP_T4  CBP_T5  CBP_T6  CBP_T7	40-47 */
	0,      0,      0,      0,      0,      0,      0,      0,
     /* CBP_T8  CBP_T9  CBP_T10 CBP_T11 CBP_T12 CBP_T13 CBP_T14 CBP_T15	48-63 */
	0,      0,      0,      0,      0,      0,      0,      0,
};

const abe_uint32 abe_atc_srcid[ABE_ATC_DESC_SIZE >> 3] = {
     /* DMA_0   DMIC    PDM_DL  PDM_UL  McB1TX  McB1RX  McB2TX  McB2RX   0-7 */
	0,      12,     0,      13,     0,      1,      0,      2,
     /* McB3TX  McB3RX  SLIMT0  SLIMT1  SLIMT2  SLIMT3  SLIMT4  SLIMT5   8-15 */
	0,      3,      0,      0,      0,      0,      0,      0,
     /* SLIMT6  SLIMT7  SLIMR0  SLIMR1  SLIMR2  SLIMR3  SLIMR4  SLIMR5  16-23 */
	0,      0,      4,      5,      6,      7,      8,      9,
     /* SLIMR6  SLIMR7  McASP1X ------  ------  McASP1R ------  ------  24-31 */
	10,     11,     0,      0,      0,      14,     0,      0,
     /* CBPrT0  CBPrT1  CBPrT2  CBPrT3  CBPrT4  CBPrT5  CBPrT6  CBPrT7  32-39 */
	63,     63,     63,     63,     63,     63,     63,     63,
     /* CBP_T0  CBP_T1  CBP_T2  CBP_T3  CBP_T4  CBP_T5  CBP_T6  CBP_T7  40-47 */
	0,      0,      0,      0,      0,      0,      0,      0,
     /* CBP_T8  CBP_T9  CBP_T10 CBP_T11 CBP_T12 CBP_T13 CBP_T14 CBP_T15 48-63 */
	0,      0,      0,      0,      0,      0,      0,      0,
};

/*
 * Router tables
 */
/* preset routing configurations */
const abe_router_t abe_router_ul_table_preset[NBROUTE_CONFIG][NBROUTE_UL] = {
	/* Voice uplink with Phoenix microphones */
	{
		/* 0  .. 9   = MM_UL */
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		/* 10 .. 11  = MM_UL2 */
		ZERO_labelID,
		ZERO_labelID,
		/* 12 .. 13  = VX_UL */
		AMIC_L_labelID,
		AMIC_R_labelID,
		/* 14 .. 15  = RESERVED */
		ZERO_labelID,
		ZERO_labelID,
	},
	/* Voice uplink with the first DMIC pair */
	{
		/* 0  .. 9   = MM_UL */
		DMIC1_L_labelID,
		DMIC1_R_labelID,
		DMIC2_L_labelID,
		DMIC2_R_labelID,
		DMIC3_L_labelID,
		DMIC3_R_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		/* 10 .. 11  = MM_UL2 */
		ZERO_labelID,
		ZERO_labelID,
		/* 12 .. 13  = VX_UL */
		DMIC1_L_labelID,
		DMIC1_R_labelID,
		/* 14 .. 15  = RESERVED */
		ZERO_labelID,
		ZERO_labelID,
	},
	/* Voice uplink with the second DMIC pair */
	{
		/* 0  .. 9   = MM_UL */
		DMIC1_L_labelID,
		DMIC1_R_labelID,
		DMIC2_L_labelID,
		DMIC2_R_labelID,
		DMIC3_L_labelID,
		DMIC3_R_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		/* 10 .. 11  = MM_UL2 */
		ZERO_labelID,
		ZERO_labelID,
		/* 12 .. 13  = VX_UL */
		DMIC2_L_labelID,
		DMIC2_R_labelID,
		/* 14 .. 15  = RESERVED */
		ZERO_labelID,
		ZERO_labelID,
	},
	/* Voice uplink with the last DMIC pair */
	{
		/* 0  .. 9   = MM_UL */
		DMIC1_L_labelID,
		DMIC1_R_labelID,
		DMIC2_L_labelID,
		DMIC2_R_labelID,
		DMIC3_L_labelID,
		DMIC3_R_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		ZERO_labelID,
		/* 10 .. 11  = MM_UL2 */
		ZERO_labelID,
		ZERO_labelID,
		/* 12 .. 13  = VX_UL */
		DMIC3_L_labelID,
		DMIC3_R_labelID,
		/* 14 .. 15  = RESERVED */
		ZERO_labelID,
		ZERO_labelID,
	},
};

/* all default routing configurations */
abe_router_t abe_router_ul_table[NBROUTE_CONFIG_MAX][NBROUTE_UL];

/*
 * ABE_GLOBAL DATA
 */
/* flag, indicates the allowed control of Phoenix through McPDM slot 6 */
abe_uint32 abe_global_mcpdm_control;
abe_event_id abe_current_event_id;

/*
 * ABE SUBROUTINES AND SEQUENCES
 */

/*
const abe_seq_t abe_seq_array [MAXNBSEQUENCE] [MAXSEQUENCESTEPS] =
	   {{0,      0,      0,      0}, {-1, 0, 0, 0}},
	   {{0,      0,      0,      0}, {-1, 0, 0, 0}},
const seq_t setup_hw_sequence2 [ ] = { 0, C_AE_FUNC1,  0, 0, 0, 0,
				      -1, C_CALLBACK1, 0, 0, 0, 0 };

const abe_subroutine2 abe_sub_array [MAXNBSUBROUTINE] =
	    abe_init_atc,   0,      0,
	    abe_init_atc,   0,      0,

 typedef double (*PtrFun) (double);
PtrFun pFun;
pFun = sin;
y = (* pFun) (x);
*/

const abe_sequence_t seq_null =	{
	NOMASK,
	{
		CL_M1,
		0,
		{0, 0, 0, 0},
		0,
	},
	{
		CL_M1,
		0,
		{0, 0, 0, 0},
		0,
	},
};

abe_subroutine2 abe_all_subsubroutine[MAXNBSUBROUTINE];	/* table of new subroutines called in the sequence */
abe_uint32 abe_all_subsubroutine_nparam[MAXNBSUBROUTINE]; /* number of parameters per calls */
abe_uint32 abe_subroutine_id[MAXNBSUBROUTINE];		/* index of the subroutine */
abe_uint32 abe_subroutine_write_pointer;

/* table of all sequences */
abe_sequence_t abe_all_sequence[MAXNBSEQUENCE];

abe_uint32 abe_sequence_write_pointer;

/* current number of pending sequences (avoids to look in the table) */
abe_uint32 abe_nb_pending_sequences;

/* pending sequences due to ressource collision */
abe_uint32 abe_pending_sequences[MAXNBSEQUENCE];

/* mask of unsharable ressources among other sequences */
abe_uint32 abe_global_sequence_mask;

/* table of active sequences */
abe_seq_t abe_active_sequence[MAXACTIVESEQUENCE][MAXSEQUENCESTEPS];

/* index of the plugged subroutine doing ping-pong cache-flush DMEM accesses */
abe_uint32 abe_irq_pingpong_player_id;

/* base addresses of the ping pong buffers in bytes addresses */
abe_uint32 abe_base_address_pingpong[MAX_PINGPONG_BUFFERS];

/* size of each ping/pong buffers */
abe_uint32 abe_size_pingpong;

/* number of ping/pong buffer being used */
abe_uint32 abe_nb_pingpong;

/*
 * ABE_DEBUG DATA
 */

/* General circular buffer used to trace APIs calls and AE activity */
abe_uint32 abe_dbg_activity_log[DBG_LOG_SIZE];
abe_uint32 abe_dbg_activity_log_write_pointer;
abe_uint32 abe_dbg_mask;

/* Global variable holding parameter errors */
abe_uint32 abe_dbg_param;

/* Output of messages selector */
abe_uint32 abe_dbg_output;

/* Last parameters */
#define SIZE_PARAM      10

abe_uint32 param1[SIZE_PARAM];
abe_uint32 param2[SIZE_PARAM];
abe_uint32 param3[SIZE_PARAM];
abe_uint32 param4[SIZE_PARAM];
abe_uint32 param5[SIZE_PARAM];

#ifdef __cplusplus
}
#endif

#endif	/* _ABE_DAT_H_ */
