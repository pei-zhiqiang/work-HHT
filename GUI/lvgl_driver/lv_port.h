/**
 *  @file xxx.h
 *
 *  @date 2021-01-25
 *
 *  @author zgl
 *
 *  @brief 
 *  
 *  @version V1.0
 */
#ifndef _LV_INIT__
#define _LV_INIT__
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
/**
 * @name 
 * @{
 */
/** @}*/
/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
 void LV_Init(void);
 void LV_Task_Start(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/

