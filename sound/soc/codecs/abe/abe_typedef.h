/*
 *Texas Instruments OMAP(TM) Platform Firmware
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this firmware is controlled by the terms and conditions found 
 *  in the license agreement under which this firmware has been supplied.
* @file ABE_typedef.h
*
*  ABE Firmware generic types definition
*
* @path  $(ABEPATH)\firmware\src\
*
* @rev  00.05.01
*/

#ifndef __ABE_TYPEDEF_H_
#define __ABE_TYPEDEF_H_

#include "abe_define.h"
//-------------------------------------------------------------
// Basic types definition
//-------------------------------------------------------------
#ifdef __chess__
typedef unsigned char   ABE_uchar;
typedef signed char   	ABE_char;
typedef unsigned        ABE_uint16;
typedef int             ABE_int16;
typedef long            ABE_int32;
typedef unsigned long	  ABE_uint32;
#else
typedef unsigned char   ABE_uchar;
typedef char            ABE_char;
typedef unsigned short  ABE_uint16;
typedef short           ABE_int16;
typedef long            ABE_int32;
typedef unsigned long   ABE_uint32;
#endif


typedef ABE_uchar*	pABE_uchar;
typedef ABE_char*	  pABE_char;
typedef ABE_uint16*	pABE_uint16;
typedef ABE_int16*	pABE_int16;
typedef ABE_int32*	pABE_int32;
typedef ABE_uint32*	pABE_uint32;

//-------------------------------------------------------------
// Commonly used structures
//-------------------------------------------------------------

#ifdef __chess__

/**
* This is a structure description for ABE_SAtcDescriptor. This structuce contains 
* descriptor for ATC regristers
*
* @param  readPointer       7 bits for read pointer index
* @param  bufSize           7 bits for circular buffer size
* @param  IRQdest           IRQ destination 0:DSP 1:MCU
* @param  error             report buffer access errors
* @param  direction         Circular buffer direction 0:IN 1:OUT
* @param  empty             Set to 1 is circular buffer empty
* @param  writePointer      7 bits for write pointer index
* @param  baseAddr          12 bits for circular buffer base address
* @param  iteration_lower4  4 lower bits of iteration flied
* @param  iteration_upper3  3 upper bits of iteration flied
* @param  source            Source ID
* @param  dest              Destination ID
* @param  DescrAct          0: descriptor is not active 1:descriptor is active
*/
/* ========================================================================== */
typedef struct abeatcdescTag {
    unsigned readPointer      :  7; // Desc0
    unsigned reserved0        :  1;
    unsigned bufSize          :  7;
    unsigned IRQdest          :  1;
    unsigned error            :  1;
    unsigned reserved1        :  5;
    unsigned direction        :  1;
    unsigned empty            :  1;
    unsigned writePointer     :  7;
    unsigned reserved2        :  1;
    unsigned baseAddr         : 12; // Desc1
    unsigned iteration_lower4 :  4;  // iteration field overlaps the 16 bits boundary
    unsigned iteration_upper3 :  3;
    unsigned source           :  6;
    unsigned dest             :  6;
    unsigned DescrAct         :  1;
} ABE_SAtcDescriptor;
#else

/*
* This is a structure description for ABE_SAtcDescriptor. This structuce contains 
* descriptor for ATC regristers
*
* @param  readPointer       7 bits for read pointer index
* @param  bufSize           7 bits for circular buffer size
* @param  IRQdest           IRQ destination 0:DSP 1:MCU
* @param  error             report buffer access errors
* @param  direction         Circular buffer direction 0:IN 1:OUT
* @param  empty             Set to 1 is circular buffer empty
* @param  writePointer      7 bits for write pointer index
* @param  baseAddr          12 bits for circular buffer base address
* @param  iteration         7 bits of iteration flied
* @param  source            Source ID
* @param  dest              Destination ID
* @param  DescrAct          0: descriptor is not active 1:descriptor is active
*/

