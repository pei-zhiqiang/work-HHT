/**                                                                             
 *  @file Tips_Audio_Port.h                                                    
 *                                                                              
 *  @date 2021年05月21日 15:34:28 星期五
 *                                                                              
 *  @author Copyright (c) 2021 aron566 <aron566@163.com>.                       
 *                                                                              
 *  @brief None.                                                                
 *                                                                              
 *  @version V1.0                                                               
 */                                                                             
#ifndef TIPS_AUDIO_PORT_H                                                          
#define TIPS_AUDIO_PORT_H                                                          
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */                          
#include <stddef.h> /**< need definition of NULL    */                          
#include <stdbool.h>/**< need definition of BOOL    */                        
#include <stdio.h>  /**< if need printf             */                          
#include <stdlib.h>                                                             
#include <string.h>                                                             
#include <limits.h> /**< need variable max value    */                          
/** Private includes ---------------------------------------------------------*/
#include "User_Main.h"                                                                        
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler                                          
extern "C" {                                                                  
#endif                                                                          
/** Private defines ----------------------------------------------------------*/
                                                                     
/** Exported typedefines -----------------------------------------------------*/
                                                                     
/** Exported constants -------------------------------------------------------*/
                                                                                
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*提示音频任务初始化*/
void Tips_Audio_Port_Init(MISCELL_TASK_LIST_TABLE_Typedef_t Task);
/*更新提示音频数据*/
TASK_PROCESS_RET_Typedef_t Tips_Audio_Port_Start(int16_t *Result_Audio_Buf, MISCELL_TASK_LIST_TABLE_Typedef_t Task);
/*获得音频文件大小*/
uint32_t Tips_Audio_Port_Get_File_Size(const char *File_Name);
/*写入新音频文件信息*/
int Tips_Audio_Port_Write_New_File_Info(const char *File_Name, uint32_t File_Size);
/*删除音频文件*/
int Tips_Audio_Port_Erase_File(const char *File_Name);
/*写入音频文件*/
int Tips_Audio_Port_Write_Data(const uint8_t *Data, uint32_t Offset, uint32_t Size);
/*读取音频文件*/
int Tips_Audio_Port_Read_Data(uint8_t *Dest_Buf, uint32_t Offset, uint32_t Size);
/*获取当前操作音频文件大小*/
uint32_t Tips_Audio_Port_Get_Current_Opt_File_Size(void);

#ifdef __cplusplus ///<end extern c                                             
}                                                                               
#endif                                                                          
#endif                                                                          
/******************************** End of file *********************************/
