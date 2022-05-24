/**
 *  @file RTC_Port.h
 *
 *  @date 2021-04-14
 *
 *  @author PZQ
 *
 *  @brief RTC操作接口，用于时间显示
 *  
 *  @version V1.0
 */
#ifndef __RTC_PORT_H__
#define __RTC_PORT_H__
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
#include "rtc.h"
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
extern RTC_DateTypeDef sdatestructure;   /**< 用于获取年月日*/
extern RTC_TimeTypeDef stimestructure;   /**< 用于获取时分秒*/
/** Exported functions prototypes --------------------------------------------*/
/* 获取RTC时间 */
void Get_Time(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
