/**
 *  @file Tips_Audio_Port.c
 *
 *  @date 2021年05月21日 15:33:50 星期五
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 播放提示音频.
 *
 *  @details 每音频分区前32个字节存放音频大小，有效字节4个.
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Tips_Audio_Port.h"
#include "Utilities.h"
#include "utilities.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
#define AUDIO_PATITION_OFFSET_SIZE                    32U/**< 32字节偏移：起始4个字节文件大小，后面对齐*/
#define AUDIO_PARTITION_SIZE_MAX                      (512*1024U-AUDIO_PATITION_OFFSET_SIZE)/**< 音频数据分区最大字节数*/

#define ENTRY_CONFIG_AUDIO_PARTITION_NAME             "entry_config.pcm"
#define EXIT_CONFIG_AUDIO_PARTITION_NAME              "exit_config.pcm"
#define ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME  ENTRY_CONFIG_AUDIO_PARTITION_NAME
#define EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME   EXIT_CONFIG_AUDIO_PARTITION_NAME
/** Private constants --------------------------------------------------------*/

/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
/*flash音频接收区*/
volatile int16_t Flash_Audio_Data[STEREO_FRAME_SIZE] MATH_PORT_SECTION("USE_DMA_BUF_SPACE") = {0};
static int Play_Tips_Audio_Data_Size = 0;
static int Play_Tips_Audio_Data_Index = 0;

static uint32_t Get_Audio_Data_Cnt = 0;

static char Current_Opt_File_Name[24] = ENTRY_CONFIG_AUDIO_PARTITION_NAME;
static uint32_t Current_Opt_File_Size = 0;
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
  * @brief   读取音频数据
  * @param   [in]None.
  * @return  -1读取出错 0读取完成.
  * @author  aron566
  * @version V1.0
  * @date    2020-05-21
  ******************************************************************
  */
static int Read_Audio_Data(const char *Partition_Name)
{
  /*外部Flash取音频*/
  Play_Tips_Audio_Data_Size = (Play_Tips_Audio_Data_Size >= (Flash_Port_Get_Partition_Size(Partition_Name)-AUDIO_PATITION_OFFSET_SIZE))?AUDIO_PARTITION_SIZE_MAX:Play_Tips_Audio_Data_Size;
  uint32_t Size = (Play_Tips_Audio_Data_Size >= Play_Tips_Audio_Data_Index)?Play_Tips_Audio_Data_Size - Play_Tips_Audio_Data_Index:0;
  Size = (Size > sizeof(int16_t)*MONO_FRAME_SIZE)?sizeof(int16_t)*MONO_FRAME_SIZE:Size;

  /*再次触发读取*/
  if(Get_Audio_Data_Cnt < Play_Tips_Audio_Data_Size && Play_Tips_Audio_Data_Index >= sizeof(int16_t)*MONO_FRAME_SIZE)
  {
    if(Flash_Port_Read_Partition_Data(Partition_Name, (uint8_t *)Flash_Audio_Data+(Get_Audio_Data_Cnt % (STEREO_FRAME_SIZE*sizeof(int16_t))), AUDIO_PATITION_OFFSET_SIZE+Play_Tips_Audio_Data_Index, Size) < 0)
    {
      return -1;
    }
    Get_Audio_Data_Cnt += Size;
  }

  Play_Tips_Audio_Data_Index += (sizeof(int16_t)*MONO_FRAME_SIZE);
  if(Play_Tips_Audio_Data_Index >= Play_Tips_Audio_Data_Size)
  {
    memset((uint8_t *)Flash_Audio_Data+(Play_Tips_Audio_Data_Size % (STEREO_FRAME_SIZE*sizeof(int16_t))), 0, (sizeof(int16_t)*MONO_FRAME_SIZE) - Size);
    return 0;
  }
  return Play_Tips_Audio_Data_Index;
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
  * @brief   音频文件大小
  * @param   [in]None
  * @return  文件大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
uint32_t Tips_Audio_Port_Get_Current_Opt_File_Size(void)
{
  return Current_Opt_File_Size;
}

/**
  ******************************************************************
  * @brief   读取音频文件
  * @param   [in]Dest_Buf 文件流存储区域
  * @param   [in]Offset 偏移
  * @param   [in]Size 读取文件流大小
  * @return  -1失败，读取长度
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
int Tips_Audio_Port_Read_Data(uint8_t *Dest_Buf, uint32_t Offset, uint32_t Size)
{
  return Flash_Port_Read_Partition_Data(Current_Opt_File_Name, Dest_Buf+AUDIO_PATITION_OFFSET_SIZE, Offset, Size);
}

/**
  ******************************************************************
  * @brief   写入音频文件
  * @param   [in]Data 文件流
  * @param   [in]Offset 偏移
  * @param   [in]Size 文件流大小
  * @return  -1失败，写入长度
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
int Tips_Audio_Port_Write_Data(const uint8_t *Data, uint32_t Offset, uint32_t Size)
{
  return Flash_Port_Write_Partition_Data(Current_Opt_File_Name, Data, Offset+AUDIO_PATITION_OFFSET_SIZE, Size);
}

/**
  ******************************************************************
  * @brief   删除音频文件
  * @param   [in]File_Name 文件名
  * @return  0成功.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
int Tips_Audio_Port_Erase_File(const char *File_Name)
{
  if(Flash_Port_Erase_Partition_Data(File_Name, 0, Current_Opt_File_Size+AUDIO_PATITION_OFFSET_SIZE) == false)
  {
    return -1;
  }

  /*先写入文件大小*/
  if(Flash_Port_Write_Partition_Data(Current_Opt_File_Name, (const uint8_t*)&Current_Opt_File_Size, 0, 4) < 0)
  {
    return -1;
  }
  return 0;
}

