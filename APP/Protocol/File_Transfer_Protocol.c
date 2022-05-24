/**
 *  @file File_Transfer_Protocol.c
 *
 *  @date 2021-05-21
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 文件传输协议
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "File_Transfer_Protocol.h"
#include "UART_Port.h"
#include "utilities_crc.h"
#include "utilities.h"
#include "Timer_Port.h"
#include "Flash_Port.h"
#include "Tips_Audio_Port.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define USE_FRAME_NOT_FULL_TIME_OUT_MODULE  1       /**< 是否使用帧超时检测模块*/
#define EXIT_STACK_TIMEOUT_SEC              15U     /**< 协议栈超时退出时间秒*/
#define ACK_FRAME_LEN                       5       /**< ACK帧长*/
#define FRAME_SIZE_MAX                      256
#define SHORT_FRAME_PACKAGE_LEN             8       /**< 短帧长度*/
#define IS_LESS_MIN_FRAME_SIZE(size)        ((size < SHORT_FRAME_PACKAGE_LEN)?1:0)
#define LONG_FRAME_PACKAGE_LEN(data_len)    (data_len == 0?10:data_len+10)
#define GET_SHORT_FRAME_PACKAGE_LEN(package_num)  ((package_num == 0)?72:SHORT_FRAME_PACKAGE_LEN)
#define GET_SHORT_FRAME_PACKAGE_DATA_SIZE(package_num) ((package_num == 0)?64:0)
#define GET_SHORT_FRAME_PACKAGE_DATA_OFFSET(package_num) ((package_num == 0)?6:0)
#define WAIT_DATA_ACK_TIMEOUT               5U      /**< 等待ACK超时时间s*/
#define RETRY_NUM_MAX                       3U      /**< 超时重试次数*/
#define NOT_FULL_TIMEOUT_SEC_MAX            15U     /**< 允许帧不全超时时间*/

/* 包重复检测 */
#define PACKAGE_REPEAT_CHECK_SIZE           10U      /**< 10包循环检测 */
#define UPDATE_PACKAGE_NUM_MAX              4096+1U /**< 最大传输包数*/

/** Private typedef ----------------------------------------------------------*/
/*协议栈句柄*/
typedef struct
{
  Uart_Dev_Handle_t *Uart_Opt_Handle;               /**< 串口操作句柄*/
#if USE_FRAME_NOT_FULL_TIME_OUT_MODULE
  uint32_t File_Transfer_Time;                      /**< 更新时间*/
  uint32_t NotFull_LastTime;                        /**< 上次帧不全允许超时时间*/
  uint32_t Stack_Run_Start_Time;                    /**< 协议栈启动时间记录*/
  bool Stack_Run_State;                             /**< 协议栈运行状态*/
#endif
}PROTOCOL_STACK_HANDLE_Typedef_t;

/*回复报文*/
typedef struct
{
  uint16_t                  frame_head;
  FRAME_TYPE_Typedef_t      frame_type;
  COMMAND_TYPE_Typedef_t    cmd;
  uint16_t                  frame_num;
  uint16_t                  data_len;
  uint8_t                   data_buf[256];
  REPLY_ACK_Typedef_t       ack;
  uint16_t                  crc;
}UPGRADE_REPLY_FRAME_Typedef_t;

/*更新区域类型*/
typedef enum
{
  DOWN_LOAD_TYPE = 0,
  FACTORY_TYPE,
}UPDATE_FRIMWARE_TYPE_Typedef_t;

typedef enum
{
  RETURN_ACK = 0,
  RETURN_NO_ACK,
  RETURN_UNKNOW_ERR,
}PEROCESS_RESULT_Typedef_t;

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static PROTOCOL_STACK_HANDLE_Typedef_t *Stack_Opt_Handle = NULL;/**< 协议栈操作句柄*/
static PROTOCOL_STACK_HANDLE_Typedef_t Ble_StackHandle;

static volatile uint8_t File_Transfer_Enable_State = 0;

