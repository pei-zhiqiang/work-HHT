/**
 *  @file User_Main.c
 *
 *  @date 2021-01-11
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 主线任务
 *
 *  @details v0.4.3 减少文件升级及固件升级用于标记的缓冲区过大问题，减少空间占用分解为10包检测重复.
 *           v0.4.4 升级状态时停止音频输出.
 *           v0.4.5 分区表修改.
 *           v0.4.6 适配新硬件板.
 *           v0.4.7 修改默认WDRC参数，修复用户默认参数失败问题，环形缓冲区DSP优化加速.
 *           v0.4.8 4ms+4ms+硬件延时1.5 ~ 1.8ms版本，取消每1s打印输出能量数值.
 *           v0.4.9 算法更新至v0.0.2，单VC算法，且EMD关闭，默认时域VC计算.
 *           v0.4.10 修复AGC算法使能未起作用.
 *           v0.4.11 修复拔掉耳机未进入低功耗模式问题.
 *           v0.4.12 协议增加对MIC灵敏级、补偿值，speaker补偿值的设置，修复VC音量调节问题，算法更新至v0.0.4.
 *           v0.4.13 算法更新至v0.0.5.
 *           v0.4.14 修改硬件EQ作用路径改为DAC.
 *           v0.4.15 算法版本更新至v0.0.6.
 *           v0.5.0  算法版本更新至v0.0.7增加FIR滤波器.
 *           v0.5.1 增加FIR协议.
 *           v0.5.2 修复算法参数溢出问题.
 *           v0.5.3 修复EMD无法使能问题.
 *           v0.5.4 修复降噪失效问题，优化降噪空间占用，算法更新至v0.0.8.
 *           v0.5.5 增加触摸屏UI.
 *           v0.5.6 增加UI（音量，降噪，BF算法控制功能）
 *  @version v0.5.6
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "User_Main.h"
#include "main.h"
#include "Tips_Audio_Port.h"
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/* 算法库版本信息 */
#define APP_AIDI_VER_MAJOR              0U        /**< v0. */
#define APP_AIDI_VER_MINOR              5U        /**< x. */
#define APP_AIDI_VER_REVISION           6U        /**< x */
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/* 固件版本信息 */
const uint16_t APP_AIDS_Version = ((uint16_t)APP_AIDI_VER_MAJOR << 8) | \
                                  ((uint16_t)APP_AIDI_VER_MINOR << 4) | \
                                  ((uint16_t)APP_AIDI_VER_REVISION);

/*其他任务*/
MISCELL_INIT_TASK_LIST_TABLE_Typedef_t Miscell_Init_Task_Table = NO_MISCELL_INIT_TASK_RUNNING;
MISCELL_TASK_LIST_TABLE_Typedef_t Miscell_Task_Table = NO_MISCELL_TASK_RUNNING;
/** Private variables --------------------------------------------------------*/
#ifdef USE_USB_CDC_CONFIG
/* 串口句柄 */
static Uart_Dev_Handle_t *Uart_Handle = NULL;
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
  ******************************************************************
  * @brief   发送数据接口
  * @param   Data 数据.
  * @param   Len 数据字节数.
  * @return  已发送长度.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-02
  ******************************************************************
  */
static uint32_t Send_Data_Func_Port(uint8_t *Data, uint32_t Len)
{
//  int16_t *Ptr = (int16_t *)Data;
//  /*分离左右通道数据*/
//  int16_t Left_Audio[MONO_FRAME_SIZE], Right_Audio[MONO_FRAME_SIZE];
//  int index = 0;
//  for(int i = 0; i < MONO_FRAME_SIZE; i++)
//  {
//    Left_Audio[i] = Ptr[index++];
//    Right_Audio[i] = Ptr[index++];
//  }
//  /*发送音频数据到USB*/
//  USB_Audio_Port_Put_Data(Left_Audio, Right_Audio, Len/sizeof(int16_t));

  /* 发送音频数据到串口 */
  Uart_Port_Transmit_Data(Uart_Handle, Data, (uint16_t)Len, 0);
  return Len;
}

/**
  ******************************************************************
  * @brief   获取发送空闲状态
  * @param   None.
  * @return  true 空闲.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-02
  ******************************************************************
  */
static bool Get_Idel_State_Port(void)
{
  UART_STATE_Typedef_t State = UART_Port_Get_Idel_State(Uart_Handle);
  if(State == UART_PORT_TX_BUSY || State == UART_PORT_RX_TX_BUSY)
  {
    return false;
  }
  return true;
}
#endif

/**
 * @brief 助听器算法参数重置
 *
 */
