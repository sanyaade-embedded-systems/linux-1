/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_TYP.H
 *
 * AESS/ABE types
 *
 * @path 
 * @rev     01.00
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */


#ifndef __ABETYP_
#define __ABETYP_

/* 
 *     basic types
 */

typedef char    abe_flag;
typedef unsigned char abe_uint8;
typedef char abe_int8;
typedef unsigned short abe_uint16;
typedef short   abe_int16;
typedef unsigned long abe_uint32;
typedef long abe_int32;

typedef abe_uint32 abe_errc_t;

/*
 *      OPENSL_ES TYPES
 */

typedef abe_int8    SLint8;
typedef abe_uint8   SLuint8;
typedef abe_int16   SLint16;
typedef abe_uint16  SLuint16;
typedef abe_int32   SLint32;
typedef abe_uint32  SLuint32;

typedef SLint16 abe_millibel;
typedef SLuint32 abe_millisecond;
typedef SLuint32 abe_milliHertz;
typedef SLint32 abe_millimeter;
typedef SLint32 abe_millidegree;
typedef SLint16 abe_permille;
typedef SLuint32 abe_microsecond;
typedef SLuint32 abe_result;

typedef abe_int16   abe_gain_t; /* 2 bytes	millibel (OPENSL_ES)    */
typedef abe_int32   abe_freq_t; /* 4 bytes	millihertz              */
typedef abe_int32   abe_millis_t; /* 4 bytes	milliseconds            */
typedef abe_int32   abe_micros_t; /* 4 bytes	microseconds            */

typedef abe_uint32  abe_dbg_mask_t;
/* 4 bytes	Bit field indicating the type of informations to be traced */
typedef abe_uint32  abe_time_stamp_t;
/* 4 bytes	infinite loop 32bits counter incremented on each firmware loop */
/* sched task loops (250us / 272us with respectively 48kHz/44.1kHz on Phoenix)*/

typedef abe_int32 abe_fw_code_id; /* OMX type */
typedef abe_int32 abe_dbg_t; /* debug filter */

typedef void (* abe_subroutine0) (void); /* subroutine with two parameters */
typedef void (* abe_subroutine2) (abe_uint32, abe_uint32);
/* subroutine with two parameters */
typedef void (* abe_subroutine4) (abe_uint32, abe_uint32, abe_uint32,
					abe_uint32);
/* subroutine with four parameters */
/*  
 *      CODE PORTABILITY - FUTURE PATCHES
 *
 *      32bits field for having the code compatible with future revisions of
 *      the hardware (audio integration) or evolution of the software
 *      partitionning. Used for the highest level APIs (launch_sequences)
 */
typedef abe_uint32 abe_patch_rev;

/* 
 * enums
 */
 
/*
 *MEMORY CONFIG TYPE
 *
 *         0: Ultra Lowest power consumption audio player 
 *         1: OPP 25% (simple multimedia features)
 *         2: OPP 50% (multimedia and voice calls)
 *         3: OPP100% (EANC, multimedia complex use-cases)
 */
typedef enum {
		OMAP4_1 = 1,
		OMAP4_2 
	} abe_memconf_t;


/*      
 *      OPP TYPE
 *
 *         0: Ultra Lowest power consumption audio player 
 *         1: OPP 25% (simple multimedia features)
 *         2: OPP 50% (multimedia and voice calls)
 *         3: OPP100% (EANC, multimedia complex use-cases)
 */
typedef enum {
		OPP0 = 0,
		OPP25, OPP50, OPP100
	} abe_opp_t;


/*      
 *      DMIC DECIMATION RATIO
 *
 */
typedef enum {
		DEC16 = 16,
		DEC25 = 25,
		DEC32 = 32,
		DEC40 = 40
} abe_dmic_ratio_t;

/*      
 *      SEQUENCE CODE TYPE
 *
 *      Index to the table of subroutines called in the sequence
 */
typedef enum    { S1 = 1,
                } abe_seq_code_t;


