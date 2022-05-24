/**
 *  @file lv_app_test.c
 *
 *  @date 2022-05-7
 *
 *  @author PZQ
 *
 *  @copyright None.
 *
 *  @brief 用于测试屏幕好坏和通过屏幕测试板子上一些硬件的好坏
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "lv_app_test.h"
#include "lv_app.h"
#include "main.h"
#include "MAX17048_Driver.h"
#include "Flash_Port.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
lv_obj_t* cont_test = NULL;
lv_obj_t* cont_tp = NULL;
lv_obj_t* tp_label1 = NULL;
/** Private variables --------------------------------------------------------*/
LV_FONT_DECLARE(hht_font_30);      /**< 申明字体,大小30 */ 

static lv_obj_t* cont_ear = NULL;
static lv_obj_t* cont_charge = NULL;
static lv_obj_t* cont_lcd = NULL;
static lv_obj_t* cont_gyro = NULL; 
static lv_obj_t* cont_flash = NULL;
static lv_obj_t* cont_mic = NULL;
static lv_obj_t* cont_inspect = NULL;
static lv_obj_t* btn_ear = NULL;     /**< 该按键用于进入耳机测试界面 */
static lv_obj_t* btn_charge = NULL;   /**< 该按键用于进入充电测试界面 */
static lv_obj_t* btn_lcd = NULL;
static lv_obj_t* btn_gyro = NULL;
static lv_obj_t* btn_flash = NULL;
static lv_obj_t* btn_mic = NULL;
static lv_obj_t* btn_tp = NULL;
static lv_obj_t* btn_lcd_test = NULL;
static lv_obj_t* btn_mic_test = NULL;
static lv_obj_t* btn_inspect = NULL;
static lv_task_t* task_ear = NULL;
static lv_task_t* task_charge = NULL;
static lv_task_t* task_gyro = NULL;
static lv_obj_t* label_gyro_x = NULL;
static lv_obj_t* label_gyro_y = NULL;
static lv_style_t lable_ear_style2;
static lv_style_t lcd_bg_style;
static lv_style_t btn_release_style;  /* 按键释放时样式 */
static lv_style_t btn_press_style;    /* 按键按下时样式 */
static lv_style_t g_font_style;
static lv_obj_t* label_mic = NULL;
static lv_obj_t* label_charge = NULL;
static uint8_t mic_channel_num = 0;
/** Private function prototypes ----------------------------------------------*/
void enter_ear_ui(void);
void enter_charge_test_ui(void);
void enter_lcd_test_ui(void);
void enter_gyro_test_ui(void);
void enter_flash_test_ui(void);
void enter_mic_test_ui(void);
void enter_inspect_test_ui(void);
void btn_event_handler(lv_obj_t* obj, lv_event_t event);
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
 * @brief 测试硬件任务
 *
 */