/**
  ******************************************************************
  * @brief   写入新音频文件信息
  * @param   [in]File_Name 文件名
  * @param   [in]File_Size 文件大小
  * @return  0成功.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
int Tips_Audio_Port_Write_New_File_Info(const char *File_Name, uint32_t File_Size)
{

  if(strncmp(File_Name, ENTRY_CONFIG_AUDIO_PARTITION_NAME, strlen(ENTRY_CONFIG_AUDIO_PARTITION_NAME)) == 0 ||
     strncmp(File_Name, EXIT_CONFIG_AUDIO_PARTITION_NAME, strlen(EXIT_CONFIG_AUDIO_PARTITION_NAME)) == 0 ||
     strncmp(File_Name, ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME, strlen(ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME)) == 0 ||
     strncmp(File_Name, EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME, strlen(EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME)) == 0)
  {
    strncopy(Current_Opt_File_Name, File_Name, 24);
    Current_Opt_File_Size = File_Size;
    return 0;
  }
  return -1;
}

/**
  ******************************************************************
  * @brief   获得音频文件大小
  * @param   [in]File_Name 音频文件名
  * @return  0错误.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-24
  ******************************************************************
  */
uint32_t Tips_Audio_Port_Get_File_Size(const char *File_Name)
{
  uint32_t File_Size = 0;
  if(strncmp(File_Name, ENTRY_CONFIG_AUDIO_PARTITION_NAME, strlen(ENTRY_CONFIG_AUDIO_PARTITION_NAME)) == 0 ||
     strncmp(File_Name, EXIT_CONFIG_AUDIO_PARTITION_NAME, strlen(EXIT_CONFIG_AUDIO_PARTITION_NAME)) == 0 ||
     strncmp(File_Name, ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME, strlen(ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME)) == 0 ||
     strncmp(File_Name, EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME, strlen(EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME)) == 0)
  {
    if(Flash_Port_Read_Partition_Data(File_Name, (uint8_t *)&File_Size, 0, 4) < 0)
    {
      return 0;
    }
    strncopy(Current_Opt_File_Name, File_Name, 24);
    Current_Opt_File_Size = File_Size;
    return File_Size;
  }
  return 0;
}

/**
  ******************************************************************
  * @brief   更新提示音频数据
  * @param   [in]Result_Audio_Buf 更新缓冲区
  * @param   [in]Task 任务号
  * @return  PROCESS_RESULT_RUNNING 任务未完成 PROCESS_RESULT_OK任务已完成.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-21
  ******************************************************************
  */
TASK_PROCESS_RET_Typedef_t Tips_Audio_Port_Start(int16_t *Result_Audio_Buf, MISCELL_TASK_LIST_TABLE_Typedef_t Task)
{
  int ret = 0;
  switch(Task)
  {
    case PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK:
      ret = Read_Audio_Data(ENTRY_CONFIG_AUDIO_PARTITION_NAME);
      break;
    case PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK:
      ret = Read_Audio_Data(EXIT_CONFIG_AUDIO_PARTITION_NAME);
      break;
    case PLAY_ENTRY_LINE_COMPENSATION_AUDIO_TIPS_TASK:
      ret = Read_Audio_Data(ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME);
      break;
    case PLAY_EXIT_LINE_COMPENSATION_AUDIO_TIPS_TASK:
      ret = Read_Audio_Data(EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME);
      break;
    case RESERVED_TASK:
    default:
      break;
  }

  /*音频读取失败*/
  if(ret < 0)
  {
    memset(Result_Audio_Buf, 0, sizeof(int16_t)*STEREO_FRAME_SIZE);
    return PROCESS_RESULT_ERROR;
  }

  /*音频正在读取*/
  else if(ret > 0)
  {
    memmove(Result_Audio_Buf, (int8_t *)Flash_Audio_Data+((Play_Tips_Audio_Data_Index-(sizeof(int16_t)*MONO_FRAME_SIZE)) % (STEREO_FRAME_SIZE*sizeof(int16_t))), sizeof(int16_t)*MONO_FRAME_SIZE);
    memmove(&Result_Audio_Buf[MONO_FRAME_SIZE], &Result_Audio_Buf[0], sizeof(int16_t)*MONO_FRAME_SIZE);
    return PROCESS_RESULT_RUNNING;
  }

  /*音频读取完成*/
  else if(ret == 0)
  {
    memmove(Result_Audio_Buf, (int8_t *)Flash_Audio_Data+((Play_Tips_Audio_Data_Index-(sizeof(int16_t)*MONO_FRAME_SIZE)) % (STEREO_FRAME_SIZE*sizeof(int16_t))), sizeof(int16_t)*MONO_FRAME_SIZE);
    memmove(&Result_Audio_Buf[MONO_FRAME_SIZE], &Result_Audio_Buf[0], sizeof(int16_t)*MONO_FRAME_SIZE);
    Play_Tips_Audio_Data_Index = 0;
  }
  return PROCESS_RESULT_OK;
}

