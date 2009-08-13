/*
 *             Texas Instruments OMAP(TM) Platform Firmware
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this firmware is controlled by the terms and conditions found
 *  in the license agreement under which this firmware has been supplied.
 */
#ifndef __ABE_DM_ADDR_H_
#define __ABE_DM_ADDR_H_

#define D_atcDescriptors_ADDR                               0 
#define D_atcDescriptors_ADDR_END                           511 
#define D_atcDescriptors_sizeof                             512 

#define D_McPDM_UL_Fifo_ADDR                                512 
#define D_McPDM_UL_Fifo_ADDR_END                            1019 
#define D_McPDM_UL_Fifo_sizeof                              508 

#define D_McPDM_DL_Fifo_ADDR                                1024 
#define D_McPDM_DL_Fifo_ADDR_END                            1531 
#define D_McPDM_DL_Fifo_sizeof                              508 

#define D_DMIC_UL_Fifo_ADDR                                 1536 
#define D_DMIC_UL_Fifo_ADDR_END                             2043 
#define D_DMIC_UL_Fifo_sizeof                               508 

#define D_MM_UL_FIFO_ADDR                                   2048 
#define D_MM_UL_FIFO_ADDR_END                               2535 
#define D_MM_UL_FIFO_sizeof                                 488 

#define D_MM_DL_ADDR                                        2560 
#define D_MM_DL_ADDR_END                                    2847 
#define D_MM_DL_sizeof                                      288 

#define stack_ADDR                                          2848 
#define stack_ADDR_END                                      2975 
#define stack_sizeof                                        128 

#define D_version_ADDR                                      2976 
#define D_version_ADDR_END                                  2979 
#define D_version_sizeof                                    4 

#define D_tasksList_ADDR                                    2980 
#define D_tasksList_ADDR_END                                4003 
#define D_tasksList_sizeof                                  1024 

#define D_multiFrame_ADDR                                   4004 
#define D_multiFrame_ADDR_END                               4195 
#define D_multiFrame_sizeof                                 192 

#define D_nextMultiFrame_ADDR                               4196 
#define D_nextMultiFrame_ADDR_END                           4387 
#define D_nextMultiFrame_sizeof                             192 

#define D_taskClusters_ADDR                                 4388 
#define D_taskClusters_ADDR_END                             4451 
#define D_taskClusters_sizeof                               64 

#define D_idleTask_ADDR                                     4452 
#define D_idleTask_ADDR_END                                 4453 
#define D_idleTask_sizeof                                   2 

#define D_typeLengthCheck_ADDR                              4454 
#define D_typeLengthCheck_ADDR_END                          4455 
#define D_typeLengthCheck_sizeof                            2 

#define D_taskStep_ADDR                                     4456 
#define D_taskStep_ADDR_END                                 4459 
#define D_taskStep_sizeof                                   4 

#define D_pCurrentTask_ADDR                                 4460 
#define D_pCurrentTask_ADDR_END                             4461 
#define D_pCurrentTask_sizeof                               2 

#define D_pFastLoopBack_ADDR                                4462 
#define D_pFastLoopBack_ADDR_END                            4463 
#define D_pFastLoopBack_sizeof                              2 

#define D_pNextFastLoopBack_ADDR                            4464 
#define D_pNextFastLoopBack_ADDR_END                        4467 
#define D_pNextFastLoopBack_sizeof                          4 

#define D_ppCurrentTask_ADDR                                4468 
#define D_ppCurrentTask_ADDR_END                            4469 
#define D_ppCurrentTask_sizeof                              2 

#define D_loopCounter_ADDR                                  4470 
#define D_loopCounter_ADDR_END                              4471 
#define D_loopCounter_sizeof                                2 

#define D_idleSlotsReq_ADDR                                 4472 
#define D_idleSlotsReq_ADDR_END                             4473 
#define D_idleSlotsReq_sizeof                               2 

#define D_PostMcuIrq_ADDR                                   4476 
#define D_PostMcuIrq_ADDR_END                               4479 
#define D_PostMcuIrq_sizeof                                 4 

#define D_PostDspIrq_ADDR                                   4480 
#define D_PostDspIrq_ADDR_END                               4483 
#define D_PostDspIrq_sizeof                                 4 

