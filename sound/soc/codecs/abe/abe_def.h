/*
 * Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_DEF.H
 *
 * Constants definitions
 *
 * @path 
 * @rev     01.00
 *! 
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */

#ifndef __ABEDEF_
#define __ABEDEF_

/* 
 * HARDWARE AND PERIPHERAL DEFINITIONS 
 */

#define ABE_PMEM_SIZE                   8192
/* PMEM SIZE in bytes (1024 words of 64 bits: : #32bits words x 4)*/
#define ABE_CMEM_SIZE                   8192
/* CMEM SIZE in bytes (2048 coeff :             #32bits words x 4)*/
#define ABE_SMEM_SIZE                   24576
/* SMEM SIZE in bytes (3072 stereo samples :    #32bits words x 4)*/
#define ABE_DMEM_SIZE                   65536L
/* DMEM SIZE in bytes */
#define ABE_ATC_DESC_SIZE               512
/* ATC REGISTERS SIZE in bytes */

#define ABE_ABE_REVISION                0x00
/* revision number */
#define ABE_DUMMY_ADDRESS               ABE_ABE_REVISION
/* used as dummy register for generic IO tasks */
#define ABE_MCU_IRQSTATUS_RAW           0x24
/* holds the MCU Irq signal */
#define ABE_DSP_IRQSTATUS_RAW           0x4C
/* holds the DSP Irq signal */
#define ABE_DMASTATUS_RAW               0x84
/* holds the DMA req lines to the sDMA  */

#define EVENT_GENERATOR_COUNTER         0x68
#define EVENT_GENERATOR_COUNTER_DEFAULT 2046
/* see computation in "abe_event_generator_switch" */

#define EVENT_GENERATOR_START           0x6C
/* start / stop the EVENT generator */
#define EVENT_GENERATOR_ON              1
#define EVENT_GENERATOR_OFF             0

#define EVENT_SOURCE_SELECTION          0x70
/* selection of the EVENT generator source */
#define EVENT_SOURCE_DMA                0
#define EVENT_SOURCE_COUNTER            1

#define AUDIO_ENGINE_SCHEDULER          0x74
/* selection of the ABE DMA req line from ATC */
#define ABE_ATC_DMIC_DMA_REQ            1
#define ABE_ATC_MCPDMDL_DMA_REQ         2
#define ABE_ATC_MCPDMUL_DMA_REQ         3

#define ABE_ATC_DIRECTION_IN            0
/* Direction=0 means input from ABE point of view */
#define ABE_ATC_DIRECTION_OUT           1
/* Direction=1 means output from ABE point of view */

#define ABE_DMAREQ_REGISTER(desc) \
(abe_uint32 *)((desc/8) + CIRCULAR_BUFFER_PERIPHERAL_R__0)


#define ABE_CBPR0_IDX		0	/* MM_DL */
#define ABE_CBPR1_IDX		1	/* VX_DL */
#define ABE_CBPR2_IDX		2	/* VX_UL */
#define ABE_CBPR3_IDX		3	/* MM_UL */
#define ABE_CBPR4_IDX		4	/* MM_UL2 */
#define ABE_CBPR5_IDX		5	/* TONES */
#define ABE_CBPR6_IDX		6	/* VIB */
#define ABE_CBPR7_IDX		7	/* DEBUG/CTL */

#define CIRCULAR_BUFFER_PERIPHERAL_R__0 \
	(0x100                           + ABE_CBPR0_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__1 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR1_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__2 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR2_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__3 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR3_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__4 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR4_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__5 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR5_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__6 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR6_IDX*4)
#define CIRCULAR_BUFFER_PERIPHERAL_R__7 \
	(CIRCULAR_BUFFER_PERIPHERAL_R__0 + ABE_CBPR7_IDX*4)


/*
 * DMA requests
 */
