/*
* Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
 * @file  ABE_LIB.H
 *
 * Library APIs
 *
 * @path 
 * @rev     01.00
*! 
*! Revision History
*! 27-Nov-2008 Original
*! 05-Jun-2009 V05 release
*/
#ifndef __ABE_LIB_H_
#define __ABE_LIB_H_


/*
 *  ABE_TRANSLATE_GAIN_FORMAT
 *
 *  Parameter  : 
 *  Operations : 
 *    f: original format name for gain or frequency.
 *	1=linear Q8, max=0x80 (0dB)
 *	2=linear Q16, max=0x8000 (0dB)
 *	3=linear Q16, max=0xFFFF (0dB)
 *	4=percentage, max=0x64 (0dB)
 *	5=dB in Q14.1 (-50dB = 0xFF9C)
 *	6=floating point (IEEE 64bits) linear format
 *	7=audio 16bits format +127.9 dB (0x7FFF) down to -127.9 dB (0x8001)
 *	in steps of 1/256 dB.
 *	8=audio 8bits format +31.75 dB (0x7F) down to -32.00 dB (0x80) 
 *	in steps of 0.25 dB (0x01).
 *
 *    g1: pointer to the original data
 *    g2: pointer to the translated gain data 
 *
 *  Return value : 
 *      None.
 */
void   abe_translate_gain_format (abe_uint32 f, void *g1, abe_gain_t *g2);



/*
 *  ABE_FPRINTF
 *
 *  Parameter  : 
 *      character line to be printed
 *
 *  Operations : 
 *
 *  Return value : 
 *      None.
 */
void abe_fprintf (char *line);
 

/*
 *  ABE_GAIN_TRANSLATE
 *
 *  Parameter  : 
 *      x : d
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void abe_gain_translate (abe_uint32 x);

/*
 *  ABE_READ_FEATURE_FROM_PORT
 *
 *  Parameter  : 
 *      x : d
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void abe_read_feature_from_port (abe_uint32 x);


/*
 *  ABE_WRITE_FEATURE_TO_PORT
 *
 *  Parameter  : 
 *      x : d
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void abe_write_feature_to_port (abe_uint32 x);

/*
 *  ABE_READ_FIFO
 *
 *  Parameter  : 
 *      x : d
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void abe_read_fifo (abe_uint32 x);


/*
 *  ABE_WRITE_FIFO
 *
 *  Parameter  : 
 *      x : d
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void abe_write_fifo (abe_uint32 x);



/*
 *  ABE_BLOCK_COPY
 *
 *  Parameter  : 
 *      direction of the data move (Read/Write)
 *      memory bank among PMEM, DMEM, CMEM, SMEM, ATC/IO
 *      address of the memory copy (byte addressing)
 *      long pointer to the data
 *      number of data to move
 *
 *  Operations : 
 *      block data move
 *
 *  Return value : 
 *      none
 */
 
void abe_block_copy  (abe_int32 direction, abe_int32 memory_bank,
		abe_int32 address, abe_uint32 *data, abe_uint32 nb);

#endif