#define D_McuIrqFifo_ADDR                                   4484 
#define D_McuIrqFifo_ADDR_END                               4507 
#define D_McuIrqFifo_sizeof                                 24 

#define D_DspIrqFifo_ADDR                                   4508 
#define D_DspIrqFifo_ADDR_END                               4531 
#define D_DspIrqFifo_sizeof                                 24 

#define D_McuIrqFifoDesc_ADDR                               4532 
#define D_McuIrqFifoDesc_ADDR_END                           4543 
#define D_McuIrqFifoDesc_sizeof                             12 

#define D_DspIrqFifoDesc_ADDR                               4544 
#define D_DspIrqFifoDesc_ADDR_END                           4555 
#define D_DspIrqFifoDesc_sizeof                             12 

#define D_debugFifo_ADDR                                    4556 
#define D_debugFifo_ADDR_END                                4619 
#define D_debugFifo_sizeof                                  64 

#define D_debugFifoDesc_ADDR                                4620 
#define D_debugFifoDesc_ADDR_END                            4631 
#define D_debugFifoDesc_sizeof                              12 

#define D_DCFifoDesc_ADDR                                   4632 
#define D_DCFifoDesc_ADDR_END                               4655 
#define D_DCFifoDesc_sizeof                                 24 

#define D_DCFifo_ADDR                                       4656 
#define D_DCFifo_ADDR_END                                   4847 
#define D_DCFifo_sizeof                                     192 

#define D_IODescr_ADDR                                      4848 
#define D_IODescr_ADDR_END                                  5227 
#define D_IODescr_sizeof                                    380 

#define D_IODescrList_ADDR                                  5228 
#define D_IODescrList_ADDR_END                              5275 
#define D_IODescrList_sizeof                                48 

#define D_PingPongDesc_ADDR                                 5276 
#define D_PingPongDesc_ADDR_END                             5467 
#define D_PingPongDesc_sizeof                               192 

#define D_IRQMask_ADDR                                      5468 
#define D_IRQMask_ADDR_END                                  5499 
#define D_IRQMask_sizeof                                    32 

#define D_DebugAbe2hal_ADDR                                 5500 
#define D_DebugAbe2hal_ADDR_END                             5627 
#define D_DebugAbe2hal_sizeof                               128 

#define D_Debug_hal2abe_ADDR                                5628 
#define D_Debug_hal2abe_ADDR_END                            5755 
#define D_Debug_hal2abe_sizeof                              128 

#define D_Cmd2PhenixFifoDesc_ADDR                           5756 
#define D_Cmd2PhenixFifoDesc_ADDR_END                       5767 
#define D_Cmd2PhenixFifoDesc_sizeof                         12 

#define D_ctrlPortFifo_ADDR                                 5776 
#define D_ctrlPortFifo_ADDR_END                             5791 
#define D_ctrlPortFifo_sizeof                               16 

#define D_StatusFromPhoenixFifoDesc_ADDR                    5792 
#define D_StatusFromPhoenixFifoDesc_ADDR_END                5803 
#define D_StatusFromPhoenixFifoDesc_sizeof                  12 

#define D_Cmd2PhoenixFifo_ADDR                              5804 
#define D_Cmd2PhoenixFifo_ADDR_END                          5835 
#define D_Cmd2PhoenixFifo_sizeof                            32 

#define D_StatusFromPhoenixFifo_ADDR                        5836 
#define D_StatusFromPhoenixFifo_ADDR_END                    5867 
#define D_StatusFromPhoenixFifo_sizeof                      32 

#define D_Data_LSB_2_ADDR                                   5868 
#define D_Data_LSB_2_ADDR_END                               5871 
#define D_Data_LSB_2_sizeof                                 4 

#define D_Ref0_ADDR                                         5872 
#define D_Ref0_ADDR_END                                     5881 
#define D_Ref0_sizeof                                       10 

#define D_Ping_ADDR                                         5884 
#define D_Ping_ADDR_END                                     6083 
#define D_Ping_sizeof                                       200 

#define D_VIB_DL_ADDR                                       6084 
#define D_VIB_DL_ADDR_END                                   6275 
#define D_VIB_DL_sizeof                                     192 

#define D_Pong_ADDR                                         6276 
#define D_Pong_ADDR_END                                     6475 
#define D_Pong_sizeof                                       200 

