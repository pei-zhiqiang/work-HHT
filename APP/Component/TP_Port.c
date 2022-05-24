/**
 *  @file TP_Port.c
 *
 *  @date 2022-4-1
 *
 *  @author PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief   触摸屏操作接口
 *
 *  @details
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "TP_Port.h"
#include "TP_Driver.h"
#include "LCD_Port.h"
#include "main.h"
#include "lv_app.h"
#include "lvgl.h"
#include "lv_app_test.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define TP_KEY_TRIGGER_TIMR  30              // TP_KEY_TRIGGER_TIMR * 5 MS
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static TP_KEY_TYPE_Typedef_t Pre_TP_Key_Type = TP_KEY_NONE;
static TP_KEY_TYPE_Typedef_t TP_Key_Type = TP_KEY_NONE;
static uint16_t Key_Type_Count = 0;

/* 触摸句柄 */
static TP_HANDLE_Typedef_t TP_Handle;
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
 * @brief 触摸是否按下
 *
 * @return true 已按下
 * @return false 未按下
 */
bool TP_Port_Is_Pressed(void)
{
  if(0 == TP_Handle.sta)
  {
    return false;
  }
  else
  {
    TP_Handle.sta = 0;
    return true;
  }
}

/**
 * @brief 获取当前触摸坐标
 *
 * @param x x坐标存储区
 * @param y y坐标存储区
 */
void TP_Port_Get_Coordinate(uint16_t *x, uint16_t *y)
{
  *x = TP_Handle.x;
  *y = TP_Handle.y;
}

/**
 * @brief 设置当前按键类型
 *
 * @param Key_Type 按键类型
 */
void TP_Port_Set_Key_Type(TP_KEY_TYPE_Typedef_t Key_Type)
{
  TP_Key_Type = Key_Type;
}

/**
 * @brief 触摸中断回调，更新坐标点
 *
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == TP_EINT_Pin)
  {
    TP_Driver_Coordinate_Update();
  }
}

/**
 * @brief 触摸键事件检测
 *
 */
void TP_Port_Detect_Start(void)
{
  if(TP_Key_Type == TP_KEY_NONE)
  {
    return;
  }
  if(TP_Key_Type == Pre_TP_Key_Type)
  {
    Key_Type_Count++;
    if(Key_Type_Count > TP_KEY_TRIGGER_TIMR)
    {
      switch(TP_Key_Type)
      {
        case TP_KEY_VOL_UP:
          if(Current_UI_Inf->ui_idx == MAIN_IDX)
          {
            Vol_L = Vol_L >= 7 ? 7 : ++Vol_L;
            Vol_R = Vol_R >= 7 ? 7 : ++Vol_R;
            Algorithm_Port_Set_VC_Level(Vol_L, ALGORITHM_CHANNEL_CH1);
            Algorithm_Port_Set_VC_Level(Vol_R, ALGORITHM_CHANNEL_CH2);
            lv_bar_set_value(main_bar_l,Vol_L,LV_ANIM_ON);
            lv_bar_set_value(main_bar_r, Vol_R,LV_ANIM_ON);
          }
          else if(Current_UI_Inf->ui_idx == TEST_TP_IDX)
          {
            lv_label_set_text(tp_label1, " 左按键正常 ");
          }
          break;
        case TP_KEY_VOL_DOWN:
          if(Current_UI_Inf->ui_idx == MAIN_IDX)
          {
            Vol_L = Vol_L <= 0 ? 0 : --Vol_L;
            Vol_R = Vol_R <= 0 ? 0 : --Vol_R;
            Algorithm_Port_Set_VC_Level(Vol_L, ALGORITHM_CHANNEL_CH1);
            Algorithm_Port_Set_VC_Level(Vol_R, ALGORITHM_CHANNEL_CH2);
            lv_bar_set_value(main_bar_l,Vol_L,LV_ANIM_ON);
            lv_bar_set_value(main_bar_r, Vol_R,LV_ANIM_ON);
          }
          else if(Current_UI_Inf->ui_idx == TEST_TP_IDX)
          {
            lv_label_set_text(tp_label1, " 右按键正常 ");
          }
          break;
        case TP_KEY_BACK:
          HHT_Touch_Back();
          break;
        default:
          break;
      }
      TP_Key_Type = Pre_TP_Key_Type = TP_KEY_NONE;
      Key_Type_Count = 0;
    }
  }
  else
  {
    Key_Type_Count = 0;
    Pre_TP_Key_Type = TP_Key_Type;
  }
}

/**
 * @brief 触摸接口初始化
 *
 */
void TP_Port_Init(void)
{
  /* 触摸驱动初始化 */
  TP_Driver_Init(&TP_Handle);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
