/**
 *  @file SAI_Port.c
 *
 *  @date 2021-04-19
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief SAI操作接口
 *
 *  @details 1、I2S采集音频数据
 *           2、将音频数据给WM8978模块播放
 *           3、SAIA3 MIC3右- 4左数据、SAIA1采样 MIC1右 - 2左数据，SAIB1输出PCM数据到WM8978 DAC
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "SAI_Port.h"
#include "utilities.h"
#include "Algorithm_Port.h"
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern SAI_HandleTypeDef hsai_BlockA1;
extern SAI_HandleTypeDef hsai_BlockB1;
extern SAI_HandleTypeDef hsai_BlockA2;
extern DMA_HandleTypeDef hdma_sai1_a;
extern DMA_HandleTypeDef hdma_sai2_a;
extern DMA_HandleTypeDef hdma_sai1_b;

/* 音频PCM数据接收缓冲区 */
volatile int32_t PCM_ChannelBuf[MIC_CHANNEL_NUM/2][STEREO_FRAME_SIZE * 2] MATH_PORT_SECTION("USE_DMA_BUF_SPACE") = {0};
/* 音频PCM数据发送缓冲区 */
volatile int32_t PCM_SendBuf[SAI_PORT_DAC_SEND_DATA_SIZE_MAX * 2] MATH_PORT_SECTION("USE_DMA_BUF_SPACE") = {0};

static int16_t Tips_Audio_Buf[MONO_FRAME_SIZE];
static int32_t Tips_Audio_Send_Buf[MONO_FRAME_SIZE];

#if ENABLE_EXPORT_FUNC
  static int16_t Export_Audio_Buf_CH1[MONO_FRAME_SIZE];
  static int16_t Export_Audio_Buf_CH2[MONO_FRAME_SIZE];
#endif
/** Private variables --------------------------------------------------------*/
static int32_t *PCM_Data_Ptr[2] =
{
  [0] = (int32_t *)&PCM_ChannelBuf[0][0],
  [1] = (int32_t *)&PCM_ChannelBuf[1][0]
};

static volatile uint8_t SAI_DmaReceived_Flag  = 0;

/*4MIC数据拷贝缓冲区*/
static int32_t MIC_Sample_Audio_Buf[MIC_CHANNEL_NUM][MONO_FRAME_SIZE] = {0};

// static int32_t MIC1_2_STEREO_Aidio_TempBuf[STEREO_FRAME_SIZE] = {0};
// static int32_t MIC3_4_STEREO_Aidio_TempBuf[STEREO_FRAME_SIZE] = {0};

static int32_t *Result_CH1_Data_Ptr = NULL;
static int32_t *Result_CH2_Data_Ptr = NULL;
static int32_t *Send_Data_Ptr = (int32_t *)&PCM_SendBuf[0];
/** Private function prototypes ----------------------------------------------*/
/* 外设时钟配置 */
extern void PeriphCommonClock_Config(void);
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
  * @brief   强定义SAI半接收完成中断
  * @param   [in]hsai sai句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	if(hsai == &hsai_BlockA2)
	{
    PCM_Data_Ptr[0] = (int32_t *)&PCM_ChannelBuf[0][0];
	}
  if(hsai == &hsai_BlockB1)
  {
    PCM_Data_Ptr[1] = (int32_t *)&PCM_ChannelBuf[1][0];
  }
  SAI_DmaReceived_Flag = 1;
}

/**
  ******************************************************************
  * @brief   强定义SAI接收完成中断
  * @param   [in]hsai sai句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	if(hsai == &hsai_BlockA2)
	{
    PCM_Data_Ptr[0] = (int32_t *)&PCM_ChannelBuf[0][STEREO_FRAME_SIZE];
	}
  if(hsai == &hsai_BlockB1)
  {
    PCM_Data_Ptr[1] = (int32_t *)&PCM_ChannelBuf[1][STEREO_FRAME_SIZE];
  }
  SAI_DmaReceived_Flag = 1;
}

/**
  ******************************************************************
  * @brief   SAI半发送完成中断
  * @param   [in]hsai SAI句柄
  * @return  HAL_StatusTypeDef
  * @author  aron66
  * @version V1.0
  * @date    2021-03-11
  ******************************************************************
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  UNUSED(hsai);
  /*可以更新缓冲区0*/
	Send_Data_Ptr = (int32_t *)&PCM_SendBuf[0];
  DAC_Port_Update_Send_Buf(Send_Data_Ptr);
}

