/**
 *  @file LCD_Port.h
 *
 *  @date 2022-04-1
 *
 *  @author PZQ
 *
 *  @brief LCD操作接口
 *
 *  @version V1.0
 */
#ifndef __LCD_PORT_H__
#define __LCD_PORT_H__
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
#define LCD_DMA_ENABLE  1 /**< 是否使用DMA加速发送 */
/** Exported typedefines -----------------------------------------------------*/
typedef enum
{
  ADJUST_STEP_1,
  ADJUST_STEP_2,
  ADJUST_STEP_3,
  ADJUST_STEP_4,
  ADJUST_STEP_5,
  ADJUST_STEP_6,
  ADJUST_STEP_7,
}LCD_BL_ADJUST_Typedef_t;

typedef enum
{
  ADJUST_BL,
  OFF_BL,
  OFF_ADJUST,
}LCD_BL_ADJUST_FUNC_Typedef_t;

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
void LCD_Port_Init(void);
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);

/**
 * @brief LCD SPI发送完成中断回调
 *
 */
void LCD_Port_SPI_TxCpltCallback(void);

/**
 * @brief LCD启动
 *
 */
void LCD_Port_Start(void);

/**
 * @brief LCD背光调节开启
 */
void LCD_BL_Adjust_Start(LCD_BL_ADJUST_FUNC_Typedef_t func);

/**
 * @brief  LCD背光调节
 */
void LCD_BL_Adjust_Stat_Switch(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