#define External_DMA_0	        0
#define DMIC_DMA_REQ	        1	/*tx request digital microphone */
#define McPDM_DMA_DL	        2	/*Multichannel PDM downlink */
#define McPDM_DMA_UP	        3	/*Multichannel PDM uplink */
#define MCBSP1_DMA_TX	        4	/*MCBSP module 1 - tx request*/
#define MCBSP1_DMA_RX	        5	/*MCBSP module 1 - rx request */
#define MCBSP2_DMA_TX	        6	/*MCBSP module 2 - tx request */
#define MCBSP2_DMA_RX	        7	/*MCBSP module 2 - rx request */
#define MCBSP3_DMA_TX	        8	/*MCBSP module 3 - tx request */
#define MCBSP3_DMA_RX	        9	/*MCBSP module 3 - rx request */
#define SLIMBUS1_DMA_TX0        10	/*SLIMBUS mod1 - tx request channel 0 */
#define SLIMBUS1_DMA_TX1        11	/*SLIMBUS mod1 - tx request channel 1 */
#define SLIMBUS1_DMA_TX2        12	/*SLIMBUS mod1 - tx request channel 2 */
#define SLIMBUS1_DMA_TX3        13	/*SLIMBUS mod1 - tx request channel 3 */
#define SLIMBUS1_DMA_TX4        14	/*SLIMBUS mod1 - tx request channel 4 */
#define SLIMBUS1_DMA_TX5        15	/*SLIMBUS mod1 - tx request channel 5 */
#define SLIMBUS1_DMA_TX6        16	/*SLIMBUS mod1 - tx request channel 6 */
#define SLIMBUS1_DMA_TX7        17	/*SLIMBUS mod1 - tx request channel 7 */
#define SLIMBUS1_DMA_RX0        18	/*SLIMBUS mod1 - rx request channel 0 */
#define SLIMBUS1_DMA_RX1        19	/*SLIMBUS mod1 - rx request channel 1 */
#define SLIMBUS1_DMA_RX2        20	/*SLIMBUS mod1 - rx request channel 2 */
#define SLIMBUS1_DMA_RX3        21	/*SLIMBUS mod1 - rx request channel 3 */
#define SLIMBUS1_DMA_RX4        22	/*SLIMBUS mod1 - rx request channel 4 */
#define SLIMBUS1_DMA_RX5        23	/*SLIMBUS mod1 - rx request channel 5 */
#define SLIMBUS1_DMA_RX6        24	/*SLIMBUS mod1 - rx request channel 6 */
#define SLIMBUS1_DMA_RX7        25	/*SLIMBUS mod1 - rx request channel 7 */
#define McASP1_AXEVT            26	/*McASP  - Data tx DMA request line */
#define McASP1_AREVT	        29	/*McASP  - Data receive DMA req line */
#define CBPr_DMA_RTX0	        32	/*DMA of the Cbuf peripheral 0 */
#define CBPr_DMA_RTX1	        33	/*DMA of the Cbuf peripheral 1 */
#define CBPr_DMA_RTX2	        34	/*DMA of the Cbuf peripheral 2 */
#define CBPr_DMA_RTX3	        35	/*DMA of the Cbuf peripheral 3 */
#define CBPr_DMA_RTX4	        36	/*DMA of the Cbuf peripheral 4 */
#define CBPr_DMA_RTX5	        37	/*DMA of the Cbuf peripheral 5 */
#define CBPr_DMA_RTX6	        38	/*DMA of the Cbuf peripheral 6 */
#define CBPr_DMA_RTX7	        39	/*DMA of the Cbuf peripheral 7 */

/*
 * ATC DESCRIPTORS - DESTINATIONS
 */
#define DEST_DMEM access        0x00
#define DEST_MCBSP1_ TX	        0x01
#define DEST_MCBSP2_ TX	        0x02
#define DEST_MCBSP3_TX	        0x03
#define DEST_SLIMBUS1_TX0       0x04
#define DEST_SLIMBUS1_TX1       0x05
#define DEST_SLIMBUS1_TX2       0x06
#define DEST_SLIMBUS1_TX3       0x07
#define DEST_SLIMBUS1_TX4       0x08
#define DEST_SLIMBUS1_TX5       0x09
#define DEST_SLIMBUS1_TX6       0x0A
#define DEST_SLIMBUS1_TX7       0x0B
#define DEST_MCPDM_DL           0x0C
#define DEST_MCASP_TX0          0x0D
#define DEST_MCASP_TX1          0x0E
#define DEST_MCASP_TX2          0x0F
#define DEST_MCASP_TX3          0x10
#define DEST_EXTPORT0           0x11
#define DEST_EXTPORT1           0x12
#define DEST_EXTPORT2           0x13
#define DEST_EXTPORT3           0x14
#define DEST_MCPDM_ON           0x15
#define DEST_CBP_CBPr           0x3F

