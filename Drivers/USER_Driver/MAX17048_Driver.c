/**
 *  @file MAX17048_Driver.c
 *
 *  @date 2021-03-04
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 电池电量监控接口
 *
 *  @details 1、SCL RISE MIN KEEP 20ns
 *           2、SCL FALL MIN KEEP 20ns
 *           3、写： S. SAddr W. A. MAddr. A. Data0. A. Data1. A. P
 *           4、读： 先写S. SAddr W. A. MAddr. A 再读 Sr. SAddr R. A. Data0. A. Data1. N. P
 *           5、使用Fastmode 最高速率400Khz
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "MAX17048_Driver.h"
#include "utilities.h"
#include "main.h"
/** Private typedef ----------------------------------------------------------*/

/** Public variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c2;
extern MISCELLANEOUS_PAR_Typedef_t g_Miscellaneous_Par;/**< 辅助参数*/
/** Private macros -----------------------------------------------------------*/
#define ENABLE_MAX17048_DEBUG   0/**< 启动打印调试*/
/*MAX17048设备I2C地址*/
#define DEV_ADDR                0x6C
/*MAX17048设备I2C写操作地址*/
#define DEV_ADDR_WRITE          (DEV_ADDR|0x00)
/*MAX17048设备I2C读操作地址*/
#define DEV_ADDR_READ           (DEV_ADDR|0x01)

/*MAX17048寄存器地址*/
#define REG_V_CELL              0x02
#define REG_SOC                 0x04
#define REG_MODE                0x06
#define REG_VERSION             0x08
#define REG_HIBRT               0x0A
#define REG_CONFIG              0x0C
#define REG_VALRT               0x14
#define REG_CRATE               0x16
#define REG_V_RESET             0x18
#define REG_STATUS              0x1A

#define REG_TABLE               0x40/**< REG RANGE:0x40 to 0x70*/
#define REG_TABLE_END           0x70

#define REG_CMD                 0xFE

#define RCOMP0                  0x97/**< 温度补偿值*/

/*16BIT位*/
#define REG_BIT0                (1U<<0)
#define REG_BIT1                (1U<<1)
#define REG_BIT2                (1U<<2)
#define REG_BIT3                (1U<<3)
#define REG_BIT4                (1U<<4)
#define REG_BIT5                (1U<<5)
#define REG_BIT6                (1U<<6)
#define REG_BIT7                (1U<<7)
#define REG_BIT8                (1U<<8)
#define REG_BIT9                (1U<<9)
#define REG_BIT10               (1U<<10)
#define REG_BIT11               (1U<<11)
#define REG_BIT12               (1U<<12)
#define REG_BIT13               (1U<<13)
#define REG_BIT14               (1U<<14)
#define REG_BIT15               (1U<<15)

#define MAX17048_I2C_HANDLE     &hi2c2/**< I2C句柄*/
/** Private constants --------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/
static uint16_t Battery_Soc = 0;    /**< 电池电量百分比*/
static uint16_t Battery_Voltage = 0;/**< 电池电压*/
static int8_t Rcomp = RCOMP0;
static volatile uint8_t Step_Record = 0;
static uint8_t Send_Val_Buf[2] MATH_PORT_SECTION("USE_DMA_BUF_SPACE");
static uint8_t Reg_Val_Buf[2] MATH_PORT_SECTION("USE_DMA_BUF_SPACE");
/** Private function prototypes ----------------------------------------------*/
#if ENABLE_MAX17048_DEBUG
static void Display_Alarm_Info(uint8_t Alarm_Value);
#endif
static void I2C_State_Reset(void);
static void Power_On_Reset(void);
static uint16_t Read_Vcell_Update(uint32_t Block_Time);
static uint16_t Read_Soc_Update(uint32_t Block_Time);
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
  * @brief   I2C 异常状态恢复
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-09
  ******************************************************************
  */
static inline void I2C_State_Reset(void)
{
//  if(HAL_OK != HAL_I2C_IsDeviceReady(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, 3, 100))
//  {
    __HAL_I2C_DISABLE(MAX17048_I2C_HANDLE);
//    HAL_Delay(1);
    __HAL_I2C_ENABLE(MAX17048_I2C_HANDLE);
//  }
}