/*      
 *      SAMPLES TYPE
 *
 *	mono 16bits sample LSB aligned, 16 MSB bits are unused
 *	mono sample MSB aligned (16/24/32bits)
 *      two successive mono samples in one 32bits container
 *	Two L/R 16bits samples in a 32bits container,  
 *	Two channels defined with two MSB aligned samples
 *	Three channels defined with three MSB aligned samples (MIC)
 *	Four channels defined with four MSB aligned samples (MIC)
    . . . 
 *	Eight channels defined with six MSB aligned samples (MIC)
 */
typedef enum {
		MONO_16_LSB = 1,
		MONO_MSB, TWO_MONO_MSB, STEREO_16_16,
		STEREO_MSB, THREE_MSB, FOUR_MSB, FIVE_MSB,
		SIX_MSB, SEVEN_MSB, EIGHT_MSB
} abe_samp_t;

/* 
 *      PORT PROTOCOL TYPE
 */
typedef enum {
		SLIMBUS_PORT_PROT = 1,
		SERIAL_PORT_PROT,
		DMIC_PORT_PROT,
		MCPDMDL_PORT_PROT,
		MCPDMUL_PORT_PROT, 
		PINGPONG_PORT_PROT,
		DMAREQ_PORT_PROT,
		FIFO_PORT_PROT, 
		CIRCULAR_PORT_PROT, 
} abe_port_protocol_switch_id;

/* 
 *      PORT IDs
 */
typedef enum   {
/* AE sink ports - Uplink */
	DMIC_PORT1,             /* digital microphones pairs */
	DMIC_PORT2,              
	DMIC_PORT3,          
	PDM_UL_PORT,            /* analog MICs */
	BT_VX_UL_PORT,          /* BT uplink */

	/* AE source ports - Uplink */
	MM_UL_PORT,             /* up to 5 stereo channels */
	MM_UL2_PORT,            /* stereo FM record path */
	VX_UL_PORT,             /* stereo FM record path */

	/* AE sink ports - Downlink */
	MM_DL_PORT,             /* multimedia player audio path */
	VX_DL_PORT,
	TONES_DL_PORT,
	VIB_DL_PORT,

	BT_VX_DL_PORT,          /* AE source ports - Downlink */
	PDM_DL1_PORT,
	MM_EXT_OUT_PORT,
	PDM_DL2_PORT,
	PDM_VIB_PORT,

	SCHD_DBG_PORT,          /* dummy port used to declare the other tasks
	of the scheduler */

	LAST_PORT_ID

} abe_port_id;


/* 
 *      GAIN IDs
 */
typedef enum {
	VX_DL_IN_GAIN = 1,                             // mixer's gain
	MM_DL_IN_GAIN,           
	TONES_DL_IN_GAIN,        
	MM_VX_DL_IN_GAIN,        
	MM_IHF_DL_IN_GAIN,                              // mixer's gain
	MM_HS_DL_OUT_GAIN,                              // Output Left gain
	MM_IHF_L_DL_OUT_GAIN,                           // Output Left gain
	MM_IHF_R_DL_OUT_GAIN,                           // Output Right gain
	MM_VIB1_DL_GAIN,           MM_VIB2_DL_GAIN,     // no gain in fact ...
	DMIC_UL_IN_GAIN_0,         DMIC_UL_IN_GAIN_1,   // = GAIN on DMIC pairs
	DMIC_UL_IN_GAIN_2,         DMIC_UL_IN_GAIN_3,
	DMIC_UL_IN_GAIN_4,         DMIC_UL_IN_GAIN_5,
	AMIC_UL_IN_GAIN_L,         AMIC_UL_IN_GAIN_R,   // = gain on AMIC pair
	ECHO_REF_GAIN,        
	BT_VX_DL_OUT_GAIN,
	BT_VX_UL_IN_GAIN,
} abe_gain_id;

/* 
 *      ANA_PORT_ID     Analog companion audio port
 */
typedef enum    {
	EAR_PHOENIX = 1,
	HS_L,   HS_R,
	IHF_L,  IHF_R,
	VIBRA1, VIBRA2
} abe_ana_port_id ;

typedef abe_int32 headset_offset_t; /* Calibration data from the anlg companion */

/* 
 *      Signal processing module names - EQ APS MIX ROUT
 */