typedef struct abeatcdescTag{
    unsigned readPointer   :  7; // Desc0
    unsigned reserved0     :  1;
    unsigned bufSize       :  7;
    unsigned IRQdest       :  1;
    unsigned error         :  1;
    unsigned reserved1     :  5;
    unsigned direction     :  1;
    unsigned empty         :  1;
    unsigned writePointer  :  7;
    unsigned reserved2     :  1;
    unsigned baseAddr      : 12; // Desc1
    unsigned iteration     :  7;  // iteration field overlaps the 16 bits boundary
    unsigned source        :  6;
    unsigned dest          :  6;
    unsigned DescrAct      :  1;
} ABE_SAtcDescriptor;
#endif
typedef ABE_SAtcDescriptor* pABE_SAtcDescriptor;


typedef struct abetaskTag{
    ABE_uchar iF;			// Index of called function
    ABE_uchar A0, A1, A2_3, A4_5;	// Pointers for initprt
    ABE_uchar R;			// Pointer for initreg
    ABE_uchar misc0, misc1;		// Filling data to 32 bits alignment
} ABE_STask;
typedef ABE_STask* pABE_STask;
typedef ABE_STask** ppABE_STask;

typedef struct abefifodescrTag{ //General purpose FIFO descriptor
	union{
		struct{
			ABE_uint16 writePtr;
			ABE_uint16 fifoStartAddr;
		} s16;
		ABE_uint32 writePtr_fifoStartAddr;
	} Wp;
	union{
		struct{
			ABE_uint16 readPtr;
			ABE_uint16 fifoEndAddr;
		} s16;
		ABE_uint32 readPtr_fifoEndAddr;
	} Rp;
	ABE_uint16 fifoSize;
	ABE_uint16 elementSize;
} ABE_SFiFoDescr;
typedef ABE_SFiFoDescr* pABE_SFiFoDescr;

/* most of the times: p0 = dest addr, p1 = src addr, p2 = count */
typedef struct abedelayedcommandTag{
	ABE_uint16 delay;	// 0: immediat action, count down otherwise
	ABE_uint16 p0;
	ABE_uint16 p1;
	ABE_uint16 p2;
	ABE_uchar  p3;		// Added p3 param so that structure size is even
	ABE_uchar  code;	// Action ID
	ABE_uchar misc0, misc1;	// Filling data to 32 bits alignment
} ABE_SDelayedCmd;
typedef ABE_SDelayedCmd* pABE_SDelayedCmd;

typedef ABE_SDelayedCmd ABE_TDelayedCmdFifo[DCMD_FIFO_LENGTH];
typedef ABE_TDelayedCmdFifo* pABE_TDelayedCmdFifo;
/*
typedef struct abedelayedcommandfifo{
	pABE_SDelayedCmd writePtr;
	ABE_uint16 fill;	// Filling data to 32 bits alignment
	pABE_SDelayedCmd readPtr;
	pABE_SDelayedCmd fifoEndAddr;
} ABE_SDelayedCmdFiFoDescr;
typedef ABE_SDelayedCmdFiFoDescr* pABE_SDelayedCmdFiFoDescr;
*/
//-------------------------------------------------------------
//-------------------------------------------------------------
#ifdef __chess__
typedef void (*pABE_voidFunction)()clobbers(R0, R1, R2, R3, R4, R5, R6, R7, R13);
typedef void (*pABE_voidFunctionsList[])()clobbers(R0, R1, R2, R3, R4, R5, R6, R7, R13);
typedef void (*pABE_cmdFunction)(pABE_SDelayedCmd) clobbers(R0, R1, R2, R3, R4, R5, R6, R7, R13);
typedef void (*pABE_cmdFunctionsList[])(pABE_SDelayedCmd) clobbers(R0, R1, R2, R3, R4, R5, R6, R7, R13);
typedef void (*pABE_copyFunction)(ABE_uint16 chess_storage(R13))clobbers(R13);
typedef void (*pABE_copyFunctionsList[])(ABE_uint16 chess_storage(R13))clobbers(R13);

#endif
//-------------------------------------------------------------
//-------------------------------------------------------------
// ABE Enum IO defines
enum ABE_EIOAction {ABE_IOAction_disabled, ABE_IOAction_read, ABE_IOAction_write};
enum ABE_EIOType {ABE_EIOType_ATC, ABE_EIOType_pingPong, ABE_EIOType_DMAreq, ABE_EIOType_slimbus};
enum ABE_EIOFormat {ABE_EIOFormat_32_lsb, ABE_EIOFormat_32_msb, ABE_EIOFormat_32_2x16, ABE_EIOFormat_32_2xMono};


