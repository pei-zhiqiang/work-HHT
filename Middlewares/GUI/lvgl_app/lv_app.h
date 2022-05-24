/**
 *  @file lv_app.h
 *
 *  @date 2022-04-22
 *
 *  @author PZQ
 *
 *  @brief  UI
 *  
 *  @version V1.0
 */
#ifndef __LV_APP_H__
#define __LV_APP_H__
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
#include "lvgl.h"
/** Private defines ----------------------------------------------------------*/
#define LOGO_LEVEL         0
#define MIAN_LEVEL         0
#define SET_LEVEL          1
#define VOL_LEVEL          2
#define NS_LEVEL           2
#define BF_LEVEL           2
#define MORE_SET_LEVEL     2
#define BRIGHTNESS_LEVEL   3
#define MODE_LEVEL         3
#define RESET_LEVEL        3
#define BLE_LEVEL          3
#define TEST_LEVEL         4
#define TEST_EAR_LEVEL     5
#define TEST_CHARGE_LEVEL  5
#define TEST_LCD_LEVEL     5
#define TEST_GYRO_LEVEL    5
#define TEST_TP_LEVEL      5
#define TEST_FLASH_LEVEL   5
#define TEST_MIC_LEVEL     5
#define TEST_INSPECT_LEVEL 5
/** Exported typedefines -----------------------------------------------------*/
/* 界面个数,并表示处于哪一个界面 */
typedef enum
{
  LOGO_IDX = 0,    /**< 开机界面 */
  MAIN_IDX,        /**< 主界面 */
  SET_IDX,         /**< 设置界面 */
  VOL_IDX,         /**< 音量设置界面 */
  NS_IDX,          /**< 降噪设置界面 */
  BF_IDX,          /**< BF设置界面 */
  MORE_SET_IDX,    /**< 更多设置界面 */
  BRIGHTNESS_IDX,  /**< 亮度调节界面 */
  MODE_IDX,        /**< 模式选择界面 */
  RESET_IDX,       /**< 恢复出厂设置界面 */
  BLE_IDX,         /**< 蓝牙设置界面 */
  TEST_IDX,        /**< 测试界面 */
  TEST_EAR_IDX,    /**< 耳机功能测试界面 */
  TEST_CHARGE_IDX, /**< 充电功能测试界面 */
  TEST_LCD_IDX,    /**< LCD测试界面 */
  TEST_GYRO_IDX,   /**< 陀螺仪测试界面 */
  TEST_TP_IDX,     /**< 触摸测试界面 */
  TEST_FLASH_IDX,  /**< FLASH测试界面 */
  TEST_MIC_IDX,    /**< 麦克风测试界面 */
  TEST_INSPECT_IDX,/**< 进入送检功能 */
  _MAX_IDX,
}UI_IDX_Typedef_t;

typedef void (*ui_func)(void);

/* 记录该界面的一些信息 */
typedef struct
{
  uint8_t ui_idx;          /**< 用于表示当前的所处界面 */
  uint8_t ui_level;        /**< 表示当前界面等级 */
  ui_func current_ui_func; /**< 进入该界面的函数 */
  uint8_t ui_prior_level;     /**< 上一级界面 */ 
}UI_Inf_Typedef_t;
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
extern lv_obj_t* scr;
extern lv_obj_t* cont_mode;
extern UI_Inf_Typedef_t *Current_UI_Inf;
extern UI_Inf_Typedef_t UI_Base_Inf[_MAX_IDX];
extern int8_t Vol_R;              /** < 用于获取右耳音量 */
extern int8_t Vol_L;              /** < 用于获取左耳音量 */
extern lv_obj_t* main_bar_l;
extern lv_obj_t* main_bar_r;
/** Exported functions prototypes --------------------------------------------*/
/* 界面显示 */
void HHT_UI_Init(void);
/* 触摸键返回功能 */
void HHT_Touch_Back(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
