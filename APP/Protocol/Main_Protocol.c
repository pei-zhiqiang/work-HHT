/**
 *  @file Main_Protocol.c
 *
 *  @date 2021-01-01
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 通讯协议报文解析
 *
 *  @details 1、
 *
 *  @version V2.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Main_Protocol.h"
#include "Update_Protocol.h"
#include "File_Transfer_Protocol.h"
#include "utilities_crc.h"
#include "Parameter_Port.h"
#include "Utilities.h"
#include "utilities.h"
#include "Algorithm_Port.h"
/** Private macros -----------------------------------------------------------*/
/* 分包发送配置 */
#define ENABLE_DEBUG_PROTOCL          0       /**< 启动调试打印报文 */
#define ENABLE_SEND_DELAY             1       /**< 为1开启分包发送 */
#define ENABLE_SEND_DELAY_MS          100U    /**< 分包发送间隔ms >30ms */
#define ENABLE_SEND_DELAY_LIMIT_SIZE  200U    /**< >100Bytes时开启发送 */
#define SEND_ONE_PACKET_SIZE_MAX      100U    /**< 每包发送大小 */

/* 协议解析 */
#define NOT_FULL_TIMEOUT_SEC_MAX      15U     /**< 帧不满超时时间 */

#define GET_FRAME_SIZE_MAX            1024U
#define REPLY_FRAME_SIZE_MAX          2048U
#define FRAME_SIZE_MIN                8/**< 最小帧大小 */
#define IS_LESS_MIN_FRAME_SIZE(size)  ((size < FRAME_SIZE_MIN)?1:0)
#define LONG_FRAME_PACKAGE_LEN(data_len)   (data_len + 10)
#define SHORT_FRAME_PACKAGE_LEN       FRAME_SIZE_MIN

/* 寄存器地址 */
#define REG_SOFT_VER                  0x0000  /**< 软件版本 */
#define REG_VOL_PAR                   0x0001  /**< 音量参数 */
#define REG_EQ_PAR                    0x0002  /**< EQ参数 */
#define REG_WDRC_PAR                  0x0003  /**< WDRC参数 */
#define REG_AGC_PAR                   0x0004  /**< AGC参数 */
#define REG_DENOISE_PAR               0x0005  /**< 降噪参数 */
#define REG_ALGORI_SEL                0x0006  /**< 算法功能选择 */
#define REG_READ_ALL_PAR              0x0007  /**< 读取全部参数不带WOLA */
#define REG_UPDATE_ENTRY              0x0008  /**< 进入升级模式 */
#define REG_UPDATE_EXIT               0x0009  /**< 退出升级模式 */
#define REG_BATTERY_INFO              0x000A  /**< 电池电量信息 */
#define REG_BATTERY_CHARGE_STATE      0x000B  /**< 电池充电状态 */
#define REG_BF_ANGLE_PAR              0x000C  /**< BF入射角参数 */
#define REG_L_CHANNEL_COMPENSATION    0x000D  /**< 左耳音量补偿 */
#define REG_R_CHANNEL_COMPENSATION    0x000E  /**< 右耳音量补偿 */
#define REG_PREAMPLIFICATION_FACTOR   0x000F  /**< 前置放大系数 */
#define REG_EQ_BAND_PAR               0x0010  /**< EQ均衡器调节 */
#define REG_HEAR_REF_PAR              0x0011  /**< 左右耳听力曲线参数 */
#define REG_TEST_MODE_PAR             0x0012  /**< 听力测试参数 */
#define REG_AUTO_EQ_PAR               0x0013  /**< 自动EQ微调 */
#define REG_EQ_EN                     0x0014  /**< EQ使能开关 */
#define REG_WDRC_EN                   0x0015  /**< WDRC使能开关 */
#define REG_FILE_TRANSFER_PAR         0x0016  /**< 启停文件传输 */

#define REG_RESET_ALL_PAR_EN          0x0021  /**< 重置参数为默认值 */
#define REG_AUTO_SAVE_PAR_EN          0x0022  /**< 自动保存参数设置 */

#define REG_BF_EN                     0x0023  /**< BF使能开关 */
#define REG_VC_EN                     0x0024  /**< VC使能开关 */
#define REG_DENOISE_EN                0x0025  /**< DENOISE使能开关 */
#define REG_AGC_EN                    0x0026  /**< AGC使能开关 */
#define REG_EMD_EN                    0x0027  /**< EMD使能开关 */

#define REG_BF_DEFAULT_MIC_PAR        0x0028  /**< BF默认MIC参数 */

#define REG_MIC_SENSITIVITY           0x0029  /**< MIC灵敏度 */
#define REG_MIC_COMPENSATION          0x002A  /**< MIC补偿 */

#define REG_ALG_VER                   0x002B  /**< 算法版本 */
#define REG_SPK_COMPENSATION          0x002C  /**< 喇叭补偿 */
#define REG_FIR_EN                    0x002D  /**< FIR使能开关 */
#define REG_FIR_PAR                   0x002E  /**< FIR参数 */
#define REG_AEC_EN                    0x002F  /**< AEC使能开关 */
#define REG_AEC_PAR                   0x0030  /**< AEC参数 */
/** Private typedef ----------------------------------------------------------*/
/* 协议栈句柄 */
typedef struct
{
  Uart_Dev_Handle_t *Uart_Opt_Handle;         /**< 串口操作句柄 */
  uint32_t Update_Time;                       /**< 更新时间 */
  uint32_t NotFull_LastTime;                  /**< 上次帧不全允许超时时间 */
}PROTOCOL_STACK_HANDLE_Typedef_t;

/* 分包分送句柄 */
typedef struct
{
  PROTOCOL_STACK_HANDLE_Typedef_t *Stack_Opt_Handle_Ptr;
  uint32_t Last_Send_Time_ms;
  uint32_t Wait_Send_Size;
  uint32_t Current_Send_Index;
  uint32_t Data_Total_Size;
  uint8_t *Buf_Ptr;
}SEND_TASK_LIST_Typedef_t;