static void User_Algorithm_Par_RESET_Port(void)
{
  /* 通知算法硬件前置增益值 */
  Algorithm_Port_Set_MIC_Compensation_Val(0);

  /* 设置MIC灵敏级 + 模拟MIC REF电压补偿 */
  Algorithm_Port_Set_MIC_Sensitivity_Level(ALGORITHM_MIC_SENSITIVITY_LEVEL);

  /* 设置算法模式功能 */
#if USE_ALGORITHM_FUNCTION
  Algorithm_Port_Set_Function(FUNC_MAX_SEL);
#else
  Algorithm_Port_Set_Function(NO_FUNC_SEL);
#endif

  /* 设置AGC参数,类时域wdrc */
  // uint8_t dB[4] = {0, 70, 100, 120};
  // Algorithm_Port_Set_AGC_Par_V2(dB, 0, 0, 35, 10);

  /* 设置AGC参数 */
  Algorithm_Port_Set_AGC_Par(110, 10);
  // uint8_t dB[4] = {0, 70, 100, 120};
  // Algorithm_Port_Set_AGC_Par_V2(dB, 0, 0, 35, 10);

  /* 设置测听参数 */
  // Algorithm_Port_Set_Test_Speaker_Par(1000, 56-65);
  //Algorithm_Port_Update_Test_Par(1000, 40, ALGORITHM_CHANNEL_CH1);

  /* 设置校准30s */
  Algorithm_Port_Set_Calibration_Time(30);

  /* 设置音量等级 */
  Algorithm_Port_Set_VC_Level(3, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Set_VC_Level(3, ALGORITHM_CHANNEL_CH2);

  /* 设置耳机补偿参数 */
  Algorithm_Port_Set_Test_Speaker_Par(250, 47, 105);
  Algorithm_Port_Set_Test_Speaker_Par(500, 43, 104);
  Algorithm_Port_Set_Test_Speaker_Par(1000, 47, 110);
  Algorithm_Port_Set_Test_Speaker_Par(2000, 49, 112);
  Algorithm_Port_Set_Test_Speaker_Par(4000, 47, 114);
  Algorithm_Port_Set_Test_Speaker_Par(8000, 45, 97);

  /* 设置算法使能 */
  // Algorithm_Port_Set_Function_State(DENOISE_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(EQ_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(WDRC_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(VOL_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(AGC_FUNC_SEL, 0);

  /* 设置EQ参数 */
  // Algorithm_Port_Set_EQ_Par(-20, 1, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(-6, 2, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(0, 3, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(8, 4, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(10, 5, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(-20, 1, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(-6, 2, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(0, 3, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(8, 4, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(10, 5, ALGORITHM_CHANNEL_CH2);
}

/**
  ******************************************************************
  * @brief   算法接口切换功能
  * @param   [in]Function_Sel 算法选择.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
void Algorithm_Port_Function_Switch(int Function_Sel)
{
#if USE_ALGORITHM_FUNCTION
  ALGORITHM_FUNCTION_Typdef_t Algorithm_Func_Select;
  Algorithm_Port_Get_Function(&Algorithm_Func_Select);
  /* 进入 */
  if((ALGORITHM_FUNCTION_Typdef_t)Function_Sel == TEST_FUNC_SEL)
  {
    /* 重置SIN WAVE数据 */
    Algorithm_Port_Reset_Test_Wave_Data();

    /* 建立播放提示音任务 */
    Miscell_Init_Task_Table |= PLAY_TIPS_AUDIO_TASK_INIT;
    Miscell_Task_Table |= PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK;
  }

  /* 退出 */
  if((ALGORITHM_FUNCTION_Typdef_t)Function_Sel != TEST_FUNC_SEL && Algorithm_Func_Select == TEST_FUNC_SEL)
  {
    /* 配置为16Khz采样率 */
    SAI_Port_Reset_SAI1_Init(SAI_AUDIO_FREQUENCY_32K);
    Algorithm_Port_Set_SAMPLE_FS(32000);

    /* 建立播放提示音任务 */
    Miscell_Init_Task_Table |= PLAY_TIPS_AUDIO_TASK_INIT;
    Miscell_Task_Table |= PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK;
  }
  else
  {
    /* TODO:Nothing */
  }

  Algorithm_Port_Set_Function((ALGORITHM_FUNCTION_Typdef_t)Function_Sel);
#endif
}

/**
  ******************************************************************
  * @brief   辅助任务处理
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-21
  ******************************************************************
  */
