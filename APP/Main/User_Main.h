/**
 *  @file User_Mian.h
 *
 *  @date 2021-01-11
 *
 *  @author aron566
 *
 *  @brief 主线任务
 *
 *  @version V1.0
 */
#ifndef USER_MAIN_H
#define USER_MAIN_H
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
#include "UART_Port.h"
#include "Flash_Port.h"
#include "LED_Port.h"
#include "Timer_Port.h"
#include "SAI_Port.h"
#include "Power_Management_Port.h"
#include "PressKey_Port.h"

#include "Algorithm_Port.h"
#include "Parameter_Port.h"

#include "Update_Port.h"

#include "Main_Protocol.h"

#include "Voltameter_Port.h"
#include "Gyroscope_Port.h"
#include "DAC_Port.h"
#include "ADC_Port.h"

#ifndef USE_USB_CDC_CONFIG
  #include "USB_Audio_Port.h"
#else
  #include "Audio_Debug.h"
#endif

#include "Algorithm_Port.h"

#include "LCD_Port.h"
#include "TP_Port.h"

#include "cm_backtrace_port.h"
/** Private defines ----------------------------------------------------------*/
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
#define USE_ALGORITHM_FUNCTION  1 /**< 是否使用算法功能 */

/** Exported typedefines -----------------------------------------------------*/

/*任务处理结果*/
typedef enum
{
  PROCESS_RESULT_OK = 0,  /**< 处理完成*/
  PROCESS_RESULT_RUNNING, /**< 处理中*/
  PROCESS_RESULT_ERROR
}TASK_PROCESS_RET_Typedef_t;

/*辅助初始化任务列表*/
typedef enum
{
  NO_MISCELL_INIT_TASK_RUNNING                  = 0,
  PLAY_TIPS_AUDIO_TASK_INIT                     = 1<<1,
  RESERVED_INIT_TASK                            = 1<<2,
}MISCELL_INIT_TASK_LIST_TABLE_Typedef_t;

/*辅助任务列表*/
typedef enum
{
  NO_MISCELL_TASK_RUNNING                       = 0,
  PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK             = 1<<0,
  PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK              = 1<<1,
  PLAY_ENTRY_LINE_COMPENSATION_AUDIO_TIPS_TASK  = 1<<2,
  PLAY_EXIT_LINE_COMPENSATION_AUDIO_TIPS_TASK   = 1<<3,
  RESERVED_TASK                                 = 1<<4,
}MISCELL_TASK_LIST_TABLE_Typedef_t;

/** Exported variables -------------------------------------------------------*/
/* 固件版本信息 */
extern const uint16_t APP_AIDS_Version;

extern MISCELL_INIT_TASK_LIST_TABLE_Typedef_t Miscell_Init_Task_Table;/**< 其初始化任务*/
extern MISCELL_TASK_LIST_TABLE_Typedef_t Miscell_Task_Table;/**< 其他任务*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 中断向量偏移
 *
 */
void User_InterruptVectorTable_Move(void);

/**
 * @brief 主任务初始化
 *
 */
void User_Main_Task_Init(void);

/**
 * @brief 主任务循环
 *
 */
void User_Main_Task_Process_Loop(void);

/**
 * @brief 辅助任务处理
 *
 * @param Arg 参数
 */
void User_Miscell_Task_Process_Start(void *Arg);

/**
 * @brief 算法功能切换
 *
 * @param Function_Sel 功能项 @ref Algorithm_Function_Sel_Typedef_t
 */
void Algorithm_Port_Function_Switch(int Function_Sel);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
