/**
 *  @file LCD_Port.c
 *
 *  @date 2022-4-1
 *
 *  @author PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief  LCD操作接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "LCD_Port.h"
#include "LCD_Driver.h"
//#include "lv_app.h"
#include "lv_port.h"
#include "lv_port_disp.h"
#include "main.h"
#include "lv_app.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
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
 * @brief LCD SPI发送完成中断回调
 *
 */
void LCD_Port_SPI_TxCpltCallback(void)
{
  LCD_SPI_TxCpltCallback();
}

void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
#if LCD_DMA_ENABLE
  uint16_t width = ex - sx + 1;
  uint16_t height = ey - sy + 1;
  BlockWrite(sx, ex, sy, ey);
  uint32_t size = width * height;
  WriteData_DMA(color, size);
#else
  uint16_t i, j;
  uint16_t width = ex - sx + 1;
  uint16_t height = ey - sy + 1;
  BlockWrite(sx, ex, sy, ey);
  for(i = 0; i < height; i++)
  {
    for(j = 0; j < width; j++)
    {
      WriteData_16bit(color[i*width+j]);
    }
  }
#endif
}

/**
 * @brief LCD启动
 *
 */
void LCD_Port_Start(void)
{
  LV_Task_Start();
}

/**
 * @brief LCD接口初始化
 *
 */
void LCD_Port_Init(void)
{
  /* 显示驱动初始化 */
  LCD_Driver_Init();

  /* LTVGL初始化 */
  LV_Init();

  /* UI界面初始化 */
  HHT_UI_Init();
}
/**
 * @brief LCD亮度调节开关
 *
 */
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

/**
 * @brief 亮度等级转换
 *
 */
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
          lcd_bl_step = ADJUST_STEP_4;
					LCD_BL_HIGH;
					//lcd_bl_func = OFF_ADJUST;
				}
				break;
      case ADJUST_STEP_4:
				if(lcd_bl_adjust_count++ > LCD_BL_ADJUST_SIGNAL_DELAY)
				{
          lcd_bl_step = ADJUST_STEP_5;
					LCD_BL_LOW;
				}
        break;
      case ADJUST_STEP_5:
				if(lcd_bl_adjust_count++ > LCD_BL_ADJUST_SIGNAL_DELAY)
				{
          lcd_bl_step = ADJUST_STEP_6;
					LCD_BL_HIGH;
          //lcd_bl_func = OFF_ADJUST;
				}
				break;
     case ADJUST_STEP_6:
				if(lcd_bl_adjust_count++ > LCD_BL_ADJUST_SIGNAL_DELAY)
				{
					lcd_bl_step = ADJUST_STEP_7;
					LCD_BL_LOW;
				}
				break;
     case ADJUST_STEP_7:
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