typedef struct abeiodescriptorTag{
    ABE_uint16 drift_ASRC;      //0        /* [W] asrc output used for the next ASRC call (+/- 1 / 0)*/
    ABE_uint16 drift_io;        //2       /* [W] asrc output used for controlling the number of samples to be exchanged (+/- 1 / 0) */
    ABE_uint16 hw_ctrl_addr;    //4      /* DMAReq address or HOST IRQ buffer address (ATC ADDRESS) */
    ABE_uchar  copy_func_index; //6      /* index of the copy subroutine */
    ABE_uchar x_io;            //7      /* X number of SMEM samples to move */
    ABE_uchar threshold_1;     //8      /* THR1; For stereo data, THR1 is provided by HAL as THR1<<1 */
    ABE_uchar threshold_2;     //9       /* THR2; For stereo data, THR2 is provided by HAL as THR2<<1 */
    ABE_uchar update_1;        //10       /* UP_1; For stereo data, UP_1 is provided by HAL as UP_1<<1 */
    ABE_uchar update_2;        //11      /* UP_2; For stereo data, UP_2 is provided by HAL as UP_2<<1 */
    ABE_uchar data_size; 	   //12      /* 0 for mono data, 1 for stereo data */
    ABE_uchar smem_addr;       //13       /* internal SMEM buffer INITPTR pointer index */
    ABE_uchar flow_counter;    //14      /* Flow error counter */
    ABE_uchar atc_irq_data;    //15      /* data content to be loaded to "hw_ctrl_addr" */
    ABE_uchar atc_address;     //16      /* ATC descriptor address */
    ABE_uchar direction_rw;    //17      /* Read DMEM =0, Write DMEM =3 (ATC offset of the access pointer) */
    ABE_uchar padding16[2];	   // To keep 32bits alignment,
} ABE_SIODescriptor;

typedef ABE_SIODescriptor*  pABE_SIODescriptor;
typedef ABE_SIODescriptor** ppABE_SIODescriptor;

#ifdef __chess__
#define drift_ASRC      0       /* [W] asrc output used for the next ASRC call (+/- 1 / 0)*/
#define drift_io        2       /* [W] asrc output used for controlling the number of samples to be exchanged (+/- 1 / 0) */
#define hw_ctrl_addr    4       /* DMAReq address or HOST IRQ buffer address (ATC ADDRESS) */
#define copy_func_index 6       /* index of the copy subroutine */
#define x_io            7       /* X number of SMEM samples to move */
#define threshold_1     8       /* THR1; For stereo data, THR1 is provided by HAL as THR1<<1 */
#define threshold_2     9       /* THR2; For stereo data, THR2 is provided by HAL as THR2<<1 */
#define update_1        10      /* UP_1; For stereo data, UP_1 is provided by HAL as UP_1<<1 */
#define update_2        11      /* UP_2; For stereo data, UP_2 is provided by HAL as UP_2<<1 */
#define data_size 		12      /* 0 for mono data, 1 for stereo data */
#define smem_addr       13      /* internal SMEM buffer INITPTR pointer index */
#define flow_counter    14      /* Flow error counter */
#define atc_irq_data    15      /* data content to be loaded to "hw_ctrl_addr"  */
#define atc_address     16      /* ATC descriptor address */
#define direction_rw    17      /* Read DMEM =0, Write DMEM =3 (ATC offset of the access pointer) */
#endif

//-------------------------------------------------------------
//-------------------------------------------------------------
typedef struct abepackedbasesizetag{
	ABE_uint16 baseAddr;	// Buffer base address in DM
	ABE_uint16 samples;	// number of samples (as written in SM)
} ABE_SPackedBaseSamples;
typedef ABE_SPackedBaseSamples* pABE_SPackedBaseSamples;
typedef union {
	ABE_SPackedBaseSamples s16;
	ABE_uint32 base_size;
} ABE_UBufferBaseSamples;
typedef ABE_UBufferBaseSamples* pABE_UBufferBaseSamples;

