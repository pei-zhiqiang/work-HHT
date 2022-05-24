/**
 *  @file DAC_Port.c
 *
 *  @date 2021-03-11
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief DAC操作接口
 *
 *  @details 1、250Hz 4ms，500Hz 2ms，1Khz 1ms，2Khz 500us，4Khz 250us，8Khz 125us
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "DAC_Port.h"
#include "main.h"
#include "SAI_port.h"
/** Private macros -----------------------------------------------------------*/
#define DAC_SEND_DATA_UPDATE_AT_IT    1/**< 为1时，DAC数据更新在中断进行 */
/** Private typedef ----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static CQ_handleTypeDef DAC_CQ_Handle;
static uint32_t Send_DAC_Buf[CQ_BUF_256B];
static int32_t *Current_Send_DMA_Buf_Ptr = NULL;
static volatile uint8_t DAC_Port_Can_Send_Flag = 1;
/** Private function prototypes ----------------------------------------------*/
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
 * @brief 更新发送缓冲区地址
 *
 * @param Send_Buf_Ptr 缓冲区地址
 */
void DAC_Port_Update_Send_Buf(int32_t *Send_Buf_Ptr)
{
  /* 更新发送区域 */
  Current_Send_DMA_Buf_Ptr = Send_Buf_Ptr;
  DAC_Port_Can_Send_Flag = 1;
#if DAC_SEND_DATA_UPDATE_AT_IT
  CQ_32getData(&DAC_CQ_Handle, (uint32_t *)Current_Send_DMA_Buf_Ptr, SAI_PORT_DAC_SEND_DATA_SIZE_MAX);
#endif
}

/**
 * @brief 更新DAC音频数据
 *
 * @param Audio_Data_CH1 通道1音频数据
 * @param Audio_Data_CH2 通道2音频数据
 * @param Send_Audio_Buf 输出缓冲区
 */
void DAC_Port_Play_Audio_Data(const int32_t *Audio_Data_CH1, const int32_t *Audio_Data_CH2, int32_t *Send_Audio_Buf)
{
  UNUSED(Send_Audio_Buf);

  int32_t Temp_Buf[STEREO_FRAME_SIZE];
  uint16_t cnt = 0;
  for(uint16_t index = 0; index < STEREO_FRAME_SIZE; index += 2)
  {
    /* 交换左右耳数据通道 */
    Temp_Buf[index] = Audio_Data_CH2[cnt] >> 8;
    Temp_Buf[index + 1] = Audio_Data_CH1[cnt] >> 8;
    cnt++;
  }

  /* 加入数据到发送缓冲区 */
  CQ_32putData(&DAC_CQ_Handle, (uint32_t *)Temp_Buf, STEREO_FRAME_SIZE);
}

/**
 * @brief 启动DAC数据更新
 *
 */
void DAC_Port_Play_Audio_Start(void)
{
#if DAC_SEND_DATA_UPDATE_AT_IT == 0
  if(CQ_getLength(&DAC_CQ_Handle) < SAI_PORT_DAC_SEND_DATA_SIZE_MAX \
    || NULL == Current_Send_DMA_Buf_Ptr || 0 == DAC_Port_Can_Send_Flag)
  {
    return;
  }
  DAC_Port_Can_Send_Flag = 0;
  CQ_32getData(&DAC_CQ_Handle, (uint32_t *)Current_Send_DMA_Buf_Ptr, SAI_PORT_DAC_SEND_DATA_SIZE_MAX);
#endif
}

/**
* @brief 设置DAC输出1使能状态，有线耳机端
 *
 * @param State true打开
 */
void DAC_Port_Set_LR_OUT1_On_Off(bool State)
{
 WM8978_Set_LR_OUT1_On_Off(State);
}

/**
 * @brief 设置DAC输出2使能状态，蓝牙端
 *
 * @param State true打开
 */
void DAC_Port_Set_LR_OUT2_On_Off(bool State)
{
  WM8978_Set_LR_OUT2_On_Off(State);
}

/**
  ******************************************************************
  * @brief   设置DAC采样率
  * @param   [in]sr 采样率
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-17
  ******************************************************************
  */
void DAC_Port_Set_Sample_Rate(DAC_SAMPLE_RATE_Typedef_t sr)
{
  switch(sr)
  {
    case DAC_16KHZ_SR:
      WM8978_Set_SampRate(SAMP_RATE_16KHZ);
      break;
    case DAC_32KHZ_SR:
      WM8978_Set_SampRate(SAMP_RATE_32KHZ);
      break;
    case DAC_48KHZ_SR:
      WM8978_Set_SampRate(SAMP_RATE_48KHZ);
      break;
    default:
      break;
  }
}

/**
  ******************************************************************
  * @brief   调节前置放大
  * @param   [in]Channel 通道号
  * @param   [in]val 增益系数
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-23
  ******************************************************************
  */
void DAC_Port_Set_Preamp(DAC_CHANNEL_NUM_Typedef_t Channel, float Gain)
{
  if(Channel == DAC_CHANNEL_LEFT)
  {
    WM8978_Set_Preamp_Gain(LEFT_CHANNEL, Gain);
  }
  else if(Channel == DAC_CHANNEL_RIGHT)
  {
    WM8978_Set_Preamp_Gain(RIGHT_CHANNEL, Gain);
  }
}