/* 寄存器处理映射 */
typedef RETURN_TYPE_Typedef_t (*REG_CALLBACK_FUNC_t)(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
typedef struct
{
  uint16_t reg_addr;
  REG_CALLBACK_FUNC_t func;
}REG_PROCESS_MAP_Typedef_t;

/** Public variables ---------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/
static PROTOCOL_STACK_HANDLE_Typedef_t *Stack_Opt_Handle = NULL;/**< 协议栈操作句柄 */

static PROTOCOL_STACK_HANDLE_Typedef_t Ble_StackHandle;
static PROTOCOL_STACK_HANDLE_Typedef_t USB_StackHandle;

static SEND_TASK_LIST_Typedef_t Send_Task_Handle;
/** Private function prototypes ----------------------------------------------*/
/*回复处理*/
static RETURN_TYPE_Typedef_t get_soft_version(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_algorithm_para(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_algorithm_func_switch(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t get_all_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_update_entry_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_update_exit_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t get_battery_info(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t get_battery_charge_state(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_left_channel_compensation_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_right_channel_compensation_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_preamplification_factor_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_eq_band_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_hear_ref_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_test_mode_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_auto_eq_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_get_algorithm_en_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_file_transfer_start_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_rest_all_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);
static RETURN_TYPE_Typedef_t set_par_auto_save_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data);

/* 解析处理 */
static RETURN_TYPE_Typedef_t Decode_Frame_Data(REPLY_FRAME_Typedef_t *reply_data);
static FRAME_CHECK_STATUS_Typedef_t Frame_Check_Parse(CQ_handleTypeDef *cb, FRAME_CHECK_RESULT_Typedef_t *result);
static void Reply_Data_Frame(REPLY_FRAME_Typedef_t *reply_data);
static int16_t Get_Reg_Index(uint16_t reg_addr);
static void Check_Frame_Not_Full(PROTOCOL_STACK_HANDLE_Typedef_t *pStack_Opt_Handle);
/** Private constants --------------------------------------------------------*/
/* 寄存器处理映射 */
static const REG_PROCESS_MAP_Typedef_t reg_process_map_list[] =
{
  {REG_SOFT_VER, get_soft_version},
  {REG_VOL_PAR, set_get_algorithm_para},
  {REG_EQ_PAR, set_get_algorithm_para},
  {REG_WDRC_PAR, set_get_algorithm_para},
  {REG_AGC_PAR, set_get_algorithm_para},
  {REG_DENOISE_PAR, set_get_algorithm_para},
  {REG_FIR_PAR, set_get_algorithm_para},
  {REG_AEC_PAR, set_get_algorithm_para},
  {REG_BF_DEFAULT_MIC_PAR, set_get_algorithm_para},
  {REG_BF_ANGLE_PAR, set_get_algorithm_para},
  {REG_MIC_SENSITIVITY, set_get_algorithm_para},
  {REG_MIC_COMPENSATION, set_get_algorithm_para},
  {REG_SPK_COMPENSATION, set_get_algorithm_para},
  {REG_ALG_VER, set_get_algorithm_para},
  {REG_ALGORI_SEL, set_get_algorithm_func_switch},
  {REG_READ_ALL_PAR, get_all_par},
  {REG_UPDATE_ENTRY, set_update_entry_par},
  {REG_UPDATE_EXIT, set_update_exit_par},
  {REG_BATTERY_INFO, get_battery_info},
  {REG_BATTERY_CHARGE_STATE, get_battery_charge_state},
  {REG_L_CHANNEL_COMPENSATION, set_get_left_channel_compensation_par},
  {REG_R_CHANNEL_COMPENSATION, set_get_right_channel_compensation_par},
  {REG_PREAMPLIFICATION_FACTOR, set_get_preamplification_factor_par},
  {REG_EQ_BAND_PAR, set_get_eq_band_par},
  {REG_HEAR_REF_PAR, set_get_hear_ref_par},
  {REG_TEST_MODE_PAR, set_get_test_mode_par},
  {REG_AUTO_EQ_PAR, set_get_auto_eq_par},
  {REG_EMD_EN, set_get_algorithm_en_par},
  {REG_BF_EN, set_get_algorithm_en_par},
  {REG_DENOISE_EN, set_get_algorithm_en_par},
  {REG_VC_EN, set_get_algorithm_en_par},
  {REG_EQ_EN, set_get_algorithm_en_par},
  {REG_WDRC_EN, set_get_algorithm_en_par},
  {REG_AGC_EN, set_get_algorithm_en_par},
  {REG_FIR_EN, set_get_algorithm_en_par},
  {REG_AEC_EN, set_get_algorithm_en_par},
  {REG_FILE_TRANSFER_PAR, set_file_transfer_start_par},
  {REG_RESET_ALL_PAR_EN, set_rest_all_par},
  {REG_AUTO_SAVE_PAR_EN, set_par_auto_save_par},
};

static const uint16_t reg_num_max = sizeof(reg_process_map_list)/sizeof( reg_process_map_list[0]);
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
  * @brief   待回复任务检测
  * @param   [in]force true强制发送.
  * @return  true 存在待回复任务.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-29
  ******************************************************************
  */
static bool Check_Wait_Send_Task(bool force)
{
  if(Send_Task_Handle.Wait_Send_Size == 0)
  {
    return false;
  }
  uint32_t Elapsed_Time_ms = Timer_Port_Get_Current_Time(TIMER_MS) - Send_Task_Handle.Last_Send_Time_ms;
  if(Elapsed_Time_ms < ENABLE_SEND_DELAY_MS && force == false)
  {
    return true;
  }
  Send_Task_Handle.Last_Send_Time_ms = Timer_Port_Get_Current_Time(TIMER_MS);
  uint16_t Can_Send_Size = (Send_Task_Handle.Data_Total_Size - Send_Task_Handle.Current_Send_Index)>SEND_ONE_PACKET_SIZE_MAX?SEND_ONE_PACKET_SIZE_MAX:(Send_Task_Handle.Data_Total_Size - Send_Task_Handle.Current_Send_Index);
  Uart_Port_Transmit_Data(Send_Task_Handle.Stack_Opt_Handle_Ptr->Uart_Opt_Handle, Send_Task_Handle.Buf_Ptr+Send_Task_Handle.Current_Send_Index, Can_Send_Size, 0);
  Send_Task_Handle.Current_Send_Index = (Send_Task_Handle.Current_Send_Index + SEND_ONE_PACKET_SIZE_MAX) > Send_Task_Handle.Data_Total_Size?Send_Task_Handle.Current_Send_Index:(Send_Task_Handle.Current_Send_Index + SEND_ONE_PACKET_SIZE_MAX);
  Send_Task_Handle.Wait_Send_Size -= Can_Send_Size;
  return true;
}

/**
  ******************************************************************
  * @brief   帧未满超时检测
  * @param   [in]pStack_Opt_Handle 协议栈句柄.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-22
  ******************************************************************
  */
static inline void Check_Frame_Not_Full(PROTOCOL_STACK_HANDLE_Typedef_t *pStack_Opt_Handle)
{
  uint32_t Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);
  if(Stack_Opt_Handle->NotFull_LastTime > 0)
  {
    if(Time_Sec != Stack_Opt_Handle->Update_Time)
    {
      Stack_Opt_Handle->Update_Time = Time_Sec;
      Stack_Opt_Handle->NotFull_LastTime--;
    }
  }
  else
  {
    CQ_ManualOffsetInc(pStack_Opt_Handle->Uart_Opt_Handle->cb, 1);
    Stack_Opt_Handle->NotFull_LastTime = NOT_FULL_TIMEOUT_SEC_MAX;
  }
}

/**
  ******************************************************************
  * @brief   协议解析入口
  * @param   [out]Reply_Data 发送数据信息.
  * @return  RETURN_TYPE_Typedef_t
  * @author  aron66
  * @version V1.0
  * @date    2021-05-27
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t Decode_Frame_Data(REPLY_FRAME_Typedef_t *Reply_Data)
{
  FRAME_CHECK_RESULT_Typedef_t result;
  RETURN_TYPE_Typedef_t ret = RETURN_OK;


  /*串口操作句柄为空时直接返回*/
  if(Stack_Opt_Handle->Uart_Opt_Handle == NULL)
  {
    return RETURN_ERROR;
  }

  /*升级模式检测*/
  if(Is_Upgrade_Mode_Entry(Stack_Opt_Handle->Uart_Opt_Handle) == true)
  {
    return Update_Frame_Decode_Start();
  }

  /*文件传输模式检测*/
  if(Is_File_Transfer_Mode_Entry(Stack_Opt_Handle->Uart_Opt_Handle) == true)
  {
    return File_Transfer_Frame_Decode_Start();
  }

  /*报文检测*/
  FRAME_CHECK_STATUS_Typedef_t check_state = Frame_Check_Parse(Stack_Opt_Handle->Uart_Opt_Handle->cb, &result);
  if(check_state != LONG_FRAME_CHECK_OK && check_state != SHORT_FRAME_CHECK_OK)
  {
    return RETURN_ERROR;
  }

  /*获得功能码*/
  uint8_t cmd = result.frame_data[3];
  uint16_t reg_addr = result.frame_data[5] << 8;
  reg_addr |= result.frame_data[4];
  uint8_t *data = result.frame_data + result.DataOffset;

  /*调用功能*/
  int16_t idx = Get_Reg_Index(reg_addr);
  if(idx == -1)
  {
    return RETURN_ERROR;
  }

  Reply_Data->cmd = (COMMAND_TYPE_Typedef_t)cmd;
  Reply_Data->frame_type = (FRAME_TYPE_Typedef_t)result.frame_data[2];
  ret = reg_process_map_list[idx].func(reg_addr, data, Reply_Data);
  if(ret != RETURN_OK)
  {
    return RETURN_ERROR;
  }

  /*发送回复数据*/
  Reply_Data_Frame(Reply_Data);

  return RETURN_OK;
}

/**
  ******************************************************************
  * @brief   报文检查
  * @param   [in]cb 缓冲区句柄
  * @param   [out]result 报文检测结果
  * @return  FRAME_CHECK_STATUS_Typedef_t 报文检测状态
  * @author  aron566
  * @version V1.0
  * @date    2020-01-01
  ******************************************************************
  */
static FRAME_CHECK_STATUS_Typedef_t Frame_Check_Parse(CQ_handleTypeDef *cb, FRAME_CHECK_RESULT_Typedef_t *result)
{
  static uint8_t databuf[GET_FRAME_SIZE_MAX];

  uint32_t rxnb = CQ_getLength(cb);
  if(IS_LESS_MIN_FRAME_SIZE(rxnb))
  {
    return FRAME_DATA_NOT_FULL;
  }

  /*判断帧头,跳过无效帧头*/
  if(CQ_ManualGet_Offset_Data(cb, 0) != 0x7A || CQ_ManualGet_Offset_Data(cb, 1) != 0x55)
  {
    CQ_ManualOffsetInc(cb, 1);
    if(CQ_skipInvaildU8Header(cb, 0x7A) == 0)
    {
      return FRAME_DATA_NOT_FULL;
    }
  }

  /*判断可读数据是否小于最小帧长*/
  rxnb = CQ_getLength(cb);
  if(IS_LESS_MIN_FRAME_SIZE(rxnb))
  {
    return FRAME_DATA_NOT_FULL;
  }

  /*判断帧类型*/
  uint8_t frame_type = CQ_ManualGet_Offset_Data(cb, 2);
  uint16_t length = 0;
  uint32_t package_len = 0;
  switch((FRAME_TYPE_Typedef_t)frame_type)
  {
    /*长帧*/
    case LONG_FRAME_TYPE:
      length = CQ_ManualGet_Offset_Data(cb, 7) << 8;
      length += CQ_ManualGet_Offset_Data(cb, 6);
      package_len = LONG_FRAME_PACKAGE_LEN(length);
      package_len = (package_len > GET_FRAME_SIZE_MAX)?GET_FRAME_SIZE_MAX:package_len;
      if(rxnb < package_len)
      {
        Check_Frame_Not_Full(Stack_Opt_Handle);
        return FRAME_DATA_NOT_FULL;
      }

      CQ_ManualGetData(cb, databuf, package_len);
      if(modbus_get_crc_result(databuf, package_len-2) == true)
      {
        result->frame_data = databuf;
        result->DataLen = length;
        result->DataOffset = 8;
        CQ_ManualOffsetInc(cb, package_len);
        return LONG_FRAME_CHECK_OK;
      }
      break;
    /*短帧*/
    case SHORT_FRAME_TYPE:
      package_len = SHORT_FRAME_PACKAGE_LEN;
      package_len = (package_len > GET_FRAME_SIZE_MAX)?GET_FRAME_SIZE_MAX:package_len;

      if(rxnb < package_len)
      {
        Check_Frame_Not_Full(Stack_Opt_Handle);
        return FRAME_DATA_NOT_FULL;
      }

      CQ_ManualGetData(cb, databuf, package_len);
      if(modbus_get_crc_result(databuf, package_len-2) == true)
      {
        result->frame_data = databuf;
        CQ_ManualOffsetInc(cb, package_len);
        return SHORT_FRAME_CHECK_OK;
      }
      break;
    default:
      /*TODO*/
      CQ_ManualOffsetInc(cb, 1);
      return UNKNOW_FRAME_ERROR;
  }

  CQ_ManualOffsetInc(cb, 1);
  return SHORT_FRAME_CRC_ERROR;

}

/**
  ******************************************************************
  * @brief   回复数据
  * @param   [out]reply_data 发送数据信息.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-1-5
  ******************************************************************
  */
static void Reply_Data_Frame(REPLY_FRAME_Typedef_t *reply_data)
{
  static uint8_t reply_buf[REPLY_FRAME_SIZE_MAX] MATH_PORT_SECTION("USE_DMA_BUF_SPACE") = {0x75, 0xAA};
  uint16_t index = 2;
  reply_buf[index++] = (uint8_t)reply_data->frame_type;
  reply_buf[index++] = (uint8_t)reply_data->cmd;

  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_buf[index++] = reply_data->data_len&0x00FF;
    reply_buf[index++] = reply_data->data_len>>8;
    for(uint16_t data_index = 0; data_index < reply_data->data_len; data_index++)
    {
      reply_buf[index++] = reply_data->data_buf[data_index];
    }
    uint16_t crc_val = modbus_crc_return_with_table(reply_buf, index);
    reply_buf[index++] = (uint8_t)(crc_val&0x00FF);
    reply_buf[index++] = (uint8_t)((crc_val>>8)&0xFF);
  }
  else if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    reply_buf[index++] = (uint8_t)reply_data->ack;
  }

  /*检测发送大小是否需要分包发送*/
#if ENABLE_SEND_DELAY
  if(index > ENABLE_SEND_DELAY_LIMIT_SIZE && Stack_Opt_Handle == &Ble_StackHandle)
  {
    Send_Task_Handle.Current_Send_Index = 0;
    Send_Task_Handle.Data_Total_Size = index;
    Send_Task_Handle.Wait_Send_Size = index;
    Send_Task_Handle.Stack_Opt_Handle_Ptr = Stack_Opt_Handle;
    Send_Task_Handle.Buf_Ptr = reply_buf;
    Send_Task_Handle.Last_Send_Time_ms = Timer_Port_Get_Current_Time(TIMER_MS);
    Check_Wait_Send_Task(true);
    return;
  }
#endif

  /*直接发送*/
  Uart_Port_Transmit_Data(Stack_Opt_Handle->Uart_Opt_Handle, reply_buf, index, 0);
#if ENABLE_DEBUG_PROTOCL
  debug_print(reply_buf, index);
#endif
}

