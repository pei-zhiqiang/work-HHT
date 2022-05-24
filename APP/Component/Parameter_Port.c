/**
 *  @file Parameter_Port.c
 *
 *  @date 2021-02-26
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 参数存储操作
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
#include "Flash_Port.h"
#include "DAC_Port.h"
#include "utilities.h"
#include "utilities_crc.h"
#include "Power_Management_Port.h"
#include "Timer_Port.h"
#include "Algorithm_Port.h"

/** Private macros -----------------------------------------------------------*/
#define PARAMETER_PARTITION_NAME  "kvdb"/**< 参数存储分区 128KB*/
#define PARAMETER_BAK_OFFSET      128*1024U
#define PARAMETER_RESTORE_OFFSET  256*1024U/**< 暂未使用*/

#define PARAMETER_ALG_PAR_MAX     1024U/**< 参数最大长度*/
/** Private typedef ----------------------------------------------------------*/
/*参数存储结构*/
typedef struct
{
  uint8_t Algorithm_Par_Buf[PARAMETER_ALG_PAR_MAX];
  MISCELLANEOUS_PAR_Typedef_t Miscellaneous_Par;
  uint16_t crc;
}ALGORITHM_PAR_SAVE_Typedef_t;

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/

/*辅助参数*/
MISCELLANEOUS_PAR_Typedef_t g_Miscellaneous_Par =
{
  .Preamplification_Factor    = 100,  /**< 前置放大系数*/
  .Left_Channel_Compensation  = 100,  /**< 左通道音量补偿*/
  .Right_Channel_Compensation = 100,  /**< 右通道音量补偿*/
  .EQ_5_Band_FQ =                     /**< EQ均衡器5波段中心频率参数*/
  {
    [0] = 105,
    [1] = 300,
    [2] = 850,
    [3] = 2400,
    [4] = 6900,
  },
  .EQ_5_Band_Gain =                   /**< EQ均衡器5波段中心频率对应增益参数*/
  {
    [0] = 12,
    [1] = 12,
    [2] = 12,
    [3] = 12,
    [4] = 12,
  },
  .DAC_AGC_Par =
  {
    .atk = DAC_LIMATK_US_375,
    .dcy = DAC_LIMDCY_MS_6,
    .boost = 0,
    .level_l = 0
  },
  .Left_6_Band_Ref_Min_Gain =             /**< 左耳听力曲线6缎dB数值*/
  {
    [0] = 1,
    [1] = 1,
    [2] = 1,
    [3] = 1,
    [4] = 1,
    [5] = 1,
  },
  .Right_6_Band_Ref_Min_Gain =            /**< 右耳听力曲线6缎dB数值*/
  {
    [0] = 1,
    [1] = 1,
    [2] = 1,
    [3] = 1,
    [4] = 1,
    [5] = 1,
  },
  .Left_6_Band_Ref_Max_Gain =             /**< 左耳听力曲线6缎dB数值*/
  {
    [0] = 1,
    [1] = 1,
    [2] = 1,
    [3] = 1,
    [4] = 1,
    [5] = 1,
  },
  .Right_6_Band_Ref_Max_Gain =            /**< 右耳听力曲线6缎dB数值*/
  {
    [0] = 1,
    [1] = 1,
    [2] = 1,
    [3] = 1,
    [4] = 1,
    [5] = 1,
  },
  .Left_Auto_EQ_Gain =                    /**< 左耳听力曲线6缎dB自动计算数值*/
  {
    [0] = 1,
    [1] = 1,
    [2] = 1,
    [3] = 1,
    [4] = 1,
    [5] = 1,
  },
  .Right_Auto_EQ_Gain =                   /**< 右耳听力曲线6缎dB自动计算数值*/
  {
    [0] = 1,
    [1] = 1,
    [2] = 1,
    [3] = 1,
    [4] = 1,
    [5] = 1,
  },
  .Meter_Device_Is_Init = 0,          /**< 仪表初始化状态*/
  .Par_Auto_Save_Cs = 0,              /**< 参数自动保存设置*/
};
/** Private variables --------------------------------------------------------*/
static ALGORITHM_PAR_SAVE_Typedef_t Flash_Save_Par;
static bool Parameter_Port_Reset_En = false;/**< 重置参数标识*/
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
  * @brief   检测参数是否需要更新
  * @param   [in]None
  * @return  true 需要.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-26
  ******************************************************************
  */