#define FEAT_EQ1        1                       
/* equalizer downlink path headset + earphone */
#define FEAT_EQ2L       FEAT_EQ1+1              
/* equalizer downlink path integrated handsfree LEFT */
#define FEAT_EQ2R       FEAT_EQ2L+1             
/* equalizer downlink path integrated handsfree RIGHT */
#define FEAT_EQVIB1     FEAT_EQ2R+1             
/* equalizer downlink path vibra 1 */
#define FEAT_EQVIB2     FEAT_EQVIB1+1           
/* equalizer downlink path vibra 2 */
#define FEAT_EQSDT      FEAT_EQVIB2+1          
/* equalizer downlink path side-tone */
#define FEAT_EQDMIC1    FEAT_EQSDT+1           
/* equalizer uplink path first DMIC pair */
#define FEAT_EQDMIC2    FEAT_EQDMIC1+1          
/* equalizer uplink path second DMIC pair */
#define FEAT_EQDMIC3    FEAT_EQDMIC2+1          
/* equalizer uplink path third DMIC pair */
#define FEAT_EQAMIC     FEAT_EQDMIC3+1          
/* equalizer uplink path AMIC pair */
#define FEAT_APS1       FEAT_EQAMIC+1           
/* Acoustic protection for headset */
#define FEAT_APS2       FEAT_APS1+1             
/* acoustic protection high-pass filter for handsfree "Left" */
#define FEAT_APS3       FEAT_APS2+1             
/* acoustic protection high-pass filter for handsfree "Right" */
#define FEAT_ASRC1      FEAT_APS3+1             
/* asynchronous sample-rate-converter for the downlink voice path */
#define FEAT_ASRC2      FEAT_ASRC1+1            
/* asynchronous sample-rate-converter for the uplink voice path */
#define FEAT_ASRC3      FEAT_ASRC2+1            
/* asynchronous sample-rate-converter for the multimedia player */
#define FEAT_ASRC4      FEAT_ASRC3+1            
/* asynchronous sample-rate-converter for the echo reference */
#define FEAT_MIXDL1     FEAT_ASRC4+1            
/* mixer of the headset and earphone path */
#define FEAT_MIXDL2     FEAT_MIXDL1+1           
/* mixer of the hands-free path */
#define FEAT_MIXAUDUL   FEAT_MIXDL2+1           
/* mixer for audio being sent on the voice_ul path */
#define FEAT_MIXVXREC   FEAT_MIXAUDUL+1         
/* mixer for voice communication recording */
#define FEAT_MIXSDT     FEAT_MIXVXREC+1         
/* mixer for side-tone */
#define FEAT_MIXECHO    FEAT_MIXSDT+1           
/* mixer for echo reference */
#define FEAT_UPROUTE    FEAT_MIXECHO+1          
/* router of the uplink path */
#define FEAT_GAINS      FEAT_UPROUTE+1          
/* all gains */
#define FEAT_EANC       FEAT_GAINS+1            
/* active noise canceller */
#define FEAT_SEQ        FEAT_EANC+1             
/* sequencing queue of micro tasks */
#define FEAT_CTL        FEAT_SEQ+1              
/* Phoenix control queue through McPDM */

#define MAXNBFEATURE    FEAT_CTL                
/* list of features of the firmware -------------------------------*/



typedef enum {
	EQ1 =   FEAT_EQ1,               /* equalizer downlink path headset + earphone */
	EQ2L =  FEAT_EQ2L,              /* equalizer downlink path integrated handsfree LEFT */
	EQ2R =  FEAT_EQ2R, 
	EQSDT = FEAT_EQSDT,             /* equalizer downlink path side-tone */
	EQDMIC1 = FEAT_EQDMIC1,
	EQDMIC2 = FEAT_EQDMIC2,
	EQDMIC3 = FEAT_EQDMIC3,
	EQAMIC = FEAT_EQAMIC,           /* equalizer uplink path AMIC pair */
} abe_equ_id;

typedef enum    {
	APS1 = FEAT_APS1,               /* Acoustic protection for headset */
	APS2L = FEAT_APS2, 
	APS2R = FEAT_APS3
} abe_aps_id;