static UPGRADE_REPLY_FRAME_Typedef_t reply_data;
static FRAME_CHECK_RESULT_Typedef_t result;
static uint8_t File_Transfer_State_Record[PACKAGE_REPEAT_CHECK_SIZE];
/** Private function prototypes ----------------------------------------------*/
static FRAME_CHECK_STATUS_Typedef_t File_Transfer_Frame_Parse(CQ_handleTypeDef *cb, FRAME_CHECK_RESULT_Typedef_t *result);
static PEROCESS_RESULT_Typedef_t Process_Upgrade(uint16_t Frame_Num, uint8_t *Data, UPGRADE_REPLY_FRAME_Typedef_t *Reply_Data);
static void Reply_Master(UPGRADE_REPLY_FRAME_Typedef_t *Reply_Data);
static RETURN_TYPE_Typedef_t Wait_Data_Ack(REPLY_ACK_Typedef_t Ack_Val);
static uint32_t File_Data_Get(uint8_t *Dest_Buf, uint32_t Size, uint32_t Packet_Num_Cnt);
static bool Download_File_Data(const uint8_t *data, uint16_t Packet_Num);
static bool Upload_File_Data(UPGRADE_REPLY_FRAME_Typedef_t *Reply_Data);
#if USE_FRAME_NOT_FULL_TIME_OUT_MODULE
  static void Check_Frame_Not_Full(PROTOCOL_STACK_HANDLE_Typedef_t *pStack_Opt_Handle);
#endif
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
  * @brief   帧未满超时检测
  * @param   [in]pStack_Opt_Handle 协议栈句柄.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-22
  ******************************************************************
  */
#if USE_FRAME_NOT_FULL_TIME_OUT_MODULE
static inline void Check_Frame_Not_Full(PROTOCOL_STACK_HANDLE_Typedef_t *pStack_Opt_Handle)
{
  uint32_t Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);
  if(Stack_Opt_Handle->NotFull_LastTime > 0)
  {
    if(Time_Sec != Stack_Opt_Handle->File_Transfer_Time)
    {
      Stack_Opt_Handle->File_Transfer_Time = Time_Sec;
      Stack_Opt_Handle->NotFull_LastTime--;
    }
  }
  else
  {
    CQ_ManualOffsetInc(pStack_Opt_Handle->Uart_Opt_Handle->cb, 1);
    Stack_Opt_Handle->NotFull_LastTime = NOT_FULL_TIMEOUT_SEC_MAX;
  }
}
#endif

