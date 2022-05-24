/**
 *  @file Update_Protocol.h.h
 *
 *  @date 2021-03-01
 *
 *  @author aron566
 *
 *  @brief
 *
 *  @version V1.0
 */
#ifndef UPDATE_PROTOCOL_H
#define UPDATE_PROTOCOL_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
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
#include "UART_Port.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*更新协议栈初始化*/
void Upgrade_Protocol_Init(void);
/*检测是否进入固件升级报文解析*/
bool Is_Upgrade_Mode_Entry(Uart_Dev_Handle_t *Uart_Opt_Handle);
/*固件升级模式设置*/
void Upgrade_Mode_Set(uint8_t mode);
/*固件升级模式失能*/
void Upgrade_Mode_Reset(uint8_t mode);
/*固件升级帧解析*/
RETURN_TYPE_Typedef_t Update_Frame_Decode_Start(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