typedef enum    {
	ASRC1 = FEAT_ASRC1,             /* asr-converter for the vxdl path */
	ASRC2 = FEAT_ASRC2,             /* asr-converter for the vxul path */
	ASRC3 = FEAT_ASRC3,             /* asr-converter for the mm player */
	ASRC4 = FEAT_ASRC4,             /* asr-converter for the vxul echo_ref*/
} abe_asrc_id;  

typedef enum    {
	MIXDL1 = FEAT_MIXDL1, 
	MIXDL2 = FEAT_MIXDL2, 
	MIXSDT = FEAT_MIXSDT, 
	MIXECHO = FEAT_MIXECHO, 
	MIXEANC = FEAT_EANC,
	MIXAUDUL = FEAT_MIXAUDUL,   
	MIXVXREC = FEAT_MIXVXREC,
} abe_mixer_id; 

typedef enum    {
	UPROUTE = FEAT_UPROUTE,
} abe_router_id;

typedef enum    {
	GAINS = FEAT_GAINS, /* Misc tasks of the scheduler */
	SEQUENCE = FEAT_SEQ, 
	CONTROL = FEAT_CTL
	} abe_schd_id;
/* 
 *      EVENT GENERATORS
 */
typedef enum    {
	EVENT_MCPDM = 1,
	EVENT_DMIC, EVENT_TIMER,
	EVENT_McBSP, EVENT_McASP, EVENT_SLIMBUS, EVENT_DEFAULT,
} abe_event_id;

/* 
 * types for API
 */



/*
 *      GAIN_PORT_T
 *
 *      6 bytes	The gains on ports have a programmable slope. This slope corresponds to a 
 *      first order IIR filter. The micros_t field is translated to an IIR coefficient by the HAL.
 */
typedef struct {
	abe_gain_t g0;
	abe_micros_t ramp_time;
} abe_gain_port_t;


/*
 *      EANC_T
 *
 *      TBD : coefficients of the EANC
 */
typedef struct {
	abe_int32 dmic_index;
	abe_int32 fir_coef [NBEANC1];
	abe_int32 lambda;
	abe_int32 iir_filter [NBEANC2]; 
	abe_int32 loop_gain;
} abe_eanc_t;


/*
 *      EQU_T
 *
 *      coefficients of the equalizer
 */
typedef struct {
	abe_uint8 equ_param1; 
	abe_uint8 equ_param2; 
	union {	/* parameters are the direct and recursive coefficients in */
		abe_int32 type1 [NBEQ1]; /* Q6.26 integer fixed-point format. */
		struct {
			abe_int32 freq [NBEQ2];/* params are the freq 
						(type "freq") and Q factors */
			abe_int32 gain [NBEQ2];  /* (type "gain") of each band. */
		} type2;
	} coef;
	abe_int32 equ_param3; 
} abe_equ_t;

/*
 *      APS_T
 *
 *      coefficients of the Acoustics Protection and Safety
 */
typedef struct {
	abe_int32 coef1 [NBAPS1];
	abe_int32 coef2 [NBAPS2];
} abe_aps_t;

typedef struct {
	abe_millibel e1;
	abe_millibel e2;
} abe_aps_energy_t;	



/*
 *      MIXER_T
 *
 *      MIXER coeffs, Table of gain_t for the input gains of the mixer.
 */
typedef struct {
	abe_gain_t gl [ABE_MAXNBGAIN_MIXER];
} abe_mixer_t;


/*
 *      ROUTER_T
 *
 *      table of indexes in unsigned bytes      
 */
typedef abe_uint8 abe_router_t;


/*
 *      DATA_FORMAT_T
 *
 *      used in port declaration 
 */
typedef struct {
	abe_freq_t f;	/* Sampling frequency of the stream */
	abe_samp_t samp_format;     /* Sample format type  */
} abe_data_format_t;        


/*
 *      PORT_PROTOCOL_T
 *
 *      port declaration 
 */
