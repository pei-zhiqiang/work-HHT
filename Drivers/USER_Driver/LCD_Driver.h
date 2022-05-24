/**
 *  @file LCD_Driver.h
 *
 *  @date 2022-04-1
 *
 *  @author PZQ
 *
 *  @brief LCD驱动
 *
 *  @version V1.0
 */
#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__
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

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
#define Delay         HAL_Delay

#define LCD_CS_CLR    do \
                      {HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_Port, LCD_SPI_CS_Pin, GPIO_PIN_RESET);}while(0)

#define LCD_CS_SET    do \
                      {HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_Port, LCD_SPI_CS_Pin, GPIO_PIN_SET);}while(0)

#define	LCD_DC_CLR    do \
                      {HAL_GPIO_WritePin(LCD_DATA_CMD_CS_GPIO_Port, LCD_DATA_CMD_CS_Pin, GPIO_PIN_RESET);}while(0)

#define	LCD_DC_SET    do \
                      {HAL_GPIO_WritePin(LCD_DATA_CMD_CS_GPIO_Port, LCD_DATA_CMD_CS_Pin, GPIO_PIN_SET);}while(0)

#define LCD_RESRT_CLR do \
                      {HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);}while(0)

#define LCD_RESRT_SET do \
                      {HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);}while(0)

#define LCD_BL_LOW    do \
                      {HAL_GPIO_WritePin(VCC_BACK_LIGHT_EN_GPIO_Port, VCC_BACK_LIGHT_EN_Pin, GPIO_PIN_RESET);}while(0)

#define LCD_BL_HIGH   do \
                      {HAL_GPIO_WritePin(VCC_BACK_LIGHT_EN_GPIO_Port, VCC_BACK_LIGHT_EN_Pin, GPIO_PIN_SET);}while(0)
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief LCD驱动初始化
 *
 */
void LCD_Driver_Init(void);

void WriteData_DMA(uint16_t *data, uint16_t len);
void WriteData_16bit(uint16_t color);
void BlockWrite(uint32_t Xstart, uint32_t Xend, uint32_t Ystart, uint32_t Yend);

/**
 * @brief LCD cs使能控制
 *
 * @param Cs 为0时，CS脚为低，为1时，CS脚为高
 */
void LCD_Driver_CS_Ctl(uint8_t Cs);

/**
 * @brief LCD 背光使能控制
 *
 * @param Cs 为0时，为低，为1时，为高
 */
void LCD_Driver_BL_Ctl(uint8_t Cs);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
