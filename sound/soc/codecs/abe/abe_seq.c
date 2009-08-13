/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_SEQ.C
 *
 * 'ABE_SEQ.C' manages the time-sequenced operations
 *
 * @path 
 * @rev     01.00
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */
 
#include "abe_main.h"


/*
 *      SEQUENCES
 *      struct {
 *      micros_t time;          Waiting time before executing next line
 *      seq_code_t code         Subroutine index interpreted in the HAL and
 *      int32 param[2]          Two parameters
 *      char param_tag[2]       Flags used for params when launching the seq
 *      } seq_t	
 * 
 */ 

abe_seq_t all_sequence[MAXNBSEQUENCE]; 
abe_uint32 sequence_id[MAXNBSEQUENCE]; 
abe_uint32 sequence_write_pointer; 
abe_uint32 sequence_id_pointer; 
/* time         call_idx        param1  param2  */
const abe_seq_t seq_null =
{
	0, 0, 0, 0,
};
const abe_seq_t seq_power_on_phoenix =
{
	0, 0, 0, 0,
};
const abe_seq_t seq_set_uplink_phoenix =
{
	0, 0, 0, 0,
};
const abe_seq_t seq_set_downlink_phoenix =
{
	0, 0, 0, 0,
};



/*
 *  ABE_RESET_ONE_SEQUENCE
 *
 *  Parameter  : 
 *      sequence ID
 *
 *  Operations : 
 *      load default configuration for that sequence
 *      kill running activities
 *
 *  Return value : 
 *      
 */
void abe_reset_one_sequence (abe_uint32 id)
{
}

/*
 *  ABE_RESET_ALL_SEQUENCE
 *
 *  Parameter  : 
 *      none
 *
 *  Operations : 
 *      load default configuration for all sequences
 *      kill any running activities
 *
 *  Return value : 
 *      
 */
void abe_reset_all_sequence (void)
{
	sequence_id_pointer = sequence_write_pointer = 0;

}