static void test_task_cb(lv_task_t* task)
{
  if(task == task_ear)
  {
    if(Current_UI_Inf->ui_idx == TEST_EAR_IDX)
    {
      /* 检测耳机是否插入 */
      if(1 == HAL_GPIO_ReadPin(HS_DET_GPIO_Port, HS_DET_Pin))
      {
        lv_obj_t* label2 = lv_label_create(cont_ear, NULL);
        lv_label_set_body_draw(label2,true);//使能背景绘制
        lv_label_set_long_mode(label2,LV_LABEL_LONG_CROP);//设置为长文本
        lv_obj_set_size(label2, 200,40);
        lv_label_set_style(label2, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
        lv_label_set_text(label2, " 耳机已插入 ");
        lv_obj_align(label2, cont_ear, LV_ALIGN_CENTER, 0, 0);
      }
      else
      {
        lv_obj_t* label3 = lv_label_create(cont_ear, NULL);
        lv_label_set_body_draw(label3,true);//使能背景绘制
        lv_label_set_long_mode(label3,LV_LABEL_LONG_CROP);//设置为长文本
        lv_obj_set_size(label3, 200,40);
        lv_label_set_style(label3, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
        lv_label_set_text(label3, " 请插入耳机 ");
        lv_obj_align(label3, cont_ear, LV_ALIGN_CENTER, 0, 0);
      }
    }
  }
  else if(task == task_charge)
  {
    if(Current_UI_Inf->ui_idx == TEST_CHARGE_IDX)
    {
      if(1 == Voltameter_Port_Get_Battery_Charge_State())
      {
        lv_obj_t* label2 = lv_label_create(cont_charge, NULL);
        lv_label_set_body_draw(label2,true);//使能背景绘制
        lv_label_set_long_mode(label2,LV_LABEL_LONG_CROP);//设置为长文本
        lv_obj_set_size(label2, 200,40);
        lv_label_set_style(label2, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
        lv_label_set_text(label2, " 正在充电 ");
        lv_obj_align(label2, cont_charge, LV_ALIGN_CENTER, 0, 30);
      }
      else
      {
        lv_obj_t* label3 = lv_label_create(cont_charge, NULL);
        lv_label_set_body_draw(label3,true);//使能背景绘制
        lv_label_set_long_mode(label3,LV_LABEL_LONG_CROP);//设置为长文本
        lv_obj_set_size(label3, 200,40);
        lv_label_set_style(label3, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
        lv_label_set_text(label3, " 请充电 ");
        lv_obj_align(label3, cont_charge, LV_ALIGN_CENTER, 0, 30);
      }
      char buff[10];
      sprintf(buff, "%d%%", MAX17048_Driver_Get_Battery_Electric_Quantity());
      lv_label_set_text(label_charge, buff); //设置文本
    } 
  }
  else if(task == task_gyro)
  {
    if(Current_UI_Inf->ui_idx == TEST_GYRO_IDX)
    {
      if(label_gyro_x != NULL || label_gyro_y != NULL)
      {
        lv_obj_del(label_gyro_x);
        lv_obj_del(label_gyro_y);
        label_gyro_x = NULL;
        label_gyro_y = NULL;        
      }
      /* 创建一个 label 标签来显示当前的陀螺仪的X坐标值 */
      label_gyro_x = lv_label_create(cont_gyro,NULL);
      lv_obj_align(label_gyro_x,cont_gyro,LV_ALIGN_CENTER,0,0);//设置与 lmeter1 居中对齐
      //使能自动对齐功能,当文本长度发生变化时,它会自动对齐的
      lv_obj_set_auto_realign(label_gyro_x,true);
      char buff_x[10];
      sprintf(buff_x,"X:%0.2f",Gyroscope_Port_Get_X_Angle());
      lv_label_set_text(label_gyro_x,buff_x);//设置文本
      
      /* 创建一个 label 标签来显示当前的陀螺仪的Y坐标值 */
      label_gyro_y = lv_label_create(cont_gyro,NULL);
      lv_obj_align(label_gyro_y,cont_gyro,LV_ALIGN_CENTER,0,35);//设置与 lmeter1 居中对齐
      //使能自动对齐功能,当文本长度发生变化时,它会自动对齐的
      lv_obj_set_auto_realign(label_gyro_y,true);
      char buff_y[10];
      sprintf(buff_y,"Y:%0.1f",Gyroscope_Port_Get_Y_Angle());
      lv_label_set_text(label_gyro_y,buff_y);//设置文本
    }
  }
}
/**
 * @brief LCD测试回调事件
 *
 */
static void lcd_test_event_handler(lv_obj_t* obj, lv_event_t event)
{
  static uint8_t state = 0;
  if(event==LV_EVENT_CLICKED)
  {
    switch(state)
    {
      case 0:
        lcd_bg_style.body.main_color = LV_COLOR_RED;
        lcd_bg_style.body.grad_color = LV_COLOR_RED;
        lv_obj_set_style(cont_lcd, &lcd_bg_style); //设置容器的样式
        state = 1;
        break;
      case 1:
        lcd_bg_style.body.main_color = LV_COLOR_GREEN;
        lcd_bg_style.body.grad_color = LV_COLOR_GREEN;
        lv_obj_set_style(cont_lcd, &lcd_bg_style); //设置容器的样式
        state = 2;
        break; 
      case 2:
        lcd_bg_style.body.main_color = LV_COLOR_BLUE;
        lcd_bg_style.body.grad_color = LV_COLOR_BLUE;
        lv_obj_set_style(cont_lcd, &lcd_bg_style); //设置容器的样式
        state = 0;
        break;
      default:
        break;
    }
  }
}
/**
 * @brief 麦克风测试回调事件
 *
 */
static void mic_test_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_CLICKED)
  {
    switch(mic_channel_num)
    {
      case 0:
        Algorithm_Port_Set_BF_Default_MIC(0);
        lv_label_set_text(label_mic, " 0 ");
        mic_channel_num = 1;
        break;
      case 1:
        Algorithm_Port_Set_BF_Default_MIC(1);
        lv_label_set_text(label_mic, " 1 ");
        mic_channel_num = 2;
        break; 
      case 2:
        Algorithm_Port_Set_BF_Default_MIC(2);
        lv_label_set_text(label_mic, " 2 ");
        mic_channel_num = 3;
        break;
      case 3:
        Algorithm_Port_Set_BF_Default_MIC(3);
        lv_label_set_text(label_mic, " 3 ");
        mic_channel_num = 0;
        break;
      default:
        break;
    }
  }
}
/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
 * @brief 测试板子一些硬件功能
 *
 */
void lv_test_ui(void)
{
  if(cont_mode != NULL)
  {
    lv_obj_del(cont_mode);
    cont_mode = NULL;
  }
  if(cont_ear != NULL)
  {
    lv_obj_del(cont_ear);
    cont_ear = NULL;
  }
  if(cont_flash != NULL)
  {
    lv_obj_del(cont_flash);
    cont_flash = NULL;
  }
  if(cont_lcd != NULL)
  {
    lv_obj_del(cont_lcd);
    cont_lcd = NULL;
  }
  if(cont_gyro != NULL)
  {
    lv_obj_del(cont_gyro);
    cont_gyro = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_IDX];
  cont_test = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_test, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_test, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_test, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  /* 创建一个标签，显示"硬件测试" */
//  static lv_style_t lable_style;
  lv_style_copy(&g_font_style, &lv_style_plain_color);
  g_font_style.text.font = &hht_font_30;
  g_font_style.text.color = LV_COLOR_BLACK;
  lv_obj_t* label = lv_label_create(cont_test, NULL);
  lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(label, " 硬件测试 ");
  lv_obj_align(label, cont_test, LV_ALIGN_IN_TOP_MID, 0, 20);
  
  /* 设置按键样式 */
  
  /* 释放状态下的样式 */
  lv_style_copy(&btn_release_style, &lv_style_plain_color);
  btn_release_style.body.main_color = LV_COLOR_MAKE(61,158,255);
  btn_release_style.body.grad_color = btn_release_style.body.main_color;
  btn_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按下状态下的样式 */
  lv_style_copy(&btn_press_style,&lv_style_plain_color);
  btn_press_style.body.opa = LV_OPA_0;//设置背景色透明
	btn_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
	btn_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制
  
  /* 创建一个按键,用于进入耳机测试功能界面 */
  btn_ear = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_ear, 70, 70);
  lv_obj_align(btn_ear, cont_test, LV_ALIGN_IN_LEFT_MID, 30, -60);//设置对齐方式
	lv_btn_set_style(btn_ear,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_ear,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_ear = lv_label_create(cont_test, NULL);
  lv_label_set_style(label_ear, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(label_ear, " 耳机 ");
  lv_obj_align(label_ear, btn_ear, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入充电测试功能界面 */
  btn_charge = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_charge, 70, 70);
  lv_obj_align(btn_charge, btn_ear, LV_ALIGN_OUT_RIGHT_MID, 10, 0);//设置对齐方式
	lv_btn_set_style(btn_charge,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_charge,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_charge = lv_label_create(cont_test, NULL);
  lv_label_set_style(label_charge, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(label_charge, " 充电 ");
  lv_obj_align(label_charge, btn_charge, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入蓝牙测试功能界面 */
  lv_obj_t* btn3 = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn3, 70, 70);
  lv_obj_align(btn3, btn_charge, LV_ALIGN_OUT_RIGHT_MID, 10, 0);//设置对齐方式
	lv_btn_set_style(btn3,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn3,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label3 = lv_label_create(cont_test, NULL);
  lv_label_set_style(label3, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(label3, " 蓝牙 ");
  lv_obj_align(label3, btn3, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入麦克风测试功能界面 */
  btn_mic = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_mic, 70, 70);
  lv_obj_align(btn_mic, btn3, LV_ALIGN_OUT_RIGHT_MID, 10, 0);//设置对齐方式
	lv_btn_set_style(btn_mic,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_mic,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_mic = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label4, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label_mic, " Mic ");
  lv_obj_align(label_mic, btn_mic, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入RTC测试功能界面 */
  lv_obj_t* btn5 = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn5, 70, 70);
  lv_obj_align(btn5, btn_ear, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);//设置对齐方式
	lv_btn_set_style(btn5,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn5,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label5 = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label5, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label5, " RTC ");
  lv_obj_align(label5, btn5, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入陀螺仪测试功能界面 */
  btn_gyro = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_gyro, 70, 70);
  lv_obj_align(btn_gyro, btn_charge, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_gyro,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_gyro,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_gyro = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label6, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label_gyro, " Gyro ");
  lv_obj_align(label_gyro, btn_gyro, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入Flash测试功能界面 */
  btn_flash = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_flash, 70, 70);
  lv_obj_align(btn_flash, btn3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_flash,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_flash,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_flash = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label7, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label_flash, " Flash ");
  lv_obj_align(label_flash, btn_flash, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入LCD测试功能界面 */
  btn_lcd = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_lcd, 70, 70);
  lv_obj_align(btn_lcd, btn_mic, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_lcd,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_lcd,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_lcd = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label8, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label_lcd, " LCD ");
  lv_obj_align(label_lcd, btn_lcd, LV_ALIGN_CENTER, 0, 0);
  /* 创建一个按键,用于进入触摸屏测试功能界面 */
  btn_tp = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_tp, 70, 70);
  lv_obj_align(btn_tp, btn5, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_tp,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_tp,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_tp = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label8, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label_tp, " TP ");
  lv_obj_align(label_tp, btn_tp, LV_ALIGN_CENTER, 0, 0);
  
  /* 创建一个按键,用于进入设置检测界面 */
  btn_inspect = lv_btn_create(cont_test, NULL);
  lv_obj_set_size(btn_inspect, 70, 70);
  lv_obj_align(btn_inspect, btn_gyro, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_inspect,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_inspect,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_inspect = lv_label_create(cont_test, NULL);
  //lv_label_set_style(label8, LV_LABEL_STYLE_MAIN, &lable_style);
  lv_label_set_text(label_inspect, " test ");
  lv_obj_align(label_inspect, btn_inspect, LV_ALIGN_CENTER, 0, 0);
  
  lv_obj_set_event_cb(btn_ear, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_charge, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_lcd, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_gyro, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_flash, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_mic, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_tp, btn_event_handler); // 设置回调事件
  lv_obj_set_event_cb(btn_inspect, btn_event_handler); // 设置回调事件
  
  if(task_ear != NULL)
  {
    lv_task_del(task_ear); //删除耳机插拔检测任务
    task_ear = NULL;
  }
  if(task_charge != NULL)
  {
    lv_task_del(task_charge); //删除USB充电检测任务
    task_charge = NULL;
  }
  if(task_gyro != NULL)
  {
    lv_task_del(task_gyro); //删除陀螺仪检测任务
    task_charge = NULL;
  }
}

/**
 * @brief 按键事件回调，用于进入各个硬件功能测试界面
 *
 */
void btn_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event == LV_EVENT_RELEASED)
  {
    if(obj == btn_ear)
    {
      enter_ear_test_ui();
    }
    else if(obj == btn_charge)
    {
      enter_charge_test_ui();
    }
    else if(obj == btn_lcd)
    {
      enter_lcd_test_ui();
    }
    else if(obj == btn_gyro)
    {
      enter_gyro_test_ui();
    }
    else if(obj == btn_flash)
    {
      enter_flash_test_ui();
    }
    else if(obj == btn_mic)
    {
      enter_mic_test_ui();
    }
    else if(obj == btn_tp)
    {
      enter_tp_test_ui();
    }
    else if(obj == btn_inspect)
    {
      enter_inspect_test_ui();
    }
  }
}

/**
* @brief 耳机功能测试界面
 *
 */
void enter_ear_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_EAR_IDX];
  cont_ear = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_ear, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_ear, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_ear, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
//  static lv_style_t lable_ear_style1;
//  lv_style_copy(&g_font_style, &lv_style_plain_color);
//  g_font_style.text.font = &hht_font_30;
//  g_font_style.text.color = LV_COLOR_BLACK;
  lv_obj_t* label1 = lv_label_create(cont_ear, NULL);
  lv_label_set_style(label1, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(label1, " 耳机测试 ");
  lv_obj_align(label1, cont_ear, LV_ALIGN_IN_TOP_MID, 0, 20);
  
  lv_style_copy(&lable_ear_style2,&lv_style_scr);
  lable_ear_style2.body.main_color = LV_COLOR_MAKE(224,224,224);
  lable_ear_style2.body.grad_color = LV_COLOR_MAKE(224,224,224);
  lable_ear_style2.text.font = &hht_font_30;//在样式中使用这个字体
  lable_ear_style2.text.color = LV_COLOR_BLUE;
  
  /* 检测耳机是否插入 */
  if(1 == HAL_GPIO_ReadPin(HS_DET_GPIO_Port, HS_DET_Pin))
  {
    lv_obj_t* label2 = lv_label_create(cont_ear, NULL);
    lv_label_set_body_draw(label2,true);//使能背景绘制
    lv_label_set_long_mode(label2,LV_LABEL_LONG_CROP);//设置为长文本
    lv_obj_set_size(label2, 200,40);
    lv_label_set_style(label2, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
    lv_label_set_text(label2, " 耳机已插入 ");
    lv_obj_align(label2, cont_ear, LV_ALIGN_CENTER, 0, 0);
  }
  else
  {
    lv_obj_t* label3 = lv_label_create(cont_ear, NULL);
    lv_label_set_body_draw(label3,true);//使能背景绘制
    lv_label_set_long_mode(label3,LV_LABEL_LONG_CROP);//设置为长文本
    lv_obj_set_size(label3, 200,40);
    lv_label_set_style(label3, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
    lv_label_set_text(label3, " 请插入耳机 ");
    lv_obj_align(label3, cont_ear, LV_ALIGN_CENTER, 0, 0);
  }
  
  task_ear =  lv_task_create(test_task_cb,1000,LV_TASK_PRIO_MID,NULL); //创建一个检测耳机插拔任务
}

/**
 * @brief 充电检测功能
 *
 */
void enter_charge_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_CHARGE_IDX];
  cont_charge = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_charge, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_charge, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_charge, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
//  static lv_style_t lable_charge_style;
//  lv_style_copy(&g_font_style, &lv_style_plain_color);
//  g_font_style.text.font = &hht_font_30;
//  g_font_style.text.color = LV_COLOR_BLACK;
  lv_obj_t* label1 = lv_label_create(cont_charge, NULL);
  lv_label_set_style(label1, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(label1, " 充电测试 ");
  lv_obj_align(label1, cont_charge, LV_ALIGN_IN_TOP_MID, 0, 20);
  
  label_charge = lv_label_create(cont_charge, NULL);
  lv_obj_align(label_charge, cont_charge, LV_ALIGN_CENTER, 0, -25);
  lv_obj_set_auto_realign(label_charge, true); //使能自动对齐功能，当文本长度变化时，它会自动对齐
  char buff[10];
  sprintf(buff, "%d%%", MAX17048_Driver_Get_Battery_Electric_Quantity());
  lv_label_set_text(label_charge, buff); //设置文本
  
  lv_style_copy(&lable_ear_style2,&lv_style_scr);
  lable_ear_style2.body.main_color = LV_COLOR_MAKE(224,224,224);
  lable_ear_style2.body.grad_color = LV_COLOR_MAKE(224,224,224);
  lable_ear_style2.text.font = &hht_font_30;//在样式中使用这个字体
  lable_ear_style2.text.color = LV_COLOR_BLUE;
  
  /* 检测设备是否在充电 */
  if(1 == Voltameter_Port_Get_Battery_Charge_State())
  {
    lv_obj_t* label2 = lv_label_create(cont_charge, NULL);
    lv_label_set_body_draw(label2,true);//使能背景绘制
    lv_label_set_long_mode(label2,LV_LABEL_LONG_CROP);//设置为长文本
    lv_obj_set_size(label2, 200,40);
    lv_label_set_style(label2, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
    lv_label_set_text(label2, " 正在充电 ");
    lv_obj_align(label2, label_charge, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
  }
  else
  {
    lv_obj_t* label3 = lv_label_create(cont_charge, NULL);
    lv_label_set_body_draw(label3,true);//使能背景绘制
    lv_label_set_long_mode(label3,LV_LABEL_LONG_CROP);//设置为长文本
    lv_obj_set_size(label3, 200,40);
    lv_label_set_style(label3, LV_LABEL_STYLE_MAIN, &lable_ear_style2);
    lv_label_set_text(label3, " 请充电 ");
    lv_obj_align(label3, label_charge, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
  }
  
  task_charge =  lv_task_create(test_task_cb,1000,LV_TASK_PRIO_MID,NULL); //创建一个检测耳机插拔任务
}
/**
 * @brief LCD功能测试界面
 *
 */
void enter_lcd_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_LCD_IDX];
  cont_lcd = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_lcd, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_cont_set_fit(cont_lcd, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  lv_style_copy(&lcd_bg_style, &lv_style_scr);

  lcd_bg_style.body.main_color = LV_COLOR_BLUE;
  lcd_bg_style.body.grad_color = LV_COLOR_BLUE;
  lv_obj_set_style(cont_lcd, &lcd_bg_style); //设置容器的样式

  btn_lcd_test = lv_btn_create(cont_lcd, NULL);
  lv_obj_set_size(btn_lcd_test, 70, 70);
  lv_obj_align(btn_lcd_test, cont_lcd, LV_ALIGN_CENTER, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_lcd_test,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_lcd_test,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label9 = lv_label_create(cont_lcd, NULL);
  lv_label_set_text(label9, " LCD ");
  lv_obj_align(label9, btn_lcd_test, LV_ALIGN_CENTER, 0, 0);
  
  lv_obj_set_event_cb(btn_lcd_test, lcd_test_event_handler); // 设置回调事件
}
/**
 * @brief 陀螺仪功能测试
 *
 */
void enter_gyro_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_GYRO_IDX];
  cont_gyro = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_gyro, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_gyro, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_gyro, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
//  static lv_style_t lable_gyro_style;
//  lv_style_copy(&g_font_style, &lv_style_plain_color);
//  g_font_style.text.font = &hht_font_30;
//  g_font_style.text.color = LV_COLOR_BLACK;
  lv_obj_t* gyro_label = lv_label_create(cont_gyro, NULL);
  lv_label_set_style(gyro_label, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(gyro_label, " 陀螺仪测试 ");
  lv_obj_align(gyro_label, cont_gyro, LV_ALIGN_IN_TOP_MID, 0, 20);
  
  /* 检测陀螺仪工作状态 */
  if(Gyroscope_Port_Get_Work_State() == true)  
  {
    lv_obj_t* gyro_label1 = lv_label_create(cont_gyro, NULL);
    lv_label_set_style(gyro_label1, LV_LABEL_STYLE_MAIN, &g_font_style);
    lv_label_set_text(gyro_label1, " 工作正常 ");
    lv_obj_align(gyro_label1, gyro_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
  }
  else
  {
    lv_obj_t* gyro_label2 = lv_label_create(cont_gyro, NULL);
    lv_label_set_style(gyro_label2, LV_LABEL_STYLE_MAIN, &g_font_style);
    lv_label_set_text(gyro_label2, " 工作不正常 ");
    lv_obj_align(gyro_label2, gyro_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
  }
  task_gyro =  lv_task_create(test_task_cb,1000,LV_TASK_PRIO_MID,NULL); //创建一个检测耳机插拔任务
}

/**
 * @brief 触摸功能测试
 *
 */
void enter_tp_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_TP_IDX];
  cont_tp = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_tp, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_tp, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_tp, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
//  static lv_style_t lable_tp_style;
//  lv_style_copy(&g_font_style, &lv_style_plain_color);
//  g_font_style.text.font = &hht_font_30;
//  g_font_style.text.color = LV_COLOR_BLACK;
  lv_obj_t* tp_label = lv_label_create(cont_tp, NULL);
  lv_label_set_style(tp_label, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(tp_label, " 触摸测试 ");
  lv_obj_align(tp_label, cont_tp, LV_ALIGN_IN_TOP_MID, 0, 20);
  
  tp_label1 = lv_label_create(cont_tp, NULL);
  lv_label_set_style(tp_label1, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(tp_label1, " 请触摸 ");
  lv_obj_align(tp_label1, NULL, LV_ALIGN_CENTER, 0, 0);
}
/**
 * @brief Flash功能测试
 *
 */
void enter_flash_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_FLASH_IDX];
  cont_flash = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_flash, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_flash, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_flash, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
//  static lv_style_t lable_flash_style;
//  lv_style_copy(&g_font_style, &lv_style_plain_color);
//  g_font_style.text.font = &hht_font_30;
//  g_font_style.text.color = LV_COLOR_BLACK;
  
  if(Flash_Port_Get_State() == true)
  {
    lv_obj_t* flash_label1 = lv_label_create(cont_flash, NULL);
    lv_label_set_style(flash_label1, LV_LABEL_STYLE_MAIN, &g_font_style);
    lv_label_set_text(flash_label1, " 工作正常 ");
    lv_obj_align(flash_label1, cont_flash, LV_ALIGN_CENTER, 0, 0);
  }
  else
  {
    lv_obj_t* flash_label2 = lv_label_create(cont_flash, NULL);
    lv_label_set_style(flash_label2, LV_LABEL_STYLE_MAIN, &g_font_style);
    lv_label_set_text(flash_label2, " 工作不正常 ");
    lv_obj_align(flash_label2, cont_flash, LV_ALIGN_CENTER, 0, 0);
  }
}

/**
 * @brief Mic功能测试界面
 *
 */
void enter_mic_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_MIC_IDX];
  cont_mic = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_mic, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_mic, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_mic, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
//  static lv_style_t lable_mic_style;
//  lv_style_copy(&g_font_style, &lv_style_plain_color);
//  g_font_style.text.font = &hht_font_30;
//  g_font_style.text.color = LV_COLOR_BLACK;
  lv_obj_t* mic_label = lv_label_create(cont_mic, NULL);
  lv_label_set_style(mic_label, LV_LABEL_STYLE_MAIN, &g_font_style);
  lv_label_set_text(mic_label, " 麦克风测试 ");
  lv_obj_align(mic_label, cont_mic, LV_ALIGN_IN_TOP_MID, 0, 30);
  
  Algorithm_Port_Get_BF_Default_MIC(&mic_channel_num);
  
  btn_mic_test = lv_btn_create(cont_mic, NULL);
  lv_obj_set_size(btn_mic_test, 100, 100);
  lv_obj_align(btn_mic_test, cont_mic, LV_ALIGN_CENTER, 0, 10);//设置对齐方式
	lv_btn_set_style(btn_mic_test,LV_BTN_STYLE_REL, &btn_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_mic_test,LV_BTN_STYLE_PR, &btn_press_style);//设置按钮正常态下按下状态样式
  label_mic = lv_label_create(cont_mic, NULL);
  lv_label_set_text(label_mic, " 测试 ");
  lv_obj_align(label_mic, btn_mic_test, LV_ALIGN_CENTER, 0, 0);
  
  lv_obj_set_event_cb(btn_mic_test, mic_test_event_handler); // 设置回调事件
}

/**
 * @brief 送检功能
 *
 */
void enter_inspect_test_ui(void)
{
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[TEST_INSPECT_IDX];
  cont_inspect = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_inspect, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_inspect, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_inspect, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
   
}
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
