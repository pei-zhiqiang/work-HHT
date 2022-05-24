/**
 *  @file File_Transfer_Protocol.h
 *
 *  @date 2021-05-21
 *
 *  @author aron566
 *
 *  @brief 文件传输协议
 *  
 *  @version V1.0
 */
#ifndef FILE_TRANSFER_PROTOCOL_H
#define FILE_TRANSFER_PROTOCOL_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/
#include "Main_Protocol.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/*文件传输状态*/
typedef enum
{
  FILE_TRANSFER_OK = 0,     /**< 文件传输正常*/
  FILE_RECEIVE_ERROR,       /**< 文件接收错误*/
  FILE_TRANSMIT_ERROR,      /**< 文件发送错误*/
  FILE_RECEIVE_TIMEOUT,     /**< 文件接收超时*/
  FILE_TRANSMIT_TIMEOUT,    /**< 文件发送超时*/
  FILE_TRANSFER_RUNNING,    /**< 文件传输运行中*/
}FILE_TRANSFER_STATE_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*文件传输协议栈初始化*/
void File_Transfer_Protocol_Init(void);
/*文件升级模式使能*/
void File_Transfer_Protocol_Function_Enable(bool Cs);
/*检查是否进入文件升级模式*/
bool Is_File_Transfer_Mode_Entry(Uart_Dev_Handle_t *Uart_Opt_Handle);
/*启动传输协议解析*/
RETURN_TYPE_Typedef_t File_Transfer_Frame_Decode_Start(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
