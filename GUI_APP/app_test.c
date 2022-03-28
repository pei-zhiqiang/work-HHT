#include "app_test.h"
#include "lvgl.h"
#include "stdio.h"

lv_style_t my_style;
lv_obj_t * label1;

void event_handler(lv_obj_t * obj, lv_event_t event)
{
  if(obj==label1 && event==LV_EVENT_RELEASED)
  {
    printf("hello\r\n");
    my_style.text.color = LV_COLOR_RED;
    lv_obj_refresh_style(label1);
  }
}

/* 例程入口函数 */
void test_start(void)
{
  lv_obj_t * scr = lv_scr_act();
  
  lv_style_copy(&my_style, &lv_style_plain_color);
  
  
  
  label1 = lv_label_create(scr, NULL);
  lv_label_set_long_mode(label1, LV_LABEL_LONG_CROP);
  lv_obj_set_pos(label1, 50, 50);
  lv_obj_set_size(label1, 150,50);
  lv_label_set_text(label1,"I am pzq");
  lv_label_set_body_draw(label1, true);
  lv_label_set_style(label1, LV_LABEL_STYLE_MAIN,  &my_style);
  
  lv_obj_set_event_cb(label1, event_handler);
  lv_obj_set_click(label1,true);
   
  
}