/**
  ******************************************************************
  * @brief   传输帧解析
  * @param   [out]reply_data 发送数据信息.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static FRAME_CHECK_STATUS_Typedef_t File_Transfer_Frame_Parse(CQ_handleTypeDef *cb, FRAME_CHECK_RESULT_Typedef_t *result)
{
  static uint8_t databuf[FRAME_SIZE_MAX];

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
  uint16_t package_num = 0;
  switch((FRAME_TYPE_Typedef_t)frame_type)
  {
    /*长帧*/
    case LONG_FRAME_TYPE:
      length = CQ_ManualGet_Offset_Data(cb, 7) << 8;
      length += CQ_ManualGet_Offset_Data(cb, 6);
      package_len = LONG_FRAME_PACKAGE_LEN(length);
      package_len = (package_len > FRAME_SIZE_MAX)?FRAME_SIZE_MAX:package_len;
      if(rxnb < package_len)
      {
#if USE_FRAME_NOT_FULL_TIME_OUT_MODULE
        Check_Frame_Not_Full(Stack_Opt_Handle);
#endif
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
      package_num = CQ_ManualGet_Offset_Data(cb, 5) << 8;
      package_num += CQ_ManualGet_Offset_Data(cb, 4);
      package_len = GET_SHORT_FRAME_PACKAGE_LEN(package_num);
      package_len = (package_len > FRAME_SIZE_MAX)?FRAME_SIZE_MAX:package_len;
      if(rxnb < package_len)
      {
#if USE_FRAME_NOT_FULL_TIME_OUT_MODULE
        Check_Frame_Not_Full(Stack_Opt_Handle);
#endif
        return FRAME_DATA_NOT_FULL;
      }

      CQ_ManualGetData(cb, databuf, package_len);
      if(modbus_get_crc_result(databuf, package_len-2) == true)
      {
        result->frame_data = databuf;
        result->DataLen = GET_SHORT_FRAME_PACKAGE_DATA_SIZE(package_num);
        result->DataOffset = GET_SHORT_FRAME_PACKAGE_DATA_OFFSET(package_num);
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
  * @brief   文件传输报文处理
  * @param   [in]frame_num 帧号
  * @param   [in]data 数据
  * @param   [out]reply_data 回复数据存储区
  * @return  AnalyzRslt
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static PEROCESS_RESULT_Typedef_t Process_Upgrade(uint16_t Frame_Num, uint8_t *Data, UPGRADE_REPLY_FRAME_Typedef_t *Reply_Data)
{
  if((COMMAND_TYPE_Typedef_t)Reply_Data->cmd == FILE_UPGRADE_CMD)
  {
    /*写入文件信息*/
    if(Frame_Num == 0x0000)
    {
      char File_Name[64];
      uint32_t File_Size = 0;
      /* 剔除非法空格 */
      common_replace_special_char((char *)Data, ' ', '_', 64);
      common_replace_special_char((char *)Data, '+', ' ', 64);
      int ret = sscanf((const char *)Data, "%s %u", File_Name, &File_Size);
      if(ret == 2)
      {
        if(Tips_Audio_Port_Write_New_File_Info(File_Name, File_Size) < 0)
        {
          Reply_Data->ack = REPLY_ACK_ERROR;
          return RETURN_ACK;
        }
        if(Tips_Audio_Port_Erase_File(File_Name) == 0)
        {
          Reply_Data->ack = REPLY_ACK_SET_OK;
          return RETURN_ACK;
        }
      }
    }
    /*写入文件流*/
    else if(Frame_Num > 0 && Frame_Num < 0xFFFF)
    {
      if(Frame_Num >= UPDATE_PACKAGE_NUM_MAX)
      {
        Reply_Data->ack = REPLY_ACK_ERROR;
        return RETURN_ACK;
      }

      /* 每 PACKAGE_REPEAT_CHECK_SIZE 清空标记 */
      if((Frame_Num % PACKAGE_REPEAT_CHECK_SIZE) == 0)
      {
        memset(File_Transfer_State_Record, 0, sizeof(File_Transfer_State_Record));
      }

      /*检查是否重复更新包号*/
      if(File_Transfer_State_Record[Frame_Num % PACKAGE_REPEAT_CHECK_SIZE] == 1)
      {
        Reply_Data->ack = REPLY_ACK_GET_DATA_OK;
        return RETURN_ACK;
      }

      /*写入文件流*/
      if(Download_File_Data((const uint8_t *)Data, Frame_Num) == false)
      {
        Reply_Data->ack = REPLY_ACK_ERROR;
        return RETURN_ACK;
      }

      /* 标记 */
      File_Transfer_State_Record[Frame_Num % PACKAGE_REPEAT_CHECK_SIZE] = 1;
      Reply_Data->ack = REPLY_ACK_GET_DATA_OK;
      return RETURN_ACK;
    }
    /*文件流结束*/
    else if(Frame_Num == 0xFFFF)
    {
      Reply_Data->ack = REPLY_ACK_END_OK;
      return RETURN_ACK;
    }
    return RETURN_UNKNOW_ERR;
  }
  else if((COMMAND_TYPE_Typedef_t)Reply_Data->cmd == FILE_UPLOAD_CMD)
  {
    /*首包读取文件信息*/
    if(Frame_Num == 0x0000)
    {
      memset(Reply_Data->data_buf, 0, 128);
      const char *File_Name = (const char *)Data;
      uint32_t File_Size = Tips_Audio_Port_Get_File_Size(File_Name);
      if(File_Size == 0)
      {
        return RETURN_UNKNOW_ERR;
      }
      strncpy((char *)Reply_Data->data_buf, File_Name, 64);
      snprintf((char *)Reply_Data->data_buf+strlen(File_Name), 64, "+%u", File_Size);
      Reply_Data->data_len = 128;
      Reply_Data->frame_num = Frame_Num;
      return RETURN_ACK;
    }
    /*发送文件流*/
    else if(Frame_Num == 0xFFFF)
    {
      if(Upload_File_Data(Reply_Data) == true)
      {
        return RETURN_NO_ACK;
      }
    }
    return RETURN_UNKNOW_ERR;
  }
  return RETURN_UNKNOW_ERR;
}

/**
  ******************************************************************
  * @brief   数据下载写入
  * @param   [in]data 数据
  * @param   [in]Packet_Num 包号
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021-05-24
  ******************************************************************
  */
static bool Download_File_Data(const uint8_t *data, uint16_t Packet_Num)
{
  int ret = Tips_Audio_Port_Write_Data(data, (Packet_Num-1)*128, 128);
  if(ret < 0)
  {
    //printf("write file faild.\r\n");
    return false;
  }
  return true;
}

/**
  ******************************************************************
  * @brief   文件上传
  * @param   [out]reply_data 回复数据存储区
  * @return  true 传输成功
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static bool Upload_File_Data(UPGRADE_REPLY_FRAME_Typedef_t *Reply_Data)
{
  int Fill = 0;
  uint32_t Packet_Num_Cnt = 1;
  RETURN_TYPE_Typedef_t ret = RETURN_OK;
  uint32_t Error_Cnt = 0;
  while(1)
  {
    /*清空缓冲区*/
    CQ_emptyData(Stack_Opt_Handle->Uart_Opt_Handle->cb);

    /*填充文件流到发送区*/
    Fill = File_Data_Get(Reply_Data->data_buf, 128, Packet_Num_Cnt);

    /*发送文件流*/
    Reply_Data->data_len = 128;
    Reply_Data->frame_num = Packet_Num_Cnt;
    Reply_Master(Reply_Data);

    /*等待ACK*/
    ret = Wait_Data_Ack(REPLY_ACK_GET_DATA_OK);
    switch(ret)
    {
      case RETURN_OK:
        Packet_Num_Cnt++;
        Error_Cnt = 0;
        if(Fill < 128)
        {
          Error_Cnt = 0;
          /*发送结束帧并等待ACK*/
          while(1)
          {
            Reply_Data->data_len = 0;
            Reply_Data->frame_num = 0xFFFF;
            Reply_Master(Reply_Data);
            ret = Wait_Data_Ack(REPLY_ACK_END_OK);
            if(RETURN_OK == ret)
            {
              return true;
            }
            else
            {
              Error_Cnt++;
              if(Error_Cnt >= RETRY_NUM_MAX)
              {
                return false;
              }
            }
          }
        }
        break;
      case RETURN_TIMEOUT:
      default:
        Error_Cnt++;
        break;
    }

    if(Error_Cnt >= RETRY_NUM_MAX)
    {
      return false;
    }
  }
}