typedef struct {
	abe_uint32 direction;
	/* Direction=0 means input from AESS point of view */
	abe_port_protocol_switch_id protocol_switch;
	/* Protocol type (switch) during the data transfers */
		union {
			struct { /* Slimbus peripheral connected to ATC */
				abe_uint32 desc_addr1;
				/* Address of ATC Slimbus descriptor's index */
				abe_uint32 desc_addr2;      
				/* Snd ATC index for SlimBus rx (or NULL) */
				abe_uint32 buf_addr1;
				/* DMEM address 1 in bytes */
				abe_uint32 buf_addr2;
				/* DMEM address 2 in bytes */
				abe_uint32 buf_size;
				/* DMEM buffer size size in bytes */
                                abe_uint32 iter;
				/* ITERation on each DMAreq signals */
                                abe_uint32 thr_flow;
				/* Data threshold for flow management */
                        } prot_slimbus;  

                        struct {
                                abe_uint32 desc_addr;
				/* McBSP/McASP peripheral connected to ATC */
                                abe_uint32 buf_addr;
				/* Address of ATC McBSP/McASP descriptor's in bytes */
                                abe_uint32 buf_size;
				/* DMEM address in bytes */
                                abe_uint32 iter;
				/* ITERation on each DMAreq signals */
                                abe_uint32 thr_flow;
				/* Data threshold for flow management */
                        } prot_serial;

                        struct {/* DMIC peripheral connected to ATC */
                                abe_uint32 buf_addr;
				/* DMEM address in bytes */
                                abe_uint32 buf_size;
				/* DMEM buffer size in bytes */
                                abe_uint32 nbchan;
				/* Number of activated DMIC */
                        } prot_dmic;

                        struct {/* McPDMDL peripheral connected to ATC */
                                abe_uint32 buf_addr;
				/* DMEM address in bytes */
                                abe_uint32 buf_size;
				/* DMEM size in bytes */
                                abe_uint32 control;
				/* Control allowed on McPDM DL */
                        } prot_mcpdmdl;

                        struct {/* McPDMUL peripheral connected to ATC */
                                abe_uint32 buf_addr;
				/* DMEM address size in bytes */
                                abe_uint32 buf_size;
				/* DMEM buffer size size in bytes */
                        } prot_mcpdmul;

                        struct {/* Ping-Pong iface to the Host using cache-flush */
                                abe_uint32 cb_addr;
				/* Address of the read/write pointers + size */
                                abe_uint32 buf_addr;
				/* DMEM buffer base address in bytes */
                                abe_uint32 buf_size;
				/* DMEM buffer size in bytes */
                                abe_uint32 thr_irq;
				/* Data thrshold before sending an IRQ to host or DMA */
                                abe_uint32 irq_addr;
				/* IRQ address (either DMA (0) MCU (1) or DSP(2)) */
                                abe_uint32 irq_data;
				/* IRQ data content loaded in the AESS IRQ register */
                                abe_uint32 callback;
				/* Call-back function upon IRQ reception */
                        } prot_pingpong;

                        struct {/* DMAreq line to CBPr */
                                abe_uint32 desc_addr;
				/* Address of ATC descriptor's */
                                abe_uint32 buf_addr;
				/* DMEM buffer address in bytes */
                                abe_uint32 buf_size;
				/* DMEM buffer size size in bytes */
                                abe_uint32 iter;
				/* ITERation on each DMAreq signals */
                                abe_uint32 thr_flow;
				/* Data threshold for flow management */
                                abe_uint32 dma_addr;
				/* DMAreq address */
                                abe_uint32 dma_data;
				/* DMA/AESS = 1 << #DMA */
                        } prot_dmareq;

                        struct {/* DMAreq line for large Ping-Pong buffers */
                                abe_uint32 buf_addr;
				/* DMEM buffer address in bytes */
                                abe_uint32 buf_size;
				/* DMEM buffer size size in bytes */
                                abe_uint32 irq_data;
				/* DMA/AESS = 1 << #DMA */
                        } prot_pingpong_lp;

 
                        struct {/* Circular buffer - dirt addressing to DMEM */
                                abe_uint32 desc_addr;
				/* Address of the read/write pointers */
                                abe_uint32 buf_addr;
				/* DMEM buffer base address in bytes */
                                abe_uint32 buf_size;
				/* DMEM buffer size in bytes */
                                abe_uint32 thr_irq;
				/* Data threshold before sending an IRQ to host or DMA */
                                abe_uint32 irq_addr;
				/* IRQ address (either MCU or DSP) */
                                abe_uint32 irq_data;
				/* IRQ data content loaded in the AESS IRQ register */
                                abe_uint32 callback;
				/* Call-back function upon IRQ reception */
                        } prot_circular_buffer;
                }p;
        } abe_port_protocol_t;



