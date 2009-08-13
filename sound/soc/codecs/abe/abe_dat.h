/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_DAT.H
 *
 * All the visible API for the audio drivers
 *
 * @path 
 * @rev     01.00
 *! 
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */

 
#ifndef __abe_dat_def_
#define __abe_dat_def_

/*
 *      Call-backs
 */
/* 2 parameters subroutine pointers */
abe_subroutine2 callbacks [MAXCALLBACK];



/*
 *  HAL/FW ports status / format / sampling / protocol(call_back) / features /
 *  gain / name
 */

	abe_port_t abe_port [MAXNBABEPORTS];/* list of ABE ports */
const abe_port_t abe_port_init [MAXNBABEPORTS] ={

/* 
* Status    Data Format Drift  Call-Back     Protocol+selector  
* desc_addr; buf_addr;  buf_size;  iter; thr_low;  irq_addr  rq_data  DMA_T
* #Features reseted at start
* Port Name for the debug trace */
/* DMIC1 */
{	IDLE_P,
	{96000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_dmic1,
	0,
	{SNK_P, DMIC_PORT_PROT,{{dmem_dmic, dmem_dmic_size, DMIC_ITER}}},
	{0, 0},
	{EQDMIC1, 0},
	"DMIC_UL"
},
/* DMIC2 */
{
	IDLE_P,
	{96000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_dmic2,
	0,
	{SNK_P, DMIC_PORT_PROT, {{dmem_dmic, dmem_dmic_size, DMIC_ITER}}},
	{0, 0},
	{EQDMIC2, 0},"DMIC_UL"
},
/* DMIC3   */
{
	IDLE_P,
	{96000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_dmic3,
	0,
	{SNK_P, DMIC_PORT_PROT,{{dmem_dmic, dmem_dmic_size, DMIC_ITER}}},
	{0, 0},
	{EQDMIC3, 0},
	"DMIC_UL"
},
/* PDM_UL  */
{
	IDLE_P,
	{96000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_amic,
	0,
	{SNK_P, MCPDMUL_PORT_PROT,{{dmem_amic, dmem_amic_size, MCPDM_UL_ITER}}},
	{0, 0},
	{EQAMIC, 0},
	"AMIC_UL"
},
/* BT_VX_UL*/
{
	IDLE_P,
	{ 8000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_bt_vx_ul,
	0,
	{SNK_P, SERIAL_PORT_PROT, {{MCBSP1_DMA_TX*ATC_SIZE,dmem_bt_vx_ul,
		dmem_bt_vx_ul_size,   1 * SCHED_LOOP_8kHz,  DEFAULT_THR_WRITE}}},
	{0, 0},
	{0},
	"BT_VX_UL"
},
/* MM_UL   */
{
	IDLE_P,
	{48000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_mm_ul,
	0,
	{SRC_P, DMAREQ_PORT_PROT, 
	{{CBPr_DMA_RTX3*ATC_SIZE,dmem_mm_ul,dmem_mm_ul_size,
		10 * SCHED_LOOP_48kHz, DEFAULT_THR_WRITE,ABE_DMASTATUS_RAW,
		(1<<3)}}},
	{CIRCULAR_BUFFER_PERIPHERAL_R__3, 120},
	{UPROUTE, 0},
	"MM_UL"
},
/* MM_UL2  */
{
	IDLE_P,
	{48000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_mm_ul2,
	0,
	{SRC_P, DMAREQ_PORT_PROT,
		{{CBPr_DMA_RTX4*ATC_SIZE,dmem_mm_ul2,dmem_mm_ul2_size,
			2 * SCHED_LOOP_48kHz, DEFAULT_THR_WRITE,
			ABE_DMASTATUS_RAW,(1<<4)}}},
	{CIRCULAR_BUFFER_PERIPHERAL_R__4,  24},
	{UPROUTE, 0},
	"MM_UL2"
},
/* VX_UL   */
{
	IDLE_P,
	{ 8000, MONO_MSB},
	NODRIFT, NOCALLBACK,
	smem_vx_ul,
	0,
	{SRC_P, DMAREQ_PORT_PROT,
		{{CBPr_DMA_RTX2*ATC_SIZE,dmem_vx_ul,(dmem_vx_ul_size/2),
		1 * SCHED_LOOP_8kHz,  DEFAULT_THR_WRITE,ABE_DMASTATUS_RAW,
		(1<<2)}}},
	{CIRCULAR_BUFFER_PERIPHERAL_R__2,  4},
	{ASRC2, 0},
	"VX_UL"
},
/* MM_DL   */
{
	IDLE_P,
	{48000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_mm_dl,
	0,
	{SNK_P, DMAREQ_PORT_PROT, 
		{{CBPr_DMA_RTX0*ATC_SIZE,dmem_mm_dl,dmem_mm_dl_size,
			2 * SCHED_LOOP_48kHz, DEFAULT_THR_READ ,
			ABE_DMASTATUS_RAW,(1<<0)}}},
	{CIRCULAR_BUFFER_PERIPHERAL_R__0, 24},
	{ASRC3, 0},
	"MM_DL" },
/* VX_DL   */
{
	IDLE_P,
	{ 8000, MONO_MSB},
	NODRIFT, NOCALLBACK,
	smem_vx_dl,
	0,
	{SNK_P, DMAREQ_PORT_PROT,
		{{CBPr_DMA_RTX1*ATC_SIZE,dmem_vx_dl,dmem_vx_dl_size,
		1 * SCHED_LOOP_8kHz,  DEFAULT_THR_READ ,ABE_DMASTATUS_RAW,
		(1<<1)}}},
	{CIRCULAR_BUFFER_PERIPHERAL_R__1, 4},
	{ASRC1, 0},
	"VX_DL"
},
/* TONES_DL*/
{
	IDLE_P,
	{48000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_tones_dl,
	0,
	{SNK_P, DMAREQ_PORT_PROT, 
		{{CBPr_DMA_RTX5*ATC_SIZE,dmem_tones_dl,dmem_tones_dl_size,
		2 * SCHED_LOOP_48kHz, DEFAULT_THR_READ ,ABE_DMASTATUS_RAW,
		(1<<5)}}},
		{CIRCULAR_BUFFER_PERIPHERAL_R__5, 24},
	{0},
	"TONES_DL"
},
/* VIB_DL  */
{
	IDLE_P,
	{24000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	smem_vib,
	0,
	{SNK_P, DMAREQ_PORT_PROT,
		{{CBPr_DMA_RTX6*ATC_SIZE,dmem_vib_dl,dmem_vib_dl_size, 
		2 * SCHED_LOOP_24kHz, DEFAULT_THR_READ ,ABE_DMASTATUS_RAW,
		(1<<6)}}},   {CIRCULAR_BUFFER_PERIPHERAL_R__6, 12},
	{0},
	"VIB_DL"
},
/* BT_VX_DL*/
{
	IDLE_P,
	{ 8000, MONO_MSB},
	NODRIFT, NOCALLBACK,
	smem_bt_vx_dl,
	0,
	{SRC_P, SERIAL_PORT_PROT,
		{{MCBSP1_DMA_RX*ATC_SIZE,dmem_bt_vx_dl,dmem_bt_vx_dl_size,
		1 * SCHED_LOOP_8kHz,  DEFAULT_THR_WRITE}}},
	{0, 0},
	{0},
	"BT_VX_DL"
},
/* PDM_DL1 */
{
	IDLE_P,
	{96000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	0,
	0,
	{SRC_P, MCPDMDL_PORT_PROT, {{dmem_mcpdm, dmem_mcpdm_size}}},
	{0, 0},
	{MIXDL1, EQ1, APS1, 0},
	"PDM_DL1"
},
/* MM_EXT_OUT*/
{
	IDLE_P,
	{48000, STEREO_MSB},
	NODRIFT,
	NOCALLBACK,
	smem_mm_ext_out,
	0,
	{SRC_P, SERIAL_PORT_PROT,
		{{MCBSP1_DMA_RX*ATC_SIZE,dmem_mm_ext_out,dmem_mm_ext_out_size,
		2 * SCHED_LOOP_48kHz, DEFAULT_THR_WRITE}}},
	{0, 0},
	{0},
	"MM_EXT_OUT"
},
/* PDM_DL2 */
{
	IDLE_P,
	{96000, STEREO_MSB},
	NODRIFT,
	NOCALLBACK,
	0,
	0,
	{SRC_P, MCPDMDL_PORT_PROT,{{0,0,0,0,0,0,0}}},
	{0, 0},
	{MIXDL2, EQ2L, APS2L, EQ2R, APS2R, 0},
	"PDM_DL2"
},
/* PDM_VIB */
{
	IDLE_P,
	{24000, STEREO_MSB},
	NODRIFT, NOCALLBACK,
	0,
	0,
	{SRC_P, MCPDMDL_PORT_PROT, {{0,0,0,0,0,0,0}}},
	{0, 0},
	{0},
	"PDM_VIB"
},
/* SCHD_DBG_PORT*/
{
	RUN_P,
	{ 8000, MONO_MSB},
	NODRIFT,
	NOCALLBACK,
	0,
	0,
	{0, 0,{{0,0,0,0,0,0,0}}},
	{0, 0},
	{SEQUENCE, CONTROL, GAINS, 0},
	"SCHD_DBG"
},
};

/*
 * Firmware features
 */
	abe_feature_t all_feature [MAXNBFEATURE];
const abe_feature_t all_feature_init [] = {
	
/* ON_reset          OFF           READ              WRITE
   STATUS INPUT OUTPUT  SLOT/S OPP  NAME */
/* EQ1      */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq1, c_write_eq1, 0,
	0x1000, 0x1010, 2, 0, 1,"DLEQ1"
}, /* equalizer downlink path headset + earphone */
/* EQ2L     */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq2, c_write_eq2, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ2L"
}, /* equalizer downlink path integrated handsfree LEFT */
/* EQ2R     */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ2R"
}, /* equalizer downlink path integrated handsfree RIGHT */
/* EQSDT    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* equalizer downlink path side-tone */
/* EQDMIC1  */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* SRC+equalizer uplink DMIC 1st pair */
/* EQDMIC2  */
{ 
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* SRC+equalizer uplink DMIC 2nd pair */
/* EQDMIC3  */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* SRC+equalizer uplink DMIC 3rd pair */
/* EQAMIC   */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* SRC+equalizer uplink AMIC */
/* APS1     */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* Acoustic protection for headset */
/* APS2     */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* acoustic protection high-pass filter for handsfree "Left" */
/* APS3     */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* acoustic protection high-pass filter for handsfree "Right" */
/* ASRC1    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* asynchronous sample-rate-converter for the downlink voice path */
/* ASRC2    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* asynchronous sample-rate-converter for the uplink voice path */
/* ASRC3    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* asynchronous sample-rate-converter for the multimedia player */
/* MXDL1    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* mixer of the headset and earphone path */
/* MXDL2    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* mixer of the hands-free path */
/* MXSDT    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* mixer for side-tone */
/* MXECHO   */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* mixer for echo reference */
/* UPROUTE  */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* router of the uplink path */
/* GAINS    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* all gains */
/* EANC     */
{ 
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* active noise canceller */
/* VIB1     */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* noise shaper of the Vibra1 path */
/* GAINS    */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* all gains */
/* SEQ      */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* sequencing queue of micro tasks */
/* CTL      */
{
	c_feat_init_eq, c_feat_init_eq, c_feat_read_eq3, c_write_eq3, 0,
	0x1000, 0x1010, 2, 0, 1, "  DLEQ3"
}, /* Phoenix control queue through McPDM */
};


/*
 *  table indexed by the PORT_ID and returning the index to the D_IODescr table 
 *   of structures 
 */
const abe_uint32 sio_task_index [LAST_PORT_ID] = 
{
	/* AE sink ports - Uplink */
	7,          /* DMIC_PORT  - specific protocol */
	7,          /* DMIC_PORT  - specific protocol */
	7,          /* DMIC_PORT  - specific protocol */
	8,          /* PDM_UL_PORT - specific protocol */
	14,         /* BT_VX_UL_PORT BT uplink */

	/* AE source ports - Uplink */
	3,          /* MM_UL_PORT up to 5 stereo channels */
	4,          /* MM_UL2_PORT stereo FM record path */
	2,          /* VX_UL_PORT stereo FM record path */

	/* AE sink ports - Downlink */
	0,          /* MM_DL_PORT multimedia player audio path */
	1,          /* VX_DL_PORT */
	5,          /* TONES_DL_PORT */
	6,          /* VIB_DL_PORT */

	/* AE source ports - Downlink */
	13,         /* BT_VX_DL_PORT */
	9,          /* PDM_DL1_PORT */
	12,         /* MM_EXT_OUT_PORT */
	10,         /* PDM_DL2_PORT */
	11,         /* PDM_VIB_PORT */

	SCHD_DBG_PORT          /* dummy port used to declare the other tasks of
	the scheduler */

};


/*
 *  ports memory config with largest buffer allocated to the audio player--
 */
 const abe_uint32 abe_map_dmem_init_1 [] = { /* DMEM port map for the case #1*/
	/*      AMIC_UL_PORT    */      0x800,
	/*      DMIC_UL_PORT    */      0x820,
	/*      CTRL_PORT       */      0x840,
	/*      BT_VX_UL_PORT   */      0x880,
	/*      BT_VX_DL_PORT   */      0x8c0,
	/*      EANC_REF_PORT   */      0x900,
	/*      ECHOREF_UL_PORT */      0x940,
	/*      MM_DL_PORT      */      0x980,
	/*      MM_UL_PORT      */      0x9c0,
	/*      MM_EXT_OUT_PORT */      0x1000,
	/*      MM_EXT_DL_PORT  */      0x1001,
	/*      MM_EXT_UL_PORT  */      0x1002,
	/*      MM_VX_DL_PORT   */      0x1003,
	/*      MM_IHF_DL_PORT  */      0x1004,
	/*      TONES_PORT      */      0x1005,
	/*      VIB1_DL_PORT    */      0x1006,
	/*      VIB2_DL_PORT    */      0x1007,
	/*      VX_REC_PORT     */      0x1008,
	/*      VX_DL_PORT      */      0x1009,
	/*      VX_UL_PORT      */      0x100a,
	/*      PDM_DL1         */      0x1009,
	/*      PDM_DL2         */      0x100a,
};
const abe_uint32 abe_map_dmem_size_init_1 [] = {
	/* DMEM port buffer sizes  for the case #1*/
	/*      AMIC_UL_PORT    */      0x10,
	/*      DMIC_UL_PORT    */      0x10,
	/*      CTRL_PORT       */      0x10,
	/*      BT_VX_UL_PORT   */      0x10,
	/*      BT_VX_DL_PORT   */      0x10,
	/*      EANC_REF_PORT   */      0x10,
	/*      ECHOREF_UL_PORT */      0x10,
	/*      MM_DL_PORT      */      0x10,
	/*      MM_UL_PORT      */      0x10,
	/*      MM_EXT_OUT_PORT */      0x10,
	/*      MM_EXT_DL_PORT  */      0x10,
	/*      MM_EXT_UL_PORT  */      0x10,
	/*      MM_VX_DL_PORT   */      0x10,
	/*      MM_IHF_DL_PORT  */      0x10,
	/*      TONES_PORT      */      0x10,
	/*      VIB1_DL_PORT    */      0x10,
	/*      VIB2_DL_PORT    */      0x10,
	/*      VX_REC_PORT     */      0x10,
	/*      VX_DL_PORT      */      0x10,
	/*      VX_UL_PORT      */      0x10,
	/*      PDM_DL1         */      0x10,
	/*      PDM_DL2         */      0x10,
};
 
/*
 *      MEMORY MAPPING OF THE DMEM FIFOs 
 */
 
abe_uint32 abe_map_dmem [LAST_PORT_ID];			/* DMEM port map */
abe_uint32 abe_map_dmem_secondary [LAST_PORT_ID];
abe_uint32 abe_map_dmem_size [LAST_PORT_ID];	/* DMEM port buffer sizes */
 

/*
 * AESS/ATC destination and source address translation (except McASPs)
 * from the original 64bits words address
 */
const abe_uint32 abe_atc_dstid [ABE_ATC_DESC_SIZE>>3] =
{
	/* DMA_0   DMIC    PDM_DL  PDM_UL  McB1TX  McB1RX  McB2TX  McB2RX 0 7 */
		0,      0,      12,     0,      1,      0,      2,      0,
	/* McB3TX  McB3RX  SLIMT0  SLIMT1  SLIMT2  SLIMT3  SLIMT4  SLIMT5 8 15 */
		3,      0,      4,      5,      6,      7,      8,      9,
	/* SLIMT6  SLIMT7  SLIMR0  SLIMR1  SLIMR2  SLIMR3  SLIMR4  SLIMR5 16 23 */
		10,     11,     0,      0,      0,      0,      0,      0,
	/* SLIMR6  SLIMR7  McASP1X -----   -----   McASP1R -----   -----  24 31 */
		0,      0,      14,     0,      0,      0,      0,      0,
	/* CBPrT0  CBPrT1  CBPrT2  CBPrT3  CBPrT4  CBPrT5  CBPrT6  CBPrT7 32 39 */
		63,     63,     63,     63,     63,     63,     63,     63,
	/* CBP_T0  CBP_T1  CBP_T2  CBP_T3  CBP_T4  CBP_T5  CBP_T6  CBP_T7 40 47 */
		0,      0,      0,      0,      0,      0,      0,      0,
	/* CBP_T8  CBP_T9  CBP_T10 CBP_T11 CBP_T12 CBP_T13 CBP_T14 CBP_T15 48 63 */
		0,      0,      0,      0,      0,      0,      0,      0,
};

const abe_uint32 abe_atc_srcid [ABE_ATC_DESC_SIZE>>3] =
{
	/* DMA_0   DMIC    PDM_DL  PDM_UL  McB1TX  McB1RX  McB2TX  McB2RX 0 7 */
		0,      12,     0,      13,     0,      1,      0,      2,
	/* McB3TX  McB3RX  SLIMT0  SLIMT1  SLIMT2  SLIMT3  SLIMT4  SLIMT5 8 15 */
		0,      3,      0,      0,      0,      0,      0,      0,
	/* SLIMT6  SLIMT7  SLIMR0  SLIMR1  SLIMR2  SLIMR3  SLIMR4  SLIMR5 16 23 */
		0,      0,      4,      5,      6,      7,      8,      9,
	/* SLIMR6  SLIMR7  McASP1X -----   -----   McASP1R -----   ----- 24 31 */
		10,     11,     0,      0,      0,      14,     0,      0,
	/* CBPrT0  CBPrT1  CBPrT2  CBPrT3  CBPrT4  CBPrT5  CBPrT6  CBPrT7 32 39 */
		63,     63,     63,     63,     63,     63,     63,     63,
	/* CBP_T0  CBP_T1  CBP_T2  CBP_T3  CBP_T4  CBP_T5  CBP_T6  CBP_T7 40 47 */
		0,      0,      0,      0,      0,      0,      0,      0,
	/* CBP_T8  CBP_T9  CBP_T10 CBP_T11 CBP_T12 CBP_T13 CBP_T14 CBP_T15 48 63 */
		0,      0,      0,      0,      0,      0,      0,      0,
};



/*
 * Router tables
 */

	/* VX_UL, MM_UL2 and MM_UL router */
	abe_router_t abe_router_ul_table [NBROUTE_UL];
const	abe_router_t abe_router_ul_table_init_1 [NBROUTE_UL] = 
{
	ZERO_labelID, ZERO_labelID, ZERO_labelID, ZERO_labelID, /* 0  .. 3 */
	ZERO_labelID, ZERO_labelID, ZERO_labelID, ZERO_labelID, /* 4  .. 7 */
	ZERO_labelID, ZERO_labelID, ZERO_labelID, ZERO_labelID, /* 8  .. 11 */
	AMIC_L_labelID, AMIC_R_labelID, ZERO_labelID, ZERO_labelID, /* 12 15 */
};

/* 
 * ABE_GLOBAL DATA ------------------------------------------------------------
 */

/* flag, indicates the allowed control of Phoenix through McPDM slot #6 */
abe_uint32 abe_global_mcpdm_control;

/* 
 * ABE_DEBUG DATA -------------------------------------------------------------
 */

/*
 *  General circular buffer used to trace APIs calls and AE activity.
 */
abe_uint32 abe_dbg_activity_log [DBG_LOG_SIZE];
abe_uint32 abe_dbg_activity_log_write_pointer;
abe_uint32 abe_dbg_mask;

/*
 *  Global variable holding parameter errors
 */

abe_uint32 abe_dbg_param;

 
/*
 *  Output of messages selector
 */
abe_uint32 abe_dbg_output;

/*
 *  last parameters
 */

#define SIZE_PARAM      10

abe_uint32 param1[SIZE_PARAM];
abe_uint32 param2[SIZE_PARAM];
abe_uint32 param3[SIZE_PARAM];
abe_uint32 param4[SIZE_PARAM];
abe_uint32 param5[SIZE_PARAM];

#endif // abe_dat_def