#define D_DebugRegister_ADDR                                6476 
#define D_DebugRegister_ADDR_END                            6615 
#define D_DebugRegister_sizeof                              140 

#define D_Gcount_ADDR                                       6616 
#define D_Gcount_ADDR_END                                   6617 
#define D_Gcount_sizeof                                     2 

#define D_BufbaseAddr_ADDR                                  6618 
#define D_BufbaseAddr_ADDR_END                              6641 
#define D_BufbaseAddr_sizeof                                24 

#define D_BufLstDescr_ADDR                                  6644 
#define D_BufLstDescr_ADDR_END                              6667 
#define D_BufLstDescr_sizeof                                24 

#define D_VX_UL_ADDR                                        6784 
#define D_VX_UL_ADDR_END                                    6879 
#define D_VX_UL_sizeof                                      96 

#define D_VX_DL_ADDR                                        6912 
#define D_VX_DL_ADDR_END                                    7007 
#define D_VX_DL_sizeof                                      96 

#define D_MM_UL2_FIFO_ADDR                                  7040 
#define D_MM_UL2_FIFO_ADDR_END                              7143 
#define D_MM_UL2_FIFO_sizeof                                104 

#define D_TONES_DL_ADDR                                     7168 
#define D_TONES_DL_ADDR_END                                 7271 
#define D_TONES_DL_sizeof                                   104 

#define D_aUplinkRouting_ADDR                               7272 
#define D_aUplinkRouting_ADDR_END                           7287 
#define D_aUplinkRouting_sizeof                             16 

#define D_VirtAudioLoop_ADDR                                7288 
#define D_VirtAudioLoop_ADDR_END                            7291 
#define D_VirtAudioLoop_sizeof                              4 

#define D_AsrcVars_DL_VX_ADDR                               7292 
#define D_AsrcVars_DL_VX_ADDR_END                           7323 
#define D_AsrcVars_DL_VX_sizeof                             32 

#define D_CoefAddresses_DL_VX_ADDR                          7324 
#define D_CoefAddresses_DL_VX_ADDR_END                      7355 
#define D_CoefAddresses_DL_VX_sizeof                        32 

#define D_AsrcVars_UL_VX_ADDR                               7356 
#define D_AsrcVars_UL_VX_ADDR_END                           7387 
#define D_AsrcVars_UL_VX_sizeof                             32 

#define D_CoefAddresses_UL_VX_ADDR                          7388 
#define D_CoefAddresses_UL_VX_ADDR_END                      7419 
#define D_CoefAddresses_UL_VX_sizeof                        32 

#define D_AsrcVars_DL_MM_ADDR                               7420 
#define D_AsrcVars_DL_MM_ADDR_END                           7451 
#define D_AsrcVars_DL_MM_sizeof                             32 

#define D_CoefAddresses_DL_MM_ADDR                          7452 
#define D_CoefAddresses_DL_MM_ADDR_END                      7483 
#define D_CoefAddresses_DL_MM_sizeof                        32 

#define D_APS_thresholds_ADDR                               7484 
#define D_APS_thresholds_ADDR_END                           7491 
#define D_APS_thresholds_sizeof                             8 

#define D_APS_IRQ_ADDR                                      7492 
#define D_APS_IRQ_ADDR_END                                  7499 
#define D_APS_IRQ_sizeof                                    8 

#define D_TraceBufAdr_ADDR                                  7500 
#define D_TraceBufAdr_ADDR_END                              7501 
#define D_TraceBufAdr_sizeof                                2 

#define D_TraceBufOffset_ADDR                               7502 
#define D_TraceBufOffset_ADDR_END                           7503 
#define D_TraceBufOffset_sizeof                             2 

#define D_TraceBufLength_ADDR                               7504 
#define D_TraceBufLength_ADDR_END                           7505 
#define D_TraceBufLength_sizeof                             2 

#define D_AsrcVars_ECHO_REF_ADDR                            7508 
#define D_AsrcVars_ECHO_REF_ADDR_END                        7539 
#define D_AsrcVars_ECHO_REF_sizeof                          32 

#define D_MemoryPool_ADDR                                   8192 
#define D_MemoryPool_ADDR_END                               65535 
#define D_MemoryPool_sizeof                                 57344 


#endif /* _ABEDM_ADDR_H_ */