/********************************************************************
 * @brief   文件数据发送
 * @param   [out]dest_buf 数据存储区
 * @param   [in]size 存储区大小
 * @param   [in]Packet_Num_Cnt 包号
 * @return  填充大小
 * @author  aron566
 * @version V1.0
 * @date    2021-03-01
 ********************************************************************
 */
static uint32_t File_Data_Get(uint8_t *Dest_Buf, uint32_t Size, uint32_t Packet_Num_Cnt)
{
  uint32_t File_Size = 0, Fill_size = 0;

  /*依据size填充数据和本次的包号进行发送*/
  Tips_Audio_Port_Read_Data(Dest_Buf, (Packet_Num_Cnt-1)*Size, Size);
  File_Size = Tips_Audio_Port_Get_Current_Opt_File_Size();
  Fill_size = Packet_Num_Cnt*Size >= File_Size?Size-((Packet_Num_Cnt*Size)-File_Size):Size;
  return Fill_size;
}

/**
  ******************************************************************
  * @brief   回复数据
  * @param   [out]reply_data 发送数据信息.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static void Reply_Master(UPGRADE_REPLY_FRAME_Typedef_t *Reply_Data)
{
  static uint8_t reply_buf[FRAME_SIZE_MAX] = {0x75, 0xAA};
  uint16_t index = 2;
  reply_buf[index++] = (uint8_t)Reply_Data->frame_type;
  reply_buf[index++] = (uint8_t)Reply_Data->cmd;

  /*上载文件*/
  if((COMMAND_TYPE_Typedef_t)Reply_Data->cmd == FILE_UPLOAD_CMD)
  {
    reply_buf[index++] = Reply_Data->frame_num&0x00FF;
    reply_buf[index++] = Reply_Data->frame_num>>8;
    reply_buf[index++] = Reply_Data->data_len&0x00FF;
    reply_buf[index++] = Reply_Data->data_len>>8;
    for(uint16_t data_index = 0; data_index < Reply_Data->data_len; data_index++)
    {
      reply_buf[index++] = Reply_Data->data_buf[data_index];
    }
    uint16_t crc_val = modbus_crc_return_with_table(reply_buf, index);
    reply_buf[index++] = (uint8_t)(crc_val&0x00FF);
    reply_buf[index++] = (uint8_t)((crc_val>>8)&0xFF);
  }

  /*更新文件*/
  else if((COMMAND_TYPE_Typedef_t)Reply_Data->cmd == FILE_UPGRADE_CMD)
  {
    reply_buf[index++] = (uint8_t)Reply_Data->ack;
  }
  /*发送*/
  Uart_Port_Transmit_Data(Stack_Opt_Handle->Uart_Opt_Handle, reply_buf, index, 100);
}

