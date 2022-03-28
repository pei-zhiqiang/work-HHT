#ifndef __CHSC6X_MAIN_H__
#define __CHSC6X_MAIN_H__

#include "stdint.h"


#define TP_PRES_DOWN  0x02
#define TP_PRES_UP    0x01

struct ts_event {
    unsigned short x; /*x coordinate */
    unsigned short y; /*y coordinate */
    int flag; /* touch event flag: 0 -- down; 1-- up; 2 -- contact */
    int id;   /*touch ID */
};

typedef struct
{
  uint16_t x;
	uint16_t y;
	uint16_t last_x;
	uint16_t last_y;
  uint8_t  sta;
}TP_Dev_t;  

extern TP_Dev_t TP_Dev;

/* 获取触摸信息 */
void chsc6x_read_touch_info(void);
/* 复位 */
void chsc6x_resume(void);
/* 暂停触摸屏功能 */
void chsc6x_suspend(void);
/* 触摸屏初始化 */ 
void chsc6x_init(void);
#endif
