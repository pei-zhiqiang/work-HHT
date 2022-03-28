/**
 *  @file ILI9488_Driver.h
 *
 *  @date 2021-12-16
 *
 *  @author zgl
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief ILI9488驱动
 */
#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< nedd definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/

/** Private defines ----------------------------------------------------------*/

void LCD_Init(void);
void LCD_Init_1(void);   // 测试
void WriteData_DMA(unsigned short* data,unsigned short len);
void WriteData_16bit(unsigned short color);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) ;
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