/**
  ******************************************************************
  * @brief   SAI发送完成中断
  * @param   [in]hsai SAI句柄
  * @return  HAL_StatusTypeDef
  * @author  aron66
  * @version V1.0
  * @date    2021-03-11
  ******************************************************************
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  UNUSED(hsai);
  /*可以更新缓冲区1*/
	Send_Data_Ptr = (int32_t *)&PCM_SendBuf[SAI_PORT_DAC_SEND_DATA_SIZE_MAX];
  DAC_Port_Update_Send_Buf(Send_Data_Ptr);
}

/**
  ******************************************************************
  * @brief   重置SAI1采样率
  * @param   [in]AudioFrequency 采样率
  * @return  None.
  * @author  aron66
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
void SAI_Port_Reset_SAI1_Init(uint32_t AudioFrequency)
{
  if(hsai_BlockA1.Init.AudioFrequency == AudioFrequency)
  {
    return;
  }

  /*停止外设*/
  HAL_SAI_DeInit(&hsai_BlockA1);
  HAL_SAI_DeInit(&hsai_BlockA2);
  HAL_SAI_DeInit(&hsai_BlockB1);

  /*配置外设时钟*/
  if(AudioFrequency == SAI_AUDIO_FREQUENCY_48K)
  {
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_SAI2;
    PeriphClkInitStruct.PLL3.PLL3M = 25;
    PeriphClkInitStruct.PLL3.PLL3N = 246;
    PeriphClkInitStruct.PLL3.PLL3P = 10;
    PeriphClkInitStruct.PLL3.PLL3Q = 2;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
    PeriphClkInitStruct.Sai23ClockSelection = RCC_SAI23CLKSOURCE_PLL3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /*设置DAC采样率*/
    DAC_Port_Set_Sample_Rate(DAC_48KHZ_SR);
  }
  else
  {
    /*恢复外设时钟*/
    PeriphCommonClock_Config();

    /*恢复DAC采样率*/
    DAC_Port_Set_Sample_Rate(DAC_32KHZ_SR);
  }

  /*初始化SAI*/
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = AudioFrequency;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_OUTBLOCKA_ENABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }

  hsai_BlockA2.Instance = SAI2_Block_A;
  hsai_BlockA2.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockA2.Init.Synchro = SAI_SYNCHRONOUS_EXT_SAI1;
  hsai_BlockA2.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockA2.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA2.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA2.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA2.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockA2, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }

  /*重启SAI*/
  SAI_Port_Init();
}

/**
  ******************************************************************
  * @brief   SAI发送端开关静音
  * @param   [in]On_Off true 开启静音.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-11
  ******************************************************************
  */
void SAI_Port_Sender_Set_Mute(bool On_Off)
{
  if(On_Off == true)
  {
    HAL_SAI_EnableTxMuteMode(&hsai_BlockA1, SAI_ZERO_VALUE);
  }
  else
  {
    HAL_SAI_DisableTxMuteMode(&hsai_BlockA1);
  }
}

/**
  ******************************************************************
  * @brief   SAI启停数据处理
  * @param   [in]Is_Pause 为true 停止处理.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-25
  ******************************************************************
  */
void SAI_Port_Pause_Resume_Process(bool Is_Pause)
{
  /*暂停SAI传输*/
  if(Is_Pause == true)
  {
    SAI_Port_Sender_Set_Mute(true);
    HAL_SAI_DMAPause(&hsai_BlockA2);
    HAL_SAI_DMAPause(&hsai_BlockB1);
    HAL_SAI_DMAPause(&hsai_BlockA1);
  }
  /*恢复SAI传输*/
  else
  {
    HAL_SAI_DMAResume(&hsai_BlockA2);
    HAL_SAI_DMAResume(&hsai_BlockB1);
    HAL_SAI_DMAResume(&hsai_BlockA1);
    SAI_Port_Sender_Set_Mute(false);
  }
}

/**
  ******************************************************************
  * @brief   SAI启动数据处理
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-27
  ******************************************************************
  */
