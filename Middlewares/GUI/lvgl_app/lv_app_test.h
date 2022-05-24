/**
 *  @file lv_app_test.h
 *
 *  @date 2022-05-07
 *
 *  @author PZQ
 *
 *  @brief 用于测试触摸屏的好坏和板子上一些硬件功能的好坏
 *  
 *  @version V1.0
 */
#ifndef __LV_APP_TEST_H_
#define __LV_APP_TEST_H_
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private includes ---------------------------------------------------------*/
#include "lv_app.h"
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
extern lv_obj_t* cont_test;
extern lv_obj_t* cont_tp;
extern lv_obj_t* tp_label1;
/** Exported functions prototypes --------------------------------------------*/
/* 测试板子硬件功能选择UI */
void lv_test_ui(void);
/* 耳机功能测试界面 */
void enter_ear_test_ui(void);
/* 充电功能测试界面 */
void enter_charge_test_ui(void);
/* LCD功能测试界面 */
void enter_lcd_test_ui(void);
/* 陀螺仪功能测试界面 */
void enter_gyro_test_ui(void);
/* 触摸测试界面 */
void enter_tp_test_ui(void);
/* Flash测试界面 */
void enter_flash_test_ui(void);
/* 麦克风测试界面 */
void enter_mic_test_ui(void);
/* 送检功能 */
void enter_inspect_test_ui(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
