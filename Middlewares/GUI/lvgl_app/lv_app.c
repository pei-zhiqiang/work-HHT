/**
 *  @file lv_app.c
 *
 *  @date 2022-04-22
 *
 *  @author PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief   HHT盒子的UI显示
 *
 *  @details 
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "lv_app.h"
#include "lvgl.h"
#include "main.h"
#include "BT8829_Signal_Port.h"
#include "MAX17048_Driver.h"
#include "lv_app_test.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
LV_FONT_DECLARE(hht_font_30);      /**< 申明字体,大小30 */  
LV_FONT_DECLARE(hht_font_50);      /**< 申明字体,大小50*/  
LV_FONT_DECLARE(hht_font_72);      /**< 申明字体,大小72 */
LV_IMG_DECLARE(logo);              /**< 申明LOGO界面图像 */
LV_IMG_DECLARE(main_bf_170);       /**< 主界面BF图片 */
LV_IMG_DECLARE(main_set_90);       /**< 主界面设置图片 */
LV_IMG_DECLARE(main_vol_left_40);  /**< 主界面音量图片 */
LV_IMG_DECLARE(main_vol_right_40); /**< 主界面音量图片 */
LV_IMG_DECLARE(bf_100);            /**< BF图片按钮 */
LV_IMG_DECLARE(ns_100);            /**< 降噪图片按钮 */
LV_IMG_DECLARE(set_100);           /**< 设置图片按钮 */
LV_IMG_DECLARE(vol_100);           /**< 音量图片按钮 */
LV_IMG_DECLARE(ble_90);            /**< 蓝牙图片按钮 */
LV_IMG_DECLARE(brightness_90);     /**< 亮度图片按钮 */
LV_IMG_DECLARE(reset_90);          /**< 恢复图片按钮 */
LV_IMG_DECLARE(mode_90);           /**< 模式图片按钮 */  
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
lv_obj_t* scr = NULL;        /**< 当前活跃屏幕对象 */
lv_obj_t* cont_mode = NULL;
lv_obj_t* main_bar_l = NULL;   /** < 用于显示音量的进度条 */
lv_obj_t* main_bar_r = NULL;   /** < 用于显示音量的进度条 */
int8_t Vol_R = 0;              /** < 用于获取右耳音量 */
int8_t Vol_L = 0;              /** < 用于获取左耳音量 */
UI_Inf_Typedef_t *Current_UI_Inf = NULL; /**< 当前界面信息 */
UI_Inf_Typedef_t UI_Base_Inf[_MAX_IDX];  /**< 记录每个界面的信息 */
/** Private variables --------------------------------------------------------*/
static lv_obj_t* cont_main = NULL;  /**< 主界面容器对象 */
static lv_obj_t* cont_set = NULL;   /**< 设置界面容器 */
static lv_obj_t* cont_vol = NULL;
static lv_obj_t* cont_ns = NULL;
static lv_obj_t* cont_bf = NULL;
static lv_obj_t* cont_more_set = NULL;
static lv_obj_t* cont_brightness = NULL;

static lv_obj_t* cont_reset = NULL;
static lv_obj_t* cont_ble = NULL;

static lv_obj_t* lmeter_soc = NULL;      /**< 刻度指示器 */
static lv_obj_t* label_charge = NULL;    /**< 电量显示标签 */
static lv_obj_t* main_imgbtn_set = NULL; /**< 主界面设置图片按钮 */
static lv_obj_t* main_img_charge = NULL; 
static lv_obj_t* imgbtn_bf_100 = NULL;
static lv_obj_t* imgbtn_ns_100 = NULL;
static lv_obj_t* imgbtn_set_100 = NULL;
static lv_obj_t* imgbtn_vol_100 = NULL;
static lv_obj_t* imgbtn_ble_90 = NULL;
static lv_obj_t* imgbtn_brightness_90 = NULL;
static lv_obj_t* imgbtn_reset_90 = NULL;
static lv_obj_t* imgbtn_mode_90 = NULL;
static lv_obj_t* label_ns = NULL;
static lv_obj_t* bf_open = NULL;
static lv_obj_t* btn_bf[5];
static lv_obj_t* bf_label_rests = NULL;
static lv_obj_t* btn_brightness1 = NULL;
static lv_obj_t* brightness_bar1 = NULL;
static lv_obj_t* brightness_bar2 = NULL;
static lv_obj_t* brightness_bar3 = NULL;
static lv_obj_t* brightness_bar4 = NULL;
static lv_obj_t* brightness_bar5 = NULL;
static lv_obj_t* vol_btn1 = NULL;
static lv_obj_t* vol_btn2 = NULL;
static lv_obj_t* vol_btn3 = NULL;
static lv_obj_t* vol_btn4 = NULL;
static lv_obj_t* btn_reset1 = NULL;
static lv_obj_t* btn_reset2 = NULL;
static lv_obj_t* btn_ble = NULL;
lv_obj_t* btn_mode5 = NULL;  /* 此按键用于进入硬件测试界面 */

static lv_style_t main_lmeter_style;    /**< 刻度器样式 */
static lv_style_t main_bar_bg_style;    /**< 指示器背景样式 */
static lv_style_t main_bar_indic_style; /**< 指示器刻度样式 */
static lv_style_t set_imgbtn_style;     /**< 设置界面，图片按钮被下时的样式 */ 
static lv_style_t label_ns_style;
static lv_style_t btn_bf_release_style_close;
static lv_style_t btn_bf_release_style_open;

static uint8_t curr_battery = 0;     /**< 记录当前电量 */
static uint8_t last_battery = 0;     /**< 上一次电量 */
static uint8_t BF_FUNC_State = 0;    /**< BF算法开关状态 */
static uint16_t BF_Angle = 0;        /**< BF角度 */
static uint8_t Last_Power_State = 0; /**< 上一次充电状态 */
static uint8_t Curr_Power_State = 0; /**< 当前充电状态 */
static uint8_t Vol_Func_State = 0;   /** < 用于获取音量算法开关状态 */
static uint8_t NS_Func_State = 0;    /**< 降噪算法开关状态 */
static uint8_t NS_Level = 0;         /**< 降噪等级 */
/** Private function prototypes ----------------------------------------------*/
static void lv_app_start(void);
static void hht_logo_ui(void);                                   /**< LOGO界面 */
static void hht_main_ui(void);                                   /**< 主界面 */
static void battery_ble_updata_task_cb(lv_task_t* task);         /**< 电量、充电状态、蓝牙状态更新 */
static void main_set_event_cb(lv_obj_t* obj, lv_event_t event);  /**< 主界面设置事件 */
static void hht_set_ui(void);                                    /**< 设置界面 */
static void enter_vol_ui(void);                                  /**< 音量设置界面 */
static void enter_bf_ui(void);                                   /**< BF设置界面 */
static void enter_ns_ui(void);                                   /**< 降噪设置界面 */
static void enter_more_set_ui(void);                             /**< 更多设置界面 */
static void enter_brightness_ui(void);                           /**< 亮度设置界面 */
static void enter_mode_ui(void);                                 /**< 模式选择界面 */
static void enter_reset_ui(void);                                /**< 重置界面 */
static void enter_ble_ui(void);                                  /**< 蓝牙设置界面 */
static void imgbtn_event_cb(lv_obj_t* obj, lv_event_t event);    /**< 设置界面图片按钮事件 */
static void vol_event_handler(lv_obj_t* obj, lv_event_t event);  /**< 音量设置事件 */
static void bf_event_handler(lv_obj_t* obj, lv_event_t event);   /**< BF设置事件 */
static void ns_event_handler(lv_obj_t* obj, lv_event_t event);   /**< 降噪设置事件 */
static void brightness_event_handler(lv_obj_t* obj, lv_event_t event); /**< 亮度调节事件 */
static void secondary_imgbtn_event_cb(lv_obj_t* obj, lv_event_t event);
static void reset_event_handler(lv_obj_t* obj, lv_event_t event); /**< 重置事件 */
static void ble_event_handler(lv_obj_t* obj, lv_event_t event);   /**< 蓝牙设置事件 */
static void mode_event_handler(lv_obj_t* obj, lv_event_t event);  /**< 模式设置事件 */
/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
static void lv_app_start(void)
{
  /* 获取当前活跃屏幕对象 */
  scr = lv_scr_act();
}
/**
 * @brief 用于初始化界面信息
 *
 */
static void Init_UI_Inf(UI_IDX_Typedef_t IDX, uint8_t level, ui_func func, UI_IDX_Typedef_t Prior_IDX)
{
  UI_Base_Inf[IDX].ui_idx = IDX;
  UI_Base_Inf[IDX].ui_level = level;
  UI_Base_Inf[IDX].ui_prior_level = Prior_IDX;
  UI_Base_Inf[IDX].current_ui_func = func;
}
/**
 * @brief logo界面
 *
 */
static void hht_logo_ui(void)
{
  Current_UI_Inf = &UI_Base_Inf[LOGO_IDX];
  lv_obj_t* logo_img = lv_img_create(scr, NULL);
  lv_img_set_src(logo_img, &logo);
  lv_obj_align(logo_img, scr, LV_ALIGN_CENTER, 0, 0);
}
/**
 * @brief 主界面
 *
 */