/**
  ******************************************************************
  * @brief   等待主机ACK应答
  * @param   [in]等待ACK数值.
  * @return  RETURN_TYPE_Typedef_t.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
static RETURN_TYPE_Typedef_t Wait_Data_Ack(REPLY_ACK_Typedef_t Ack_Val)
{
  uint32_t LastTime = Timer_Port_Get_Current_Time(TIMER_SEC);
  uint8_t temp_buf[ACK_FRAME_LEN];
  uint32_t len = 0;
  while(1)
  {
    /*检测超时*/
    if((Timer_Port_Get_Current_Time(TIMER_SEC) - LastTime) >= WAIT_DATA_ACK_TIMEOUT)
    {
      return RETURN_TIMEOUT;
    }

    /*判断帧头,跳过无效帧头*/
    if(CQ_ManualGet_Offset_Data(Stack_Opt_Handle->Uart_Opt_Handle->cb, 0) != 0x7A)
    {
      if(CQ_skipInvaildU8Header(Stack_Opt_Handle->Uart_Opt_Handle->cb, 0x7A) < ACK_FRAME_LEN)
      {
        continue;
      }
    }

    /*检查缓冲区长度*/
    len = CQ_getLength(Stack_Opt_Handle->Uart_Opt_Handle->cb);
    if(len < ACK_FRAME_LEN)
    {
      continue;
    }
    CQ_ManualGetData(Stack_Opt_Handle->Uart_Opt_Handle->cb, temp_buf, ACK_FRAME_LEN);
    if((REPLY_ACK_Typedef_t)temp_buf[4] != Ack_Val)
    {
      CQ_ManualOffsetInc(Stack_Opt_Handle->Uart_Opt_Handle->cb, 1);
      return RETURN_ERROR;
    }
    else
    {
      CQ_ManualOffsetInc(Stack_Opt_Handle->Uart_Opt_Handle->cb, len);
      return RETURN_OK;
    }
  }
}

/**
  ******************************************************************
  * @brief   更新传输超时计时
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-19
  ******************************************************************
  */
#if USE_UPGRADE_TIME_OUT_MODULE
static void inline File_Transfer_Last_Time(void)
{
  Last_File_Transfer_Ok_Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);
}
#endif
/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/

/**
  ******************************************************************
  * @brief   启动传输协议解析
  * @param   [in]None.
  * @return  RETURN_TYPE_Typedef_t
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
RETURN_TYPE_Typedef_t File_Transfer_Frame_Decode_Start(void)
{
  /*串口操作句柄为空时直接返回*/
  if(Stack_Opt_Handle->Uart_Opt_Handle == NULL)
  {
    return RETURN_ERROR;
  }

  /*报文检测*/
  FRAME_CHECK_STATUS_Typedef_t check_state = File_Transfer_Frame_Parse(Stack_Opt_Handle->Uart_Opt_Handle->cb, &result);
  if(check_state != LONG_FRAME_CHECK_OK && check_state != SHORT_FRAME_CHECK_OK)
  {
    return RETURN_ERROR;
  }

  /*获得功能码*/
  uint8_t cmd = result.frame_data[3];
  uint16_t frame_num = result.frame_data[5] << 8;
  frame_num |= result.frame_data[4];
  uint8_t *data = result.frame_data + result.DataOffset;

  /*处理报文*/
  reply_data.cmd = (COMMAND_TYPE_Typedef_t)cmd;
  reply_data.frame_type = (FRAME_TYPE_Typedef_t)result.frame_data[2];
  PEROCESS_RESULT_Typedef_t ret = Process_Upgrade(frame_num, data, &reply_data);
  if(ret == RETURN_UNKNOW_ERR)
  {
    return RETURN_ERROR;
  }

  /*答复*/
  if(ret == RETURN_ACK)
  {
    Reply_Master(&reply_data);
  }

  return RETURN_UPGRADE_FRAME_REDIRECT;
}

