/**
 *  @file Timer_Port.c
 *
 *  @date 2021-01-01
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 定时任务接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Parameter_Port.h"
#include "Algorithm_Port.h"
#include "Timer_Port.h"
#include "Update_Port.h"
#include "Voltameter_Port.h"
#include "ADC_Port.h"
#include "Gyroscope_Port.h"
#include "Utilities_Multi_Timer.h"
#include "Utilities_Multi_Button.h"
#include "PressKey_Port.h"
#include "main.h"
#include "lvgl.h"
#include "lv_app.h"
#include "BT8829_Signal_Port.h"
#include "RTC_Port.h"

/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define ENABLE_SYSTICK_COUNT  0/**< 是否启用Systick计数*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
/** Private variables --------------------------------------------------------*/
static struct Timer timer1;
static struct Timer timer2;
static struct Timer timer3;
static struct Timer timer4;
static struct Timer timer5;
static uint32_t Timer_Port_TimeMS  = 0;
static uint32_t Timer_Port_TimeSec = 0;
/** Private function prototypes ----------------------------------------------*/
static inline void Timer_Port_IRQHandler(void);
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   定时器中断回调
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
static inline void Timer_Port_IRQHandler(void)
{
  Timer_Port_TimeMS++;
  if(Timer_Port_TimeMS == 1000)
  {
    Timer_Port_TimeMS = 0;
    Timer_Port_TimeSec++;
  }
  timer_ticks(); ///<! 1ms ticks

  /* littleVGL 提供心跳节拍 1ms */
  lv_tick_inc(1);
}

/**
  ******************************************************************
  * @brief   定时1任务
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-25
  ******************************************************************
  */
static void timer1_callback(void)
{
  /*电量信息检测*/
  Voltameter_Port_Update_Battery_Electric_Quantity();

  /*ADC采样启动*/
  //ADC_Port_Start();

  /*读取陀螺仪设备数值*/
  Gyroscope_Port_Start();
}

/**
  ******************************************************************
  * @brief   延时任务
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2020-02-25
  ******************************************************************
  */
static void timer2_callback(void)
{
  /*设备初始化完成*/
  g_Miscellaneous_Par.Meter_Device_Is_Init = 1;

  /*更新接口初始化*/
  Update_Port_Init();
}

/**
  ******************************************************************
  * @brief   定时3任务
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static void timer3_callback(void)
{
  /* 按键检测时基 */
  button_ticks();
  /* 启动触摸检测 */
  TP_Port_Detect_Start();
  /* 控制蓝牙配对 */
  elec_signal_switch(); 
}

/**
  ******************************************************************
  * @brief   定时4任务
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-10
  ******************************************************************
  */
static void timer4_callback(void)
{
  /*BF当前角度指示*/

#if USE_DEBUG_PRESSKEY
  /*按键检测事件调试*/
  PressKey_Port_Start();
#endif
}

/**
  ******************************************************************
  * @brief   定时5任务
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-08-10
  ******************************************************************
  */
static void timer5_callback(void)
{
  /*参数检测随存*/
  Parameter_Port_Start();
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   定时器周期时间到回调
  * @param   [in]htim 定时器句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-25
  ******************************************************************
  */
#if !ENABLE_SYSTICK_COUNT
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM3)
  {
    Timer_Port_IRQHandler();
  }
  else if(htim->Instance == TIM4)
  {
    /* LCD背光亮度调节 */
    LCD_BL_Adjust_Stat_Switch();
  }
}
#endif
/**
  ******************************************************************
  * @brief   滴答时钟回调
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2020-02-25
  ******************************************************************
  */
void User_HAL_SYSTICK_Callback(void)
{
#if ENABLE_SYSTICK_COUNT
  Timer_Port_IRQHandler();
#endif
}

/**
  ******************************************************************
  * @brief   获取当前运行累计时间
  * @param   [in]Cs 使能状态
  * @param   [in]Time_MS 周期时间
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-08-10
  ******************************************************************
  */
void Timer_Port_Set_Tim5(bool Cs, uint32_t Time_MS)
{
  if(Cs == false)
  {
    timer_stop(&timer5);
    return;
  }
  timer_init(&timer5, timer5_callback, 100, Time_MS);
  timer_start(&timer5);
}

/**
  ******************************************************************
  * @brief   获取当前运行累计时间
  * @param   [in]time_base 单位
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
uint32_t Timer_Port_Get_Current_Time(TIMER_TIME_UNIT_Typedef_t time_unit)
{
  return (time_unit == TIMER_MS)?Timer_Port_TimeMS:Timer_Port_TimeSec;
}

/**
  ******************************************************************
  * @brief   定时器接口启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
void Timer_Port_Start(void)
{
  timer_loop();
}

/**
  ******************************************************************
  * @brief   定时器初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
void Timer_Port_Init(void)
{
  /*初始化定时任务 5s loop*/
	timer_init(&timer1, timer1_callback, 1000, 5000);
	timer_start(&timer1);

  /* 初始化延时任务 50ms delay once */
	timer_init(&timer2, timer2_callback, 50, 0);
	timer_start(&timer2);

  /* 初始化定时任务 1s delay 5ms loop */
	timer_init(&timer3, timer3_callback, 1000, 5);
	timer_start(&timer3);

  /* 初始化定时任务 50ms delay 50ms loop */
	timer_init(&timer4, timer4_callback, 1000, 50);
	timer_start(&timer4);

  /* 初始化定时任务 10s delay 1s loop */
	timer_init(&timer5, timer5_callback, 10000, 1000);
	timer_stop(&timer5);

  /* 启动定时器 */
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);
}

/******************************** End of file *********************************/