/*
 *      IO_task_descriptor
 *
 */
typedef struct {
	char type;/* (0 = disabled, R/W, ATC,PingPong,DMAreq,Slimbus) */
	char format;/* data formats */
	char smem_buffer;
	char cmem_gain; 
	char atc1;
	char atc2;
	char n;
	char IRQ;
	char thr1;
	char thr2;
	char dma_address;
	char dma_data;
	char irq_route;
	char asrc;
} abe_io_task_descriptor_t;


/*
 *      DMA_T
 *
 *      dma structure for easing programming 
 */
typedef struct {
	void *data; /* Pointer to the first address of the */
	/* destination buffer (either DMA or Ping-Pong read/write pointers). */
	abe_uint32 iter;/* number of iterations for the DMA data moves. */
} abe_dma_t;

typedef struct {
	abe_uint32 data; /* Offset to the first address of the */
	abe_uint32 iter; /* number of iterations for the DMA data moves. */
} abe_dma_t_offset;

/*
 *  SEQ_T
 *
 *      struct {
 *      micros_t time;          Waiting time before executing next line
 *      seq_code_t code         Subroutine index interpreted in the HAL and 
 *      		 translated to 
 *                                FW subroutine codes in case of ABE tasks
 *      int32 param[2]          Two parameters
 *      } seq_t	
 * 
 */ 	

typedef struct {
	abe_micros_t time;
	abe_seq_code_t code;
	abe_int32 param1;
	abe_int32 param2;
} abe_seq_t;


/*
 *      DRIFT_T
 *
 *      ASRC drift parameter in [ppm] value
 */
typedef abe_int32 abe_drift_t;

/*
 *      ABE_PORT_T status / format / sampling / protocol(call_back) / features 
 *      / gain / name .. 
 *
 */

typedef struct {
	abe_uint16  status;	/* running / idled */
	abe_data_format_t format;	/* Sample format type  */
	abe_drift_t drift;	/* API : for ASRC */
	abe_uint16 callback;	/* opt call-back indx for errors and ack */
	abe_uint16 smem_buffer1;	/* IO tasks buffers */
	abe_uint16 smem_buffer2;
	abe_port_protocol_t protocol;
	abe_dma_t_offset dma;/* ptr and iteration countr of the xDMA */
	abe_uint16 feature_index [MAXFEATUREPORT];
		/* list of features associated to a port (EQ, APS, ... , ends with 0) */
#if 0
	abe_millibel gain_calibration;/* gain tuning, default=0dB */
		
#endif
	char name[NBCHARPORTNAME];
} abe_port_t;



/*
 *      ABE_FEATURE_T
 *
 */

typedef struct {
	abe_uint16 enable_with_default_data;         
	abe_uint16 disable_feature;
	abe_uint16 read_parameter;             
	abe_uint16 write_parameter;
	abe_uint16 running_status;                  
	abe_uint16 fw_input_buffer_address;         
	abe_uint16 fw_output_buffer_address;        
	abe_uint16 fw_scheduler_slot_position;      
	abe_uint16 fw_scheduler_subslot_position;  
	abe_uint16 min_opp;                        
	char name[NBCHARFEATURENAME];
} abe_feature_t;

  

/*
 *      CALL-BACK AND OTHER SUBROUTINES
 *
 */

#define c_feat_init_eq          1
#define c_feat_read_eq1         2
#define c_write_eq1             3
#define c_feat_read_eq2         4
#define c_write_eq2             5
#define c_feat_read_eq3         6
#define c_write_eq3             7
#define c_max_subroutine_index  1000


#endif /* ifndef ABETYP */
