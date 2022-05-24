/**
 *  @file PressKey_Port.c
 *
 *  @date 2021-04-19
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 按键接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
#include "PressKey_Port.h"
#include "Utilities_Multi_Button.h"
#include "utilities.h"
#include "Algorithm_Port.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/
/* 按键句柄 */
static struct Button Power_Key_Handle;
static struct Button Denoise_Key_Handle;
static struct Button Vol_Decrease_Key_Handle;
static struct Button Vol_Increase_Key_Handle;
static struct Button HSDet_Key_Handle;

static int8_t VC_CH1_Level = 0;
static int8_t VC_CH2_Level = 0;
/** Private function prototypes ----------------------------------------------*/

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
  * @brief   设置降噪等级按键触发增低中高：0-70-100
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static void Denoise_Key_SINGLE_Click_Handler(void* btn)
{
  UNUSED(btn);
  
  /*三档切换*/
  static int Denoise_Level = 100;
  switch(Denoise_Level)
  {
    case 100:
      Denoise_Level = 70;
      Algorithm_Port_Set_DENOISE_Par(3);
      break;
    case 70:
      Denoise_Level = 0;
      Algorithm_Port_Set_DENOISE_Par(2);
      break;
    case 0:
    default:
      Denoise_Level = 100;
      Algorithm_Port_Set_DENOISE_Par(1);
      break;
  }
}

/**
  ******************************************************************
  * @brief   手动退出线性补偿模式
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static void Denoise_Key_DOUBLE_Click_Handler(void* btn)
{
  UNUSED(btn);
}

/**
  ******************************************************************
  * @brief   设置进入线性补偿模式
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-21
  ******************************************************************
  */
static void Denoise_Key_LONG_Click_Handler(void* btn)
{
  UNUSED(btn);
}

/**
  ******************************************************************
  * @brief   设置音量减按键触发
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-17
  ******************************************************************
  */
static void Vol_Decrease_Key_SINGLE_Click_Handler(void* btn)
{
  UNUSED(btn);

  Algorithm_Port_Get_VC_Level(&VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Get_VC_Level(&VC_CH2_Level, ALGORITHM_CHANNEL_CH2);
  VC_CH1_Level = VC_CH1_Level - 1;
  VC_CH1_Level = VC_CH1_Level <= 0 ? 0 : VC_CH1_Level;
  VC_CH2_Level = VC_CH2_Level - 1;
  VC_CH2_Level = VC_CH2_Level <= 0 ? 0 : VC_CH2_Level;
  Algorithm_Port_Set_VC_Level(VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Set_VC_Level(VC_CH2_Level, ALGORITHM_CHANNEL_CH2);

  /*使用硬件调节*/
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation - 10;
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation <= 0?0:g_Miscellaneous_Par.Left_Channel_Compensation;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation - 10;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation <= 0?0:g_Miscellaneous_Par.Right_Channel_Compensation;
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LEFT, (float)g_Miscellaneous_Par.Left_Channel_Compensation);
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_RIGHT, (float)g_Miscellaneous_Par.Right_Channel_Compensation);
}

/**
  ******************************************************************
  * @brief   设置音量增按键触发增
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-17
  ******************************************************************
  */
static void Vol_Increase_Key_SINGLE_Click_Handler(void* btn)
{
  UNUSED(btn);

  Algorithm_Port_Get_VC_Level(&VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Get_VC_Level(&VC_CH2_Level, ALGORITHM_CHANNEL_CH2);
  VC_CH1_Level = VC_CH1_Level + 1;
  VC_CH1_Level = VC_CH1_Level >= 7 ? 7 : VC_CH1_Level;
  VC_CH2_Level = VC_CH2_Level + 1;
  VC_CH2_Level = VC_CH2_Level >= 7 ? 7 : VC_CH2_Level;
  Algorithm_Port_Set_VC_Level(VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Set_VC_Level(VC_CH2_Level, ALGORITHM_CHANNEL_CH2);

  /*使用硬件调节*/
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation + 10;
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation >= 100?100:g_Miscellaneous_Par.Left_Channel_Compensation;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation + 10;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation >= 100?100:g_Miscellaneous_Par.Right_Channel_Compensation;
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LEFT, (float)g_Miscellaneous_Par.Left_Channel_Compensation);
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_RIGHT, (float)g_Miscellaneous_Par.Right_Channel_Compensation);
}


/**
  ******************************************************************
  * @brief   设置音量减按键触发-双击
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-07
  ******************************************************************
  */
