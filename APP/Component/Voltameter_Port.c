/**
 *  @file Voltameter_Port.c
 *
 *  @date 2021-03-04
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 电池电量监控接口
 *
 *  @details 1、提供应用层获取电量、电压、充电状态等信息
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Voltameter_Port.h"
#include "MAX17048_Driver.h"
#include "Timer_Port.h"
#include "main.h"
/** Private typedef ----------------------------------------------------------*/
/*充电耗时记录*/
typedef struct rectification
{
  uint32_t Start_Time_Sec;
  // uint32_t End_Time_Sec;
  // uint32_t Next_Time_Sec;
  // uint32_t Elapsed;
  uint16_t Compensation_Soc;
}RECTIFICATION_Typedef_t;
/** Private macros -----------------------------------------------------------*/
/*设备参考实测功率*/
#define DEVICE_REF_POWER        150   /**< 155mA/h*/
#define BATTERY_CAPACITY        1000  /**< 1600mAh*/
#define CAN_USE_MIN_TOTAL       (BATTERY_CAPACITY*60/DEVICE_REF_POWER)
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_i2c1_rx;
/** Private variables --------------------------------------------------------*/
static uint16_t Battery_Soc     = 0;/**< 电池电量百分比*/
static uint16_t Battery_Voltage = 0;/**< 电池电压*/

/*电量时间补偿记录*/
static RECTIFICATION_Typedef_t E_Record[11] = {0};
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
  * @brief   修正补偿MAX17048 SOC(Battery state of charge)电量数值
  * @param   [in]Current_Soc 当前电量数值
  * @return  修正后电量数据
  * @author  aron566
  * @version V1.0
  * @date    2021-04-22
  ******************************************************************
  */
static uint16_t Soc_Compensation(uint16_t Current_Soc)
{
  static uint16_t last_soc_val = 0;
  uint16_t range_num = Current_Soc - 90;

  /*无需补偿*/
  if(Current_Soc == 100)
  {
    return Current_Soc;
  }

  /*充电中进行补偿运算*/
  if(last_soc_val != Current_Soc && Current_Soc >= 90 && MAX17048_Driver_Get_Battery_Charge_State() == 1)
  {
    E_Record[range_num].Start_Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);
    E_Record[range_num].Compensation_Soc = Current_Soc;
    last_soc_val = Current_Soc;
    return Current_Soc;
  }

  if(last_soc_val == Current_Soc && Current_Soc >= 90 && MAX17048_Driver_Get_Battery_Charge_State() == 1)
  {
    /*电量值在90%区域，补偿*/
    if(range_num < 2)
    {
      if((Timer_Port_Get_Current_Time(TIMER_SEC) - E_Record[range_num].Start_Time_Sec) > 60*30)
      {
        E_Record[range_num].Start_Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);
        E_Record[range_num].Compensation_Soc += 1;
      }
    }
    else
    {
      uint32_t ref_time = E_Record[range_num].Start_Time_Sec - E_Record[range_num-1].Start_Time_Sec;
      uint32_t elapsed_time = Timer_Port_Get_Current_Time(TIMER_SEC) - E_Record[range_num].Start_Time_Sec;
      if(elapsed_time > ref_time && elapsed_time > 2*ref_time)
      {
        E_Record[range_num].Start_Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);
        E_Record[range_num-1].Start_Time_Sec = E_Record[range_num].Start_Time_Sec - ref_time;
        E_Record[range_num].Compensation_Soc += 1;
      }
    }
    E_Record[range_num].Compensation_Soc = E_Record[range_num].Compensation_Soc >= 100?100:E_Record[range_num].Compensation_Soc;
    return E_Record[range_num].Compensation_Soc;
  }

  /*电量值在90%区域，放电电量控制*/
  if(Current_Soc >= 90 && MAX17048_Driver_Get_Battery_Charge_State() == 0)
  {
    /*属修正值*/
    if(E_Record[range_num].Compensation_Soc > Current_Soc)
    {
      /*计算耗电时间*/
      uint32_t elapsed_time_min = (Timer_Port_Get_Current_Time(TIMER_SEC) - E_Record[range_num].Start_Time_Sec)/60;
      float pow = (float)elapsed_time_min/(float)CAN_USE_MIN_TOTAL;
      E_Record[range_num].Compensation_Soc = (uint16_t)((float)E_Record[range_num].Compensation_Soc - (100.f * pow));
      E_Record[range_num].Compensation_Soc = E_Record[range_num].Compensation_Soc < Current_Soc?Current_Soc:E_Record[range_num].Compensation_Soc;
      return E_Record[range_num].Compensation_Soc;
    }
  }
  return Current_Soc;
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
  * @brief   I2C发送完成回调
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
void Voltameter_Port_I2C_MasterTxCpltCallback(void)
{
  /*TODO:TRANSPOND*/
  MAX17048_Driver_I2C_MasterTxCpltCallback();
}

/**
  ******************************************************************
  * @brief   I2C接收完成回调
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
void Voltameter_Port_I2C_MasterRxCpltCallback(void)
{
  /*TODO:TRANSPOND*/
  MAX17048_Driver_I2C_MasterRxCpltCallback();
}

/**
  ******************************************************************
  * @brief   更新电池电量信息
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-06
  ******************************************************************
  */
void Voltameter_Port_Update_Battery_Electric_Quantity(void)
{
  Battery_Soc = MAX17048_Driver_Get_Battery_Electric_Quantity();
}

/**
  ******************************************************************
  * @brief   获取电池电量信息
  * @param   [in]None
  * @return  电量数据
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
uint16_t Voltameter_Port_Get_Battery_Electric_Quantity(void)
{ 
  /*限制耗电状态下电量输出为0*/
  if(Battery_Soc == 0 && MAX17048_Driver_Get_Battery_Charge_State() == 0)
  {
    return 1*100;
  }
  
  return Soc_Compensation(Battery_Soc)*100;
}

/**
  ******************************************************************
  * @brief   获取电池电压信息
  * @param   [in]None
  * @return  电压数据
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
uint16_t Voltameter_Port_Get_Battery_Electric_Voltage(void)
{
  return Battery_Voltage;
}

/**
  ******************************************************************
  * @brief   获取电池充电状态
  * @param   [in]None
  * @return  1 为充电状态
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
uint8_t Voltameter_Port_Get_Battery_Charge_State(void)
{
  return MAX17048_Driver_Get_Battery_Charge_State();
}

/**
  ******************************************************************
  * @brief   电池监控接口休眠设置
  * @param   [in]Sleep_En 睡眠使能.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-07
  ******************************************************************
  */
void Voltameter_Port_OnOff_Sleep(bool Sleep_En)
{
  MAX17048_Driver_OnOff_Sleep(Sleep_En);
}

/**
  ******************************************************************
  * @brief   电池监控接口初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
void Voltameter_Port_Init(void)
{
  /*初始化MAX17048*/
  MAX17048_Driver_Init();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
