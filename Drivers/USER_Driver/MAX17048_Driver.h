/**
 *  @file MAX17048_Driver.h
 *
 *  @date 2021-03-04
 *
 *  @author aron566
 *
 *  @brief MAX17048驱动接口
 *  
 *  @version V1.0
 */
#ifndef MAX17048_DRIVER_H
#define MAX17048_DRIVER_H
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
void MAX17048_Driver_Init(void);
/*MAX17048休眠设置*/
void MAX17048_Driver_OnOff_Sleep(bool Sleep_En);
/*获取电池电量信息*/
uint16_t MAX17048_Driver_Get_Battery_Electric_Quantity(void);
/*获取电池电压信息*/
uint16_t MAX17048_Driver_Get_Battery_Electric_Voltage(void);
/*获取电池充电状态*/
uint8_t MAX17048_Driver_Get_Battery_Charge_State(void);
/*温度补偿*/
void MAX17048_Driver_Compensate(float Temperature);
/*回调接口*/
void MAX17048_Driver_I2C_MasterTxCpltCallback(void);
void MAX17048_Driver_I2C_MasterRxCpltCallback(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