/*
 * ATC DESCRIPTORS - SOURCES
 */
#define SRC_DMEM access         0x0
#define SRC_MCBSP1_ RX          0x01
#define SRC_MCBSP2_RX           0x02
#define SRC_MCBSP3_RX           0x03
#define SRC_SLIMBUS1_RX0        0x04
#define SRC_SLIMBUS1_RX1        0x05
#define SRC_SLIMBUS1_RX2        0x06
#define SRC_SLIMBUS1_RX3        0x07
#define SRC_SLIMBUS1_RX4        0x08
#define SRC_SLIMBUS1_RX5        0x09
#define SRC_SLIMBUS1_RX6        0x0A
#define SRC_SLIMBUS1_RX7        0x0B
#define SRC_DMIC_UP             0x0C
#define SRC_MCPDM_UP            0x0D
#define SRC_MCASP_RX0           0x0E
#define SRC_MCASP_RX1           0x0F
#define SRC_MCASP_RX2           0x10
#define SRC_MCASP_RX3           0x11
#define SRC_CBP_CBPr            0x3F

/* 
 *INTERNAL DEFINITIONS
 */

#define NBEANC1                 20 /* 20 Q6.26 coef for the FIR    */
#define NBEANC2                 16 /* 16 Q6.26 coef for the IIR    */

#define NBEQ1                   16 /* 16 Q6.26 coefficients        */
#define NBEQ2                   8 /* 2x8 Q6.26 coefficients       */

#define NBAPS1                  10 /* TBD APS first set of parameters      */
#define NBAPS2                  10 /* TBD APS second set of parameters      */

#define NBMIX_AUDIO_UL          2 /* Mixer for send tones to the ul vx path */
#define NBMIX_DL1               4 /* Main downlink mixer */
#define NBMIX_DL2               4 /* Handsfree downlink mixer */
#define NBMIX_SDT               2 /* Side-tone mixer */
#define NBMIX_ECHO              2 /* Echo reference mixer */
#define NBMIX_VXREC             4 /* Voice record mixer */
#define ABE_MAXNBGAIN_MIXER     4 

#define NBROUTE_UL              16 /* nb of samples to route */

#define UC1_LP                  1 /* MP3 low-power player use-case */
#define UC2_VOICE_CALL_AND_IHF_MMDL 2 /* en vx ul/dl on ep + MM_DL on IHF */
#define UC3_VOICE_CALL          3 /* basic voice-call use-case */
#define UC4_EANC                4 /* EANC test */


#define COPY_FROM_ABE_TO_HOST   1 /* ID used for LIB memory copy subroutines */
#define COPY_FROM_HOST_TO_ABE   2

#define ABE_PMEM                1
#define ABE_CMEM                2
#define ABE_SMEM                3
#define ABE_DMEM                4
#define ABE_ATC                 5

#define MAXCALLBACK             100 /* call-back indexes */
#define MAXNBSEQUENCE           100 /* time controlled sequenced */
#define MAXFEATUREPORT          8 /* max num of feature associated to a port */

#define NBCHARFEATURENAME       16
#define NBCHARPORTNAME          16
#define MAXNBABEPORTS           LAST_PORT_ID
#define MAX_MAP_DMEM            LAST_PORT_ID

#define MAXLINESEQUENCE         32

#define SNK_P                   ABE_ATC_DIRECTION_IN
#define SRC_P                   ABE_ATC_DIRECTION_OUT

#define NODRIFT                 0
#define GAIN_0dB                0

#define IDLE_P                  1
#define RUN_P                   2
#define NOCALLBACK              0
#define NOPARAMETER             0

#define MCPDM_UL_ITER           2
#define MCPDM_DL_ITER           6
#define DMIC_ITER               6


#define DEFAULT_THR_READ        1
#define DEFAULT_THR_WRITE       1
              

#define DEFAULT_CONTROL_MCPDMDL 1

/*
 * macros
 */

#define LOAD_ABEREG(reg,data) \
	{abe_uint32 *ocp_addr = (abe_uint32 *)((reg)+ABE_ATC_BASE_ADDRESS_MPU);\
	*ocp_addr= (data);}

#define MAXIMUM(a,b)	(((a)<(b))?(b):(a))


#endif /* ifndef ABEDEF */
