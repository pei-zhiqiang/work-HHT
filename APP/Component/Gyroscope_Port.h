/**
 *  @file Gyroscope_Port.h
 *
 *  @date 2021-03-25
 *
 *  @author aron566
 *
 *  @brief 陀螺仪应用层接口
 *
 *  @version V1.0
 */
#ifndef GYROSCOPE_PORT_H
#define GYROSCOPE_PORT_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*初始化陀螺仪设备*/
void Gyroscope_Port_Init(void);
/*启动读取陀螺仪设备数值*/
void Gyroscope_Port_Start(void);
/*读取陀螺仪设备X方向角度数值*/
double Gyroscope_Port_Get_X_Angle(void);
/*读取陀螺仪设备Y方向角度数值*/
double Gyroscope_Port_Get_Y_Angle(void);
/*陀螺仪设备是否是垂直状态*/
bool Gyroscope_Port_Is_Vertical_State(void);

/**
 * @brief 获取设备工作状态
 *
 * @return true 正常
 * @return false
 */
bool Gyroscope_Port_Get_Work_State(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
