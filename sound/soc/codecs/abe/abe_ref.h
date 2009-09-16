/*
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
 * @file  ABE_REF.H
 *
 * Variable references
 *
 * @path 
 * @rev     01.00
*! 
*! Revision History
*! 27-Nov-2008 Original (LLF)
*! 05-Jun-2009 V05 release
*/
 
#ifndef __abe_ref_def_
#define __abe_ref_def_
/*
 *  'ABE_PRO.H' all non-API prototypes for INI, IRQ, SEQ ...
 */

/*
 * HAL internal API
 */  

void abe_build_scheduler_table (void);
void abe_reset_one_feature (abe_uint32 x);
void abe_reset_all_features (void);
void abe_reset_one_port (abe_uint32 x);
void abe_reset_all_ports (void);
void abe_reset_all_fifo (void);
void abe_reset_all_sequence (void);
abe_uint32 abe_dma_port_iteration (abe_data_format_t *format);  
void abe_read_global_counter (abe_time_stamp_t *t, abe_millis_t *m);
void abe_read_sys_clock (abe_microsecond *time);
void abe_init_atc (abe_port_id id);
void abe_init_io_tasks (abe_port_id id, abe_data_format_t *format,
						abe_port_protocol_t *prot);
void abe_init_dma_t (abe_port_id id, abe_data_format_t *format,
						abe_port_protocol_t *prot);
abe_uint32 abe_dma_port_iter_factor (abe_data_format_t *f);
abe_uint32 abe_dma_port_copy_subroutine_id (abe_data_format_t *format,
							abe_uint32 direction);
void abe_call_subroutine (abe_uint32 idx, abe_uint32 *param1,
							abe_uint32 *param2);
void abe_monitoring (void);
void abe_set_semaphore (void);
void abe_reset_semaphore (void);
void abe_hw_configuration (void);


/*
 * HAL internal data
 */ 


extern abe_port_t               abe_port[];
extern abe_feature_t            feature [];                        
extern abe_subroutine2          callbacks [];                        

extern abe_port_t               abe_port [];                  
extern const abe_port_t         abe_port_init [];                   

extern abe_feature_t            all_feature [];
extern const abe_feature_t      all_feature_init [];

extern abe_router_t             abe_router_ul_table [NBROUTE_UL]; 
extern const abe_router_t       abe_router_ul_table_init_1 [NBROUTE_UL];
                    
extern abe_seq_t                all_sequence [];
extern const abe_seq_t          all_sequence_init[];

extern abe_uint32               abe_map_dmem [];			
extern abe_uint32               abe_map_dmem_secondary [];
extern abe_uint32               abe_map_dmem_size [];	
	
extern const abe_uint32         abe_map_dmem_init_1 [];
extern const abe_uint32         abe_map_dmem_size_init_1 [];
extern const abe_uint32         abe_map_dmem_init_2 [];
extern const abe_uint32         abe_map_dmem_size_init_2 [];

extern abe_uint32               abe_dma_map [];
extern const abe_uint32         abe_dma_map_init_1 [];
extern const abe_uint32         abe_dma_map_init_2 [];

extern abe_uint32               abe_dbg_output;
extern abe_uint32               abe_dbg_mask;
extern abe_uint32               abe_dbg_activity_log [DBG_LOG_SIZE];
extern abe_uint32               abe_dbg_activity_log_write_pointer;
extern abe_uint32               abe_dbg_param; 

extern abe_uint32               abe_global_mcpdm_control;                


#endif /* REFDEF */