/**
  ******************************************************************
  * @brief   设置左右耳增益
  * @param   [in]Channel 通道号
  * @param   [in]val 增益系数 ,当设置双通道为0时直接Mute当设置>100时取消Mute直接返回
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-23
  ******************************************************************
  */
void DAC_Port_Set_Vol_Gain(DAC_CHANNEL_NUM_Typedef_t Channel, float Gain)
{
  /* 交换左右 */
  if(Channel == DAC_CHANNEL_RIGHT)
  {
    SAI_Port_Sender_Set_Mute(false);
    WM8978_Set_Vol_Gain(LEFT_CHANNEL, Gain);
  }
  else if(Channel == DAC_CHANNEL_LEFT)
  {
    SAI_Port_Sender_Set_Mute(false);
    WM8978_Set_Vol_Gain(RIGHT_CHANNEL, Gain);
  }
  else if(Channel == DAC_CHANNEL_LR)
  {
    if(Gain <= 0)
    {
      SAI_Port_Sender_Set_Mute(true);
      return;
    }
    else if(Gain > 100)
    {
      SAI_Port_Sender_Set_Mute(false);
      return;
    }
    else
    {
      SAI_Port_Sender_Set_Mute(false);
      WM8978_Set_Vol_Gain(LEFT_CHANNEL, Gain);
      WM8978_Set_Vol_Gain(RIGHT_CHANNEL, Gain);
    }
  }
}

/**
  ******************************************************************
  * @brief   调节EQ5段中心频率及增益
  * @param   [in]fq 五段中心频率
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-26
  ******************************************************************
  */
void DAC_Port_Set_EQ_Center_Frequency(const uint16_t *fq, const uint8_t *Gain_dB)
{
  /* EQ用于DAC路径 */
  WM8978_Set_EQ_Band_Gain(fq, Gain_dB, 0x100);
}

/**
  ******************************************************************
  * @brief   调节听力曲线更新
  * @param   [in]Left_Ref_dB_Min_Gain 左听力曲线最小dB值
  * @param   [in]Right_Ref_dB_Min_Gain 右听力曲线最小dB值
  * @param   [in]Left_Ref_dB_Max_Gain 左听力曲线不舒适dB值
  * @param   [in]Right_Ref_dB_Max_Gain 右听力曲线不舒适dB值
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-31
  ******************************************************************
  */
void DAC_Port_Set_Ref_Hear_Wave(const uint8_t *Left_Ref_dB_Min_Gain, const uint8_t *Right_Ref_dB_Min_Gain,
                                const uint8_t *Left_Ref_dB_Max_Gain, const uint8_t *Right_Ref_dB_Max_Gain)
{
  UNUSED(Left_Ref_dB_Min_Gain);
  UNUSED(Right_Ref_dB_Min_Gain);
  UNUSED(Left_Ref_dB_Max_Gain);
  UNUSED(Right_Ref_dB_Max_Gain);

  /*计算参数，自动验配，修改参数*/
  /*g_Miscellaneous_Par.Left_Auto_EQ_Gain*/
  /*g_Miscellaneous_Par.Right_Auto_EQ_Gain*/

  /* 保存参数 */
  Parameter_Port_Start();
}

/**
  ******************************************************************
  * @brief   DAC反初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-19
  ******************************************************************
  */
void DAC_Port_DeInit(void)
{
  WM8978_DeInit();
}

/**
  ******************************************************************
  * @brief   DAC初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-09
  ******************************************************************
  */
void DAC_Port_Init(void)
{
  /* 配置WM8978并启动 */
  WM8978_Init_Start();

  /* 设置DAC采样率 */
  DAC_Port_Set_Sample_Rate(DAC_32KHZ_SR);

  /* 设置前置放大 */
  DAC_Port_Set_Preamp(DAC_CHANNEL_LEFT, g_Miscellaneous_Par.Preamplification_Factor);
  DAC_Port_Set_Preamp(DAC_CHANNEL_RIGHT, g_Miscellaneous_Par.Preamplification_Factor);

  /* 设置输出增益 */
  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_RIGHT, (float)g_Miscellaneous_Par.Right_Channel_Compensation);
  DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LEFT, (float)g_Miscellaneous_Par.Left_Channel_Compensation);

  /* 设置高通滤波 */
  WM8978_Set_HIPASS(ADC_HI_PASS_16KHZ_82HZ);

  /* 设置EQ */
  DAC_Port_Set_EQ_Center_Frequency(g_Miscellaneous_Par.EQ_5_Band_FQ, g_Miscellaneous_Par.EQ_5_Band_Gain);

  /* 使能DAC的AGC控制 */
  WM8978_Set_AGC_Gain((WM8978_AGC_PAR_Typedef_t *)&g_Miscellaneous_Par.DAC_AGC_Par);

  /* 初始化DAC缓冲区 */
  CQ_32_init(&DAC_CQ_Handle, Send_DAC_Buf, CQ_BUF_256B);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
