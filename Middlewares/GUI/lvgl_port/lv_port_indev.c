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
#include "TP_Port.h"
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
static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
  /* 获取最新触摸坐标 */
  TP_Port_Get_Coordinate((uint16_t *)&data->point.x, (uint16_t *)&data->point.y);

  //data->point.x = 360 -TP_Dev.x;
  //data->point.y = 360 - TP_Dev.y;

 //printf("TP_Dev.x = %d, TP_Dev.y = %d.\n", data->point.x, data->point.y);

  if(TP_Port_Is_Pressed() == true)
  {
    data->state = LV_INDEV_STATE_PR;
    if(data->point.y == 0x01f4)
    {
      if(data->point.x == 0x64)
      {
        TP_Port_Set_Key_Type(TP_KEY_VOL_UP);
      }
      else if(data->point.x == 0x1f4)
      {
        TP_Port_Set_Key_Type(TP_KEY_VOL_DOWN);
      }
      else if(data->point.x == 0x12c)
      {
        TP_Port_Set_Key_Type(TP_KEY_BACK);
      }
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }

  /* Return `false` because we are not buffering and no more data to read */
  return false;
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
