/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_LIB.C
 *
 * 'ABE_LIB.C' internal computations libraries
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
 *  ABE_TRANSLATE_GAIN_FORMAT
 *
 *  Parameter  : 
 *  Operations : 
 *    f: original format name for gain or frequency.
 *       1=linear Q8, max=0x80 (0dB)
 *       2=linear Q16, max=0x8000 (0dB)
 *       3=linear Q16, max=0xFFFF (0dB)
 *       4=percentage, max=0x64 (0dB)
 *       5=dB in Q14.1 (-50dB = 0xFF9C)
 *       6=floating point (IEEE 64bits) linear format
 *       7=audio 16bits format +127.9 dB (0x7FFF) down to -127.9 dB (0x8001)
 *         in steps of 1/256 dB.
 *       8=audio 8bits format +31.75 dB (0x7F) down to -32.00 dB (0x80)
 *        in steps of 0.25 dB (0x01).
 *
 *    g1: pointer to the original data
 *    g2: pointer to the translated gain data 
 *
 *  Return value : 
 *      None.
 */
 void   abe_translate_gain_format (abe_uint32 f, void *g1, abe_gain_t *g2)
 {

 }

/*
 *  ABE_FPRINTF
 *  Parameter  : 
 *      character line to be printed
 *  Operations : 
 *  Return value : 
 *      None.
 */
 void abe_fprintf (char *line)
 {
        switch (abe_dbg_output)
        {
                case NO_OUTPUT :       
                break; 
                case TERMINAL_OUTPUT :
                break; 
                case LINE_OUTPUT : 
                break; 
                case DEBUG_TRACE_OUTPUT :
                break; 
                default : break;
        }

 }


/*
 *  ABE_GAIN_TRANSLATE
 *  Parameter  : 
 *      x : d
 *  Operations : 
 *  Return value : 
 */
void    abe_gain_translate (abe_uint32 x)
{
}

/*
 *  ABE_READ_FEATURE_FROM_PORT
 *
 *  Parameter  : 
 *      x : d
 *  Operations : 
 *  Return value : 
 */
void    abe_read_feature_from_port (abe_uint32 x)
{
}

/*
 *  ABE_WRITE_FEATURE_TO_PORT
 *  Parameter  : 
 *      x : d
 *  Operations : 
 *  Return value : 
 */
void    abe_write_feature_to_port (abe_uint32 x)
{
}

/*
 *  ABE_READ_FIFO
 *  Parameter  : 
 *      x : d
 *  Operations : 
 *  Return value : 
 */
void    abe_read_fifo (abe_uint32 x)
{
}


/*
 *  ABE_WRITE_FIFO
 *  Parameter  : 
 *      x : d
 *  Operations : 
 *  Return value : 
 */
void    abe_write_fifo (abe_uint32 x)
{
}

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

void  abe_block_copy  (abe_int32 direction, abe_int32 memory_bank,
		abe_int32 address, abe_uint32 *data, abe_uint32 nb_bytes)
{
	abe_uint32 i;
	abe_uint32 base_address = 0, *src_ptr, *dst_ptr, n;

	nb_bytes = (nb_bytes + 3) & (-4L);
	if (address & (3L)) 
		abe_dbg_param |= ERR_LIB; abe_dbg_error_log(ABE_BLOCK_COPY_ERR);

	switch (memory_bank) {
	case ABE_PMEM:
		base_address = (abe_uint32) ABE_PMEM_BASE_ADDRESS_MPU;
		break;
	case ABE_CMEM:
		base_address = (abe_uint32) ABE_CMEM_BASE_ADDRESS_MPU;
		break;
	case ABE_SMEM:
		base_address = (abe_uint32) ABE_SMEM_BASE_ADDRESS_MPU;
		break; 
	case ABE_DMEM:
		base_address = (abe_uint32) ABE_DMEM_BASE_ADDRESS_MPU;
		break;
	case ABE_ATC:
		base_address = (abe_uint32) ABE_ATC_BASE_ADDRESS_MPU;
		break; 
	default:
		abe_dbg_param |= ERR_LIB;
		abe_dbg_error_log(ABE_BLOCK_COPY_ERR);
		break;
	}

	if (direction == COPY_FROM_HOST_TO_ABE) {
		dst_ptr = (abe_uint32 *) (base_address + address);
		src_ptr = (abe_uint32 *) data;
	} else {
		dst_ptr = (abe_uint32 *) data;
		src_ptr = (abe_uint32 *) (base_address + address);
	}

	n = (nb_bytes/4);

	for (i = 0; i < n; i++)
		*dst_ptr++ = *src_ptr++;
}

/*
 *  ABE_CALL_SUBROUTINE
 *
 *  Parameter  : 
 *      index to the table of all registered Call-backs and subroutines
 *
 *  Operations : 
 *      run and log a subroutine
 *
 *  Return value : 
 *      None.
 */

