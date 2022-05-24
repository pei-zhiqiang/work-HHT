/**
 *  @file TP_Port.h
 *
 *  @date 2022-4-1
 *
 *  @author aron566
 *
 *  @brief
 *
 *  @version V1.0
 */
#ifndef __TP_PORT_H__
#define __TP_PORT_H__
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private includes ---------------------------------------------------------*/

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/* 按键类型 */
typedef enum
{
  TP_KEY_VOL_UP,
  TP_KEY_VOL_DOWN,
  TP_KEY_BACK,
  TP_KEY_NONE,
}TP_KEY_TYPE_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/

/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 触摸初始化
 *
 */
void TP_Port_Init(void);

/**
 * @brief 触摸键事件检测
 *
 */
void TP_Port_Detect_Start(void);

/**
 * @brief 设置当前按键类型
 *
 * @param Key_Type 按键类型
 */
void TP_Port_Set_Key_Type(TP_KEY_TYPE_Typedef_t Key_Type);

/**
 * @brief 获取当前触摸坐标
 *
 * @param x x坐标存储区
 * @param y y坐标存储区
 */
void TP_Port_Get_Coordinate(uint16_t *x, uint16_t *y);

/**
 * @brief 触摸是否按下
 *
 * @return true 已按下
 * @return false 未按下
 */
bool TP_Port_Is_Pressed(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
