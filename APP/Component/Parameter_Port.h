/**
 *  @file Parameter_Port.h
 *
 *  @date 2021-02-26
 *
 *  @author aron566
 *
 *  @brief 参数存储操作
 *
 *  @version V1.0
 */
#ifndef PARAMETER_PORT_H
#define PARAMETER_PORT_H
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

#include "DAC_Port.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/*其他辅助参数*/
typedef struct
{
  float Preamplification_Factor;      /**< 前置放大系数*/
  uint8_t Left_Channel_Compensation;  /**< 左通道音量补偿*/
  uint8_t Right_Channel_Compensation; /**< 右通道音量补偿*/
  uint16_t EQ_5_Band_FQ[5];           /**< EQ均衡器5波段中心频率参数*/
  uint8_t EQ_5_Band_Gain[5];          /**< EQ均衡器5波段中心频率对应增益参数*/
  DAC_AGC_PAR_Typedef_t DAC_AGC_Par;  /**< DAC硬件AGC参数*/
  uint8_t Left_6_Band_Ref_Min_Gain[6]; /**< 左听力曲线最低dB数值*/
  uint8_t Right_6_Band_Ref_Min_Gain[6];/**< 右听力曲线最低dB数值*/
  uint8_t Left_6_Band_Ref_Max_Gain[6]; /**< 左听力曲线最大(不舒适)dB数值*/
  uint8_t Right_6_Band_Ref_Max_Gain[6];/**< 右听力曲线最大(不舒适)dB数值*/
  int8_t Left_Auto_EQ_Gain[6];        /**< 左通道自动EQ*/
  int8_t Right_Auto_EQ_Gain[6];       /**< 右通道自动EQ*/
  uint16_t Meter_Device_Is_Init;      /**< 仪表初始化状态*/
  uint8_t Par_Auto_Save_Cs;           /**< 参数自动保存设置*/
}MISCELLANEOUS_PAR_Typedef_t;

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/

extern MISCELLANEOUS_PAR_Typedef_t g_Miscellaneous_Par;/**< 辅助参数*/
/** Exported functions prototypes --------------------------------------------*/

/*参数接口初始化*/
void Parameter_Port_Init(void);
/*参数接口随存运行*/
void Parameter_Port_Start(void);
/*参数接口更新并写入参数*/
bool Parameter_Port_Update(void);
/*参数接口读取参数*/
bool Parameter_Port_Read(void);
/*恢复默认参数*/
void Parameter_Port_Reset_Task_En(uint8_t Cs);
/*参数自动保存设置*/
void Parameter_Port_Auto_Save_Set(uint8_t Cs);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
