#include "app_test.h"
#include "lvgl.h"
#include "stdio.h"
#include "stm32h7xx_hal.h"


/* 函数声明 */
void test_style(void);
void test_cont(void);
void test_btn(void);
void test_lmeter(void);

LV_FONT_DECLARE(my_font);
LV_FONT_DECLARE(my_font1);

// 定义图标
#define MY_ICON_SET  "\xEE\x98\x9f"   //0xee989f
#define MY_ICON_BLE  "\xEE\xAD\xA3"   //0xeeada3

lv_obj_t* scr = NULL;
lv_obj_t* obj1 = NULL;

lv_obj_t* label1 = NULL;

lv_obj_t* cont = NULL;
lv_obj_t* tip = NULL;

lv_obj_t* btn1 = NULL;
lv_obj_t* btn2 = NULL;

lv_obj_t* lmeter = NULL;
//lv_layout_t layout = LV_LAYOUT_CENTER; // 起始的布局方式
//lv_fit_t fit = LV_FIT_TIGHT; // 起始的自适应方式

lv_style_t style1;
lv_style_t cont_style;
lv_style_t lmeter_style;

/* 例程入口函数 */
void test_start(void)
{
  scr = lv_scr_act();  // 获取当前屏幕活跃对象
  test_lmeter();
}

void test_lmeter(void)
{
  lv_style_copy(&lmeter_style, &lv_style_plain_color);
  lmeter_style.body.main_color = LV_COLOR_RED; //活跃刻度线的起始颜色
  lmeter_style.body.grad_color = LV_COLOR_GREEN; //活跃刻度线的终止颜色
  lmeter_style.line.color = LV_COLOR_SILVER;//非活跃刻度线的颜色
  lmeter_style.line.width = 2;//每一条刻度线的宽度
  lmeter_style.body.padding.left = 16;//每一条刻度线的长度
  
  lmeter = lv_lmeter_create(scr, NULL);
  lv_obj_set_size(lmeter, 180, 180);
  lv_obj_align(lmeter,NULL,LV_ALIGN_IN_TOP_MID,0,0);//与屏幕保持居中对齐
  lv_lmeter_set_range(lmeter,0,100);//设置进度范围
  lv_lmeter_set_value(lmeter,50);//设置当前的进度值
  lv_lmeter_set_scale(lmeter,150,31);//设置角度和刻度线的数量
  lv_lmeter_set_style(lmeter,LV_LMETER_STYLE_MAIN,&lmeter_style);//设置样式
  
  lv_obj_t * label_soc = lv_label_create(scr,NULL);
  lv_obj_align(label_soc,lmeter,LV_ALIGN_CENTER,0,-25);//设置与 lmeter1 居中对齐
  //使能自动对齐功能,当文本长度发生变化时,它会自动对齐的
  lv_obj_set_auto_realign(label_soc,true);
  char buff[10];
  sprintf(buff,"%d%%",50);
  lv_label_set_text(label_soc,buff);//设置文本
  
}

/**
 * @brief 事件回调函数
 *
 */
void event_handle(lv_obj_t* obj, lv_event_t event)
{
  if (obj == btn1)
  {
    if(event == LV_EVENT_PRESSED)
    {
      printf("btn1\r\n");
    }
  }
  else if(obj == btn2)
  {
    if(event == LV_EVENT_PRESSED)
    {
      printf("btn2\r\n");
    }
  }
}

/**
 * @brief 测试btn相关API接口
 *
 */
static void test_btn(void)
{
  btn1 = lv_btn_create(scr, NULL);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);
  //lv_obj_set_pos(btn, 70, 100);
//  lv_btn_set_ink_in_time(btn, 2000);   // 设置入场动画时长
//  lv_btn_set_ink_wait_time(btn, 3000); // 设置维持等待时长
//  lv_btn_set_ink_out_time(btn, 1000);  // 设置出场动画的时长
  //lv_btn_set_toggle(btn, true);
  lv_obj_set_event_cb(btn1, event_handle);
  
  
  btn2 = lv_btn_create(scr, NULL);
  lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_obj_set_event_cb(btn2, event_handle);
  lv_obj_set_drag(btn2, true);
  
}



/**
 * @brief 测试cont相关API接口
 *
 */
static void test_cont(void)
{
  /* 创建容器并初始化 */
  cont = lv_cont_create(scr, NULL);  // 创建容器
  lv_obj_t* btn1 = lv_btn_create(cont, NULL);  // 往容器内添加子对象1
  lv_obj_t* btn2 = lv_btn_create(cont, NULL);  // 往容器内添加子对象2
  
  lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);  //设置对齐方式
  
  lv_cont_set_layout(cont, LV_LAYOUT_COL_L);  // 容器的布局方式
  lv_cont_set_fit(cont, LV_FIT_TIGHT);          // 设置容器的自适应方式
  //lv_obj_set_size(cont, 100, 100);
  
  lv_style_copy(&cont_style, &lv_style_plain_color);
  cont_style.body.padding.inner = 10;
  cont_style.body.padding.top = 30;
  cont_style.body.padding.left = 20;
  
  lv_cont_set_style(cont, LV_CONT_STYLE_MAIN, &cont_style);
  
  
  
}

/**
 * @brief 测试style相关API接口
 *
 */
static void test_style(void)
{
  obj1 = lv_obj_create(scr, NULL);     // 创建一个对象
  label1 = lv_label_create(scr, NULL); // 创建一个标签
  
  /* 样式设置 */
  lv_style_copy(&style1, &lv_style_plain_color);  // 拷贝样式
  style1.body.main_color = LV_COLOR_RED;
  style1.body.grad_color = LV_COLOR_RED;
//  style1.body.radius = 50;  // 设置圆角半径
  style1.body.opa = 255;    // 设置透明度
//  style1.body.border.color = LV_COLOR_GREEN; // 设置边框颜色
//  style1.body.border.width = 5;              // 设置边框宽度
//  style1.body.border.part = LV_BORDER_TOP;   // 显示哪个边框
  lv_obj_set_style(obj1, &style1);
  
  /* 基础对象设置 */
  lv_obj_set_pos(obj1, 50, 50);
  lv_obj_set_size(obj1, 100, 100);
  
  /* 标签设置 */
  lv_label_set_body_draw(label1, true);  // 使能标签的背景颜色
  lv_label_set_style(label1, LV_LABEL_STYLE_MAIN, &style1);
  lv_label_set_long_mode(label1, LV_LABEL_LONG_SROLL_CIRC);  // 设置文本模式
  lv_label_set_text(label1, "hello pei zhi qiang");
  lv_obj_set_pos(label1, 120, 120);
  lv_obj_set_size(label1, 100, 50); 
}


//lv_label_set_body_draw(label1, true); 要让标签有背景颜色，先使能这句话
//lv_label_set_style(label1, LV_LABEL_STYLE_MAIN, &lv_style_plain_color);
