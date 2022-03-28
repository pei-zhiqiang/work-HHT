/**
 *  @file LCD_Port.c
 *
 *  @date 2021-12-16
 *
 *  @author zgl
 *
 *  @brief LCD_Port
 *
 *  @details 1、
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "LCD_Driver.h"
#include "LCD_Port.h"
#include "main.h"
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define LCD_BL_ADJUST_SIGNAL_DELAY      3
#define LCD_BL_OFF_SIGNAL_DELAY         15

																										
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
LCD_BL_ADJUST_Typedef_t lcd_bl_step;

LCD_BL_ADJUST_FUNC_Typedef_t lcd_bl_func = OFF_ADJUST;

uint16_t lcd_bl_adjust_count = 0;
/** Private variables --------------------------------------------------------*/


/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   LCD接口初始化
  ******************************************************************
  */
void LCD_Port_Init(void)
{
	 //LCD_Init();
  LCD_Init_1();
}

void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
#ifdef LCD_DMA_ENABLE
	uint16_t width=ex-sx+1; 	
	uint16_t height=ey-sy+1;		
	BlockWrite(sx, ex, sy, ey); 	
	uint32_t size = width * height;
	WriteData_DMA(color, size);
#else
	uint16_t i,j;			
	uint16_t width=ex-sx+1; 	
	uint16_t height=ey-sy+1;		
	BlockWrite(sx, ex, sy, ey); 
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			WriteData_16bit(color[i*width+j]);	
		}
	} 
#endif
}  

void LCD_BL_Adjust_Start(LCD_BL_ADJUST_FUNC_Typedef_t func)
{
	if(func == ADJUST_BL)
	{
		lcd_bl_func = ADJUST_BL;
		lcd_bl_step = ADJUST_STEP_1;
	}
	if(func == OFF_BL)
	{
		lcd_bl_func = OFF_BL;
	}
	LCD_BL_LOW;
	lcd_bl_adjust_count = 0;
}

void LCD_BL_Adjust_Stat_Switch(void)
{
	if(lcd_bl_func == ADJUST_BL)
	{
		switch(lcd_bl_step)
		{
			case ADJUST_STEP_1:
				if(lcd_bl_adjust_count++ > LCD_BL_ADJUST_SIGNAL_DELAY)
				{
					lcd_bl_step = ADJUST_STEP_2;
					LCD_BL_HIGH;
				}
				break;
			case ADJUST_STEP_2:
				if(lcd_bl_adjust_count++ > LCD_BL_ADJUST_SIGNAL_DELAY)
				{
					lcd_bl_step = ADJUST_STEP_3;
					LCD_BL_LOW;
				}
				break;
			case ADJUST_STEP_3:
				if(lcd_bl_adjust_count++ > LCD_BL_ADJUST_SIGNAL_DELAY)
				{
					LCD_BL_HIGH;
					lcd_bl_func = OFF_ADJUST;
				}
				break;
			default:
				break;
		}
	}
	else if(lcd_bl_func == OFF_BL)
	{
		if(lcd_bl_adjust_count++ > LCD_BL_OFF_SIGNAL_DELAY)
		{
			lcd_bl_func = OFF_ADJUST;			
		}
	}
}
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