static void hht_main_ui(void)
{
  Current_UI_Inf = &UI_Base_Inf[MAIN_IDX];
  if(cont_set != NULL)
  {
    lv_obj_del(cont_set);
    cont_set = NULL;
  }
  cont_main = lv_cont_create(scr, NULL);
  lv_obj_align(cont_main, NULL, LV_ALIGN_CENTER, 0, 0); //设置容器居中对齐
  lv_obj_set_style(cont_main, &lv_style_scr);  //设置容器的样式
  lv_cont_set_fit(cont_main, LV_FIT_FLOOD); //设置当前容器平铺整个屏幕
  
  /***************************** 1、Voltameter 刻度指示器 ***************************/
  /* 1.1创建一个刻度指示器样式 */
  lv_style_copy(&main_lmeter_style, &lv_style_plain_color);
  main_lmeter_style.body.main_color = LV_COLOR_RED; //活跃刻度线的起始颜色
  main_lmeter_style.body.grad_color = LV_COLOR_GREEN; //活跃刻度线的终止颜色
  main_lmeter_style.line.color = LV_COLOR_SILVER; //非活跃刻度线的颜色
  main_lmeter_style.line.width = 2; //每一条刻度线的宽度
  main_lmeter_style.body.padding.left = 16; //每一条刻度线的长度
  
  /* 1.2创建一个刻度指示器 */
  lmeter_soc = lv_lmeter_create(cont_main, NULL);
  lv_obj_set_size(lmeter_soc, 180, 180); //设置刻度指示器大小
  lv_obj_align(lmeter_soc, NULL, LV_ALIGN_IN_TOP_MID, 0, 5); //与屏幕顶部居中对齐
  lv_lmeter_set_range(lmeter_soc, 0, 100); //设置进度范围
  lv_lmeter_set_value(lmeter_soc, MAX17048_Driver_Get_Battery_Electric_Quantity()); //设置当前进度的值
  lv_lmeter_set_scale(lmeter_soc, 150, 31); //设置角度和刻度线的数量
  lv_lmeter_set_style(lmeter_soc, LV_LMETER_STYLE_MAIN, &main_lmeter_style); //设置样式
  
  /* 1.3创建一个label标签来显示电量的百分比 */
  label_charge = lv_label_create(cont_main, NULL);
  lv_obj_align(label_charge, lmeter_soc, LV_ALIGN_CENTER, 0, -25); //与刻度指示器居中对齐
  lv_obj_set_auto_realign(label_charge, true); //使能自动对齐功能，当文本长度变化时，它会自动对齐
  char buff[10];
  sprintf(buff, "%d%%", MAX17048_Driver_Get_Battery_Electric_Quantity());
  lv_label_set_text(label_charge, buff); //设置文本
  lv_task_create(battery_ble_updata_task_cb,1000, LV_TASK_PRIO_MID, NULL);//创建一个任务
  last_battery = curr_battery = MAX17048_Driver_Get_Battery_Electric_Quantity(); //获取电量值
  
  /************************************* 2、BF 指向 ***************************************/
  /* 2.1创建一个BF图片对象 */
  lv_obj_t* main_img_bf = lv_img_create(cont_main, NULL);
  lv_img_set_src(main_img_bf, &main_bf_170);
  lv_obj_align(main_img_bf, cont_main, LV_ALIGN_CENTER, 0, 0);
  /* 获取BF算法开关状态 */
  Algorithm_Port_Get_Function_State(BF_FUNC_SEL, &BF_FUNC_State); 
  if(1 == BF_FUNC_State)
  {
    Algorithm_Port_Get_BF_Angle(&BF_Angle); //获取BF角度
    BF_Angle /= 90;
    lv_obj_t *bf_led = lv_led_create(cont_main, NULL);
    lv_obj_set_size(bf_led,30,30);//设置大小
    static lv_style_t main_bf_led_style;
    lv_style_copy(&main_bf_led_style, &lv_style_pretty_color);//样式拷贝
    main_bf_led_style.body.radius = LV_RADIUS_CIRCLE;//绘制半圆角
    main_bf_led_style.body.main_color = LV_COLOR_WHITE;
    main_bf_led_style.body.grad_color = LV_COLOR_WHITE;
    main_bf_led_style.body.border.color = LV_COLOR_WHITE;//边框颜色
    main_bf_led_style.body.border.width = 3;//边框的宽度
    main_bf_led_style.body.border.opa = LV_OPA_70;//透明度
    main_bf_led_style.body.shadow.color = LV_COLOR_YELLOW;//阴影的颜色
    main_bf_led_style.body.shadow.width = 0;//阴影的大小
    if(BF_Angle == 0)
    {
      lv_obj_align(bf_led,cont_main,LV_ALIGN_CENTER,-45,0);  // 左 0°
    }
    else if(BF_Angle == 1)
    {    
      lv_obj_align(bf_led,cont_main,LV_ALIGN_CENTER,0,-45); // 上 90°
    }
    else if(BF_Angle == 2)
    {
      lv_obj_align(bf_led,cont_main,LV_ALIGN_CENTER,45,0);  // 右 180°
    }
    else
    {
      lv_obj_align(bf_led,cont_main,LV_ALIGN_CENTER,0,45); // 下 270°
    }
    lv_obj_set_style(bf_led, &main_bf_led_style);//设置样式
    lv_led_set_bright(bf_led, 255);
    lv_obj_move_foreground(bf_led);
  }
  
  /*********************************** 3、SET 设置图标 *********************************/
  /* 3.1创建按下时的样式 */
  static lv_style_t main_set_imgbtn_style; 
  lv_style_copy(&main_set_imgbtn_style, &lv_style_plain); 
  main_set_imgbtn_style.image.color = LV_COLOR_BLACK; //图片重绘色时的混合色为黑色,这样看上去有按下的效果
  main_set_imgbtn_style.image.intense = LV_OPA_50; // 混合强度
  main_set_imgbtn_style.text.color = LV_COLOR_MAKE(0xAA, 0xAA, 0xAA); //按下时的文本色
  
  /* 3.2创建set图片按钮 */
  main_imgbtn_set = lv_imgbtn_create(cont_main, NULL);
  lv_imgbtn_set_src(main_imgbtn_set, LV_BTN_STATE_PR, &main_set_90); //设置正常态松手时的图片
  lv_imgbtn_set_src(main_imgbtn_set, LV_BTN_STATE_REL, &main_set_90);//设置正常态按下时的图片
  lv_imgbtn_set_style(main_imgbtn_set, LV_BTN_STATE_PR, &main_set_imgbtn_style); //设置正常按下时的样式
  lv_obj_align(main_imgbtn_set, cont_main, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_set_event_cb(main_imgbtn_set, main_set_event_cb); // 设置事件
  
  /************************************ 4、VOL 音量 ****************************************/
  /* 获取当前音量等级 */
  Algorithm_Port_Get_Function_State(VOL_FUNC_SEL, &Vol_Func_State);
  if(Vol_Func_State == 1)
  { 
     Algorithm_Port_Get_VC_Level(&Vol_L, ALGORITHM_CHANNEL_CH1);
	   Algorithm_Port_Get_VC_Level(&Vol_R, ALGORITHM_CHANNEL_CH2);
  }
  /* 4.1创建vol图片 */
  lv_obj_t* main_img_vol_l = lv_img_create(cont_main, NULL);
  lv_img_set_src(main_img_vol_l, &main_vol_left_40);
  lv_obj_align(main_img_vol_l,main_img_bf,LV_ALIGN_OUT_LEFT_BOTTOM,0,40);
  lv_obj_t* main_img_vol_r = lv_img_create(cont_main, NULL);
  lv_img_set_src(main_img_vol_r, &main_vol_right_40);
  lv_obj_align(main_img_vol_r,main_img_bf,LV_ALIGN_OUT_RIGHT_BOTTOM,0,40);
  
  /* 4.2.1创建进度条背景样式 */
  lv_style_copy(&main_bar_bg_style,&lv_style_plain_color);
  main_bar_bg_style.body.main_color = LV_COLOR_MAKE(0xBB,0xBB,0xBB);
  main_bar_bg_style.body.grad_color = LV_COLOR_MAKE(0xBB,0xBB,0xBB);
  main_bar_bg_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角
  /* 4.2.2创建指示器样式 */
  lv_style_copy(&main_bar_indic_style,&lv_style_plain_color);
  main_bar_indic_style.body.main_color = LV_COLOR_MAKE(0x5F,0xB8,0x78);
  main_bar_indic_style.body.grad_color = LV_COLOR_MAKE(0x5F,0xB8,0x78);
  main_bar_indic_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角
  main_bar_indic_style.body.padding.left = 0;//让指示器跟背景边框之间没有距离
  main_bar_indic_style.body.padding.top = 0;
  main_bar_indic_style.body.padding.right = 0;
  main_bar_indic_style.body.padding.bottom = 0;
  /* 4.2.3创建垂直进度条 */
  main_bar_l = lv_bar_create(cont_main, NULL);
  lv_obj_set_size(main_bar_l,24,140);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(main_bar_l,LV_BAR_STYLE_BG,&main_bar_bg_style);//
  lv_bar_set_style(main_bar_l,LV_BAR_STYLE_INDIC,&main_bar_indic_style);
  lv_obj_align(main_bar_l,main_img_vol_l,LV_ALIGN_OUT_TOP_MID,-15,-10);
  lv_bar_set_range(main_bar_l,0,7);//设置进度范围
  lv_bar_set_value(main_bar_l,Vol_L,LV_ANIM_ON);//显示当前音量值

  main_bar_r = lv_bar_create(cont_main, NULL);
  lv_obj_set_size(main_bar_r,24,140);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(main_bar_r,LV_BAR_STYLE_BG,&main_bar_bg_style);//
  lv_bar_set_style(main_bar_r,LV_BAR_STYLE_INDIC,&main_bar_indic_style);
  lv_obj_align(main_bar_r,main_img_vol_r,LV_ALIGN_OUT_TOP_MID,15,-10);
  lv_bar_set_range(main_bar_r,0,7);//设置进度范围
  lv_bar_set_value(main_bar_r, Vol_R,LV_ANIM_ON);//显示当前音量值
  
  /* 检测设备是否在充电 */
  if(1 == Voltameter_Port_Get_Battery_Charge_State())
  {
    Last_Power_State = 1;
    main_img_charge = lv_img_create(cont_main,NULL);
    lv_img_set_src(main_img_charge, LV_SYMBOL_POWER);
    lv_obj_align(main_img_charge,lmeter_soc,LV_ALIGN_CENTER,0,-55);
  }
}
/**
 * @brief 设置界面
 *
 */
static void hht_set_ui(void)
{ 
  if(cont_main != NULL)
  {
    lv_obj_del(cont_main);
    cont_main = NULL;
  }
  if(cont_bf != NULL)
  {
    lv_obj_del(cont_bf);
    cont_bf = NULL;
  }
  if(cont_ns != NULL)
  {
    lv_obj_del(cont_ns);
    cont_ns = NULL;
  }
  if(cont_vol != NULL)
  {
    lv_obj_del(cont_vol);
    cont_vol = NULL;
  }
  if(cont_more_set != NULL)
  {
    lv_obj_del(cont_more_set);
    cont_more_set = NULL;
  }
  
  Current_UI_Inf = &UI_Base_Inf[SET_IDX];
  cont_set = lv_cont_create(scr, NULL);
  lv_obj_align(cont_set, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_set, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_set, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  /* 设置图片按钮被按下时的样式 */
  static lv_style_t set_imgbtn_style;
  lv_style_copy(&set_imgbtn_style, &lv_style_plain);
  set_imgbtn_style.image.color = LV_COLOR_BLACK;
  set_imgbtn_style.image.intense = LV_OPA_50;
  set_imgbtn_style.text.color = LV_COLOR_MAKE(0xAA,0xAA,0xAA);
  /* 创建图片按键 */
  imgbtn_bf_100 = lv_imgbtn_create(cont_set,NULL);
  imgbtn_ns_100 = lv_imgbtn_create(cont_set,NULL);
  imgbtn_vol_100 = lv_imgbtn_create(cont_set,NULL);
  imgbtn_set_100 = lv_imgbtn_create(cont_set,NULL);
  /* 设置按下和释放的样子 */
  lv_imgbtn_set_src(imgbtn_bf_100,LV_BTN_STATE_PR,&bf_100);
  lv_imgbtn_set_src(imgbtn_ns_100,LV_BTN_STATE_PR,&ns_100);
  lv_imgbtn_set_src(imgbtn_vol_100,LV_BTN_STATE_PR,&vol_100);
  lv_imgbtn_set_src(imgbtn_set_100,LV_BTN_STATE_PR,&set_100);
  lv_imgbtn_set_src(imgbtn_bf_100,LV_BTN_STATE_REL,&bf_100);
  lv_imgbtn_set_src(imgbtn_ns_100,LV_BTN_STATE_REL,&ns_100);
  lv_imgbtn_set_src(imgbtn_vol_100,LV_BTN_STATE_REL,&vol_100);
  lv_imgbtn_set_src(imgbtn_set_100,LV_BTN_STATE_REL,&set_100);
  /* 设置按下时的样式 */
  lv_imgbtn_set_style(imgbtn_bf_100,LV_BTN_STATE_PR,&set_imgbtn_style);
  lv_imgbtn_set_style(imgbtn_ns_100,LV_BTN_STATE_PR,&set_imgbtn_style);
  lv_imgbtn_set_style(imgbtn_vol_100,LV_BTN_STATE_PR,&set_imgbtn_style);
  lv_imgbtn_set_style(imgbtn_set_100,LV_BTN_STATE_PR,&set_imgbtn_style);
  /* 设置对齐方式 */
  lv_obj_align(imgbtn_bf_100, cont_set, LV_ALIGN_IN_TOP_RIGHT, -60, 60);
  lv_obj_align(imgbtn_ns_100, cont_set, LV_ALIGN_IN_BOTTOM_LEFT, 60, -60);
  lv_obj_align(imgbtn_vol_100, cont_set, LV_ALIGN_IN_TOP_LEFT, 60, 60);
  lv_obj_align(imgbtn_set_100, cont_set, LV_ALIGN_IN_BOTTOM_RIGHT, -60, -60);
  /* 设置回调事件 */
  lv_obj_set_event_cb(imgbtn_bf_100,imgbtn_event_cb);
  lv_obj_set_event_cb(imgbtn_ns_100,imgbtn_event_cb);
  lv_obj_set_event_cb(imgbtn_vol_100,imgbtn_event_cb);
  lv_obj_set_event_cb(imgbtn_set_100,imgbtn_event_cb);
  
  /* 设置字体样式 */
  static lv_style_t style1;
  lv_style_copy(&style1,&lv_style_scr);
  style1.text.font = &hht_font_30;//在样式中使用这个字体
  style1.text.color = LV_COLOR_BLACK;
  /* 创建一个标签，显示"设置" */
  lv_obj_t* label = lv_label_create(cont_set, NULL);
  lv_label_set_style(label,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label," 设置 ");//设置文本
  lv_obj_align(label,cont_set,LV_ALIGN_IN_TOP_MID,0,20);
  /* 创建一个标签，显示"音量" */
  lv_obj_t* label_vol = lv_label_create(cont_set, NULL);
  lv_label_set_style(label_vol,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_vol," 音量 ");//设置文本
  lv_obj_align(label_vol,imgbtn_vol_100,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  /* 创建一个标签，显示"指向" */
  lv_obj_t* label_bf = lv_label_create(cont_set, NULL);
  lv_label_set_style(label_bf,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_bf," 指向 ");//设置文本
  lv_obj_align(label_bf,imgbtn_bf_100,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  /* 创建一个标签, 显示"降噪" */
  lv_obj_t* label_ns = lv_label_create(cont_set, NULL);
  lv_label_set_style(label_ns,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_ns," 降噪 ");//设置文本
  lv_obj_align(label_ns,imgbtn_ns_100,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  /* 创建一个标签，显示"更多" */
  lv_obj_t* label_more = lv_label_create(cont_set, NULL);
  lv_label_set_style(label_more,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_more," 更多 ");//设置文本
  lv_obj_align(label_more,imgbtn_set_100,LV_ALIGN_OUT_BOTTOM_MID,0,0);
}

/**
 * @brief 音量设置界面
 *
 */
static void enter_vol_ui(void)
{
  if(cont_set != NULL)
  {
    lv_obj_del(cont_set);
    cont_set = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[VOL_IDX];
  cont_vol = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_vol, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_vol, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_vol, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  static lv_style_t btn_vol_release_style;
  static lv_style_t btn_vol_press_style;
  /* 按键释放状态下的样式 */
  lv_style_copy(&btn_vol_release_style, &lv_style_plain_color);
  btn_vol_release_style.body.main_color = LV_COLOR_MAKE(255,133,10);
  btn_vol_release_style.body.grad_color = btn_vol_release_style.body.main_color;
  btn_vol_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_vol_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按键按下状态下的样式 */
  lv_style_copy(&btn_vol_press_style,&lv_style_plain_color);
  btn_vol_press_style.body.opa = LV_OPA_0;//设置背景色透明
  btn_vol_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_vol_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
	btn_vol_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制
  
  /* 字体样式 大小30*/
  static lv_style_t vol_label_style;
  lv_style_copy(&vol_label_style,&lv_style_scr);
  vol_label_style.text.font = &hht_font_30;//在样式中使用这个字体
  vol_label_style.text.color = LV_COLOR_BLACK;
  /* 字体样式 大小50*/
  static lv_style_t vol_label_style2;
  lv_style_copy(&vol_label_style2,&lv_style_scr);
  vol_label_style2.text.font = &hht_font_50;//在样式中使用这个字体
  vol_label_style2.text.color = LV_COLOR_BLACK;
  /* 创建一个标签，显示"音量" */
  lv_obj_t* vol_label = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label,LV_LABEL_STYLE_MAIN,&vol_label_style2);
  lv_label_set_text(vol_label," 音量 ");//设置文本
  lv_obj_align(vol_label,cont_vol,LV_ALIGN_IN_TOP_MID,0,20);
  /* 创建一个标签，显示"左" */
  lv_obj_t* vol_label1 = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label1,LV_LABEL_STYLE_MAIN,&vol_label_style);
  lv_label_set_text(vol_label1," 左 ");//设置文本
  lv_obj_align(vol_label1,cont_vol,LV_ALIGN_IN_LEFT_MID,70,0);
  /* 创建一个标签，显示"右" */
  lv_obj_t* vol_label2 = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label2,LV_LABEL_STYLE_MAIN,&vol_label_style);
  lv_label_set_text(vol_label2," 右 ");//设置文本
  lv_obj_align(vol_label2,cont_vol,LV_ALIGN_IN_RIGHT_MID,-70,0);
  
  /* 字体样式 */
  static lv_style_t vol_label_style1;
  lv_style_copy(&vol_label_style1,&lv_style_scr);
  vol_label_style1.text.color = LV_COLOR_WHITE;
  /* 创建一个按钮，音量"+" */
  vol_btn1 = lv_btn_create(cont_vol, NULL);
  lv_obj_set_size(vol_btn1,60,60);//设置大小
	lv_obj_align(vol_btn1, cont_vol, LV_ALIGN_IN_TOP_LEFT, 60, 80);//设置对齐方式
	lv_btn_set_style(vol_btn1,LV_BTN_STYLE_REL, &btn_vol_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(vol_btn1,LV_BTN_STYLE_PR, &btn_vol_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* vol_label3 = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label3,LV_LABEL_STYLE_MAIN,&vol_label_style1);
  lv_label_set_text(vol_label3, LV_SYMBOL_PLUS);
  lv_obj_align(vol_label3, vol_btn1, LV_ALIGN_CENTER, 0, 0);
  
  vol_btn2 = lv_btn_create(cont_vol, NULL);
  lv_obj_set_size(vol_btn2,60,60);//设置大小
	lv_obj_align(vol_btn2, cont_vol, LV_ALIGN_IN_BOTTOM_LEFT, 60, -80);//设置对齐方式
	lv_btn_set_style(vol_btn2,LV_BTN_STYLE_REL, &btn_vol_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(vol_btn2,LV_BTN_STYLE_PR, &btn_vol_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* vol_label4 = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label4,LV_LABEL_STYLE_MAIN,&vol_label_style1);
  lv_label_set_text(vol_label4, LV_SYMBOL_MINUS);
  lv_obj_align(vol_label4, vol_btn2, LV_ALIGN_CENTER, 0, 0);
  
  vol_btn3 = lv_btn_create(cont_vol, NULL);
  lv_obj_set_size(vol_btn3,60,60);//设置大小
	lv_obj_align(vol_btn3, cont_vol, LV_ALIGN_IN_TOP_RIGHT, -60, 80);//设置对齐方式
	lv_btn_set_style(vol_btn3,LV_BTN_STYLE_REL, &btn_vol_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(vol_btn3,LV_BTN_STYLE_PR, &btn_vol_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* vol_label5 = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label5,LV_LABEL_STYLE_MAIN,&vol_label_style1);
  lv_label_set_text(vol_label5, LV_SYMBOL_PLUS);
  lv_obj_align(vol_label5, vol_btn3, LV_ALIGN_CENTER, 0, 0);
  
  vol_btn4 = lv_btn_create(cont_vol, NULL);
  lv_obj_set_size(vol_btn4,60,60);//设置大小
  lv_obj_align(vol_btn4, cont_vol, LV_ALIGN_IN_BOTTOM_RIGHT, -60, -80);//设置对齐方式
  lv_btn_set_style(vol_btn4,LV_BTN_STYLE_REL, &btn_vol_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(vol_btn4,LV_BTN_STYLE_PR, &btn_vol_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* vol_label6 = lv_label_create(cont_vol, NULL);
  lv_label_set_style(vol_label6,LV_LABEL_STYLE_MAIN,&vol_label_style1);
  lv_label_set_text(vol_label6, LV_SYMBOL_MINUS);
  lv_obj_align(vol_label6, vol_btn4, LV_ALIGN_CENTER, 0, 0);
  
  /* 4.2.3创建垂直进度条 */
  main_bar_l = lv_bar_create(cont_vol, NULL);
  lv_obj_set_size(main_bar_l,24,140);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(main_bar_l,LV_BAR_STYLE_BG,&main_bar_bg_style);//
  lv_bar_set_style(main_bar_l,LV_BAR_STYLE_INDIC,&main_bar_indic_style);
  lv_obj_align(main_bar_l, cont_vol,LV_ALIGN_CENTER,-20,0);
  lv_bar_set_range(main_bar_l,0,7);//设置进度范围
  lv_bar_set_value(main_bar_l,Vol_L,LV_ANIM_ON);//显示当前音量值

  main_bar_r = lv_bar_create(cont_vol, NULL);
  lv_obj_set_size(main_bar_r,24,140);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(main_bar_r,LV_BAR_STYLE_BG,&main_bar_bg_style);//
  lv_bar_set_style(main_bar_r,LV_BAR_STYLE_INDIC,&main_bar_indic_style);
  lv_obj_align(main_bar_r,cont_vol,LV_ALIGN_CENTER,20,0);
  lv_bar_set_range(main_bar_r,0,7);//设置进度范围
  lv_bar_set_value(main_bar_r, Vol_R,LV_ANIM_ON);//显示当前音量值
  
  /* 设置回调事件 */
  lv_obj_set_event_cb(vol_btn1, vol_event_handler);
  lv_obj_set_event_cb(vol_btn2, vol_event_handler);
  lv_obj_set_event_cb(vol_btn3, vol_event_handler);
  lv_obj_set_event_cb(vol_btn4, vol_event_handler);
}

/**
 * @brief BF设置界面
 *
 */
static void enter_bf_ui(void)
{
  if(cont_set != NULL)
  {
    lv_obj_del(cont_set);
    cont_set = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[BF_IDX];
  cont_bf = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_bf, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_bf, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_bf, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  static lv_style_t btn_bf_press_style;
  /* 释放状态下的样式 */
  lv_style_copy(&btn_bf_release_style_close, &lv_style_plain_color);
  btn_bf_release_style_close.body.main_color = LV_COLOR_MAKE(0x80, 0x80, 0x80);//灰色
  btn_bf_release_style_close.body.grad_color = btn_bf_release_style_close.body.main_color;
  btn_bf_release_style_close.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_bf_release_style_close.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  
  lv_style_copy(&btn_bf_release_style_open, &lv_style_plain_color);
  btn_bf_release_style_open.body.main_color = LV_COLOR_MAKE(0, 184, 245);//蓝色
  btn_bf_release_style_open.body.grad_color = btn_bf_release_style_open.body.main_color;
  btn_bf_release_style_open.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_bf_release_style_open.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按下状态下的样式 */
  lv_style_copy(&btn_bf_press_style,&lv_style_plain_color);
  btn_bf_press_style.body.main_color = LV_COLOR_MAKE(71,163,255);;
  btn_bf_press_style.body.grad_color = btn_bf_press_style.body.main_color;
  btn_bf_press_style.body.opa = LV_OPA_COVER;//设置背景色透明
	btn_bf_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  
  btn_bf[1] = lv_btn_create(cont_bf,NULL);
  lv_obj_set_size(btn_bf[1],80,80);//设置大小
  lv_obj_align(btn_bf[1], cont_bf, LV_ALIGN_IN_TOP_MID, 0, 55);//设置对齐方式
  lv_btn_set_style(btn_bf[1],LV_BTN_STYLE_REL,&btn_bf_release_style_close);
  lv_btn_set_style(btn_bf[1],LV_BTN_STYLE_PR,&btn_bf_press_style); 
  
  btn_bf[0] = lv_btn_create(cont_bf,NULL);
  lv_obj_set_size(btn_bf[0],80,80);//设置大小
  lv_obj_align(btn_bf[0], cont_bf, LV_ALIGN_IN_LEFT_MID, 50, 0);//设置对齐方式
  lv_btn_set_style(btn_bf[0],LV_BTN_STYLE_REL,&btn_bf_release_style_close);
  lv_btn_set_style(btn_bf[0],LV_BTN_STYLE_PR,&btn_bf_press_style);   
  btn_bf[2] = lv_btn_create(cont_bf,NULL);
  lv_obj_set_size(btn_bf[2],80,80);//设置大小
  lv_obj_align(btn_bf[2], cont_bf, LV_ALIGN_IN_RIGHT_MID, -50, 0);//设置对齐方式
  lv_btn_set_style(btn_bf[2],LV_BTN_STYLE_REL,&btn_bf_release_style_close);
  lv_btn_set_style(btn_bf[2],LV_BTN_STYLE_PR,&btn_bf_press_style);   
  btn_bf[3] = lv_btn_create(cont_bf,NULL);
  lv_obj_set_size(btn_bf[3],80,80);//设置大小
  lv_obj_align(btn_bf[3], cont_bf, LV_ALIGN_IN_BOTTOM_MID, 0, -55);//设置对齐方式
  lv_btn_set_style(btn_bf[3],LV_BTN_STYLE_REL,&btn_bf_release_style_close);
  lv_btn_set_style(btn_bf[3],LV_BTN_STYLE_PR,&btn_bf_press_style);
  
  btn_bf[4] = lv_btn_create(cont_bf,NULL);
  lv_obj_set_size(btn_bf[4],80,80);//设置大小
  lv_obj_align(btn_bf[4], cont_bf, LV_ALIGN_CENTER, 0, 0);//设置对齐方式
  lv_btn_set_style(btn_bf[4],LV_BTN_STYLE_REL,&btn_bf_release_style_close);
  lv_btn_set_style(btn_bf[4],LV_BTN_STYLE_PR,&btn_bf_press_style);

  lv_obj_set_event_cb(btn_bf[0],bf_event_handler);//设置事件回调函数
  lv_obj_set_event_cb(btn_bf[1],bf_event_handler);//设置事件回调函数
  lv_obj_set_event_cb(btn_bf[2],bf_event_handler);//设置事件回调函数
  lv_obj_set_event_cb(btn_bf[3],bf_event_handler);//设置事件回调函数
  lv_obj_set_event_cb(btn_bf[4],bf_event_handler);//设置事件回调函数
  
  static lv_style_t label_bf_style;
  lv_style_copy(&label_bf_style,&lv_style_scr);
  label_bf_style.text.font = &hht_font_30;//在样式中使用这个字体
  label_bf_style.text.color = LV_COLOR_BLACK;
  
  static lv_style_t label_bf_style1;
  lv_style_copy(&label_bf_style1,&lv_style_scr);
  label_bf_style1.text.font = &hht_font_50;//在样式中使用这个字体
  label_bf_style1.text.color = LV_COLOR_BLACK;
  
  lv_obj_t* bf_label = lv_label_create(cont_bf,NULL);
  lv_label_set_style(bf_label,LV_LABEL_STYLE_MAIN,&label_bf_style1);
  lv_label_set_text(bf_label," 指向 ");//设置文本
  lv_obj_align(bf_label,cont_bf,LV_ALIGN_IN_TOP_MID,0,10);
  /* 创建一个标签，显示"前、后、左、右" */
  bf_label_rests = lv_label_create(cont_bf,NULL);
  lv_label_set_style(bf_label_rests,LV_LABEL_STYLE_MAIN,&label_bf_style);
  lv_obj_align(bf_label_rests,cont_bf,LV_ALIGN_IN_BOTTOM_MID,-10,-15);
 
  Algorithm_Port_Get_Function_State(BF_FUNC_SEL, &BF_FUNC_State); //获取BF算法开关状态
  if(BF_FUNC_State == 1)
  {
    Algorithm_Port_Get_BF_Angle(&BF_Angle); //获取BF角度 
    BF_Angle /= 90;   
    lv_img_set_style(btn_bf[BF_Angle], LV_BTN_STYLE_REL, &btn_bf_release_style_open);
    bf_open = btn_bf[BF_Angle];
    if(bf_open == btn_bf[0])
    {
      lv_label_set_text(bf_label_rests," 左 ");//设置文本
    }
    else if(bf_open == btn_bf[1])
    {
      lv_label_set_text(bf_label_rests," 前 ");//设置文本
    }
    else if(bf_open == btn_bf[2])
    {
      lv_label_set_text(bf_label_rests," 右 ");//设置文本
    }
    else if(bf_open == btn_bf[3])
    {
      lv_label_set_text(bf_label_rests," 后 ");//设置文本
    }
  }
  else
  {
    lv_label_set_text(bf_label_rests," 关 ");//设置文本
  }
}

/**
 * @brief 降噪设置界面
 *
 */
static void enter_ns_ui(void)
{
  if(cont_set != NULL)
  {
    lv_obj_del(cont_set);
    cont_set = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[NS_IDX];
  cont_ns = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_ns, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_ns, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_ns, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  static lv_style_t btn_ns_release_style;
  static lv_style_t btn_ns_press_style;
  /* 释放状态下的样式 */
  lv_style_copy(&btn_ns_release_style, &lv_style_plain_color);
  btn_ns_release_style.body.main_color = LV_COLOR_MAKE(147,221,255);
  btn_ns_release_style.body.grad_color = btn_ns_release_style.body.main_color;
  btn_ns_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_ns_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_ns_release_style.body.shadow.color = LV_COLOR_MAKE(0x1E,0x9F,0xFF);
  btn_ns_release_style.body.shadow.type = LV_SHADOW_FULL;//设置四边都有阴影
  btn_ns_release_style.body.shadow.width = 3;//设置阴影的宽度
  btn_ns_release_style.text.color = LV_COLOR_WHITE;
  btn_ns_release_style.body.padding.left = 10;//设置左内边距
  btn_ns_release_style.body.padding.right = 10;//设置右内边距
  /* 按下状态下的样式 */
  lv_style_copy(&btn_ns_press_style,&lv_style_plain_color);
  btn_ns_press_style.body.opa = LV_OPA_0;//设置背景色透明
  btn_ns_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_ns_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
  btn_ns_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制
  btn_ns_press_style.body.border.width = 2;//设置边框的宽度
  btn_ns_press_style.body.border.opa = LV_OPA_COVER;//设置边框完全不透明
  btn_ns_press_style.text.color = LV_COLOR_BLACK;
  btn_ns_press_style.body.padding.left = 10;//设置左内边距
  btn_ns_press_style.body.padding.right = 10;//设置右内边距
  
  lv_obj_t* btn_ns = lv_btn_create(cont_ns, NULL);
  lv_obj_set_size(btn_ns,150,150);//设置大小
  lv_obj_align(btn_ns, cont_ns, LV_ALIGN_CENTER, 0, 0);//设置对齐方式
  
  lv_btn_set_style(btn_ns,LV_BTN_STYLE_REL, &btn_ns_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_ns,LV_BTN_STYLE_PR, &btn_ns_press_style);//设置按钮正常态下按下状态样式
  
  label_ns = lv_label_create(cont_ns, NULL);
  lv_style_copy(&label_ns_style, &lv_style_plain_color);
  label_ns_style.text.font = &hht_font_72; //在样式中使用这个字体
  lv_label_set_style(label_ns, LV_LABEL_STYLE_MAIN, &label_ns_style);
  lv_obj_set_event_cb(btn_ns, ns_event_handler); //设置事件回调函数
  lv_obj_align(label_ns, cont_ns, LV_ALIGN_CENTER, -35, 0);
  
  static lv_style_t label_ns_style1;
  lv_style_copy(&label_ns_style1, &lv_style_plain_color);
  label_ns_style1.text.font = &hht_font_50;
  label_ns_style1.text.color = LV_COLOR_BLACK;
  lv_obj_t* label_ns1 = lv_label_create(cont_ns, NULL);
  lv_label_set_style(label_ns1, LV_LABEL_STYLE_MAIN, &label_ns_style1);
  lv_label_set_text(label_ns1," 降噪等级 ");//设置文本
  lv_obj_align(label_ns1, cont_ns, LV_ALIGN_IN_TOP_MID, 0, 50);
  
  /* 获取降噪算法状态 */
  Algorithm_Port_Get_Function_State(DENOISE_FUNC_SEL, &NS_Func_State); 
  if(NS_Func_State == 1)
  {
    Algorithm_Port_Get_DENOISE_Par(&NS_Level);  //获取降噪等级1-3
    switch(NS_Level)
    {
      case 1:
        lv_label_set_text(label_ns," 低 ");//设置文本
        break;
      case 2:
        lv_label_set_text(label_ns," 中 ");//设置文本
        break;
      case 3:
        lv_label_set_text(label_ns," 高 ");//设置文本
        break;
      default:
        break;
    }
  }
  else
  {
    lv_label_set_text(label_ns," 关 ");//设置文本
  }
}

/**
 * @brief 更多设置界面(不常用设置)
 *
 */
static void enter_more_set_ui(void)
{
  if(cont_set != NULL)
  {
    lv_obj_del(cont_set);
    cont_set = NULL;
  }
  if(cont_brightness != NULL)
  {
    lv_obj_del(cont_brightness);
    cont_brightness = NULL;
  }
  if(cont_reset != NULL)
  {
    lv_obj_del(cont_reset);
    cont_reset = NULL;
  }
  if(cont_ble != NULL)
  {
    lv_obj_del(cont_ble);
    cont_ble = NULL;
  }
  if(cont_mode != NULL)
  {
    lv_obj_del(cont_mode);
    cont_mode = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[NS_IDX];
  cont_more_set = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_more_set, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_more_set, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_more_set, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
   /* 设置图片按钮被按下时的样式 */
  lv_style_copy(&set_imgbtn_style, &lv_style_plain);
  set_imgbtn_style.image.color = LV_COLOR_BLACK;
  set_imgbtn_style.image.intense = LV_OPA_50;
  set_imgbtn_style.text.color = LV_COLOR_MAKE(0xAA,0xAA,0xAA);
  /* 创建图片按键 */
  imgbtn_ble_90 = lv_imgbtn_create(cont_more_set,NULL);
  imgbtn_brightness_90 = lv_imgbtn_create(cont_more_set,NULL);
  imgbtn_reset_90 = lv_imgbtn_create(cont_more_set,NULL);
  imgbtn_mode_90 = lv_imgbtn_create(cont_more_set,NULL);
  /* 设置按下和释放的样子 */
  lv_imgbtn_set_src(imgbtn_ble_90,LV_BTN_STATE_PR,&ble_90);
  lv_imgbtn_set_src(imgbtn_brightness_90,LV_BTN_STATE_PR,&brightness_90);
  lv_imgbtn_set_src(imgbtn_reset_90,LV_BTN_STATE_PR,&reset_90);
  lv_imgbtn_set_src(imgbtn_mode_90,LV_BTN_STATE_PR,&mode_90);
  lv_imgbtn_set_src(imgbtn_ble_90,LV_BTN_STATE_REL,&ble_90);
  lv_imgbtn_set_src(imgbtn_brightness_90,LV_BTN_STATE_REL,&brightness_90);
  lv_imgbtn_set_src(imgbtn_reset_90,LV_BTN_STATE_REL,&reset_90);
  lv_imgbtn_set_src(imgbtn_mode_90,LV_BTN_STATE_REL,&mode_90);
  /* 设置按下时的样式 */
  lv_imgbtn_set_style(imgbtn_ble_90,LV_BTN_STATE_PR,&set_imgbtn_style);
  lv_imgbtn_set_style(imgbtn_brightness_90,LV_BTN_STATE_PR,&set_imgbtn_style);
  lv_imgbtn_set_style(imgbtn_reset_90,LV_BTN_STATE_PR,&set_imgbtn_style);
  lv_imgbtn_set_style(imgbtn_mode_90,LV_BTN_STATE_PR,&set_imgbtn_style);
  /* 设置对齐方式 */
  lv_obj_align(imgbtn_ble_90, cont_more_set, LV_ALIGN_IN_BOTTOM_LEFT, 70, -70);
  lv_obj_align(imgbtn_brightness_90, cont_more_set, LV_ALIGN_IN_TOP_LEFT, 70, 70);
  lv_obj_align(imgbtn_reset_90, cont_more_set, LV_ALIGN_IN_BOTTOM_RIGHT, -70, -70);
  lv_obj_align(imgbtn_mode_90, cont_more_set, LV_ALIGN_IN_TOP_RIGHT, -70, 70);
  /* 设置回调事件 */
  lv_obj_set_event_cb(imgbtn_brightness_90, secondary_imgbtn_event_cb);
  lv_obj_set_event_cb(imgbtn_reset_90,secondary_imgbtn_event_cb);
  lv_obj_set_event_cb(imgbtn_ble_90,secondary_imgbtn_event_cb);
  lv_obj_set_event_cb(imgbtn_mode_90,secondary_imgbtn_event_cb);
  /* 设置字体样式 */
  static lv_style_t style1;
  lv_style_copy(&style1,&lv_style_scr);
  style1.text.font = &hht_font_30;//在样式中使用这个字体
  style1.text.color = LV_COLOR_BLACK;
  lv_obj_t* label = lv_label_create(cont_more_set, NULL);
  lv_label_set_style(label,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label," 设置 ");//设置文本
  lv_obj_align(label,cont_more_set,LV_ALIGN_IN_TOP_MID,0,20);
  
  lv_obj_t* label_brightness = lv_label_create(cont_more_set, NULL);
  lv_label_set_style(label_brightness,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_brightness," 亮度 ");//设置文本
  lv_obj_align(label_brightness,imgbtn_brightness_90,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  
  lv_obj_t* label_mode = lv_label_create(cont_more_set, NULL);
  lv_label_set_style(label_mode,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_mode," 模式 ");//设置文本
  lv_obj_align(label_mode,imgbtn_mode_90,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  
  lv_obj_t* label_ble = lv_label_create(cont_more_set, NULL);
  lv_label_set_style(label_ble,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_ble," 蓝牙 ");//设置文本
  lv_obj_align(label_ble,imgbtn_ble_90,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  
  lv_obj_t* label_reset = lv_label_create(cont_more_set, NULL);
  lv_label_set_style(label_reset,LV_LABEL_STYLE_MAIN,&style1);
  lv_label_set_text(label_reset," 恢复 ");//设置文本
  lv_obj_align(label_reset,imgbtn_reset_90,LV_ALIGN_OUT_BOTTOM_MID,0,0);
}

/**
 * @brief 亮度调节界面
 *
 */
static void enter_brightness_ui(void)
{
  if(cont_more_set != NULL)
  {
    lv_obj_del(cont_more_set);
    cont_more_set = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[BRIGHTNESS_IDX];
  cont_brightness = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_brightness, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_brightness, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_brightness, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  /* 创建一个标签,显示"屏幕亮度" */
  lv_obj_t* label_brightness = lv_label_create(cont_brightness, NULL);
  static lv_style_t label_brightness_style;
  lv_style_copy(&label_brightness_style,&lv_style_scr);
  label_brightness_style.text.font = &hht_font_50;//在样式中使用这个字体
  label_brightness_style.text.color = LV_COLOR_BLACK;
  lv_label_set_style(label_brightness,LV_LABEL_STYLE_MAIN,&label_brightness_style);
  lv_label_set_text(label_brightness," 屏幕亮度 ");//设置文本
  lv_obj_align(label_brightness,cont_brightness,LV_ALIGN_IN_TOP_MID,0,35);
  
  static lv_style_t btn_ns_release_style;
  static lv_style_t btn_ns_press_style;
  /* 释放状态下的样式 */
  lv_style_copy(&btn_ns_release_style, &lv_style_plain_color);
  btn_ns_release_style.body.main_color = LV_COLOR_MAKE(255,133,10);
  btn_ns_release_style.body.grad_color = btn_ns_release_style.body.main_color;
  btn_ns_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_ns_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按下状态下的样式 */
  lv_style_copy(&btn_ns_press_style,&lv_style_plain_color);
  btn_ns_press_style.body.opa = LV_OPA_0;//设置背景色透明
  btn_ns_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_ns_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
  btn_ns_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制  
  
  btn_brightness1 = lv_btn_create(cont_brightness, NULL);
  lv_obj_set_size(btn_brightness1,100,100);//设置大小
  lv_obj_align(btn_brightness1, cont_brightness, LV_ALIGN_IN_BOTTOM_MID, 0, -10);//设置对齐方式
  lv_obj_set_event_cb(btn_brightness1, brightness_event_handler); //设置事件回调函数
  lv_btn_set_style(btn_brightness1,LV_BTN_STYLE_REL, &btn_ns_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_brightness1,LV_BTN_STYLE_PR, &btn_ns_press_style);//设置按钮正常态下按下状态样式
  
  static lv_style_t label_brightness_style1;
  lv_style_copy(&label_brightness_style1,&lv_style_scr);
  label_brightness_style1.text.font = &hht_font_30;//在样式中使用这个字体
  label_brightness_style1.text.color = LV_COLOR_BLACK;
  lv_obj_t* label_brightness1 = lv_label_create(cont_brightness, NULL);
  lv_label_set_style(label_brightness1,LV_LABEL_STYLE_MAIN,&label_brightness_style1);
  lv_label_set_text(label_brightness1, " 设置 ");
  lv_obj_align(label_brightness1, btn_brightness1, LV_ALIGN_CENTER, 0, 0);//设置对齐方式
   
  static lv_style_t brightness_bar_bg_style;
  static lv_style_t brightness_bar_indic_style;
  /* 创建进度条背景样式 */
  lv_style_copy(&brightness_bar_bg_style,&lv_style_plain_color);
  brightness_bar_bg_style.body.main_color = LV_COLOR_MAKE(0xBB,0xBB,0xBB);
  brightness_bar_bg_style.body.grad_color = LV_COLOR_MAKE(0xBB,0xBB,0xBB);
  brightness_bar_bg_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角
  /* 创建指示器样式 */
  lv_style_copy(&brightness_bar_indic_style,&lv_style_plain_color);
  brightness_bar_indic_style.body.main_color = LV_COLOR_MAKE(255,133,10);
  brightness_bar_indic_style.body.grad_color = LV_COLOR_MAKE(255,133,10);
  brightness_bar_indic_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角
  brightness_bar_indic_style.body.padding.left = 0;//让指示器跟背景边框之间没有距离
  brightness_bar_indic_style.body.padding.top = 0;
  brightness_bar_indic_style.body.padding.right = 0;
  brightness_bar_indic_style.body.padding.bottom = 0;
  /* 创建平行进度条 */
  brightness_bar1 = lv_bar_create(cont_brightness, NULL);
  lv_obj_set_size(brightness_bar1,80,15);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(brightness_bar1,LV_BAR_STYLE_BG,&brightness_bar_bg_style);//
  lv_bar_set_style(brightness_bar1,LV_BAR_STYLE_INDIC,&brightness_bar_indic_style);
  lv_obj_align(brightness_bar1,cont_brightness,LV_ALIGN_CENTER,0,45);
  lv_bar_set_range(brightness_bar1,0,1);//设置进度范围
  lv_bar_set_value(brightness_bar1,1,LV_ANIM_ON);
  
  brightness_bar2 = lv_bar_create(cont_brightness, NULL);
  lv_obj_set_size(brightness_bar2,80,15);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(brightness_bar2,LV_BAR_STYLE_BG,&brightness_bar_bg_style);//
  lv_bar_set_style(brightness_bar2,LV_BAR_STYLE_INDIC,&brightness_bar_indic_style);
  lv_obj_align(brightness_bar2,brightness_bar1,LV_ALIGN_OUT_TOP_MID,0,-10);
  lv_bar_set_range(brightness_bar2,0,1);//设置进度范围
  lv_bar_set_value(brightness_bar2,1,LV_ANIM_ON);
  
  brightness_bar3 = lv_bar_create(cont_brightness, NULL);
  lv_obj_set_size(brightness_bar3,80,15);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(brightness_bar3,LV_BAR_STYLE_BG,&brightness_bar_bg_style);//
  lv_bar_set_style(brightness_bar3,LV_BAR_STYLE_INDIC,&brightness_bar_indic_style);
  lv_obj_align(brightness_bar3,brightness_bar2,LV_ALIGN_OUT_TOP_MID,0,-10);
  lv_bar_set_range(brightness_bar3,0,1);//设置进度范围
  lv_bar_set_value(brightness_bar3,1,LV_ANIM_ON);
  
  brightness_bar4 = lv_bar_create(cont_brightness, NULL);
  lv_obj_set_size(brightness_bar4,80,15);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(brightness_bar4,LV_BAR_STYLE_BG,&brightness_bar_bg_style);//
  lv_bar_set_style(brightness_bar4,LV_BAR_STYLE_INDIC,&brightness_bar_indic_style);
  lv_obj_align(brightness_bar4,brightness_bar3,LV_ALIGN_OUT_TOP_MID,0,-10);
  lv_bar_set_range(brightness_bar4,0,1);//设置进度范围
  lv_bar_set_value(brightness_bar4,1,LV_ANIM_ON);
  
  brightness_bar5 = lv_bar_create(cont_brightness, NULL);
  lv_obj_set_size(brightness_bar5,80,15);//设置大小,宽度比高度小就是垂直的
  lv_bar_set_style(brightness_bar5,LV_BAR_STYLE_BG,&brightness_bar_bg_style);//
  lv_bar_set_style(brightness_bar5,LV_BAR_STYLE_INDIC,&brightness_bar_indic_style);
  lv_obj_align(brightness_bar5,brightness_bar4,LV_ALIGN_OUT_TOP_MID,0,-10);
  lv_bar_set_range(brightness_bar5,0,1);//设置进度范围
  lv_bar_set_value(brightness_bar5,1,LV_ANIM_ON);
}

/**
 * @brief 模式设置界面
 *
 */
static void enter_mode_ui(void)
{
  if(cont_more_set != NULL)
  {
    lv_obj_del(cont_more_set);
    cont_more_set = NULL;
  }
  if(cont_test != NULL)
  {
    lv_obj_del(cont_test);
    cont_test = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[MODE_IDX];
  cont_mode = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_mode, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_mode, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_mode, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  /* 创建一个标签,显示"模式选择" */
  lv_obj_t* label_mode = lv_label_create(cont_mode, NULL);
  static lv_style_t label_mode_style;
  lv_style_copy(&label_mode_style,&lv_style_scr);
  label_mode_style.text.font = &hht_font_50;//在样式中使用这个字体
  label_mode_style.text.color = LV_COLOR_BLACK;
  lv_label_set_style(label_mode,LV_LABEL_STYLE_MAIN,&label_mode_style);
  lv_label_set_text(label_mode," 模式选择 ");//设置文本
  lv_obj_align(label_mode,cont_mode,LV_ALIGN_IN_TOP_MID,0,30);
  
  static lv_style_t btn_mode_release_style;
  static lv_style_t btn_mode_press_style;
  /* 释放状态下的样式 */
  lv_style_copy(&btn_mode_release_style, &lv_style_plain_color);
  btn_mode_release_style.body.main_color = LV_COLOR_MAKE(255,133,10);
  btn_mode_release_style.body.grad_color = btn_mode_release_style.body.main_color;
  btn_mode_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_mode_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按下状态下的样式 */
  lv_style_copy(&btn_mode_press_style,&lv_style_plain_color);
  btn_mode_press_style.body.opa = LV_OPA_0;//设置背景色透明
  btn_mode_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_mode_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
  btn_mode_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制
  
  /* 设置字体样式 */
  static lv_style_t label_mode_style1;
  lv_style_copy(&label_mode_style1,&lv_style_scr);
  label_mode_style1.text.color = LV_COLOR_WHITE;
  
  lv_obj_t* btn_mode1 = lv_btn_create(cont_mode, NULL);
  lv_obj_set_size(btn_mode1,90,90);//设置大小
  lv_obj_align(btn_mode1, cont_mode, LV_ALIGN_IN_LEFT_MID, 30, -30);//设置对齐方式
  lv_btn_set_style(btn_mode1,LV_BTN_STYLE_REL, &btn_mode_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_mode1,LV_BTN_STYLE_PR, &btn_mode_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_mode1 = lv_label_create(cont_mode, NULL);
  lv_label_set_text(label_mode1, "1");
  lv_obj_align(label_mode1, btn_mode1, LV_ALIGN_CENTER, 0, 0); //设置对齐方式
  lv_label_set_style(label_mode1,LV_LABEL_STYLE_MAIN,&label_mode_style1);//设置字体样式
  
  lv_obj_t* btn_mode2 = lv_btn_create(cont_mode, NULL);
  lv_obj_set_size(btn_mode2,90,90);//设置大小
  lv_obj_align(btn_mode2, btn_mode1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);//设置对齐方式
  lv_btn_set_style(btn_mode2,LV_BTN_STYLE_REL, &btn_mode_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_mode2,LV_BTN_STYLE_PR, &btn_mode_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_mode2 = lv_label_create(cont_mode, NULL);
  lv_label_set_text(label_mode2, "2");
  lv_obj_align(label_mode2, btn_mode2, LV_ALIGN_CENTER, 0, 0); //设置对齐方式
  lv_label_set_style(label_mode2,LV_LABEL_STYLE_MAIN,&label_mode_style1);//设置字体样式
  
  lv_obj_t* btn_mode3 = lv_btn_create(cont_mode, NULL);
  lv_obj_set_size(btn_mode3,90,90);//设置大小
  lv_obj_align(btn_mode3, btn_mode2, LV_ALIGN_OUT_RIGHT_MID, 10, 0);//设置对齐方式
  lv_btn_set_style(btn_mode3,LV_BTN_STYLE_REL, &btn_mode_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_mode3,LV_BTN_STYLE_PR, &btn_mode_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_mode3 = lv_label_create(cont_mode, NULL);
  lv_label_set_text(label_mode3, "3");
  lv_obj_align(label_mode3, btn_mode3, LV_ALIGN_CENTER, 0, 0); //设置对齐方式
  lv_label_set_style(label_mode3,LV_LABEL_STYLE_MAIN,&label_mode_style1);//设置字体样式
  
  lv_obj_t* btn_mode4 = lv_btn_create(cont_mode, NULL);
  lv_obj_set_size(btn_mode4,90,90);//设置大小
  lv_obj_align(btn_mode4, btn_mode1, LV_ALIGN_OUT_BOTTOM_MID, 50, 10);//设置对齐方式
  lv_btn_set_style(btn_mode4,LV_BTN_STYLE_REL, &btn_mode_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_mode4,LV_BTN_STYLE_PR, &btn_mode_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_mode4 = lv_label_create(cont_mode, NULL);
  lv_label_set_text(label_mode4, "4");
  lv_obj_align(label_mode4, btn_mode4, LV_ALIGN_CENTER, 0, 0); //设置对齐方式
  lv_label_set_style(label_mode4,LV_LABEL_STYLE_MAIN,&label_mode_style1);//设置字体样式
  
  btn_mode5 = lv_btn_create(cont_mode, NULL);
  lv_obj_set_size(btn_mode5,90,90);//设置大小
  lv_obj_align(btn_mode5, btn_mode4, LV_ALIGN_OUT_RIGHT_MID, 10, 0);//设置对齐方式
  lv_btn_set_style(btn_mode5,LV_BTN_STYLE_REL, &btn_mode_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_mode5,LV_BTN_STYLE_PR, &btn_mode_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_mode5 = lv_label_create(cont_mode, NULL);
  lv_label_set_text(label_mode5, LV_SYMBOL_EDIT);
  lv_obj_align(label_mode5, btn_mode5, LV_ALIGN_CENTER, 0, 0); //设置对齐方式
  lv_label_set_style(label_mode5,LV_LABEL_STYLE_MAIN,&label_mode_style1);//设置字体样式
  
  lv_obj_set_event_cb(btn_mode5, mode_event_handler);
}
/**
* @brief 重置界面(恢复)
 *
 */
static void enter_reset_ui(void)
{
  if(cont_more_set != NULL)
  {
    lv_obj_del(cont_more_set);
    cont_more_set = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[RESET_IDX];
  cont_reset = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_reset, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_reset, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_reset, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  /* 标签样式 */
  static lv_style_t label_reset_style;
  lv_style_copy(&label_reset_style,&lv_style_scr);
  label_reset_style.text.font = &hht_font_30;//在样式中使用这个字体
  label_reset_style.text.color = LV_COLOR_BLACK;
  /* 标签样式 */
  static lv_style_t label_reset_style2;
  lv_style_copy(&label_reset_style2,&lv_style_scr);
  label_reset_style2.text.font = &hht_font_50;//在样式中使用这个字体
  label_reset_style2.text.color = LV_COLOR_BLACK;
  /* 创建一个标签，显示"出厂设置" */
  lv_obj_t* label_reset1 = lv_label_create(cont_reset, NULL);
  lv_label_set_text(label_reset1, "出厂设置");
  lv_label_set_style(label_reset1, LV_LABEL_STYLE_MAIN, &label_reset_style2);
  lv_obj_align(label_reset1, cont_reset, LV_ALIGN_IN_TOP_MID, 0, 60);
  
  /* 创建一个标签，显示"是否恢复默认" */
  lv_obj_t* label_reset2 = lv_label_create(cont_reset, NULL);
  lv_label_set_text(label_reset2, "是否恢复默认");
  lv_label_set_style(label_reset2, LV_LABEL_STYLE_MAIN, &label_reset_style);
  lv_obj_align(label_reset2, cont_reset, LV_ALIGN_CENTER, 0, -30);
  
  lv_obj_t* label_reset3 = lv_label_create(cont_reset, NULL);
  lv_label_set_text(label_reset3, "出厂设置");
  lv_label_set_style(label_reset3, LV_LABEL_STYLE_MAIN, &label_reset_style);
  lv_obj_align(label_reset3, label_reset2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  
  static lv_style_t btn_reset_release_style;
  static lv_style_t btn_retset_press_style;
  /* 释放状态下的样式 */
  lv_style_copy(&btn_reset_release_style, &lv_style_plain_color);
  btn_reset_release_style.body.main_color = LV_COLOR_MAKE(61,158,255);
  btn_reset_release_style.body.grad_color = btn_reset_release_style.body.main_color;
  btn_reset_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_reset_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按下状态下的样式 */
  lv_style_copy(&btn_retset_press_style,&lv_style_plain_color);
  btn_retset_press_style.body.opa = LV_OPA_0;//设置背景色透明
	btn_retset_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_retset_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
	btn_retset_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制
  
  /* 设置字体样式 */
  static lv_style_t label_reset_style1;
  lv_style_copy(&label_reset_style1,&lv_style_scr);
  label_reset_style1.text.font = &hht_font_30;
  label_reset_style1.text.color = LV_COLOR_WHITE;
  
  btn_reset1 = lv_btn_create(cont_reset, NULL);
	lv_obj_set_size(btn_reset1,90,90);//设置大小
	lv_obj_align(btn_reset1, cont_mode, LV_ALIGN_IN_BOTTOM_LEFT, 80, -40);//设置对齐方式
	lv_btn_set_style(btn_reset1,LV_BTN_STYLE_REL, &btn_reset_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_reset1,LV_BTN_STYLE_PR, &btn_retset_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_reset4 = lv_label_create(cont_reset, NULL);
  lv_label_set_text(label_reset4, " 是 ");
  lv_obj_align(label_reset4, btn_reset1, LV_ALIGN_CENTER, -10, 0); //设置对齐方式
  lv_label_set_style(label_reset4,LV_LABEL_STYLE_MAIN,&label_reset_style1);//设置字体样式
  
  btn_reset2 = lv_btn_create(cont_reset, NULL);
	lv_obj_set_size(btn_reset2,90,90);//设置大小
	lv_obj_align(btn_reset2, btn_reset1, LV_ALIGN_OUT_RIGHT_MID, 30, 0);//设置对齐方式
	lv_btn_set_style(btn_reset2,LV_BTN_STYLE_REL, &btn_reset_release_style);//设置按钮正常态下释放状态样式
	lv_btn_set_style(btn_reset2,LV_BTN_STYLE_PR, &btn_retset_press_style);//设置按钮正常态下按下状态样式
  lv_obj_t* label_reset5 = lv_label_create(cont_reset, NULL);
  lv_label_set_text(label_reset5, " 否 ");
  lv_obj_align(label_reset5, btn_reset2, LV_ALIGN_CENTER, -10, 0); //设置对齐方式
  lv_label_set_style(label_reset5,LV_LABEL_STYLE_MAIN,&label_reset_style1);//设置字体样式
  
  lv_obj_set_event_cb(btn_reset1, reset_event_handler);//设置事件回调函数
  lv_obj_set_event_cb(btn_reset2, reset_event_handler);//设置事件回调函数
}
/**
 * @brief 蓝牙设置界面
 *
 */
static void enter_ble_ui(void)
{
  if(cont_more_set != NULL)
  {
    lv_obj_del(cont_more_set);
    cont_more_set = NULL;
  }
  Current_UI_Inf = &UI_Base_Inf[BLE_IDX];
  cont_ble = lv_cont_create(scr, NULL);  //创建一个容器
  lv_obj_align(cont_ble, NULL, LV_ALIGN_CENTER, 0, 0); //设置当前容器居中对齐
  lv_obj_set_style(cont_ble, &lv_style_scr); //设置容器的样式
  lv_cont_set_fit(cont_ble, LV_FIT_FLOOD);  //设置当前容器平铺整个屏幕
  
  /* 创建一个标签，显示"蓝牙" */
  static lv_style_t label_ble_style;
  lv_style_copy(&label_ble_style,&lv_style_scr);
  label_ble_style.text.font = &hht_font_30;//在样式中使用这个字体
  label_ble_style.text.color = LV_COLOR_BLACK;
  
  static lv_style_t label_ble_style2;
  lv_style_copy(&label_ble_style2,&lv_style_scr);
  label_ble_style2.text.font = &hht_font_50;//在样式中使用这个字体
  label_ble_style2.text.color = LV_COLOR_BLACK;
  
  lv_obj_t* label_ble1 = lv_label_create(cont_ble, NULL);
  lv_label_set_text(label_ble1, " 蓝牙 ");
  lv_label_set_style(label_ble1, LV_LABEL_STYLE_MAIN, &label_ble_style2);
  lv_obj_align(label_ble1, cont_ble, LV_ALIGN_IN_TOP_MID, 0, 20);
  
  static lv_style_t label_ble_style1;
  lv_style_copy(&label_ble_style1,&lv_style_scr);
  label_ble_style1.body.main_color = LV_COLOR_MAKE(224,224,224);
  label_ble_style1.body.grad_color = LV_COLOR_MAKE(224,224,224);
  label_ble_style1.text.font = &hht_font_30;//在样式中使用这个字体
  label_ble_style1.text.color = LV_COLOR_BLUE;
  /* 检测蓝牙是否连接 */
  if(Ble_Connect_Check() == Ble_Connected)
	{
    lv_obj_t* label_ble2 = lv_label_create(cont_ble, NULL);
    lv_label_set_body_draw(label_ble2,true);//使能背景绘制
    lv_label_set_long_mode(label_ble2,LV_LABEL_LONG_CROP);//设置为长文本
    lv_obj_set_size(label_ble2, 200,40);
    lv_obj_set_pos(label_ble2, 90, 100);
    lv_label_set_text(label_ble2," 蓝牙已连接 ");
    lv_label_set_align(label_ble2,LV_LABEL_ALIGN_CENTER);//设置文本居中对齐
    lv_label_set_style(label_ble2, LV_LABEL_STYLE_MAIN, &label_ble_style1);
	}
	else
	{
    lv_obj_t* label_ble3 = lv_label_create(cont_ble, NULL);
    lv_label_set_body_draw(label_ble3,true);//使能背景绘制
    lv_label_set_long_mode(label_ble3,LV_LABEL_LONG_CROP);//设置为长文本
    lv_obj_set_size(label_ble3, 180,35);
    lv_obj_set_pos(label_ble3, 90, 100);
    lv_label_set_text(label_ble3, " 蓝牙未连接 ");
    lv_label_set_style(label_ble3, LV_LABEL_STYLE_MAIN, &label_ble_style1);
    lv_label_set_align(label_ble3,LV_LABEL_ALIGN_CENTER);//设置文本居中对齐
  }
  
  static lv_style_t btn_ble_release_style;
  static lv_style_t btn_ble_press_style;
  /* 释放状态下的样式 */
  lv_style_copy(&btn_ble_release_style, &lv_style_plain_color);
  btn_ble_release_style.body.main_color = LV_COLOR_MAKE(0,184,245);
  btn_ble_release_style.body.grad_color = btn_ble_release_style.body.main_color;
  btn_ble_release_style.body.opa = LV_OPA_COVER;//设置背景色完全不透明
  btn_ble_release_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮

  /* 按下状态下的样式 */
  lv_style_copy(&btn_ble_press_style,&lv_style_plain_color);
  btn_ble_press_style.body.opa = LV_OPA_0;//设置背景色透明
  btn_ble_press_style.body.radius = LV_RADIUS_CIRCLE;//绘制圆角按钮
  btn_ble_press_style.body.border.color = LV_COLOR_MAKE(110,207,255);//设置边框的颜色
  btn_ble_press_style.body.border.part = LV_BORDER_FULL;//四条边框都绘制
  
  /* 设置字体样式 */
  static lv_style_t ble_style;
  lv_style_copy(&ble_style,&lv_style_scr);
  ble_style.text.font = &hht_font_30;
  ble_style.text.color = LV_COLOR_WHITE;
  /* 蓝牙按钮 */
  btn_ble = lv_btn_create(cont_ble, NULL);
  lv_obj_set_size(btn_ble,120,120);//设置大小
  lv_obj_align(btn_ble, cont_mode, LV_ALIGN_IN_BOTTOM_MID, 0, -60);//设置对齐方式
  lv_btn_set_style(btn_ble,LV_BTN_STYLE_REL, &btn_ble_release_style);//设置按钮正常态下释放状态样式
  lv_btn_set_style(btn_ble,LV_BTN_STYLE_PR, &btn_ble_press_style);//设置按钮正常态下按下状态样式
  /* 创建一个标签， 显示"重连" */
  lv_obj_t* label_ble = lv_label_create(cont_ble, NULL);
  lv_label_set_text(label_ble, " 重连 ");
  lv_obj_align(label_ble, btn_ble, LV_ALIGN_CENTER, -20, 0); //设置对齐方式
  lv_label_set_style(label_ble,LV_LABEL_STYLE_MAIN,&ble_style);//设置字体样式
  lv_obj_set_event_cb(btn_ble, ble_event_handler);
}
/**
 * @brief 更新电量显示,充电状态,蓝牙连接状态（时间1秒）
 *
 */
static void battery_ble_updata_task_cb(lv_task_t* task)
{
  if(Current_UI_Inf == &UI_Base_Inf[MAIN_IDX])
  {
     /* 电量显示 */
    char buff[10];
    curr_battery = MAX17048_Driver_Get_Battery_Electric_Quantity();
    if (curr_battery != last_battery)
    {
      lv_lmeter_set_value(lmeter_soc,curr_battery);
      sprintf(buff,"%d%%",curr_battery);
      lv_label_set_text(label_charge,buff);
    }
    last_battery = curr_battery;
    /* 充电状态 */
    Curr_Power_State = Voltameter_Port_Get_Battery_Charge_State();
    if(Curr_Power_State != Last_Power_State)
    {
      if(Curr_Power_State == 1)
      {
        main_img_charge = lv_img_create(cont_main,NULL);
        lv_img_set_src(main_img_charge, LV_SYMBOL_POWER);
        lv_obj_align(main_img_charge,lmeter_soc,LV_ALIGN_CENTER,0,-55);
      }
      else
      {
        lv_obj_del(main_img_charge);
        main_img_charge = NULL;
      } 
    }
    Last_Power_State = Curr_Power_State;
  }
  /* 蓝牙连接状态 */
  if(Current_UI_Inf == &UI_Base_Inf[BLE_IDX])
  {
    Ble_Connect_Stat = Ble_Connect_Check();
    if(Ble_Connect_Stat != Pre_Ble_Connect_Stat)
		{
      static lv_style_t label_ble_style1;
      lv_style_copy(&label_ble_style1,&lv_style_scr);
      label_ble_style1.body.main_color = LV_COLOR_MAKE(224,224,224);
      label_ble_style1.body.grad_color = LV_COLOR_MAKE(224,224,224);
      label_ble_style1.text.font = &hht_font_30;//在样式中使用这个字体
      label_ble_style1.text.color = LV_COLOR_BLUE;
      /* 检测蓝牙是否连接 */
			if(Ble_Connect_Stat == Ble_Connected)
			{
				lv_obj_t* label_ble2 = lv_label_create(cont_ble, NULL);
        lv_label_set_body_draw(label_ble2,true);//使能背景绘制
        lv_label_set_long_mode(label_ble2,LV_LABEL_LONG_CROP);//设置为长文本
        lv_obj_set_size(label_ble2, 200,40);//设置大小
        lv_obj_set_pos(label_ble2, 90, 100);//设置位置
        lv_label_set_text(label_ble2," 蓝牙已连接 ");
        lv_label_set_align(label_ble2,LV_LABEL_ALIGN_CENTER);//设置文本居中对齐
        lv_label_set_style(label_ble2, LV_LABEL_STYLE_MAIN, &label_ble_style1);
			}
			else
			{
				lv_obj_t* label_ble3 = lv_label_create(cont_ble, NULL);
        lv_label_set_body_draw(label_ble3,true);//使能背景绘制
        lv_label_set_long_mode(label_ble3,LV_LABEL_LONG_CROP);//设置为长文本
        lv_obj_set_size(label_ble3, 180,35);
        lv_obj_set_pos(label_ble3, 90, 100);
        lv_label_set_text(label_ble3, " 蓝牙未连接 ");
        lv_label_set_style(label_ble3, LV_LABEL_STYLE_MAIN, &label_ble_style1);
        lv_label_set_align(label_ble3,LV_LABEL_ALIGN_CENTER);//设置文本居中对齐         
			}
		}
    Pre_Ble_Connect_Stat = Ble_Connect_Stat;
  }
    
}
/**
 * @brief 主界面设置图片按钮事件
 *
 */
static void main_set_event_cb(lv_obj_t* obj, lv_event_t event)
{
  if(event == LV_EVENT_RELEASED)
  {
    hht_set_ui();
  }
}

/**
 * @brief 主设置界面图片按键事件
 *
 */
static void imgbtn_event_cb(lv_obj_t* obj, lv_event_t event)
{
  if(event == LV_EVENT_RELEASED)
  {
    if(obj == imgbtn_bf_100)
    {
      enter_bf_ui();
    }
    else if(obj == imgbtn_ns_100)
    {
      enter_ns_ui();
    }
    else if(obj == imgbtn_vol_100)
    {
      enter_vol_ui();
    }
    else if(obj == imgbtn_set_100)
    {
      enter_more_set_ui();
    }
  }
}
/**
 * @brief 音量设置事件
 *
 */
static void vol_event_handler(lv_obj_t* obj, lv_event_t event)
{
  
  if(event == LV_EVENT_RELEASED)
  {
    if(Current_UI_Inf->ui_idx == VOL_IDX)
    {
      if(obj == vol_btn1)
      {
        if(Vol_L < 7)
        {
          Vol_L += 1;
          Algorithm_Port_Set_VC_Level(Vol_L, ALGORITHM_CHANNEL_CH1);
          lv_bar_set_value(main_bar_l,Vol_L,LV_ANIM_ON);
        }
      }
      else if(obj == vol_btn3)
      {
        if(Vol_R < 7)
        {
          Vol_R += 1;
          Algorithm_Port_Set_VC_Level(Vol_R, ALGORITHM_CHANNEL_CH2);
          lv_bar_set_value(main_bar_r,Vol_R,LV_ANIM_ON);
        }
      }
      else if(obj == vol_btn2)
      {
        if(Vol_L > 0)
        {
          Vol_L -= 1;
          Algorithm_Port_Set_VC_Level(Vol_L, ALGORITHM_CHANNEL_CH1);
          lv_bar_set_value(main_bar_l,Vol_L,LV_ANIM_ON);
        }
      }
      else if(obj == vol_btn4)
      {
        if(Vol_R > 0)
        {
          Vol_R -= 1;
          Algorithm_Port_Set_VC_Level(Vol_R, ALGORITHM_CHANNEL_CH2);
          lv_bar_set_value(main_bar_r,Vol_R,LV_ANIM_ON);
        }
      }
    } 
  }
}

/**
 * @brief BF设置事件
 *
 */
static void bf_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_RELEASED)
	{
    if(obj == bf_open)
		{
			lv_btn_set_style(bf_open,LV_BTN_STYLE_REL,&btn_bf_release_style_close);
      Algorithm_Port_Set_Function_State(BF_FUNC_SEL, 0);  // 关闭BF算法
      lv_label_set_text(bf_label_rests," 关 ");//设置文本
			bf_open = NULL;
		}
    else
    {
      if(bf_open != NULL)
		  {
			  lv_btn_set_style(bf_open,LV_BTN_STYLE_REL,&btn_bf_release_style_close);
		  }
      Algorithm_Port_Set_Function_State(BF_FUNC_SEL, 1);  // 打开BF算法
      lv_btn_set_style(obj,LV_BTN_STYLE_REL,&btn_bf_release_style_open);
      bf_open =  obj;
      if(obj == btn_bf[0])
		  {
			  Algorithm_Port_Set_BF_Angle(0);
        lv_label_set_text(bf_label_rests," 左 ");//设置文本
		  }
		  else if(obj == btn_bf[1])
		  {
				Algorithm_Port_Set_BF_Angle(90);
        lv_label_set_text(bf_label_rests," 前 ");//设置文本
		  }
		  else if(obj == btn_bf[2])
		  {
				Algorithm_Port_Set_BF_Angle(180);
        lv_label_set_text(bf_label_rests," 右 ");//设置文本
		  }
		  else if(obj == btn_bf[3])
		  {
				Algorithm_Port_Set_BF_Angle(270);
        lv_label_set_text(bf_label_rests," 后 ");//设置文本
		  }
		  else if(obj == btn_bf[4])
		  {
        /* 自适应 */
		  }
    }
	}
}

/**
 * @brief 降噪设置事件
 *
 */
static void ns_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_CLICKED)
  { 
    switch(NS_Level)
    {
      case 1:
        lv_label_set_text(label_ns," 中 ");//设置文本
        NS_Level = 2;
			  Algorithm_Port_Set_DENOISE_Par(NS_Level);
        break;
      case 2:
			  lv_label_set_text(label_ns," 高 ");//设置文本
		    NS_Level = 3;
			  Algorithm_Port_Set_DENOISE_Par(NS_Level);
			  break;
      case 3:
		    lv_label_set_text(label_ns," 关 ");//设置文本
		    NS_Level = 0;
        Algorithm_Port_Set_Function_State( DENOISE_FUNC_SEL, 0);  //关闭降噪
			  break;
      default:
        Algorithm_Port_Set_Function_State( DENOISE_FUNC_SEL, 1); //打开降噪
        lv_label_set_text(label_ns," 低 ");//设置文本
		    NS_Level = 1;
			  Algorithm_Port_Set_DENOISE_Par(NS_Level);
			  break;
    }       
  }
}

/**
 * @brief 不常用设置事件
 *
 */
static void secondary_imgbtn_event_cb(lv_obj_t* obj, lv_event_t event)
{
   if(event==LV_EVENT_RELEASED)
   {
     if(obj == imgbtn_brightness_90)
     {
        enter_brightness_ui();
     }
     else if(obj == imgbtn_mode_90)
     {
       enter_mode_ui();
     }
     else if(obj == imgbtn_reset_90)
     {
       enter_reset_ui();
     }
     else if(obj == imgbtn_ble_90)
     {
       enter_ble_ui();
     }
   }
}
/**
 * @brief 亮度调节事件
 *
 */
static void brightness_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_CLICKED)
  {
    if(obj == btn_brightness1)
    {
      static int stat = 0;
      LCD_BL_Adjust_Start(ADJUST_BL);
      switch(stat)
      {
        case 0:
          stat = 1;
          lv_bar_set_value(brightness_bar5,0,LV_ANIM_ON);
          break;
        case 1:
          stat = 2;
          lv_bar_set_value(brightness_bar4,0,LV_ANIM_ON);
          break;
        case 2:
          stat = 3;
          lv_bar_set_value(brightness_bar3,0,LV_ANIM_ON);
          break;
        case 3:
          stat = 0;
          lv_bar_set_value(brightness_bar3,1,LV_ANIM_ON);
          lv_bar_set_value(brightness_bar4,1,LV_ANIM_ON);
          lv_bar_set_value(brightness_bar5,1,LV_ANIM_ON);
          break;
        default:
          break;
      }
    }
  }
}
/**
 * @brief 出厂设置事件
 *
 */
static void reset_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_CLICKED)
  {
    if(obj == btn_reset1)
    {
      Algorithm_Port_RESET_Par();
    }
    else if(obj == btn_reset2)
    {
      
    }
  }
}

/**
 * @brief 蓝牙设置事件
 *
 */
static void ble_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_CLICKED)
  {
    elec_signal_start();
  }
}
/**
 * @brief 模式设置事件
 *
 */
static void mode_event_handler(lv_obj_t* obj, lv_event_t event)
{
  if(event==LV_EVENT_CLICKED)
  {
    if(obj == btn_mode5)
    {
      lv_test_ui();
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
* @brief 图片显示初始化，用于显示图片
 *
 */
void HHT_UI_Init(void)
{
  POWER_KEY_HIGH;  //BT8829C引脚控制，平时需要拉高，用来控制蓝牙重新配对
  Init_UI_Inf(LOGO_IDX, LOGO_LEVEL, hht_logo_ui, LOGO_IDX);
  Init_UI_Inf(MAIN_IDX, MIAN_LEVEL, hht_main_ui, LOGO_IDX);
  Init_UI_Inf(SET_IDX, SET_LEVEL, hht_set_ui, MAIN_IDX);
  Init_UI_Inf(VOL_IDX, VOL_LEVEL, enter_vol_ui, SET_IDX);
  Init_UI_Inf(NS_IDX, NS_LEVEL, enter_ns_ui, SET_IDX);
  Init_UI_Inf(BF_IDX, BF_LEVEL, enter_bf_ui, SET_IDX);
  Init_UI_Inf(MORE_SET_IDX, MORE_SET_LEVEL, enter_more_set_ui, SET_IDX);
  Init_UI_Inf(BRIGHTNESS_IDX, BRIGHTNESS_LEVEL, enter_brightness_ui, MORE_SET_IDX);
  Init_UI_Inf(MODE_IDX, MODE_LEVEL, enter_mode_ui, MORE_SET_IDX);
  Init_UI_Inf(RESET_IDX, RESET_LEVEL, enter_reset_ui, MORE_SET_IDX);
  Init_UI_Inf(BLE_IDX, BLE_LEVEL, enter_ble_ui, MORE_SET_IDX);
  Init_UI_Inf(TEST_IDX, TEST_LEVEL, lv_test_ui, MODE_IDX);
  Init_UI_Inf(TEST_EAR_IDX, TEST_EAR_LEVEL, enter_ear_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_CHARGE_IDX, TEST_CHARGE_LEVEL, enter_charge_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_LCD_IDX, TEST_LCD_LEVEL, enter_lcd_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_GYRO_IDX, TEST_GYRO_LEVEL, enter_gyro_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_TP_IDX, TEST_TP_LEVEL, enter_tp_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_FLASH_IDX, TEST_FLASH_LEVEL, enter_flash_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_MIC_IDX, TEST_MIC_LEVEL, enter_mic_test_ui, TEST_IDX);
  Init_UI_Inf(TEST_INSPECT_IDX, TEST_INSPECT_LEVEL, enter_inspect_test_ui, TEST_IDX);
  lv_app_start();
  hht_logo_ui();
  lv_task_handler();
  HAL_Delay(3000);
  hht_main_ui();
}
/**
 * @brief 触摸键返回功能
 *
 */
void HHT_Touch_Back(void)
{
  if(NULL != Current_UI_Inf)
  {
    if(Current_UI_Inf->ui_level != 0)
    {
      UI_Base_Inf[Current_UI_Inf->ui_prior_level].current_ui_func();
    }
  }
}
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