/**
  ******************************************************************
  * @brief   MAX17048 报警信息打印
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-08
  ******************************************************************
  */
#if ENABLE_MAX17048_DEBUG
static void Display_Alarm_Info(uint8_t Alarm_Value)
{
  if(Alarm_Value & REG_BIT0)
  {
    printf("POWER ON IC NO CONFIG.\r\n");
  }
  if(Alarm_Value & REG_BIT1)
  {
    printf("VCELL HIGH THAN ALRT.VALRTMAX.\r\n");
  }
  if(Alarm_Value & REG_BIT2)
  {
    printf("VCELL LOW THAN ALRT.VALRTMIN.\r\n");
  }
  if(Alarm_Value & REG_BIT3)
  {
    printf("VOLTAGE RESET.\r\n");
  }
  if(Alarm_Value & REG_BIT4)
  {
    printf("SOC LOW THAN CONFIG.ATHD.\r\n");
  }
  if(Alarm_Value & REG_BIT5)
  {
    printf("SOC CHANGE VALUE ARRIVE %%1.\r\n");
  }
  if(Alarm_Value & REG_BIT6)
  {
    printf("ENABLE VOLTAGE RESET ALARM.\r\n");
  }
}
#endif
/**
  ******************************************************************
  * @brief   MAX17048 上电复位
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-08
  ******************************************************************
  */
static void Power_On_Reset(void)
{
  /*复位上电*/
  uint8_t WriteData[3] = {REG_CMD, 0x54, 0x00};
  if(g_Miscellaneous_Par.Meter_Device_Is_Init == 0)
  {
    HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, WriteData, 3, 100);
  }

  /*快速启动*/
  WriteData[0] = REG_MODE;
  WriteData[1] = 0x00;
  WriteData[2] |= REG_BIT6;

  /*使能休眠模式*/
  WriteData[2] |= REG_BIT5;

  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, WriteData, 3, 100);
}

/**
  ******************************************************************
  * @brief   清除MAX17048 报警状态
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
static void Alarm_State_Reset(void)
{
  uint8_t RegData = 0;
  uint8_t RecData[2] = {0};
  uint8_t SendData[3] = {0};

  /*读取报警状态信息*/
  RegData = REG_STATUS;
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);
#if ENABLE_MAX17048_DEBUG
  printf("Status REG:%02X:VAL:%02X%02X.\r\n", RegData, RecData[0], RecData[1]);
  Display_Alarm_Info(RecData[0]);
#endif
  /*清除报警信息*/
  SendData[0] = RegData;
  SendData[1] = (RecData[0]&(~0x7F));
  SendData[2] = RecData[1];
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, SendData, 3, 100);
#if ENABLE_MAX17048_DEBUG
  printf("Set Status REG:%02X:VAL:%02X%02X.\r\n", RegData, SendData[1], SendData[2]);
#endif
#if ENABLE_MAX17048_DEBUG
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);
  printf("After Status REG:%02X:VAL:%02X%02X.\r\n", RegData, RecData[0], RecData[1]);
#endif

  /*读取配置信息*/
  RegData = REG_CONFIG;
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);
#if ENABLE_MAX17048_DEBUG
  printf("Config REG:%02X:VAL:%02X%02X.\r\n", RegData, RecData[0], RecData[1]);
#endif
  /*设置报警阈值 BIT0-4 可设范围1-32% 设置10%则32-10*/
  RecData[1] = ((RecData[1]&0xE0)|(32-10));
  /*清除报警状态位 BIT5*/
  RecData[1] &= (~REG_BIT5);
  SendData[0] = RegData;
  SendData[1] = RecData[0];
  SendData[2] = RecData[1];
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, SendData, 3, 100);
#if ENABLE_MAX17048_DEBUG
  printf("Set Config REG:%02X:VAL:%02X%02X.\r\n", RegData, SendData[1], SendData[2]);
#endif
#if ENABLE_MAX17048_DEBUG
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);
  printf("After Config REG:%02X:VAL:%02X%02X.\r\n", RegData, RecData[0], RecData[1]);