static void Vol_Decrease_Key_DOUBLE_Click_Handler(void* btn)
{
  UNUSED(btn);
  
  Algorithm_Port_Get_VC_Level(&VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Get_VC_Level(&VC_CH2_Level, ALGORITHM_CHANNEL_CH2);
  VC_CH1_Level = VC_CH1_Level - 1;
  VC_CH1_Level = VC_CH1_Level <= 0?0:VC_CH1_Level;
  VC_CH2_Level = VC_CH2_Level - 1;
  VC_CH2_Level = VC_CH2_Level <= 0?0:VC_CH2_Level;
  Algorithm_Port_Set_VC_Level(VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Set_VC_Level(VC_CH2_Level, ALGORITHM_CHANNEL_CH2);

//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation - 10;
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation <= 0?0:g_Miscellaneous_Par.Left_Channel_Compensation;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation - 10;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation <= 0?0:g_Miscellaneous_Par.Right_Channel_Compensation;
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LEFT, (float)g_Miscellaneous_Par.Left_Channel_Compensation);
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_RIGHT, (float)g_Miscellaneous_Par.Right_Channel_Compensation);
}

/**
  ******************************************************************
  * @brief   设置音量增按键触发增-双击
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-07
  ******************************************************************
  */
static void Vol_Increase_Key_DOUBLE_Click_Handler(void* btn)
{
  UNUSED(btn);
 
  Algorithm_Port_Get_VC_Level(&VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Get_VC_Level(&VC_CH2_Level, ALGORITHM_CHANNEL_CH2);
  VC_CH1_Level = VC_CH1_Level + 1;
  VC_CH1_Level = VC_CH1_Level >= 7 ? 7 : VC_CH1_Level;
  VC_CH2_Level = VC_CH2_Level + 1;
  VC_CH2_Level = VC_CH2_Level >= 7 ? 7 : VC_CH2_Level;
  Algorithm_Port_Set_VC_Level(VC_CH1_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Set_VC_Level(VC_CH2_Level, ALGORITHM_CHANNEL_CH2);

  /*使用硬件调节*/
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation + 10;
//  g_Miscellaneous_Par.Left_Channel_Compensation = g_Miscellaneous_Par.Left_Channel_Compensation >= 100?100:g_Miscellaneous_Par.Left_Channel_Compensation;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation + 10;
//  g_Miscellaneous_Par.Right_Channel_Compensation = g_Miscellaneous_Par.Right_Channel_Compensation >= 100?100:g_Miscellaneous_Par.Right_Channel_Compensation;
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LEFT, (float)g_Miscellaneous_Par.Left_Channel_Compensation);
//  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_RIGHT, (float)g_Miscellaneous_Par.Right_Channel_Compensation);
}

/**
  ******************************************************************
  * @brief   设置禁触按键触发
  * @param   [in]btn 按键句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-17
  ******************************************************************
  */
static void Power_Key_SINGLE_Click_Handler(void* btn)
{
  UNUSED(btn);
  /* 禁止触发按键功能-关机按键除外 */
 button_stop(&Denoise_Key_Handle);
 button_stop(&Vol_Decrease_Key_Handle);
 button_stop(&Vol_Increase_Key_Handle);
}

/**
  ******************************************************************
  * @brief   设置允许按键触发
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-17
  ******************************************************************
  */
static void Power_Key_DOUBLE_Click_Handler(void* btn)
{
  UNUSED(btn);
  /* 允许触发按键功能 */
 button_start(&Denoise_Key_Handle);
 button_start(&Vol_Decrease_Key_Handle);
 button_start(&Vol_Increase_Key_Handle);
}

/**
  ******************************************************************
  * @brief   设置开关电源按键触发
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-17
  ******************************************************************
  */
static void Power_Key_LONG_PRESS_START_Handler(void* btn)
{
  UNUSED(btn);
  /*进入停机模式*/
  Power_Management_Port_Enter_Stop_Mode();
}

/**
  ******************************************************************
  * @brief   设置耳机检测触发
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-19
  ******************************************************************
  */
static void HSDet_Key_LONG_PRESS_START_Handler(void* btn)
{
  UNUSED(btn);
  /*进入停机模式*/
  Power_Management_Port_Enter_Stop_Mode();
}

/**
  ******************************************************************
  * @brief   读取开关按键状态
  * @param   [in]None
  * @return  0 已触发.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static uint8_t Read_Power_Key_State(void)
{
  // if(HAL_GPIO_ReadPin(POWER_KEY_GPIO_Port, POWER_KEY_Pin) == GPIO_PIN_RESET)
  // {
  //   return 0;
  // }
  // return 1;
  return 1;
}

/**
  ******************************************************************
  * @brief   读取降噪按键状态
  * @param   [in]None
  * @return  0 已触发.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static uint8_t Read_Denoise_Key_State(void)
{
  // if(HAL_GPIO_ReadPin(DENOISE_LEVEL_CHANGE_KEY_GPIO_Port, DENOISE_LEVEL_CHANGE_KEY_Pin) == GPIO_PIN_RESET)
  // {
  //   return 0;
  // }
  // return 1;
  return 1;
}

/**
  ******************************************************************
  * @brief   读取音量增按键状态
  * @param   [in]None
  * @return  0 已触发.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static uint8_t Read_Vol_Increase_Key_State(void)
{
  // if(HAL_GPIO_ReadPin(VOL_INCREASE_KEY_GPIO_Port, VOL_INCREASE_KEY_Pin) == GPIO_PIN_RESET)
  // {
  //   return 0;
  // }
  // return 1;
  return 1;
}

/**
  ******************************************************************
  * @brief   读取音量减按键状态
  * @param   [in]None
  * @return  0 已触发.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static uint8_t Read_Vol_Decrease_Key_State(void)
{
  // if(HAL_GPIO_ReadPin(VOL_DECREASE_KEY_GPIO_Port, VOL_DECREASE_KEY_Pin) == GPIO_PIN_RESET)
  // {
  //   return 0;
  // }
  // return 1;
  return 1;
}

/**
  ******************************************************************
  * @brief   读取耳机检测按键状态
  * @param   [in]None
  * @return  0 已触发.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-19
  ******************************************************************
  */
static uint8_t Read_HSDet_Key_State(void)
{
  if(HAL_GPIO_ReadPin(HS_DET_GPIO_Port, HS_DET_Pin) == GPIO_PIN_RESET)
  {
    return 0;
  }
  return 1;
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
  * @brief   按键检测事件调试
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-25
  ******************************************************************
  */
#if USE_DEBUG_PRESSKEY
void PressKey_Port_Start(void)
{
	switch(get_button_event(&HSDet_Key_Handle))
  {
    case PRESS_DOWN:/**< 按键按下，每次按下都触发*/
      printf("PRESS_DOWN.\r\n");
      break;
    case PRESS_UP:/**< 按键弹起，每次松开都触发*/
      printf("PRESS_UP.\r\n");
      break;
    case PRESS_REPEAT:/**< 重复按下触发，变量repeat计数连击次数*/
      printf("PRESS_REPEAT.\r\n");
      break;
    case SINGLE_CLICK:/**< 单击按键事件*/
      printf("SINGLE_CLICK.\r\n");
      break;
    case DOUBLE_CLICK:/**< 双击按键事件*/
      printf("DOUBLE_CLICK.\r\n");
      break;
    case LONG_PRESS_START:/**< 达到长按时间阈值时触发一次*/
      printf("LONG_PRESS_START.\r\n");
      break;
    case LONG_PRESS_HOLD:/**< 长按期间一直触发*/
      printf("LONG_PRESS_HOLD.\r\n");
      break;
    default:
      break;
  }
  if(Read_HSDet_Key_State() == 0)
  {
    printf("0");
  }
  else
  {
    printf("1");
  }
}
#endif
/**
  ******************************************************************
  * @brief   按键检测初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-10
  ******************************************************************
  */
void PressKey_Port_Init(void)
{
  /* 初始化按键句柄,设定激活状态值0 */
  button_init(&Power_Key_Handle, Read_Power_Key_State, 0);
  button_init(&Denoise_Key_Handle, Read_Denoise_Key_State, 0);
  button_init(&Vol_Decrease_Key_Handle, Read_Vol_Decrease_Key_State, 0);
  button_init(&Vol_Increase_Key_Handle, Read_Vol_Increase_Key_State, 0);
  button_init(&HSDet_Key_Handle, Read_HSDet_Key_State, 0);

  /* 注册触发事件 */
  button_attach(&Denoise_Key_Handle, SINGLE_CLICK, Denoise_Key_SINGLE_Click_Handler);
  button_attach(&Denoise_Key_Handle, DOUBLE_CLICK, Denoise_Key_DOUBLE_Click_Handler);
  button_attach(&Denoise_Key_Handle, LONG_PRESS_START, Denoise_Key_LONG_Click_Handler);

  button_attach(&Vol_Decrease_Key_Handle, SINGLE_CLICK, Vol_Decrease_Key_SINGLE_Click_Handler);
  button_attach(&Vol_Increase_Key_Handle, SINGLE_CLICK, Vol_Increase_Key_SINGLE_Click_Handler);
  button_attach(&Vol_Decrease_Key_Handle, DOUBLE_CLICK, Vol_Decrease_Key_DOUBLE_Click_Handler);
  button_attach(&Vol_Increase_Key_Handle, DOUBLE_CLICK, Vol_Increase_Key_DOUBLE_Click_Handler);

  button_attach(&Power_Key_Handle, SINGLE_CLICK, Power_Key_SINGLE_Click_Handler);
  button_attach(&Power_Key_Handle, DOUBLE_CLICK, Power_Key_DOUBLE_Click_Handler);
  button_attach(&Power_Key_Handle, LONG_PRESS_START, Power_Key_LONG_PRESS_START_Handler);

  button_attach(&HSDet_Key_Handle, LONG_PRESS_START, HSDet_Key_LONG_PRESS_START_Handler);

  /* 使能 */
  // button_start(&Denoise_Key_Handle);
  // button_start(&Vol_Decrease_Key_Handle);
  // button_start(&Vol_Increase_Key_Handle);
  // button_start(&Power_Key_Handle);

  //button_start(&HSDet_Key_Handle);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
