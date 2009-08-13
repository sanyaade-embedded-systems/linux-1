/*
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * @file  ABE_IRQ.C
 *
 * Interrupt routines
 *
 * @path 
 * @rev     01.00
*! 
*! Revision History
*! ===================================
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