void abe_call_subroutine (abe_uint32 idx, abe_uint32 *param1, abe_uint32 *param2)
{
	abe_dbg_log (id_call_subroutine);

	if (idx == 0)
		return;
	if (idx > c_max_subroutine_index) 

	switch (idx) {
	case c_feat_init_eq :
		abe_fprintf ("AAA");
		break;
	case c_feat_read_eq1 :
		abe_fprintf ("BBB");
		break;
	case c_write_eq1 :
		abe_fprintf ("CCC");
		break;
	default :
		(callbacks[idx]) (*param1, *param2);
		break;
	}
}

/*
 *  ABE_MONITORING
 *  Parameter  : 
 *  Operations : 
 *      checks the internal status of ABE and HAL
 *  Return value : 
 *      Call Backs on Errors
 */

void    abe_monitoring (void)
{
        abe_dbg_param = 0;
}


/*
 *  ABE_FORMAT_SWITCH
 *
 *  Parameter  : 
 *
 *  Operations : 
 *      translates the sampling and data length to ITER number for the DMA
 *      and the multiplier factor to apply during data move with DMEM
 *
 *  Return value : 
 *      Call Backs on Errors
 */
void abe_format_switch (abe_data_format_t *f, abe_uint32 *iter,
				abe_uint32 *mulfac, abe_uint32 *move_type)
{
	abe_uint32 n_freq, n_samp;

	n_freq = (f->f) / FW_SCHED_LOOP_FREQ;

	switch (f->samp_format) {
	case MONO_16_LSB:
		*mulfac = 1; n_samp = 2;
		break;
	case MONO_MSB:
		*mulfac = 1; n_samp = 2;
		break;
	case TWO_MONO_MSB:
		*mulfac = 1; n_samp = 1;
		break;
	case STEREO_16_16:
		*mulfac = 1; n_samp = 2;
		break;
	case STEREO_MSB:
		*mulfac = 2; n_samp = 4;
		break;
	case THREE_MSB:
		*mulfac = 3; n_samp = 6;
		break;
	case FOUR_MSB:
		*mulfac = 4; n_samp = 8;
		break;
	case FIVE_MSB:
		*mulfac = 5; n_samp = 10;
		break;
	case SIX_MSB:
		*mulfac = 6; n_samp = 12;
		break;
	case SEVEN_MSB:
		*mulfac = 7; n_samp = 14;
		break;
	case EIGHT_MSB:
		*mulfac = 8; n_samp = 16;
		break;
	default:
		*mulfac = 1; n_samp = 2;
		break;
	}
	*iter = (n_freq * n_samp) / 2;
}

/*
 *  ABE_DMA_PORT_ITERATION
 *
 *  Parameter  : 
 *  Operations : 
 *      translates the sampling and data length to ITER number for the DMA
 *  Return value : 
 *  Call Backs on Errors
 */
abe_uint32 abe_dma_port_iteration (abe_data_format_t *f)
{
	abe_uint32 iter, mulfac, move_type;
	abe_format_switch (f, &iter, &mulfac, &move_type);

	return iter;
}

/*
 *  ABE_DMA_PORT_ITER_FACTOR
 *
 *  Parameter  : 
 *
 *  Operations : 
 *      returns the multiplier factor to apply during data move with DMEM
 *
 *  Return value : 
 *      Call Backs on Errors
 */
abe_uint32 abe_dma_port_iter_factor (abe_data_format_t *f)
{
	abe_uint32 iter, mulfac, move_type;
	abe_format_switch (f, &iter, &mulfac, &move_type);

	return mulfac;
}

/*
 *  ABE_DMA_PORT_COPY_SUBROUTINE_ID
 *
 *  Parameter  : 
 *
 *  Operations : 
 *      returns the index of the function doing the copy in I/O tasks
 *
 *  Return value : 
 *      Call Backs on Errors
 */
abe_uint32 abe_dma_port_copy_subroutine_id (abe_data_format_t *f, abe_uint32 direction)
{
	abe_uint32 id;

	if (direction == ABE_ATC_DIRECTION_IN) {
		switch (f->samp_format) {
		case THREE_MSB:
		case FOUR_MSB:
		case FIVE_MSB:
		case SIX_MSB:
		case SEVEN_MSB:
		case EIGHT_MSB:
		case TWO_MONO_MSB:
		case MONO_16_LSB:
			id = ERR_DEFAULT_DATA_READ;
			break;
		case MONO_MSB:
			id = sub_copy_d2s_mono;
			break;
		case STEREO_16_16:
			id = sub_copy_d2s_1616;
			break;
		default:
			case STEREO_MSB:
			id = sub_copy_d2s;
			break;
		}
	} else {
		switch (f->samp_format) {
		case THREE_MSB:
		case FOUR_MSB:
		case FIVE_MSB:
		case SIX_MSB:
		case SEVEN_MSB:
		case EIGHT_MSB:
		case TWO_MONO_MSB:
		case STEREO_16_16:
		case MONO_16_LSB:
			id = ERR_DEFAULT_DATA_WRITE;
			break;
		case MONO_MSB:
			id = sub_copy_s2d_mono;
			break;
			id = ERR_DEFAULT_DATA_WRITE;
			break;
		default:
		id = sub_copy_s2d; break;
		}
	}
return id;
}

