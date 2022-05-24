/**
 *  @file PressKey_Port.h
 *
 *  @date 2021-04-19
 *
 *  @author aron566
 *
 *  @brief
 *
 *  @version V1.0
 */
#ifndef PRESS_KEY_PORT_H
#define PRESS_KEY_PORT_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/
#define USE_DEBUG_PRESSKEY  0/**< 启用按键事件检测调试打印*/
/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 按键检测初始化
 *
 */
void PressKey_Port_Init(void);
#if USE_DEBUG_PRESSKEY
  /*按键检测事件调试*/
  void PressKey_Port_Start(void);
#endif

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
