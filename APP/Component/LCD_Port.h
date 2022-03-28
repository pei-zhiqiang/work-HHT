/**
 *  @file LCD_Port.h
 *
 *  @date 2021-12-16
 *
 *  @author zgl
 *  
 *  @version V1.0
 */
#ifndef LCD_PORT_H
#define LCD_PORT_H
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
typedef enum 
{
	ADJUST_STEP_1,   
	ADJUST_STEP_2,  
	ADJUST_STEP_3,
	ADJUST_STEP_4,
}LCD_BL_ADJUST_Typedef_t;

typedef enum 
{
	ADJUST_BL,   
	OFF_BL, 
   OFF_ADJUST,	
}LCD_BL_ADJUST_FUNC_Typedef_t;

/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

void LCD_Port_Init(void);
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);
void LCD_BL_Adjust_Start(LCD_BL_ADJUST_FUNC_Typedef_t func);
void LCD_BL_Adjust_Stat_Switch(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
