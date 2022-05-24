/**
 *  @file SAI_Port.h
 *
 *  @date 2021-04-19
 *
 *  @author aron566
 *
 *  @brief sai操作接口-MIC
 *
 *  @version V1.0
 */
#ifndef SAI_PORT_H
#define SAI_PORT_H
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
#include "main.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
#define SAI_PORT_DAC_SEND_ONE_CHANNEL_SIZE  (8U)
#define SAI_PORT_DAC_SEND_DATA_SIZE_MAX     (SAI_PORT_DAC_SEND_ONE_CHANNEL_SIZE * 2U)
/** Exported variables -------------------------------------------------------*/

/** Exported functions prototypes --------------------------------------------*/

/*SAI操作初始化*/
void SAI_Port_Init(void);
/*SAI启动数据处理*/
void SAI_Port_Start(void);
/*SAI发送端开关静音*/
void SAI_Port_Sender_Set_Mute(bool On_Off);
/*SAI启停数据处理*/
void SAI_Port_Pause_Resume_Process(bool Is_Pause);
/*重置SAI1采样率*/
void SAI_Port_Reset_SAI1_Init(uint32_t AudioFrequency);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