/**
  ******************************************************************
  * @brief   提示音频任务初始化
  * @param   [in]Task 任务号.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-05-25
  ******************************************************************
  */
void Tips_Audio_Port_Init(MISCELL_TASK_LIST_TABLE_Typedef_t Task)
{
  /*重置缓冲区*/
  memset((void *)Flash_Audio_Data, 0, sizeof(int16_t)*STEREO_FRAME_SIZE);

  switch(Task)
  {
    case PLAY_ENTRY_CONFIG_AUDIO_TIPS_TASK:
      if(Flash_Port_Read_Partition_Data(ENTRY_CONFIG_AUDIO_PARTITION_NAME, (uint8_t *)&Play_Tips_Audio_Data_Size, 0, 4) < 0)
      {
        printf("read " ENTRY_CONFIG_AUDIO_PARTITION_NAME "data size error.\r\n");
      }
      /*读取*/
      if(Flash_Port_Read_Partition_Data(ENTRY_CONFIG_AUDIO_PARTITION_NAME, (uint8_t *)Flash_Audio_Data, AUDIO_PATITION_OFFSET_SIZE, (STEREO_FRAME_SIZE*sizeof(int16_t))) < 0)
      {
        return;
      }
      break;
    case PLAY_EXIT_CONFIG_AUDIO_TIPS_TASK:
      if(Flash_Port_Read_Partition_Data(EXIT_CONFIG_AUDIO_PARTITION_NAME, (uint8_t *)&Play_Tips_Audio_Data_Size, 0, 4) < 0)
      {
        printf("read " EXIT_CONFIG_AUDIO_PARTITION_NAME "data size error.\r\n");
      }
      /*读取*/
      if(Flash_Port_Read_Partition_Data(EXIT_CONFIG_AUDIO_PARTITION_NAME, (uint8_t *)Flash_Audio_Data, AUDIO_PATITION_OFFSET_SIZE, (STEREO_FRAME_SIZE*sizeof(int16_t))) < 0)
      {
        return;
      }
      break;
    case PLAY_ENTRY_LINE_COMPENSATION_AUDIO_TIPS_TASK:
      if(Flash_Port_Read_Partition_Data(ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME, (uint8_t *)&Play_Tips_Audio_Data_Size, 0, 4) < 0)
      {
        printf("read " ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME "data size error.\r\n");
      }
      /*读取*/
      if(Flash_Port_Read_Partition_Data(ENTRY_LINE_COMPENSATION_AUDIO_PARTITION_NAME, (uint8_t *)Flash_Audio_Data, AUDIO_PATITION_OFFSET_SIZE, (STEREO_FRAME_SIZE*sizeof(int16_t))) < 0)
      {
        return;
      }
      break;
    case PLAY_EXIT_LINE_COMPENSATION_AUDIO_TIPS_TASK:
      if(Flash_Port_Read_Partition_Data(EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME, (uint8_t *)&Play_Tips_Audio_Data_Size, 0, 4) < 0)
      {
        printf("read " EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME "data size error.\r\n");
      }
      /*读取*/
      if(Flash_Port_Read_Partition_Data(EXIT_LINE_COMPENSATION_AUDIO_PARTITION_NAME, (uint8_t *)Flash_Audio_Data, AUDIO_PATITION_OFFSET_SIZE, (STEREO_FRAME_SIZE*sizeof(int16_t))) < 0)
      {
        return;
      }
      break;
    case RESERVED_TASK:
    default:
      return;
  }
  Get_Audio_Data_Cnt = (STEREO_FRAME_SIZE*sizeof(int16_t));
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
