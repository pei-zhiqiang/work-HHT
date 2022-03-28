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
#include "main.h"
#include "lvgl.h"
#include "tim.h"
#include "Utilities_Multi_Timer.h"
//#include "Utilities_Multi_Button.h"
//#include "Gyroscope_Port.h"
//#include "BT8829_Signal_Port.h"
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
#define ENABLE_SYSTICK_COUNT  0/**< 是否启用Systick计数*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static struct Timer timer1;
static struct Timer timer2;
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
	/*读取陀螺仪设备数值*/
	//Gyroscope_Port_Start();
//	Voltameter_Port_Update_Battery_Electric_Quantity();
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
	/*按键检测时基*/
//	TP_Key_Detect_Start();
//	inf_display_ui_updata();
//	ble_ui_updata();
//	elec_signal_switch();
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
  /* Prevent unused argument(s) compilation warning */
	//添加回调后的程序逻辑　
	if (htim->Instance == htim3.Instance)
	{
		//LCD_BL_Adjust_Stat_Switch();
	}
	else if (htim->Instance == htim4.Instance) //判断是否定时器4
	{
      lv_tick_inc(1); //LCD接口初始化
		Timer_Port_IRQHandler();
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
void HAL_SYSTICK_Callback(void)
{
#if ENABLE_SYSTICK_COUNT
  Timer_Port_IRQHandler();
#endif
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
//uint32_t Timer_Port_Get_Current_Time(TIMER_TIME_UNIT_Typedef_t time_unit)
//{
//   return (time_unit == TIMER_MS)?Timer_Port_TimeMS:Timer_Port_TimeSec;
//}

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
	
  /*初始化延时任务 1s delay*/
	timer_init(&timer2, timer2_callback, 1000, 5);
	timer_start(&timer2);
	
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
}

/******************************** End of file *********************************/
