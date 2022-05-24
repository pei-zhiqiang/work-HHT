/**
 *  @file TP_Port.h
 *
 *  @date 2022-4-1
 *
 *  @author PZQ
 *
 *  @brief 触摸屏驱动（泰斯强厂商）
 *
 *  @version V1.0
 */
#ifndef __TP_DRIVER_H__
#define __TP_DRIVER_H__
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
typedef struct
{
  uint8_t ID;
  uint8_t Version;
  uint16_t x;
  uint16_t y;
  uint16_t last_x;
  uint16_t last_y;
  uint8_t sta;
}TP_HANDLE_Typedef_t;
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/

/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 触摸重置
 *
 */
void TP_Driver_Reset(void);

/**
 * @brief 触摸初始化
 *
 * @param TP_Handle 初始化句柄
 */
void TP_Driver_Init(TP_HANDLE_Typedef_t *TP_Handle);

/**
 * @brief 触摸坐标更新
 *
 */
void TP_Driver_Coordinate_Update(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
