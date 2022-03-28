/**
 *  @file TP_Port.h
 *
 *  @date 2022-3-28
 *
 *  @author PZQ
 *  
 *  @version V1.0
 */
#ifndef __TP_PORT_H_
#define __TP_PORT_H_
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

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/


/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
/* 触摸屏初始化 */
void TP_Port_Init(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