void SAI_Port_Start(void)
{
  if(SAI_DmaReceived_Flag == 0)
  {
    return;
  }
  SAI_DmaReceived_Flag = 0;

  LED_Port_Set_TP_Open();

  // memmove(MIC1_2_STEREO_Aidio_TempBuf, PCM_Data_Ptr[0], sizeof(int32_t)*STEREO_FRAME_SIZE);
  // memmove(MIC3_4_STEREO_Aidio_TempBuf, PCM_Data_Ptr[1], sizeof(int32_t)*STEREO_FRAME_SIZE);

  /*分离通道数据：(MIC4/3)-(MIC1/2)-L-R-L-R......*/
  for(int i = 0; i < MONO_FRAME_SIZE; i++)
  {
    MIC_Sample_Audio_Buf[3][i] = (int32_t)((*(PCM_Data_Ptr[0]+i*2+1))<<8);
    MIC_Sample_Audio_Buf[2][i] = (int32_t)((*(PCM_Data_Ptr[0]+i*2))<<8);
    MIC_Sample_Audio_Buf[0][i] = (int32_t)((*(PCM_Data_Ptr[1]+i*2+1))<<8);
    MIC_Sample_Audio_Buf[1][i] = (int32_t)((*(PCM_Data_Ptr[1]+i*2))<<8);
  }

  /*任务检查*/
  if(Miscell_Task_Table || Miscell_Init_Task_Table)
  {
    User_Miscell_Task_Process_Start(Tips_Audio_Buf);
    /*更新到DAC输出*/
    for(uint16_t index = 0; index < MONO_FRAME_SIZE; index++)
    {
      Tips_Audio_Send_Buf[index] = (int32_t)Tips_Audio_Buf[index] << 16;
    }
    DAC_Port_Play_Audio_Data(Tips_Audio_Send_Buf, Tips_Audio_Send_Buf, Send_Data_Ptr);
    return;
  }

  /*调用算法处理*/
  Algorithm_Port_Start((const int32_t (*)[])MIC_Sample_Audio_Buf, &Result_CH1_Data_Ptr, &Result_CH2_Data_Ptr);

  /* 发送16bit数据 */
#ifndef USE_USB_CDC_CONFIG
  #if ENABLE_EXPORT_FUNC == 1
    uint8_t MIC_Channel_Num;
    Algorithm_Port_Get_BF_Default_MIC(&MIC_Channel_Num);
    for(uint16_t index = 0; index < MONO_FRAME_SIZE; index++)
    {
      Export_Audio_Buf_CH1[index] = MIC_Sample_Audio_Buf[MIC_Channel_Num][index] >> 16;
      Export_Audio_Buf_CH2[index] = Result_CH2_Data_Ptr[index] >> 16;
    }
    /*更新USB音频数据*/
    USB_Audio_Port_Put_Data(Export_Audio_Buf_CH1, Export_Audio_Buf_CH2);
  #endif
#elif ENABLE_EXPORT_FUNC
  uint8_t MIC_Channel_Num;
  Algorithm_Port_Get_BF_Default_MIC(&MIC_Channel_Num);
  for(uint16_t index = 0; index < MONO_FRAME_SIZE; index++)
  {
    Export_Audio_Buf_CH1[index] = MIC_Sample_Audio_Buf[MIC_Channel_Num][index] >> 16;
    Export_Audio_Buf_CH2[index] = Result_CH2_Data_Ptr[index] >> 16;
  }
  /*数据更新到音频调试器*/
  Audio_Debug_Put_Data(Export_Audio_Buf_CH1, Export_Audio_Buf_CH2, 0);
#endif

  /*更新到DAC输出*/
  DAC_Port_Play_Audio_Data(Result_CH1_Data_Ptr, Result_CH2_Data_Ptr, Send_Data_Ptr);

  LED_Port_Set_TP_Close();
}

/**
  ******************************************************************
  * @brief   SAI操作初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
void SAI_Port_Init(void)
{
  /*启动双缓冲接收,双声道大小*/
  HAL_SAI_Receive_DMA(&hsai_BlockA2, (uint8_t *)&PCM_ChannelBuf[0][0], STEREO_FRAME_SIZE * 2);
  HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)&PCM_ChannelBuf[1][0], STEREO_FRAME_SIZE * 2);

  /*启动循环发送*/
  HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)PCM_SendBuf, SAI_PORT_DAC_SEND_DATA_SIZE_MAX * 2);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
