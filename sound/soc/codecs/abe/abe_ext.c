/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_EXT.C
 *
 * 'ABE_EXT.C' external interfaces
 *
 * @path 
 * @rev     01.00
 *! Revision History
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */
 
 
#include "abe_main.h"

/*
 *  initialize the default values for call-backs to subroutines
 *      - FIFO IRQ call-backs for sequenced tasks
 *      - FIFO IRQ call-backs for audio player/recorders (ping-pong protocols)
 *      - Remote debugger interface
 *      - Error monitoring
 *      - Activity Tracing 
 */
 
 
 
/*
 *  ABE_READ_SYS_CLOCK
 *
 *  Parameter  : 
 *      pointer to the system clock
 *
 *  Operations : 
 *      returns the current time indication for the LOG 
 *
 *  Return value : 
 *      None.
 */

void abe_read_sys_clock (abe_microsecond *time)
{
        static abe_microsecond clock;
        
        *time = clock;
        clock ++;
      
}
        
        
        
/*
 *  ABE_APS_TUNING
 *
 *  Parameter  : 
 *      
 *
 *  Operations : 
 *      
 *
 *  Return value : 
 *      
 */
void    abe_aps_tuning (void)
{
}

/**
* @fn abe_set_semaphore() 
*
*  Operations : set a spin-lock and wait in case of collision
*/

void abe_set_semaphore (void)
{
}

/*
 * @fn abe_reset_semaphore() 
 *
 *  Operations : reset a spin-lock (end of subroutine)
 *
 *
 * @see        ABE_API.h
 */
void abe_reset_semaphore (void)
{
}