/**
  ******************************************************************
  * @brief   检测是否进入文件传输报文解析
  * @param   [in]Uart_Opt_Handle 串口句柄
  * @return  true 是
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
bool Is_File_Transfer_Mode_Entry(Uart_Dev_Handle_t *Uart_Opt_Handle)
{
  Stack_Opt_Handle->Uart_Opt_Handle = Uart_Opt_Handle;

  /*协议运行超时检测*/
  if((Timer_Port_Get_Current_Time(TIMER_SEC) - Stack_Opt_Handle->Stack_Run_Start_Time) > EXIT_STACK_TIMEOUT_SEC)
  {
    File_Transfer_Protocol_Function_Enable(false);
  }

  /*判断帧头,跳过无效帧头*/
  if(CQ_ManualGet_Offset_Data(Stack_Opt_Handle->Uart_Opt_Handle->cb, 0) != 0x7A || CQ_ManualGet_Offset_Data(Stack_Opt_Handle->Uart_Opt_Handle->cb, 1) != 0x55)
  {
    if(CQ_skipInvaildU8Header(Stack_Opt_Handle->Uart_Opt_Handle->cb, 0x7A) == 0)
    {
      return false;
    }
  }

  /*判断可读数据是否小于最小帧长*/
  uint32_t len = CQ_getLength(Stack_Opt_Handle->Uart_Opt_Handle->cb);
  if(IS_LESS_MIN_FRAME_SIZE(len))
  {
    return false;
  }

  /*判断帧类型*/
  COMMAND_TYPE_Typedef_t cmd_type = (COMMAND_TYPE_Typedef_t)CQ_ManualGet_Offset_Data(Stack_Opt_Handle->Uart_Opt_Handle->cb, 3);

  bool Is_File_Transfer_Mode = (cmd_type == FILE_UPGRADE_CMD || cmd_type == FILE_UPLOAD_CMD)?true:false;

  if(Is_File_Transfer_Mode == true)
  {
    /*更新协议栈运行时间*/
    Stack_Opt_Handle->Stack_Run_Start_Time = Timer_Port_Get_Current_Time(TIMER_SEC);
  }

  return Is_File_Transfer_Mode;
}

/**
  ******************************************************************
  * @brief   文件传输模式使能
  * @param   [in]Cs True 使能.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-27
  ******************************************************************
  */
void File_Transfer_Protocol_Function_Enable(bool Cs)
{
  if(Cs == Stack_Opt_Handle->Stack_Run_State)
  {
    return;
  }
#if USE_FRAME_NOT_FULL_TIME_OUT_MODULE
  /*重置帧不全计时*/
  Stack_Opt_Handle->NotFull_LastTime = NOT_FULL_TIMEOUT_SEC_MAX;
#endif

  /*启停音频处理*/
  SAI_Port_Pause_Resume_Process(Cs);

  /*重置更新状态*/
  memset(File_Transfer_State_Record, 0, sizeof(File_Transfer_State_Record));

  Stack_Opt_Handle->Stack_Run_Start_Time = Timer_Port_Get_Current_Time(TIMER_SEC);
  Stack_Opt_Handle->Stack_Run_State = Cs;
}

/**
  ******************************************************************
  * @brief   文件传输协议栈初始化
  * @param   [in]None.
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
void File_Transfer_Protocol_Init(void)
{
  /*获取串口操作句柄*/
  memset(&Ble_StackHandle, 0, sizeof(PROTOCOL_STACK_HANDLE_Typedef_t));
  Ble_StackHandle.NotFull_LastTime = NOT_FULL_TIMEOUT_SEC_MAX;
  Ble_StackHandle.Uart_Opt_Handle = Uart_Port_Get_Handle(UART_NUM_6);
  // if(Ble_StackHandle.Uart_Opt_Handle == NULL)
  // {
  //   printf("get uart opt handle faild.\r\n");
  //   return;
  // }

  Stack_Opt_Handle = &Ble_StackHandle;
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