#endif

  /*读取版本信息*/
#if ENABLE_MAX17048_DEBUG
  RegData = REG_VERSION;
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);
  printf("Version REG:%02X:VAL:%02X%02X.\r\n", RegData, RecData[0], RecData[1]);
#endif
}

/**
  ******************************************************************
  * @brief   读取MAX17048 Vcell电压数值 78.125uV/Vcell
  * @param   [in]Block_Time 阻塞读取时间
  * @return  电量数据
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
__attribute__((unused))static uint16_t Read_Vcell_Update(uint32_t Block_Time)
{
  Send_Val_Buf[0] = REG_V_CELL;
  if(Block_Time == 0)
  {
    if(Step_Record == 0)
    {
      if(HAL_OK != HAL_I2C_Master_Transmit_IT(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, Send_Val_Buf, 1))
      {
        return Battery_Voltage;
      }
      Step_Record = 1;
      return Battery_Voltage;
    }
    if(Step_Record == 1)
    {
      if(HAL_OK != HAL_I2C_Master_Receive_IT(MAX17048_I2C_HANDLE, DEV_ADDR_READ, Reg_Val_Buf, 2))
      {
        return Battery_Voltage;
      }
      Step_Record = 0;
    }
  }
  else if(Block_Time > 0)
  {
    if(HAL_OK != HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, Send_Val_Buf, 1, Block_Time))
    {
      I2C_State_Reset();
      return Battery_Voltage;
    }

    if(HAL_OK != HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, Reg_Val_Buf, 2, Block_Time))
    {
      I2C_State_Reset();
      return Battery_Voltage;
    }

    Battery_Voltage = (Reg_Val_Buf[0]<<8)+Reg_Val_Buf[1];
    Battery_Voltage = (uint16_t)(((double)Battery_Voltage * 78.125)/1000000);
    return Battery_Voltage;
  }
  return Battery_Voltage;
}

/**
  ******************************************************************
  * @brief   读取MAX17048 SOC(Battery state of charge)电量数值 1%/256
  * @param   [in]Block_Time 阻塞读取时间
  * @return  电量数据
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
static uint16_t Read_Soc_Update(uint32_t Block_Time)
{
  Send_Val_Buf[0] = REG_SOC;
  if(Block_Time == 0)
  {
    if(Step_Record == 0)
    {
      Step_Record = 1;
      if(HAL_OK != HAL_I2C_Master_Transmit_IT(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, Send_Val_Buf, 1))
      {
        return Battery_Soc;
      }
      return Battery_Soc;
    }
    if(Step_Record == 1)
    {
      Step_Record = 0;
      if(HAL_OK != HAL_I2C_Master_Receive_IT(MAX17048_I2C_HANDLE, DEV_ADDR_READ, Reg_Val_Buf, 2))
      {
        I2C_State_Reset();
        return Battery_Soc;
      }
    }
  }
  else if(Block_Time > 0)
  {
    if(Step_Record == 0)
    {
      if(HAL_OK != HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, Send_Val_Buf, 1, Block_Time))
      {
        return Battery_Soc;
      }
      Step_Record = 1;
    }
    if(Step_Record == 1)
    {
      if(HAL_OK != HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, Reg_Val_Buf, 2, Block_Time))
      {
        I2C_State_Reset();
        Step_Record = 0;
        return Battery_Soc;
      }
      Step_Record = 0;
    }
    uint16_t Battery_Soc_Temp = (Reg_Val_Buf[0]<<8) + Reg_Val_Buf[1];
    Battery_Soc_Temp /= 256;
    Battery_Soc = Battery_Soc_Temp > 100?100:Battery_Soc_Temp;
    return Battery_Soc;
  }
  return Battery_Soc;
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
void MAX17048_Driver_I2C_MasterTxCpltCallback(void)
{
  /*TODO:READ*/
  Step_Record = 1;
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
void MAX17048_Driver_I2C_MasterRxCpltCallback(void)
{
  /*TODO:UPDATE VAL*/
  if(Send_Val_Buf[0] == REG_V_CELL)
  {
    uint16_t Battery_Voltage_Temp = (Reg_Val_Buf[0]<<8) + Reg_Val_Buf[1];
    Battery_Voltage_Temp = (uint16_t)(((double)Battery_Voltage_Temp * 78.125)/1000000);
    Battery_Voltage = Battery_Voltage_Temp;
  }
  else if(Send_Val_Buf[0] == REG_SOC)
  {
    uint16_t Battery_Soc_Temp = (Reg_Val_Buf[0]<<8) + Reg_Val_Buf[1];
    Battery_Soc_Temp /= 256;
    Battery_Soc = Battery_Soc_Temp > 100?100:Battery_Soc_Temp;
  }
}