static bool Check_Par_Can_Update(void)
{
  int ret = 0;
  uint16_t Size;
  uint8_t Algorithm_Par_Buf[PARAMETER_ALG_PAR_MAX];
  if(Algorithm_Port_Get_ALL_Par(Algorithm_Par_Buf, &Size, PARAMETER_ALG_PAR_MAX) == false)
  {
    printf("Get ALL Par Error, Size = %u!\r\n", Size);
    return false;
  }
  ret  = memcmp(&Flash_Save_Par.Algorithm_Par_Buf[0], Algorithm_Par_Buf, Size);

  /*辅助参数检查*/
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Left_Channel_Compensation, &g_Miscellaneous_Par.Left_Channel_Compensation, sizeof(g_Miscellaneous_Par.Left_Channel_Compensation));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Right_Channel_Compensation, &g_Miscellaneous_Par.Right_Channel_Compensation, sizeof(g_Miscellaneous_Par.Right_Channel_Compensation));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Preamplification_Factor, &g_Miscellaneous_Par.Preamplification_Factor, sizeof(g_Miscellaneous_Par.Preamplification_Factor));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.EQ_5_Band_FQ[0], &g_Miscellaneous_Par.EQ_5_Band_FQ[0], sizeof(g_Miscellaneous_Par.EQ_5_Band_FQ));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.EQ_5_Band_Gain[0], &g_Miscellaneous_Par.EQ_5_Band_Gain[0], sizeof(g_Miscellaneous_Par.EQ_5_Band_Gain));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.DAC_AGC_Par, &g_Miscellaneous_Par.DAC_AGC_Par, sizeof(g_Miscellaneous_Par.DAC_AGC_Par));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Left_6_Band_Ref_Min_Gain[0], &g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain[0], sizeof(g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Right_6_Band_Ref_Min_Gain[0], &g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain[0], sizeof(g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Left_6_Band_Ref_Max_Gain[0], &g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain[0], sizeof(g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Right_6_Band_Ref_Max_Gain[0], &g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain[0], sizeof(g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Left_Auto_EQ_Gain[0], &g_Miscellaneous_Par.Left_Auto_EQ_Gain[0], sizeof(g_Miscellaneous_Par.Left_Auto_EQ_Gain));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Right_Auto_EQ_Gain[0], &g_Miscellaneous_Par.Right_Auto_EQ_Gain[0], sizeof(g_Miscellaneous_Par.Right_Auto_EQ_Gain));


  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Meter_Device_Is_Init, &g_Miscellaneous_Par.Meter_Device_Is_Init, sizeof(g_Miscellaneous_Par.Meter_Device_Is_Init));
  ret |= memcmp(&Flash_Save_Par.Miscellaneous_Par.Par_Auto_Save_Cs, &g_Miscellaneous_Par.Par_Auto_Save_Cs, sizeof(g_Miscellaneous_Par.Par_Auto_Save_Cs));
  if(ret == 0)
  {
    return false;
  }
  return true;
}

/**
  ******************************************************************
  * @brief   从flash读取参数
  * @param   [in]None
  * @return  true 读取成功.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-08
  ******************************************************************
  */
static bool Read_Flash_Par(void)
{
  /*读取主参数区*/
  int ret = Flash_Port_Read_Partition_Data(PARAMETER_PARTITION_NAME, (uint8_t *)&Flash_Save_Par, 0, sizeof(ALGORITHM_PAR_SAVE_Typedef_t));
  if(ret < 0)
  {
    printf("read main kv error.\r\n");
    /*读取备份参数区*/
    ret = Flash_Port_Read_Partition_Data(PARAMETER_PARTITION_NAME, (uint8_t *)&Flash_Save_Par, PARAMETER_BAK_OFFSET, sizeof(ALGORITHM_PAR_SAVE_Typedef_t));
    if(ret < 0)
    {
      printf("read backup kv error.\r\n");
      return false;
    }
  }

  /*校验CRC*/
  uint16_t crc_val = modbus_crc_return_with_table((uint8_t *)&Flash_Save_Par, (uint16_t)OFFSETOF(ALGORITHM_PAR_SAVE_Typedef_t, crc));
  if(crc_val != Flash_Save_Par.crc)
  {
    return false;
  }
  return true;
}

