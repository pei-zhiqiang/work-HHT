/**
 *  @file BT8829_Signal_Port.c
 *
 *  @date 2021-02-7
 *
 *  @author zgl
 *
 *  @copyright None.
 *
 *  @brief
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
#include "BT8829_Signal_Port.h"
#include "main.h"
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define ELECTRIC_Delay_LOW     20
#define ELECTRIC_Delay_HIGH    30

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
Elec_Opt_Typedef_t  Electric_Opt =
{
	 .Elec_Stat = Elec_STOP,
};

Ble_Connect_Stat_Typedef_t Ble_Connect_Stat;
Ble_Connect_Stat_Typedef_t Pre_Ble_Connect_Stat;
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
  * @brief   读取BT8922蓝牙连接状态
  * @param   [in]None
  * @return  0 已触发.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
static uint8_t Ble_Connect_State(void)
{
  if(HAL_GPIO_ReadPin(BT_CONNECT_STAT_GPIO_Port, BT_CONNECT_STAT_Pin) == GPIO_PIN_RESET)
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
void elec_signal_switch(void)
{
  switch(Electric_Opt.Elec_Stat)
  {
    case Elec_Fst_LOW:
      if(Electric_Opt.Elec_Delay < ELECTRIC_Delay_LOW)
      {
        Electric_Opt.Elec_Delay++;
      }
      else
      {
        Electric_Opt.Elec_Stat = Elec_Fst_HIGH;
        Electric_Opt.Elec_Delay = 0;
        POWER_KEY_HIGH;
      }
      break;
    case Elec_Fst_HIGH:
      if(Electric_Opt.Elec_Delay < ELECTRIC_Delay_HIGH)
      {
        Electric_Opt.Elec_Delay++;
      }
      else
      {
        Electric_Opt.Elec_Stat = Elec_Sec_LOW;
        Electric_Opt.Elec_Delay = 0;
        POWER_KEY_LOW;
      }
      break;
    case Elec_Sec_LOW:
      if(Electric_Opt.Elec_Delay < ELECTRIC_Delay_LOW)
      {
          Electric_Opt.Elec_Delay++;
      }
      else
      {
        Electric_Opt.Elec_Stat = Elec_STOP;
        Electric_Opt.Elec_Delay = 0;
        POWER_KEY_HIGH;
      }
      break;
    case Elec_STOP:
      break;
  }
}

void elec_signal_start(void)
{
  Electric_Opt.Elec_Stat = Elec_Fst_LOW;
  Electric_Opt.Elec_Delay = 0;
  POWER_KEY_LOW;
}

Ble_Connect_Stat_Typedef_t Ble_Connect_Check(void)
{
  if(Ble_Connect_State() == 1)
  {
    return Ble_Connected;
  }
  else
  {
    return Ble_Disconnected;
  }
}

void Ble_Dev_Init(void)
{
  if(Ble_Connect_State() == 1)
  {
    Pre_Ble_Connect_Stat = Ble_Connect_Stat = Ble_Connected;
  }
  else
  {
    Pre_Ble_Connect_Stat = Ble_Connect_Stat = Ble_Disconnected;
  }
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
