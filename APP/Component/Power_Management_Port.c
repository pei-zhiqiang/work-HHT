/**
 *  @file Power_Management_Port.c
 *
 *  @date 2021-04-28
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 电源管理接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Power_Management_Port.h"
#include "main.h"
#include "usbd_core.h"
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
  #include "cmsis_armcc.h"
#elif defined(__ICCARM__) || defined(__ICCRX__)/**< IAR方式*/
  #include "cmsis_iccarm.h"
#elif defined(__GNUC__)
  #include "cmsis_gcc.h"
#else
  #error not supported compiler, please use command table mode
#endif
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef hUsbDeviceFS;
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/
extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);
/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   设备进入停机模式
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-17
  ******************************************************************
  */
void Power_Management_Port_Enter_Stop_Mode(void)
{
  /* TODO:初始化引脚为模拟输入 */

  /* 配置外部中断模式-上升沿 */
  HAL_GPIO_DeInit(HS_DET_GPIO_Port, HS_DET_Pin);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = HS_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HS_DET_GPIO_Port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* 关闭LED指示 */
  LED_Toggle_LED(0xFF);

  /* 蓝牙休眠 */
  //Bluetooth_Port_Stop();

  /* 电量计休眠 */
  Voltameter_Port_OnOff_Sleep(true);

  /* 挂起USB */
  USBD_Stop(&hUsbDeviceFS);
  HAL_GPIO_WritePin(USB_PULL_IO_GPIO_Port, USB_PULL_IO_Pin, GPIO_PIN_RESET);

  /* 关闭DAC */
  DAC_Port_DeInit();

  /* 关闭Flash电源 */
  HAL_PWREx_EnableFlashPowerDown();

  /* 进入停机模式 */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* 恢复Flash电源 */
  HAL_PWREx_DisableFlashPowerDown();

  /* 恢复HSE时钟 */
  SystemClock_Config();

  /* 恢复外设时钟 */
  PeriphCommonClock_Config();

  /* 恢复IO配置 */
  if(__HAL_GPIO_EXTI_GET_IT(HS_DET_Pin) != 0x00U)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(HS_DET_Pin);
  }
  HAL_GPIO_DeInit(HS_DET_GPIO_Port, HS_DET_Pin);
  GPIO_InitStruct.Pin = HS_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(HS_DET_GPIO_Port, &GPIO_InitStruct);
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);

  /* 初始化DAC */
  DAC_Port_Init();

  /* 蓝牙唤醒 */
  //Bluetooth_Port_Wakeup();

  /* 电量计唤醒 */
  Voltameter_Port_OnOff_Sleep(false);

  /* 唤醒USB */
  USBD_Start(&hUsbDeviceFS);
  HAL_GPIO_WritePin(USB_PULL_IO_GPIO_Port, USB_PULL_IO_Pin, GPIO_PIN_SET);
}

/**
  ******************************************************************
  * @brief   掉电检测
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-28
  ******************************************************************
  */
void HAL_PWR_PVDCallback(void)
{
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO))
  {
    /*掉电参数检测保存*/
    //Parameter_Port_Start();
  }
}

/**
  ******************************************************************
  * @brief   设备复位
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-28
  ******************************************************************
  */
void PowerManagement_Port_Restart(void)
{
  /* 停止音频输出 */
  SAI_Port_Pause_Resume_Process(true);

  /* Wait */
  HAL_Delay(1000);

  /*关闭全局中断*/
  __set_FAULTMASK(1);

  /*系统复位*/
  NVIC_SystemReset();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