/**
  ******************************************************************
  * @brief   存储参数到flash
  * @param   [in]None
  * @return  true 存储成功.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-08
  ******************************************************************
  */
static bool Save_Flash_Par(void)
{
  /*擦除主分区*/
  Flash_Port_Erase_Partition_Data(PARAMETER_PARTITION_NAME, 0, sizeof(Flash_Save_Par));

  /*校验CRC*/
  Flash_Save_Par.crc = modbus_crc_return_with_table((uint8_t *)&Flash_Save_Par, (uint16_t)OFFSETOF(ALGORITHM_PAR_SAVE_Typedef_t, crc));
  int ret = Flash_Port_Write_Partition_Data(PARAMETER_PARTITION_NAME, (const uint8_t *)&Flash_Save_Par, 0, sizeof(Flash_Save_Par));
  if(ret < 0)
  {
    return false;
  }

  /*存储到备份分区*/
  Flash_Port_Erase_Partition_Data(PARAMETER_PARTITION_NAME, PARAMETER_BAK_OFFSET, sizeof(Flash_Save_Par));
  ret = Flash_Port_Write_Partition_Data(PARAMETER_PARTITION_NAME, (const uint8_t *)&Flash_Save_Par, PARAMETER_BAK_OFFSET, sizeof(Flash_Save_Par));
  if(ret < 0)
  {
    return false;
  }
  return true;
}

/**
  ******************************************************************
  * @brief   参数重置为默认
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-29
  ******************************************************************
  */