void User_Miscell_Task_Process_Start(void *Arg)
{
  TASK_PROCESS_RET_Typedef_t ret = PROCESS_RESULT_OK;

  /* 初始化任务检测 */
  if(Miscell_Init_Task_Table & PLAY_TIPS_AUDIO_TASK_INIT)
  {
    Tips_Audio_Port_Init(Miscell_Task_Table);
    Miscell_Init_Task_Table &= ~PLAY_TIPS_AUDIO_TASK_INIT;
  }

  /* 主任务检测 */
  if(Miscell_Task_Table & PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK)
  {
    ret = Tips_Audio_Port_Start((int16_t *)Arg, PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK);
    if(ret == PROCESS_RESULT_OK || ret == PROCESS_RESULT_ERROR)
    {
      Miscell_Task_Table &= ~PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK;
      /* 配置为48Khz采样率 */
      SAI_Port_Reset_SAI1_Init(SAI_AUDIO_FREQUENCY_48K);
      Algorithm_Port_Set_SAMPLE_FS(48000);
    }
    return;
  }
  if(Miscell_Task_Table & PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK)
  {
    ret = Tips_Audio_Port_Start((int16_t *)Arg, PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK);
    if(ret == PROCESS_RESULT_OK || ret == PROCESS_RESULT_ERROR)
    {
      Miscell_Task_Table &= ~PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK;
    }
    return;
  }
  if(Miscell_Task_Table & PLAY_ENTRY_LINE_COMPENSATION_AUDIO_TIPS_TASK)
  {
    ret = Tips_Audio_Port_Start((int16_t *)Arg, PLAY_ENTRY_LINE_COMPENSATION_AUDIO_TIPS_TASK);
    if(ret == PROCESS_RESULT_OK || ret == PROCESS_RESULT_ERROR)
    {
      Miscell_Task_Table &= ~PLAY_ENTRY_LINE_COMPENSATION_AUDIO_TIPS_TASK;
    }
    return;
  }
  if(Miscell_Task_Table & PLAY_EXIT_LINE_COMPENSATION_AUDIO_TIPS_TASK)
  {
    ret = Tips_Audio_Port_Start((int16_t *)Arg, PLAY_EXIT_LINE_COMPENSATION_AUDIO_TIPS_TASK);
    if(ret == PROCESS_RESULT_OK || ret == PROCESS_RESULT_ERROR)
    {
      Miscell_Task_Table &= ~PLAY_EXIT_LINE_COMPENSATION_AUDIO_TIPS_TASK;
    }
    return;
  }
}

/**
  ******************************************************************
  * @brief   任务处理循环
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-25
  ******************************************************************
  */
void User_Main_Task_Process_Loop(void)
{
  for(;;)
  {
    /* 启动协议栈 */
    Protocol_Stack_Start();

    /* 启动SAI */
    SAI_Port_Start();

    /* 启动DAC */
    DAC_Port_Play_Audio_Start();

    /* 启动定时器模块 */
    Timer_Port_Start();

    /* 启动littleVGL任务 */
    LCD_Port_Start();

#ifdef USE_USB_CDC_CONFIG
    /* 启动音频调试器 */
    Audio_Debug_Start();
#endif
  }
  /* never return */
}

/**
  ******************************************************************
  * @brief   初始化各模块功能
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-25
  ******************************************************************
  */
void User_Main_Task_Init(void)
{
  /* 错误追踪初始化 */
  CM_Backtrace_Port_Init();

  /* 串口操作初始化 */
  Uart_Port_Init();

  /* Flash接口初始化 */
  Flash_Port_Init();

  /* 定时器初始化 */
  Timer_Port_Init();

  /* 协议栈初始化 */
  Protocol_Stack_Init();

  /* 算法接口初始化 */
  Algorithm_Port_Init();

  /* 获取固件版本 */
  printf("Firmware Version: v%u.%u.%u\r\n", (APP_AIDS_Version >> 16) & 0xFF, \
        (APP_AIDS_Version & 0xF0) >> 4, \
        APP_AIDS_Version & 0x0F);

  /* 获取算法版本 */
  printf("Aid Version: v%u.%u.%u\r\n", (Algorithm_Port_Get_Lib_Version() >> 16) & 0xFF, \
        (Algorithm_Port_Get_Lib_Version() & 0xF0) >> 4, \
        Algorithm_Port_Get_Lib_Version() & 0x0F);

  /* 设置用户参数重置接口 */
  Algorithm_Port_Set_RESET_Callback(User_Algorithm_Par_RESET_Port);

  /* 用户参数设置 */
  User_Algorithm_Par_RESET_Port();

  /* 参数接口初始化 */
  Parameter_Port_Init();

  /* 电量计初始化 */
  Voltameter_Port_Init();

  /* 陀螺仪初始化 */
  Gyroscope_Port_Init();

  /* 按键检测初始化 */
  PressKey_Port_Init();

  /* ADC接口初始化 */
  //ADC_Port_Init();

  /* DAC初始化 */
  DAC_Port_Init();

  /* SAI初始化 */
  SAI_Port_Init();

  /* 触摸初始化 */
  TP_Port_Init();

  /* LCD初始化 */
  LCD_Port_Init();

  /* other initialization task code */

#ifdef USE_USB_CDC_CONFIG
  /*获取串口句柄*/
  Uart_Handle = Uart_Port_Get_Handle(UART_NUM_0);
  /*初始化音频调试接口*/
  /*音频调试缓冲区*/
  static int16_t Debug_Auido_Buf[MONO_FRAME_SIZE * 8];
  Audio_Debug_Init((uint16_t *)Debug_Auido_Buf, Send_Data_Func_Port, Get_Idel_State_Port);
#endif
}

/**
  ******************************************************************
  * @brief   中断向量表迁移
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-25
  ******************************************************************
  */
void User_InterruptVectorTable_Move(void)
{
  SCB->VTOR = (FLASH_BANK1_BASE+(128*1024));
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
