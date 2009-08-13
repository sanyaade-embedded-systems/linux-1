/*
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
*
 * @file  ABE_DBG.H
 *
 * 'ABE_DBG.H' global data definitions for debug
 *
 * @path 
 * @rev     01.00
 *! 
 *! Revision History
 *! ===================================
 *! 27-Nov-2008 Original (LLF)
 *! 05-Jun-2009 V05 release
 */
#ifndef __ABE_DBG_H_
#define __ABE_DBG_H_

#include "abe_main.h"

/* generates the time-stamps used for the AESS verification */
#define HAL_TIME_STAMP                  0

/* pipe connection to the TARGET simulator */
#define ABE_DEBUG_CHECKERS              0
/* simulator data extracted from a text-file */
#define ABE_DEBUG_HWFILE                0
/* low-level log files */
#define ABE_DEBUG_LL_LOG                0

#define ABE_DEBUG (ABE_DEBUG_CHECKERS | ABE_DEBUG_HWFILE | ABE_DEBUG_LL_LOG)
 
#define NO_OUTPUT                       0
#define TERMINAL_OUTPUT                 1
#define LINE_OUTPUT                     2
#define DEBUG_TRACE_OUTPUT              3




#define DBG_LOG_SIZE 1000

#define dbg_bitfield_offset	8

#define dbg_api_calls	0
#define dbg_mapi	(1L << (dbg_api_calls + dbg_bitfield_offset))

#define dbg_external_data_access	1
#define dbg_mdata	(1L << (dbg_external_data_access + dbg_bitfield_offset))

#define dbg_err_codes	2
#define dbg_merr	 (1L << (dbg_api_calls + dbg_bitfield_offset))


/*
 *  IDs used for traces
 */
 
#define id_reset_hal                            (1 + dbg_mapi)
#define id_load_fw                              (2 + dbg_mapi)
#define id_default_configuration                (3 + dbg_mapi)
#define id_irq_processing                       (4 + dbg_mapi)
#define id_event_generator_switch               (5 + dbg_mapi)
#define id_set_memory_config                    (6 + dbg_mapi)
#define id_read_lowest_opp                      (7 + dbg_mapi)
#define id_set_opp_processing                   (8 + dbg_mapi)
#define id_set_ping_pong_buffer                 (9 + dbg_mapi)
#define id_plug_subroutine                      (10 + dbg_mapi)
#define id_unplug_subroutine                    (11 + dbg_mapi)
#define id_plug_sequence                        (12 + dbg_mapi)
#define id_launch_sequence                      (13 + dbg_mapi)
#define id_launch_sequence_param                (14 + dbg_mapi)
#define id_set_analog_control                   (15 + dbg_mapi)
#define id_read_analog_gain_dl                  (16 + dbg_mapi)
#define id_read_analog_gain_ul                  (17 + dbg_mapi)
#define id_enable_dyn_ul_gain                   (18 + dbg_mapi)
#define id_disable_dyn_ul_gain                  (19 + dbg_mapi)
#define id_enable_dyn_extension                 (20 + dbg_mapi)
#define id_disable_dyn_extension                (21 + dbg_mapi)
#define id_notify_analog_gain_changed           (22 + dbg_mapi)
#define id_reset_port                           (23 + dbg_mapi)
#define id_read_remaining_data                  (24 + dbg_mapi)
#define id_disable_data_transfer                (25 + dbg_mapi)
#define id_enable_data_transfer                 (26 + dbg_mapi)
#define id_read_global_counter                  (27 + dbg_mapi)
#define id_set_dmic_filter                      (28 + dbg_mapi)
#define id_write_port_descriptor                (29 + dbg_mapi)
#define id_read_port_descriptor                 (30 + dbg_mapi)
#define id_read_port_address                    (31 + dbg_mapi)
#define id_write_port_gain                      (32 + dbg_mapi)
#define id_write_headset_offset                 (33 + dbg_mapi)
#define id_read_gain_ranges                     (34 + dbg_mapi)
#define id_write_equalizer                      (35 + dbg_mapi)
#define id_write_asrc                           (36 + dbg_mapi)
#define id_write_aps                            (37 + dbg_mapi)
#define id_write_mixer                          (38 + dbg_mapi)
#define id_write_eanc                           (39 + dbg_mapi)
#define id_write_router                         (40 + dbg_mapi)
#define id_read_port_gain                       (41 + dbg_mapi)
#define id_read_asrc                            (42 + dbg_mapi)
#define id_read_aps                             (43 + dbg_mapi)
#define id_read_aps_energy                      (44 + dbg_mapi)
#define id_read_mixer                           (45 + dbg_mapi)
#define id_read_eanc                            (46 + dbg_mapi)
#define id_read_router                          (47 + dbg_mapi)
#define id_read_debug_trace                     (48 + dbg_mapi)
#define id_set_debug_trace                      (49 + dbg_mapi)
#define id_set_debug_pins                       (50 + dbg_mapi)

#define id_call_subroutine                      (51 + dbg_mapi)


/*
 *  IDs used for error codes 
 */
#define NOERR                                   0
#define ABE_SET_MEMORY_CONFIG_ERR               (1 + dbg_merr)
#define ABE_BLOCK_COPY_ERR                      (2 + dbg_merr)
#define ABE_SEQTOOLONG                          (3 + dbg_merr)
#define ABE_BADSAMPFORMAT                       (4 + dbg_merr)      
#define ABE_SET_ATC_MEMORY_CONFIG_ERR           (5 + dbg_merr)      
#define ABE_PROTOCOL_ERROR                      (6 + dbg_merr)      
#define ABE_PARAMETER_ERROR                     (7 + dbg_merr)      
/* port programmed while still running */
#define ABE_PORT_REPROGRAMMING                  (8 + dbg_merr) 


/*
 *  IDs used for error codes
 */

#define ERR_LIB                 (1 << 1)        /* error in the LIB.C file */
#define ERR_API                 (1 << 2)        /* error in the API.C file */
#define ERR_INI                 (1 << 3)        /* error in the INI.C file */
#define ERR_SEQ                 (1 << 4)        /* error in the SEQ.C file */
#define ERR_DBG                 (1 << 5)        /* error in the DBG.C file */
#define ERR_EXT                 (1 << 6)        /* error in the EXT.C file */


    
/* 
 * MACROS 
 */


#if HAL_TIME_STAMP
#define _log(x) ((x&abe_dbg_mask)?abe_dbg_log(x);   \
                 abe_dbg_time_stamp(x);             \
                 abe_dbg_printf(x);                 \
                )
#else
#define _log(x) {if(x&abe_dbg_mask)abe_dbg_log(x);}
#endif


/* 
 * PROTOTYPES
 */
 
/*
 *  ABE_DBG_LOG
 *
 *  Parameter  : 
 *      x : data to be logged
 *
 *      abe_dbg_activity_log : global circular buffer holding the data 
 *      abe_dbg_activity_log_write_pointer : circular write pointer
 *
 *  Operations : 
 *      saves data in the log file
 *
 *  Return value : 
 *      none
 */
void    abe_dbg_log (abe_uint32 x);


/*
 *  ABE_DBG_ERROR_LOG
 *
 *  Parameter  : 
 *      x : d
 *
 *  Operations : 
 *      log the error codes
 *
 *  Return value : 
 *      
 */
void    abe_dbg_error_log (abe_uint32 x);

#endif