static void Parameter_Port_Reset_Par(void)
{
  /*擦除主分区*/
  Flash_Port_Erase_Partition_Data(PARAMETER_PARTITION_NAME, 0, sizeof(Flash_Save_Par));
  /*擦除副分区*/
  Flash_Port_Erase_Partition_Data(PARAMETER_PARTITION_NAME, PARAMETER_BAK_OFFSET, sizeof(Flash_Save_Par));
  /*重启设备*/
  PowerManagement_Port_Restart();
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
  * @brief   读取参数
  * @param   [in]None
  * @return  true 读取成功.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-26
  ******************************************************************
  */
bool Parameter_Port_Read(void)
{
  /* 读取参数 */
  if(Read_Flash_Par() == false)
  {
    return false;
  }

  /* 恢复算法参数 */
  uint16_t Size;
  Algorithm_Port_Get_ALL_Par(NULL, &Size, 1);
  Algorithm_Port_RESTORE_ALL_Par(&Flash_Save_Par.Algorithm_Par_Buf[0], Size);

  /* 恢复辅助参数 */
  memmove(&g_Miscellaneous_Par.Left_Channel_Compensation, &Flash_Save_Par.Miscellaneous_Par.Left_Channel_Compensation, sizeof(g_Miscellaneous_Par.Left_Channel_Compensation));
  memmove(&g_Miscellaneous_Par.Right_Channel_Compensation, &Flash_Save_Par.Miscellaneous_Par.Right_Channel_Compensation, sizeof(g_Miscellaneous_Par.Right_Channel_Compensation));
  memmove(&g_Miscellaneous_Par.Preamplification_Factor, &Flash_Save_Par.Miscellaneous_Par.Preamplification_Factor, sizeof(g_Miscellaneous_Par.Preamplification_Factor));
  memmove(&g_Miscellaneous_Par.EQ_5_Band_FQ[0], &Flash_Save_Par.Miscellaneous_Par.EQ_5_Band_FQ[0], sizeof(g_Miscellaneous_Par.EQ_5_Band_FQ));
  memmove(&g_Miscellaneous_Par.EQ_5_Band_Gain[0], &Flash_Save_Par.Miscellaneous_Par.EQ_5_Band_Gain[0], sizeof(g_Miscellaneous_Par.EQ_5_Band_Gain));
  memmove(&g_Miscellaneous_Par.DAC_AGC_Par, &Flash_Save_Par.Miscellaneous_Par.DAC_AGC_Par, sizeof(g_Miscellaneous_Par.DAC_AGC_Par));

  memmove(&g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain[0], &Flash_Save_Par.Miscellaneous_Par.Left_6_Band_Ref_Min_Gain[0], sizeof(g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain));
  memmove(&g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain[0], &Flash_Save_Par.Miscellaneous_Par.Right_6_Band_Ref_Min_Gain[0], sizeof(g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain));
  memmove(&g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain[0], &Flash_Save_Par.Miscellaneous_Par.Left_6_Band_Ref_Max_Gain[0], sizeof(g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain));
  memmove(&g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain[0], &Flash_Save_Par.Miscellaneous_Par.Right_6_Band_Ref_Max_Gain[0], sizeof(g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain));

  memmove(&g_Miscellaneous_Par.Left_Auto_EQ_Gain[0], &Flash_Save_Par.Miscellaneous_Par.Left_Auto_EQ_Gain[0], sizeof(g_Miscellaneous_Par.Left_Auto_EQ_Gain));
  memmove(&g_Miscellaneous_Par.Right_Auto_EQ_Gain[0], &Flash_Save_Par.Miscellaneous_Par.Right_Auto_EQ_Gain[0], sizeof(g_Miscellaneous_Par.Right_Auto_EQ_Gain));


  memmove(&g_Miscellaneous_Par.Meter_Device_Is_Init, &Flash_Save_Par.Miscellaneous_Par.Meter_Device_Is_Init, sizeof(g_Miscellaneous_Par.Meter_Device_Is_Init));
  memmove(&g_Miscellaneous_Par.Par_Auto_Save_Cs, &Flash_Save_Par.Miscellaneous_Par.Par_Auto_Save_Cs, sizeof(g_Miscellaneous_Par.Par_Auto_Save_Cs));
  printf("Restore Algorithm Par OK.\r\n");
  return true;
}

/**
  ******************************************************************
  * @brief   更新参数
  * @param   [in]None
  * @return  true 更新flash成功.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-26
  ******************************************************************
  */
bool Parameter_Port_Update(void)
{
  /* 拷贝算法参数 */
  uint16_t Size;
  uint8_t Algorithm_Par_Buf[PARAMETER_ALG_PAR_MAX];
  if(Algorithm_Port_Get_ALL_Par(Algorithm_Par_Buf, &Size, PARAMETER_ALG_PAR_MAX) == false)
  {
    printf("Get ALL Par Error, Size = %u!\r\n", Size);
    return false;
  }
  memmove(&Flash_Save_Par.Algorithm_Par_Buf[0], Algorithm_Par_Buf, Size);

  /* 辅助参数 */
  memmove(&Flash_Save_Par.Miscellaneous_Par.Left_Channel_Compensation, &g_Miscellaneous_Par.Left_Channel_Compensation, sizeof(g_Miscellaneous_Par.Left_Channel_Compensation));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Right_Channel_Compensation, &g_Miscellaneous_Par.Right_Channel_Compensation, sizeof(g_Miscellaneous_Par.Right_Channel_Compensation));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Preamplification_Factor, &g_Miscellaneous_Par.Preamplification_Factor, sizeof(g_Miscellaneous_Par.Preamplification_Factor));
  memmove(&Flash_Save_Par.Miscellaneous_Par.EQ_5_Band_FQ[0], &g_Miscellaneous_Par.EQ_5_Band_FQ[0], sizeof(g_Miscellaneous_Par.EQ_5_Band_FQ));
  memmove(&Flash_Save_Par.Miscellaneous_Par.EQ_5_Band_Gain[0], &g_Miscellaneous_Par.EQ_5_Band_Gain[0], sizeof(g_Miscellaneous_Par.EQ_5_Band_Gain));
  memmove(&Flash_Save_Par.Miscellaneous_Par.DAC_AGC_Par, &g_Miscellaneous_Par.DAC_AGC_Par, sizeof(g_Miscellaneous_Par.DAC_AGC_Par));

  memmove(&Flash_Save_Par.Miscellaneous_Par.Left_6_Band_Ref_Min_Gain[0], &g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain[0], sizeof(g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Right_6_Band_Ref_Min_Gain[0], &g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain[0], sizeof(g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Left_6_Band_Ref_Max_Gain[0], &g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain[0], sizeof(g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Right_6_Band_Ref_Max_Gain[0], &g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain[0], sizeof(g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain));

  memmove(&Flash_Save_Par.Miscellaneous_Par.Left_Auto_EQ_Gain[0], &g_Miscellaneous_Par.Left_Auto_EQ_Gain[0], sizeof(g_Miscellaneous_Par.Left_Auto_EQ_Gain));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Right_Auto_EQ_Gain[0], &g_Miscellaneous_Par.Right_Auto_EQ_Gain[0], sizeof(g_Miscellaneous_Par.Right_Auto_EQ_Gain));


  memmove(&Flash_Save_Par.Miscellaneous_Par.Meter_Device_Is_Init, &g_Miscellaneous_Par.Meter_Device_Is_Init, sizeof(g_Miscellaneous_Par.Meter_Device_Is_Init));
  memmove(&Flash_Save_Par.Miscellaneous_Par.Par_Auto_Save_Cs, &g_Miscellaneous_Par.Par_Auto_Save_Cs, sizeof(g_Miscellaneous_Par.Par_Auto_Save_Cs));

  /* 存储参数 */
  return Save_Flash_Par();
}

/**
  ******************************************************************
  * @brief   参数自动保存设置
  * @param   [in]Cs Bit8自动使能状态，剩余位设置时间，放大5倍，单位秒
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-08-10
  ******************************************************************
  */
void Parameter_Port_Auto_Save_Set(uint8_t Cs)
{
  bool Parameter_Port_Auto_Save_En = false;
  /* 立即检测一次 */
  Parameter_Port_Start();
  if(g_Miscellaneous_Par.Par_Auto_Save_Cs == Cs)
  {
    return;
  }

  g_Miscellaneous_Par.Par_Auto_Save_Cs = Cs;
  /* 检测使能状态 */
  if(Cs & 0x80)
  {
    Parameter_Port_Auto_Save_En = true;
  }
  else
  {
    Parameter_Port_Auto_Save_En = false;
  }

  /* 检测时间设置 */
  uint32_t Save_Time = (Cs & 0x7F) * 5;
  if(Save_Time < 1)
  {
    g_Miscellaneous_Par.Par_Auto_Save_Cs &= 0x7F;
    return;
  }
  Timer_Port_Set_Tim5(Parameter_Port_Auto_Save_En, Save_Time * 1000U);
}

/**
  ******************************************************************
  * @brief   参数重置为默认
  * @param   [in]Cs 为1 重置启动
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-29
  ******************************************************************
  */
void Parameter_Port_Reset_Task_En(uint8_t Cs)
{
  Parameter_Port_Reset_En = (Cs == 1) ? true : false;
  Parameter_Port_Start();
}

/**
  ******************************************************************
  * @brief   参数接口随存运行
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-26
  ******************************************************************
  */
void Parameter_Port_Start(void)
{
  /* 重置参数检测 */
  if(Parameter_Port_Reset_En == true)
  {
    Parameter_Port_Reset_Par();
  }

  bool ret = Check_Par_Can_Update();
  if(ret == false)
  {
    return;
  }
  /*静音*/
  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LR, 0);

  Parameter_Port_Update();

  /* 取消静音 */
  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LR, 101.f);
}

/**
  ******************************************************************
  * @brief   参数接口初始化,flash及算法接口初始化完成后调用
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-26
  ******************************************************************
  */
void Parameter_Port_Init(void)
{
  bool ret = Parameter_Port_Read();
  if(ret == true)
  {
    /* 恢复参数保存设置 */
    Parameter_Port_Auto_Save_Set(g_Miscellaneous_Par.Par_Auto_Save_Cs);
    return;
  }
  Parameter_Port_Update();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
