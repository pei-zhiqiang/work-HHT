/**
 * @file lv_port_indev.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "chsc6x_main.h" 
#include "stdio.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);



/**********************
 *  STATIC VARIABLES
 **********************/



/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/* lvgl的输入设备的初始化 */
void lv_port_indev_init(void)
{
	lv_indev_drv_t indev_drv;

	/*------------------
	* Touchpad
	* -----------------*/

	/*Register a touchpad input device*/
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = touchpad_read;
	lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/* Will be called by the library to read the touchpad */
/* 将会被lvgl周期性调用，周期值是通过lv_conf.h中的 LV_INDEV_DEF_READ_PERIOD宏来定义
* 此值不要设置的太大，否则会感觉触摸不灵敏，默认值为30ms
 */
static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	//data->point.x = 360 -TP_Dev.x;
	//data->point.y = 360 - TP_Dev.y;
	data->point.x = TP_Dev.x;
	data->point.y = TP_Dev.y;
  
  //printf("TP_Dev.sta = 0x%02x", TP_Dev.sta);  
  //printf("data->point.x = %d, data->point.y = %d\r\n", data->point.x, data->point.y);
  
#if 0
	if(TP_Dev.sta & TP_PRES_DOWN)
	{
		data->state = LV_INDEV_STATE_PR;
		if(TP_Dev.y == 400)
		{
			if(TP_Dev.x == 80)
			{
				TP_Key_Type = TP_KEY_VOL_UP;
			}
			else if(TP_Dev.x == 160)
			{
				TP_Key_Type = TP_KEY_VOL_DOWN;
			}
			else if(TP_Dev.x == 120)
			{
				TP_Key_Type = TP_KEY_BACK;
			}
		}
	}
	else
	{
		data->state = LV_INDEV_STATE_REL;
	}
#else
	if(TP_Dev.sta & TP_PRES_DOWN)
	{
		data->state = LV_INDEV_STATE_PR;
		TP_Dev.sta = TP_PRES_UP;
		if(TP_Dev.y == 400)
		{
			if(TP_Dev.x == 80)
			{
        printf("TP_KEY_VOL_UP\r\n");
				//TP_Key_Type = TP_KEY_VOL_UP;
			}
			else if(TP_Dev.x == 160)
			{
        printf("TP_KEY_VOL_DOWN\r\n");
				//TP_Key_Type = TP_KEY_VOL_DOWN;
			}
			else if(TP_Dev.x == 120)
			{
        printf("TP_KEY_BACK\r\n");
				//TP_Key_Type = TP_KEY_BACK;
			}
		}
	}
	else 
	{
		data->state = LV_INDEV_STATE_REL;
	}
#endif
	/*Return `false` because we are not buffering and no more data to read*/
	return false;
}





#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