/*typedef struct abepackedirqflagtag{
	ABE_uint16 flag;
} ABE_SPackedIrqFlag;
typedef ABE_SPackedIrqFlag* pABE_SPackedIrqFlag;
typedef union {
	ABE_SPackedIrqFlag s16;
	ABE_uint32 IrqFlag;
} ABE_UIrqFlag;
typedef ABE_UIrqFlag* pABE_UIrqFlag;
*/
enum ABE_EPPNotificationMode {ABE_PP_IRQ = 1, ABE_PP_DMAReq};
typedef struct abebuflistdescrTag {
	ABE_uchar itCount;		//
	ABE_uchar itLimit;		// Control IT sent
	ABE_char  drift_IO;		// Filled by ASRC -1/0/+1, cleared by IO_task
	ABE_uchar copy_subroutineID; 	// DM -> SM copy proc id
	ABE_uchar notificationMode;	// ABE_EPPNotificationMode
	ABE_uchar dummy;		// Used for 32 bits alignment
	ABE_uint16 IRQ_DMA;		// &postedIRQ or DMA flag (2^k, k=0..7)
	ABE_uint16 sampleCount;		// Number of samples to transfer each call
	ABE_uint16 remainingSamples;	// number of samples available in current buffer
	ABE_uint16 workingBufAddr;	// Not a pointer as it should be compiled by different systems...
	ABE_uint16 lastBufAddr;		// &B[lastUsedBuffer + 1]
	ABE_UBufferBaseSamples B[MAX_MM_BUFFERS];
} ABE_SBufferListDescriptor;
typedef ABE_SBufferListDescriptor* pABE_SBufferListDescriptor;
//-------------------------------------------------------------
//-------------------------------------------------------------
/*
typedef struct abedatafifodescrTag{
	ABE_int32* writePtr;
	ABE_int32* readPtr;
	ABE_int32* fifoStartAddr;
	ABE_int32* fifoEndAddr;
} ABE_SDataFiFoDescr;
typedef ABE_SDataFiFoDescr* pABE_SDataFiFoDescr;
//-------------------------------------------------------------
//-------------------------------------------------------------
typedef struct abedebugitemTag{
	ABE_uint16 time;
	ABE_uint16 data;
} ABE_SDebugItem;
typedef ABE_SDebugItem* pABE_SDebugItem;

typedef ABE_SDebugItem ABE_TDebugFifo[DEBUG_FIFO_LENGTH];
typedef ABE_TDebugFifo* pABE_TDebugFifo;

typedef struct abedebugfifo{
	pABE_SDebugItem readPtr;
	ABE_uint16 fill;	// Filling data to 32 bits alignment
	pABE_SDebugItem writePtr;
	pABE_SDebugItem fifoEndAddr;
} ABE_SDebugFifoDescr;
typedef ABE_SDebugFifoDescr* pABE_SDebugFifoDescr;
//-------------------------------------------------------------
//-------------------------------------------------------------
typedef struct abesettaskTag{	// Used for multiFrame update
	ABE_uint16 index;	// index in multiFrame table
	pABE_STask taskPtr;	// task pointer to be set at multiFrame[0][index]
} ABE_SSetTask;
typedef ABE_SSetTask* pABE_SSetTask;
//-------------------------------------------------------------
//-------------------------------------------------------------
typedef struct abebigcircbufTag{	// 
	ABE_int16 tableAddr;	// Byte address
	ABE_int16 tableLength;	// In samples
	ABE_int16 writeIndex;	// 
	ABE_uint16 fill;	// Filling data to 32 bits alignment
	ABE_int16 readIndex;	// 
	ABE_int16 threshold;	// 
	ABE_int16 IRQ;		// 
	ABE_int16 IRQValue;	// 
} ABE_SBigCircularBufferDescr;
typedef ABE_SBigCircularBufferDescr* pABE_SBigCircularBufferDescr;
//-------------------------------------------------------------
//-------------------------------------------------------------
*/

#endif /* _ABE_TYPEDEF_H_ */