/**
  ******************************************************************
  * @brief   获取功能码索引号
  * @param   [in]reg_addr 寄存器地址
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static int16_t Get_Reg_Index(uint16_t Reg_Addr)
{
  for(int16_t index = 0; index < reg_num_max; index++)
  {
    if(Reg_Addr == reg_process_map_list[index].reg_addr)
    {
      return index;
    }
  }
  return -1;
}

/**
  ******************************************************************
  * @brief   功能-获取软件版本
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  RETURN_OK 正常
  * @author  aron566
  * @version V1.0
  * @date    2020-01-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t get_soft_version(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = sizeof(APP_AIDS_Version);
    memcpy(reply_data->data_buf, &APP_AIDS_Version, sizeof(APP_AIDS_Version));
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   功能-设置算法启用选择
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2020-01-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_algorithm_func_switch(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    uint8_t indx = *((uint8_t *)data);
    Algorithm_Port_Function_Switch((int)indx);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  else if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 1;
    ALGORITHM_FUNCTION_Typdef_t Func_Sel;
    Algorithm_Port_Get_Function(&Func_Sel);
    reply_data->data_buf[0] = (uint8_t)Func_Sel;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   获取全部参数
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2022-03-09
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t get_all_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    /* 获取设备固件版本 */
    memcpy(&reply_data->data_buf[0], &APP_AIDS_Version, 2);
    reply_data->data_len = 2;

    /* 获取算法版本信息 */
    uint16_t AlgoVersion = Algorithm_Port_Get_Lib_Version();
    memmove(reply_data->data_buf + reply_data->data_len, &AlgoVersion, 2);
    reply_data->data_len += 2;

    /* 获取参数自动保存设置 */
    reply_data->data_buf[reply_data->data_len] = g_Miscellaneous_Par.Par_Auto_Save_Cs;
    reply_data->data_len += 1;

    /* 获取电量信息 */
    uint16_t Soc_Val = Voltameter_Port_Get_Battery_Electric_Quantity();
    reply_data->data_buf[reply_data->data_len] = (uint8_t)Soc_Val;
    reply_data->data_buf[reply_data->data_len+1] = (uint8_t)(Soc_Val>>8&0xFF);
    reply_data->data_len += 2;

    /* 获取电池充电状态 */
    reply_data->data_buf[reply_data->data_len] = Voltameter_Port_Get_Battery_Charge_State();
    reply_data->data_len += 1;

    /* 获取左耳补偿 */
    reply_data->data_buf[reply_data->data_len] = g_Miscellaneous_Par.Left_Channel_Compensation;
    reply_data->data_len += 1;
    /* 获取右耳补偿 */
    reply_data->data_buf[reply_data->data_len] = g_Miscellaneous_Par.Right_Channel_Compensation;
    reply_data->data_len += 1;
    /* 获取前置放大系数 */
    memmove(reply_data->data_buf+reply_data->data_len, &g_Miscellaneous_Par.Preamplification_Factor, 4);
    reply_data->data_len += 4;
    /* 获取EQ均衡器波段设置频率参数 */
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.EQ_5_Band_FQ, 10);
    reply_data->data_len += 10;
    /* 获取EQ均衡器波段设置频率对应增益参数 */
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.EQ_5_Band_Gain, 5);
    reply_data->data_len += 5;

    /* 获取MIC灵敏级 */
    Algorithm_Port_Get_MIC_Sensitivity_Level((int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;

    /* 获取MIC补偿值 */
    Algorithm_Port_Get_MIC_Compensation_Val(&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;

    /* 获取Speaker补偿值 */
    Algorithm_Port_Get_Test_Speaker_Par(250, (int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;
    Algorithm_Port_Get_Test_Speaker_Par(500, (int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;
    Algorithm_Port_Get_Test_Speaker_Par(1000, (int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;
    Algorithm_Port_Get_Test_Speaker_Par(2000, (int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;
    Algorithm_Port_Get_Test_Speaker_Par(4000, (int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;
    Algorithm_Port_Get_Test_Speaker_Par(8000, (int8_t *)&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;

    /* 获取双通道音量 */
    int8_t Level;
    Algorithm_Port_Get_VC_Level(&Level, ALGORITHM_CHANNEL_CH1);
    memmove(reply_data->data_buf + reply_data->data_len, &Level, 1);
    reply_data->data_len += 1;
    Algorithm_Port_Get_VC_Level(&Level, ALGORITHM_CHANNEL_CH2);
    memmove(reply_data->data_buf + reply_data->data_len, &Level, 1);
    reply_data->data_len += 1;

    /* 获取BF方向 */
    uint16_t BF_Angle;
    Algorithm_Port_Get_BF_Angle(&BF_Angle);
    reply_data->data_buf[reply_data->data_len] = (uint8_t)BF_Angle;
    reply_data->data_buf[reply_data->data_len + 1] = (uint8_t)(BF_Angle>>8&0xFF);
    reply_data->data_len += 2;

    /* 获取EQ参数 */
    int8_t dB[ALGORITHM_FREQ_BAND_NUM];
    for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
    {
      Algorithm_Port_Get_EQ_Par(&dB[i], i + 1, ALGORITHM_CHANNEL_CH1);
    }
    memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
    reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;

    for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
    {
      Algorithm_Port_Get_EQ_Par(&dB[i], i + 1, ALGORITHM_CHANNEL_CH2);
    }
    memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
    reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;

    /* 获取WDRC */
    uint8_t L_gain;
    uint8_t H_gain;
    for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
    {
      Algorithm_Port_Get_WDRC_Par((uint8_t *)dB, &L_gain, &H_gain, i + 1, ALGORITHM_CHANNEL_CH1);
      memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
      reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;
      memmove(reply_data->data_buf + reply_data->data_len, &L_gain, 1);
      reply_data->data_len += 1;
      memmove(reply_data->data_buf + reply_data->data_len, &H_gain, 1);
      reply_data->data_len += 1;
    }
    for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
    {
      Algorithm_Port_Get_WDRC_Par((uint8_t *)dB, &L_gain, &H_gain, i + 1, ALGORITHM_CHANNEL_CH2);
      memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
      reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;
      memmove(reply_data->data_buf + reply_data->data_len, &L_gain, 1);
      reply_data->data_len += 1;
      memmove(reply_data->data_buf + reply_data->data_len, &H_gain, 1);
      reply_data->data_len += 1;
    }

    /* 获取AGC */
    uint8_t UP_dB;
    uint8_t DOWN_dB;
    Algorithm_Port_Get_AGC_Par(&UP_dB, &DOWN_dB);
    memmove(reply_data->data_buf + reply_data->data_len, &UP_dB, 1);
    reply_data->data_len += 1;
    memmove(reply_data->data_buf + reply_data->data_len, &DOWN_dB, 1);
    reply_data->data_len += 1;

    /* 获取DNOISE Level */
    uint8_t Denoise_Level;
    Algorithm_Port_Get_DENOISE_Par(&Denoise_Level);
    memmove(reply_data->data_buf + reply_data->data_len, &Denoise_Level, 1);
    reply_data->data_len += 1;

    /* 获取FIR */
    int16_t Coeff_Q10[65] = {0}; uint8_t Order = 0;
    Algorithm_Port_Get_FIR_Par(Coeff_Q10, &Order);
    reply_data->data_buf[reply_data->data_len] = Order;
    reply_data->data_len += 1;
    memmove(reply_data->data_buf + reply_data->data_len, Coeff_Q10, ((uint16_t)Order + 1) * 2);
    reply_data->data_len += (((uint16_t)Order + 1) * 2);

    /* 获取AEC */
    uint16_t AEC_Level; uint8_t AEC_Order = 0;
    Algorithm_Port_Get_AEC_Par(&AEC_Level, &AEC_Order);
    memmove(reply_data->data_buf + reply_data->data_len, &AEC_Level, 2);
    reply_data->data_len += 2;
    reply_data->data_buf[reply_data->data_len] = AEC_Order;
    reply_data->data_len += 1;

    /* 获取当前算法功能 */
    ALGORITHM_FUNCTION_Typdef_t Func_Name;
    Algorithm_Port_Get_Function(&Func_Name);
    reply_data->data_buf[reply_data->data_len] = (uint8_t)Func_Name;
    reply_data->data_len += 1;

    /* BF使能状态 */
    uint8_t State;
    Algorithm_Port_Get_Function_State(BF_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* 降噪使能状态 */
    Algorithm_Port_Get_Function_State(DENOISE_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* EMD使能状态 */
    Algorithm_Port_Get_Function_State(MASK_EMD_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* EQ使能状态 */
    Algorithm_Port_Get_Function_State(EQ_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* VC使能状态 */
    Algorithm_Port_Get_Function_State(VOL_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* WDRC使能状态 */
    Algorithm_Port_Get_Function_State(WDRC_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* AGC使能状态 */
    Algorithm_Port_Get_Function_State(AGC_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* FIR使能状态 */
    Algorithm_Port_Get_Function_State(FIR_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* AEC使能状态 */
    Algorithm_Port_Get_Function_State(AEC_FUNC_SEL, &State);
    reply_data->data_buf[reply_data->data_len] = State;
    reply_data->data_len += 1;

    /* BF默认MIC */
    Algorithm_Port_Get_BF_Default_MIC(&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;

    /* WDRC平滑系数 */
    int16_t Smooth_Coeff[ALGORITHM_FREQ_BAND_NUM];
    Algorithm_Port_Get_Smooth_Coeff(Smooth_Coeff, WDRC_FUNC_SEL);
    memmove(reply_data->data_buf + reply_data->data_len, Smooth_Coeff, ALGORITHM_FREQ_BAND_NUM * 2);
    reply_data->data_len += (ALGORITHM_FREQ_BAND_NUM * 2);

    /* WDRC的Atk Rsl参数 */
    uint16_t Atk, Rsl;
    Algorithm_Port_Get_WDRC_Atk_Rsl_Par(&Atk, &Rsl);
    memmove(reply_data->data_buf + reply_data->data_len, &Atk, 2);
    reply_data->data_len += 2;
    memmove(reply_data->data_buf + reply_data->data_len, &Rsl, 2);
    reply_data->data_len += 2;

    /* AGC的Atk Rsl参数 */
    Algorithm_Port_Get_AGC_Atk_Rsl_Par(&Atk, &Rsl);
    memmove(reply_data->data_buf + reply_data->data_len, &Atk, 2);
    reply_data->data_len += 2;
    memmove(reply_data->data_buf + reply_data->data_len, &Rsl, 2);
    reply_data->data_len += 2;

    /* 子带能量 */
    uint8_t *dB_Ptr = NULL; uint8_t Subband_Num;
    Algorithm_Port_Get_Freqency_Domain_Energy(&dB_Ptr, &Subband_Num);
    memmove(reply_data->data_buf + reply_data->data_len, dB_Ptr, Subband_Num);
    reply_data->data_len += Subband_Num;

    /* 时域能量 */
    Algorithm_Port_Get_Time_Domain_Energy(&reply_data->data_buf[reply_data->data_len]);
    reply_data->data_len += 1;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   进入升级
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_update_entry_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    uint8_t mode = *((uint8_t *)data);
    Upgrade_Mode_Set(mode);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   退出升级
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_update_exit_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    uint8_t mode = *((uint8_t *)data);
    Upgrade_Mode_Reset(mode);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   获取电量信息
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t get_battery_info(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 2;
    uint16_t Soc_Val = Voltameter_Port_Get_Battery_Electric_Quantity();
    reply_data->data_buf[1] = (uint8_t)Soc_Val;
    reply_data->data_buf[0] = (uint8_t)(Soc_Val>>8&0xFF);
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   获取电池充电状态
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t get_battery_charge_state(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 1;
    reply_data->data_buf[0] = Voltameter_Port_Get_Battery_Charge_State();
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   左通道音量补偿
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-03-20
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_left_channel_compensation_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 1;
    reply_data->data_buf[0] = g_Miscellaneous_Par.Left_Channel_Compensation;
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    g_Miscellaneous_Par.Left_Channel_Compensation = pData[0];
    DAC_Port_Set_Vol_Gain(DAC_CHANNEL_LEFT, (float)g_Miscellaneous_Par.Left_Channel_Compensation);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   右通道音量补偿
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-03-20
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_right_channel_compensation_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 1;
    reply_data->data_buf[0] = g_Miscellaneous_Par.Right_Channel_Compensation;
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    g_Miscellaneous_Par.Right_Channel_Compensation = pData[0];
    DAC_Port_Set_Vol_Gain(DAC_CHANNEL_RIGHT, (float)g_Miscellaneous_Par.Right_Channel_Compensation);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   前置放大系数设置读取
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-03-20
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_preamplification_factor_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 4;
    memmove(reply_data->data_buf, &g_Miscellaneous_Par.Preamplification_Factor, 4);
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    memmove(&g_Miscellaneous_Par.Preamplification_Factor, data, 4);
    DAC_Port_Set_Preamp(DAC_CHANNEL_LEFT, g_Miscellaneous_Par.Preamplification_Factor);
    DAC_Port_Set_Preamp(DAC_CHANNEL_RIGHT, g_Miscellaneous_Par.Preamplification_Factor);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   EQ均衡器波段参数设置读取
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-04-25
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_eq_band_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    memmove(reply_data->data_buf, g_Miscellaneous_Par.EQ_5_Band_FQ, 10);
    reply_data->data_len = 10;
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.EQ_5_Band_Gain, 5);
    reply_data->data_len += 5;
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    memmove(g_Miscellaneous_Par.EQ_5_Band_FQ, pData, 10);
    memmove(g_Miscellaneous_Par.EQ_5_Band_Gain, pData+10, 5);
    DAC_Port_Set_EQ_Center_Frequency(g_Miscellaneous_Par.EQ_5_Band_FQ, g_Miscellaneous_Par.EQ_5_Band_Gain);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   左右耳听力曲线参数设置读取
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-05-10
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_hear_ref_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    memmove(reply_data->data_buf, g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain, 6);
    reply_data->data_len = 6;
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain, 6);
    reply_data->data_len += 6;
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain, 6);
    reply_data->data_len += 6;
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain, 6);
    reply_data->data_len += 6;
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    uint8_t index = 0;
    memmove(g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain, pData, 6);
    index += 6;
    memmove(g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain, pData+index, 6);
    index += 6;
    memmove(g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain, pData+index, 6);
    index += 6;
    memmove(g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain, pData+index, 6);
    DAC_Port_Set_Ref_Hear_Wave(g_Miscellaneous_Par.Left_6_Band_Ref_Min_Gain, g_Miscellaneous_Par.Right_6_Band_Ref_Min_Gain,
                               g_Miscellaneous_Par.Left_6_Band_Ref_Max_Gain, g_Miscellaneous_Par.Right_6_Band_Ref_Max_Gain);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   测试听力参数设置读取
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-05-17
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_test_mode_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    uint16_t Test_Hz = 0;
    uint8_t Test_dB = 0;
    uint8_t Test_Channel = 0;
    memmove(&Test_Hz, pData, 2);
    memmove(&Test_dB, pData + 2, 1);
    memmove(&Test_Channel, pData + 2 + 1, 1);
    if(Algorithm_Port_Update_Test_Par(Test_Hz, Test_dB, (ALGORITHM_CHANNEL_Typedef_t)Test_Channel) == false)
    {
      reply_data->ack = REPLY_ACK_ERROR;
    }
    else
    {
      reply_data->ack = REPLY_ACK_SET_OK;
    }
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   调节自动EQ参数
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-05-31
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_auto_eq_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    memmove(reply_data->data_buf, g_Miscellaneous_Par.Left_Auto_EQ_Gain, 6);
    reply_data->data_len = 6;
    memmove(reply_data->data_buf+reply_data->data_len, g_Miscellaneous_Par.Right_Auto_EQ_Gain, 6);
    reply_data->data_len += 6;
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    uint8_t index = 0;
    memmove(g_Miscellaneous_Par.Left_Auto_EQ_Gain, pData, 6);
    index += 6;
    memmove(g_Miscellaneous_Par.Right_Auto_EQ_Gain, pData+index, 6);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   使能算法设置
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-05-31
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_algorithm_en_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    switch(reg_addr)
    {
      case REG_AEC_EN:
        Algorithm_Port_Get_Function_State(AEC_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_EMD_EN:
        Algorithm_Port_Get_Function_State(MASK_EMD_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_FIR_EN:
        Algorithm_Port_Get_Function_State(FIR_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_BF_EN:
        Algorithm_Port_Get_Function_State(BF_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_DENOISE_EN:
        Algorithm_Port_Get_Function_State(DENOISE_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_VC_EN:
        Algorithm_Port_Get_Function_State(VOL_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_EQ_EN:
        Algorithm_Port_Get_Function_State(EQ_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_WDRC_EN:
        Algorithm_Port_Get_Function_State(WDRC_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      case REG_AGC_EN:
        Algorithm_Port_Get_Function_State(AGC_FUNC_SEL, &reply_data->data_buf[0]);
        reply_data->data_len = 1;
        return RETURN_OK;
      default:
        return RETURN_ERROR;
    }
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    switch(reg_addr)
    {
      case REG_AEC_EN:
        Algorithm_Port_Set_Function_State(AEC_FUNC_SEL, pData[0]);
        break;
      case REG_EMD_EN:
        Algorithm_Port_Set_Function_State(MASK_EMD_FUNC_SEL, pData[0]);
        break;
      case REG_FIR_EN:
        Algorithm_Port_Set_Function_State(FIR_FUNC_SEL, pData[0]);
        break;
      case REG_BF_EN:
        Algorithm_Port_Set_Function_State(BF_FUNC_SEL, pData[0]);
        break;
      case REG_DENOISE_EN:
        Algorithm_Port_Set_Function_State(DENOISE_FUNC_SEL, pData[0]);
        break;
      case REG_VC_EN:
        Algorithm_Port_Set_Function_State(VOL_FUNC_SEL, pData[0]);
        break;
      case REG_EQ_EN:
        Algorithm_Port_Set_Function_State(EQ_FUNC_SEL, pData[0]);
        break;
      case REG_WDRC_EN:
        Algorithm_Port_Set_Function_State(WDRC_FUNC_SEL, pData[0]);
        break;
      case REG_AGC_EN:
        Algorithm_Port_Set_Function_State(AGC_FUNC_SEL, pData[0]);
        break;
      default:
        return RETURN_ERROR;
    }
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   启停文件传输
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_file_transfer_start_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    File_Transfer_Protocol_Function_Enable(*pData);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   功能-读写操作算法参数
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-2-4
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_get_algorithm_para(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    switch(reg_addr)
    {
      case REG_AEC_PAR:
        {
          uint16_t Level;
          memmove(&Level, pData, 2);
          if(Algorithm_Port_Set_AEC_Par(Level, pData[2]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_FIR_PAR:
        {
          int16_t Coeff_Q10[65] = {0};
          uint8_t Order = pData[0];
          memmove(Coeff_Q10, &pData[1], (Order + 1) * 2);
          if(Algorithm_Port_Set_FIR_Par(Coeff_Q10, Order)== false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_SPK_COMPENSATION:
        {
          int8_t Set_dB[6], Out_dB[6];
          memmove(Set_dB, pData, 6);
          memmove(Out_dB, pData + 6, 6);
          if(Algorithm_Port_Set_Test_Speaker_Par(250, Set_dB[0], Out_dB[0]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          if(Algorithm_Port_Set_Test_Speaker_Par(500, Set_dB[1], Out_dB[1]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          if(Algorithm_Port_Set_Test_Speaker_Par(1000, Set_dB[2], Out_dB[2]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          if(Algorithm_Port_Set_Test_Speaker_Par(2000, Set_dB[3], Out_dB[3]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          if(Algorithm_Port_Set_Test_Speaker_Par(4000, Set_dB[4], Out_dB[4]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          if(Algorithm_Port_Set_Test_Speaker_Par(8000, Set_dB[5], Out_dB[5]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
        }
        reply_data->ack = REPLY_ACK_SET_OK;
        break;
      case REG_MIC_SENSITIVITY:
        if(Algorithm_Port_Set_MIC_Sensitivity_Level((int8_t)pData[0]) == false)
        {
          reply_data->ack = REPLY_ACK_ERROR;
          break;
        }
        reply_data->ack = REPLY_ACK_SET_OK;
        break;
      case REG_MIC_COMPENSATION:
        if(Algorithm_Port_Set_MIC_Compensation_Val(pData[0]) == false)
        {
          reply_data->ack = REPLY_ACK_ERROR;
          break;
        }
        reply_data->ack = REPLY_ACK_SET_OK;
        break;
      case REG_BF_DEFAULT_MIC_PAR:
        if(Algorithm_Port_Set_BF_Default_MIC(pData[0]) == false)
        {
          reply_data->ack = REPLY_ACK_ERROR;
          break;
        }
        reply_data->ack = REPLY_ACK_SET_OK;
        break;
      case REG_BF_ANGLE_PAR:
        {
          uint16_t Angle;
          memmove(&Angle, pData, 2);
          if(Algorithm_Port_Set_BF_Angle(Angle) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_VOL_PAR:
        {
          if(Algorithm_Port_Set_VC_Level(*(int8_t *)pData, ALGORITHM_CHANNEL_CH1) == false || \
          Algorithm_Port_Set_VC_Level(*(int8_t *)(pData + 1), ALGORITHM_CHANNEL_CH2) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_EQ_PAR:
        {
          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            if(Algorithm_Port_Set_EQ_Par(*(int8_t *)(pData + i), i + 1, ALGORITHM_CHANNEL_CH1) == false)
            {
              reply_data->ack = REPLY_ACK_ERROR;
              break;
            }
            if(Algorithm_Port_Set_EQ_Par(*(int8_t *)(pData + 4 + i), i + 1, ALGORITHM_CHANNEL_CH2) == false)
            {
              reply_data->ack = REPLY_ACK_ERROR;
              break;
            }
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_WDRC_PAR:
        {
          uint8_t Temp_Buf[ALGORITHM_FREQ_BAND_NUM + 2];
          uint16_t index = 0;
          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            memmove(Temp_Buf, pData + index, sizeof(Temp_Buf));
            if(Algorithm_Port_Set_WDRC_Par(Temp_Buf, Temp_Buf[ALGORITHM_FREQ_BAND_NUM], Temp_Buf[ALGORITHM_FREQ_BAND_NUM + 1], i + 1, ALGORITHM_CHANNEL_CH1) == false)
            {
              reply_data->ack = REPLY_ACK_ERROR;
              break;
            }
            index += sizeof(Temp_Buf);
          }
          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            memmove(Temp_Buf, pData + index, sizeof(Temp_Buf));
            if(Algorithm_Port_Set_WDRC_Par(Temp_Buf, Temp_Buf[ALGORITHM_FREQ_BAND_NUM], Temp_Buf[ALGORITHM_FREQ_BAND_NUM + 1], i + 1, ALGORITHM_CHANNEL_CH2) == false)
            {
              reply_data->ack = REPLY_ACK_ERROR;
              break;
            }
            index += sizeof(Temp_Buf);
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_AGC_PAR:
        {
          if(Algorithm_Port_Set_AGC_Par(pData[0], pData[1]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      case REG_DENOISE_PAR:
        {
          if(Algorithm_Port_Set_DENOISE_Par(pData[0]) == false)
          {
            reply_data->ack = REPLY_ACK_ERROR;
            break;
          }
          reply_data->ack = REPLY_ACK_SET_OK;
        }
        break;
      default:
        return RETURN_ERROR;
    }
  }
  else if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_len = 0;
    switch(reg_addr)
    {
      case REG_AEC_PAR:
        {
          /* 获取AEC Level Order*/
          uint16_t AEC_Level;
          Algorithm_Port_Get_AEC_Par(&AEC_Level, &reply_data->data_buf[2]);
          memmove(reply_data->data_buf + reply_data->data_len, &AEC_Level, 2);
          reply_data->data_len += 3;
        }
        break;
      case REG_FIR_PAR:
        {
          int16_t Coeff_Q10[65] = {0}; uint8_t Order = 0;
          Algorithm_Port_Get_FIR_Par(Coeff_Q10, &Order);
          reply_data->data_buf[reply_data->data_len] = Order;
          reply_data->data_len += 1;
          memmove(reply_data->data_buf + reply_data->data_len, Coeff_Q10, ((uint16_t)Order + 1) * 2);
          reply_data->data_len += (((uint16_t)Order + 1) * 2);
        }
        break;
      case REG_SPK_COMPENSATION:
        Algorithm_Port_Get_Test_Speaker_Par(250, (int8_t *)&reply_data->data_buf[0]);
        Algorithm_Port_Get_Test_Speaker_Par(500, (int8_t *)&reply_data->data_buf[1]);
        Algorithm_Port_Get_Test_Speaker_Par(1000, (int8_t *)&reply_data->data_buf[2]);
        Algorithm_Port_Get_Test_Speaker_Par(2000, (int8_t *)&reply_data->data_buf[3]);
        Algorithm_Port_Get_Test_Speaker_Par(4000, (int8_t *)&reply_data->data_buf[4]);
        Algorithm_Port_Get_Test_Speaker_Par(8000, (int8_t *)&reply_data->data_buf[5]);
        reply_data->data_len += 6;
        break;
      case REG_ALG_VER:
        {
          uint16_t AlgoVersion = Algorithm_Port_Get_Lib_Version();
          memmove(reply_data->data_buf, &AlgoVersion, 2);
          reply_data->data_len += 2;
        }
        break;
      case REG_MIC_SENSITIVITY:
        Algorithm_Port_Get_MIC_Sensitivity_Level((int8_t *)&reply_data->data_buf[0]);
        reply_data->data_len += 1;
        break;
      case REG_MIC_COMPENSATION:
        Algorithm_Port_Get_MIC_Compensation_Val(&reply_data->data_buf[0]);
        reply_data->data_len += 1;
        break;
      case REG_BF_DEFAULT_MIC_PAR:
        Algorithm_Port_Get_BF_Default_MIC(&reply_data->data_buf[0]);
        reply_data->data_len += 1;
        break;
      case REG_BF_ANGLE_PAR:
        {
          /* 获取BF方向 */
          uint16_t BF_Angle;
          Algorithm_Port_Get_BF_Angle(&BF_Angle);
          reply_data->data_buf[reply_data->data_len] = (uint8_t)BF_Angle;
          reply_data->data_buf[reply_data->data_len + 1] = (uint8_t)(BF_Angle>>8&0xFF);
          reply_data->data_len += 2;
        }
        break;
      case REG_VOL_PAR:
        {
          /* 获取双通道音量 */
          int8_t Level;
          Algorithm_Port_Get_VC_Level(&Level, ALGORITHM_CHANNEL_CH1);
          memmove(reply_data->data_buf + reply_data->data_len, &Level, 1);
          reply_data->data_len += 1;
          Algorithm_Port_Get_VC_Level(&Level, ALGORITHM_CHANNEL_CH2);
          memmove(reply_data->data_buf + reply_data->data_len, &Level, 1);
          reply_data->data_len += 1;
        }
        break;
      case REG_EQ_PAR:
        {
          /* 获取EQ参数 */
          int8_t dB[ALGORITHM_FREQ_BAND_NUM];
          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            Algorithm_Port_Get_EQ_Par(&dB[i], i + 1, ALGORITHM_CHANNEL_CH1);
          }
          memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
          reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;

          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            Algorithm_Port_Get_EQ_Par(&dB[i], i + 1, ALGORITHM_CHANNEL_CH2);
          }
          memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
          reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;
        }
        break;
      case REG_WDRC_PAR:
        {
          /* 获取WDRC */
          uint8_t L_gain;
          uint8_t H_gain;
          uint8_t dB[ALGORITHM_FREQ_BAND_NUM];
          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            Algorithm_Port_Get_WDRC_Par((uint8_t *)dB, &L_gain, &H_gain, i + 1, ALGORITHM_CHANNEL_CH1);
            memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
            reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;
            memmove(reply_data->data_buf + reply_data->data_len, &L_gain, 1);
            reply_data->data_len += 1;
            memmove(reply_data->data_buf + reply_data->data_len, &H_gain, 1);
            reply_data->data_len += 1;
          }
          for(uint8_t i = 0; i < ALGORITHM_FREQ_BAND_NUM; i++)
          {
            Algorithm_Port_Get_WDRC_Par((uint8_t *)dB, &L_gain, &H_gain, i + 1, ALGORITHM_CHANNEL_CH2);
            memmove(reply_data->data_buf + reply_data->data_len, dB, ALGORITHM_FREQ_BAND_NUM);
            reply_data->data_len += ALGORITHM_FREQ_BAND_NUM;
            memmove(reply_data->data_buf + reply_data->data_len, &L_gain, 1);
            reply_data->data_len += 1;
            memmove(reply_data->data_buf + reply_data->data_len, &H_gain, 1);
            reply_data->data_len += 1;
          }
        }
        break;
      case REG_AGC_PAR:
        {
          /* 获取AGC */
          uint8_t UP_dB;
          uint8_t DOWN_dB;
          Algorithm_Port_Get_AGC_Par(&UP_dB, &DOWN_dB);
          memmove(reply_data->data_buf + reply_data->data_len, &UP_dB, 1);
          reply_data->data_len += 1;
          memmove(reply_data->data_buf + reply_data->data_len, &DOWN_dB, 1);
          reply_data->data_len += 1;
        }
        break;
      case REG_DENOISE_PAR:
        {
          /* 获取DNOISE Level */
          uint8_t Denoise_Level;
          Algorithm_Port_Get_DENOISE_Par(&Denoise_Level);
          memmove(reply_data->data_buf + reply_data->data_len, &Denoise_Level, 1);
          reply_data->data_len += 1;
        }
        break;
      default:
        return RETURN_ERROR;
    }
  }
  return RETURN_OK;
}

/**
  ******************************************************************
  * @brief   功能-重置参数为默认值
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-6-29
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_rest_all_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    Parameter_Port_Reset_Task_En(pData[0]);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
}

/**
  ******************************************************************
  * @brief   参数自动保存设置
  * @param   [in]reg_addr 寄存器地址
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-08-10
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t set_par_auto_save_par(uint16_t reg_addr, void *data, REPLY_FRAME_Typedef_t *reply_data)
{
  uint8_t *pData = (uint8_t *)data;
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == GET_PAR_CMD)
  {
    reply_data->data_buf[0] = g_Miscellaneous_Par.Par_Auto_Save_Cs;
    reply_data->data_len = 1;
    return RETURN_OK;
  }
  if((COMMAND_TYPE_Typedef_t)reply_data->cmd == SET_PAR_CMD)
  {
    Parameter_Port_Auto_Save_Set(pData[0]);
    reply_data->ack = REPLY_ACK_SET_OK;
    return RETURN_OK;
  }
  return RETURN_ERROR;
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
  * @brief   启动协议栈
  * @param   [in]None.
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-1-5
  ******************************************************************
  */
void Protocol_Stack_Start(void)
{
  static REPLY_FRAME_Typedef_t reply_data = {0};

  /*分包回复任务检测*/
  if(Check_Wait_Send_Task(false) == true)
  {
    return;
  }

  /*蓝牙数据解析*/
  Stack_Opt_Handle = &Ble_StackHandle;
  Decode_Frame_Data(&reply_data);

  /*USB数据解析*/
  Stack_Opt_Handle = &USB_StackHandle;
  Decode_Frame_Data(&reply_data);
}

/**
  ******************************************************************
  * @brief   协议栈初始化
  * @param   [in]None.
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-1-5
  ******************************************************************
  */
void Protocol_Stack_Init(void)
{
  /*获取蓝牙串口操作句柄*/
  memset(&Ble_StackHandle, 0, sizeof(PROTOCOL_STACK_HANDLE_Typedef_t));
  Ble_StackHandle.NotFull_LastTime = NOT_FULL_TIMEOUT_SEC_MAX;
  Ble_StackHandle.Uart_Opt_Handle = Uart_Port_Get_Handle(UART_NUM_6);
  // if(Ble_StackHandle.Uart_Opt_Handle == NULL)
  // {
  //   printf("get uart opt ble handle faild.\r\n");
  //   return;
  // }

  /*获取USB串口操作句柄*/
  memset(&USB_StackHandle, 0, sizeof(PROTOCOL_STACK_HANDLE_Typedef_t));
  USB_StackHandle.NotFull_LastTime = NOT_FULL_TIMEOUT_SEC_MAX;
  USB_StackHandle.Uart_Opt_Handle = Uart_Port_Get_Handle(UART_NUM_0);
  if(USB_StackHandle.Uart_Opt_Handle == NULL)
  {
    printf("get uart opt usb handle faild.\r\n");
    return;
  }

  /*更新协议初始化*/
  Upgrade_Protocol_Init();

  /*文件传输协议初始化*/
  File_Transfer_Protocol_Init();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
