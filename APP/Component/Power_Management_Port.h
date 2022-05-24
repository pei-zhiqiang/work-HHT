/**
 *  @file Power_Management_Port.h
 *
 *  @date 2021-04-28
 *
 *  @author aron566
 *
 *  @brief 电源管理接口
 *  
 *  @version V1.0
 */
#ifndef POWER_MANAGEMENT_PORT_H
#define POWER_MANAGEMENT_PORT_H
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

/*设备复位*/
void PowerManagement_Port_Restart(void);
/*设备进入停机模式*/
void Power_Management_Port_Enter_Stop_Mode(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