/**
  ******************************************************************
  * @brief   温度补偿
  * @param   [in]Temperatur.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-16
  ******************************************************************
  */
void MAX17048_Driver_Compensate(float Temperature)
{
  uint8_t RegData = 0;
  uint8_t RecData[2] = {0};
  uint8_t SendData[3] = {0};

  if(Temperature > 20)
  {
    Rcomp = (int8_t)(RCOMP0 + (Temperature - 20) * -0.5);
  }
  else
  {
    Rcomp = (int8_t)(RCOMP0 + (Temperature - 20) * -5.0);
  }

  /*读取配置数据*/
  RegData = REG_CONFIG;
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);

  /*设置温度补偿RCOMP*/
  SendData[0] = RegData;
  SendData[1] = (uint8_t)Rcomp;
  SendData[2] = RecData[1];
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, SendData, 3, 100);
}

/**
  ******************************************************************
  * @brief   获取更新电池电量及电压信息
  * @param   [in]None
  * @return  电量数据
  * @author  aron566
  * @version V1.0
  * @date    2021-03-04
  ******************************************************************
  */
uint16_t MAX17048_Driver_Get_Battery_Electric_Quantity(void)
{
  Read_Soc_Update(2);
//  Read_Vcell_Update(0);

  return Battery_Soc;
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
uint16_t MAX17048_Driver_Get_Battery_Electric_Voltage(void)
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
uint8_t MAX17048_Driver_Get_Battery_Charge_State(void)
{
  // if(HAL_GPIO_ReadPin(USB_5V_DET_GPIO_Port, USB_5V_DET_Pin) == GPIO_PIN_SET)
  // {
  //   return 1;
  // }
  // else
  // {
  //   return 0;
  // }
  if(HAL_GPIO_ReadPin(CHARGE_STATE_GPIO_Port, CHARGE_STATE_Pin) == GPIO_PIN_RESET)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  ******************************************************************
  * @brief   MAX17048 休眠设置
  * @param   [in]Sleep_En 睡眠使能.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-07
  ******************************************************************
  */
void MAX17048_Driver_OnOff_Sleep(bool Sleep_En)
{
  uint8_t RegData = 0;
  uint8_t RecData[2] = {0};
  uint8_t SendData[3] = {0};

  /*读取配置信息*/
  RegData = REG_CONFIG;
  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, &RegData, 1, 100);
  HAL_I2C_Master_Receive(MAX17048_I2C_HANDLE, DEV_ADDR_READ, RecData, 2, 100);
#if ENABLE_MAX17048_DEBUG
  printf("Config REG:%02X:VAL:%02X%02X.\r\n", RegData, RecData[0], RecData[1]);
#endif

  SendData[1] = RegData;
  SendData[1] = RecData[0];
  SendData[2] = RecData[1];

  if(Sleep_En == true)
  {
    /*进入休眠 config.sleep = 1*/
    SendData[1] |= REG_BIT7;
  }
  else
  {
    /*退出休眠 config.sleep = 0*/
    SendData[1] &= ~REG_BIT7;
  }

  HAL_I2C_Master_Transmit(MAX17048_I2C_HANDLE, DEV_ADDR_WRITE, SendData, 3, 100);
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
void MAX17048_Driver_Init(void)
{
  /*上电复位MAX17048*/
  Power_On_Reset();

  /*报警复位*/
  Alarm_State_Reset();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
