/**
 *  @file Voltameter_Port.h
 *
 *  @date 2021-03-04
 *
 *  @author aron566
 *
 *  @brief 电池电量监控接口
 *  
 *  @version V1.0
 */
#ifndef VOLTAMETER_PORT_H
#define VOLTAMETER_PORT_H
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

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*电池监控接口初始化*/
void Voltameter_Port_Init(void);
/*获取电池电量信息*/
uint16_t Voltameter_Port_Get_Battery_Electric_Quantity(void);
/*更新电池电量信息*/
void Voltameter_Port_Update_Battery_Electric_Quantity(void);
/*获取电池电压信息*/
uint16_t Voltameter_Port_Get_Battery_Electric_Voltage(void);
/*获取电池充电状态*/
uint8_t Voltameter_Port_Get_Battery_Charge_State(void);
/*回调接口*/
void Voltameter_Port_I2C_MasterTxCpltCallback(void);
void Voltameter_Port_I2C_MasterRxCpltCallback(void);
/*电池监控接口休眠设置*/
void Voltameter_Port_OnOff_Sleep(bool Sleep_En);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
