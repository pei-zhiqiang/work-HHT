/**
 *  @file UART_Port.c
 *
 *  @date 2020-02-22
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 串口操作
 *
 *  @details None.
 *
 *  @version V1.0
 */
#ifdef __cplusplus //use C compiler
extern "C" {
#endif
#include "UART_Port.h"/*外部接口*/
#include "utilities.h"
#if USE_USB_CDC
  #include "usbd_cdc_if.h"
#endif
/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
// extern UART_HandleTypeDef huart6;
// extern DMA_HandleTypeDef hdma_usart6_rx;
// extern DMA_HandleTypeDef hdma_usart6_tx;

/*CDC*/
#if USE_USB_CDC
  extern USBD_HandleTypeDef hUsbDeviceFS;
  extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define USE_LOOPBACK 	      0
#define DEBUG_UART          &huart2
// #define BLE_UART		        &huart6
// #define BLE_RX              &hdma_usart6_rx
// #define BLE_TX              &hdma_usart6_tx
#define UART_MAX_NUM        7/**< 最大允许串口数量*/
#define USE_IDEL_CALLBACK   0/**< 为1 使用空闲中断不定长接收 为0 使用中断单字节接收*/
#define USE_DMA_SEND_FIRST  1/**< 发送设置：为1阻塞时间为0优先DMA发送模式否则中断发送*/
#define USE_DMA_BUF_ADDR    1/**< DMA发送判断数据地址，为1需判断*/
/*定义内存管理接口*/
#if !defined(uartport_malloc) && !defined(uartport_free)
#define uartport_malloc  calloc//pvPortMalloc
#define uartport_free  free//vPortFree
#endif

/** Private constants --------------------------------------------------------*/
#if USE_DMA_SEND_FIRST
  #if USE_DMA_BUF_ADDR
    #if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
      extern const uint32_t USE_DMA_BUF_SPACE$$Base;
      extern const uint32_t USE_DMA_BUF_SPACE$$Limit;
      static const uint32_t *DMA_Buf_Addr_Start = (&USE_DMA_BUF_SPACE$$Base);
      static const uint32_t *DMA_Buf_Addr_End   = (&USE_DMA_BUF_SPACE$$Limit);
    #elif defined(__ICCARM__) || defined(__ICCRX__)/**< IAR方式*/
      #pragma section="USE_DMA_BUF_SPACE"
      static const uint32_t DMA_Buf_Addr_Start = (__section_begin("USE_DMA_BUF_SPACE"));
      static const uint32_t DMA_Buf_Addr_End = (__section_end("USE_DMA_BUF_SPACE"));
    #elif defined(__GNUC__)
        extern const uint32_t _USE_DMA_BUF_SPACE_start;
        extern const uint32_t _USE_DMA_BUF_SPACE_end;
        static const uint32_t *DMA_Buf_Addr_Start = (&_USE_DMA_BUF_SPACE_start);
        static const uint32_t *DMA_Buf_Addr_End = (&_USE_DMA_BUF_SPACE_end);
    #else
      #error not supported compiler, please use command table mode
    #endif
  #else
    static const uint32_t DMA_Buf_Addr_Start = 0;
    static const uint32_t DMA_Buf_Addr_End = 0xFFFFFFFF;
  #endif
#endif
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
/*预定义串口设备信息*/
Uart_Dev_Handle_t *Uart_pDevice[UART_MAX_NUM+1];

__attribute__ ((section("USE_DMA_BUF_SPACE"))) static uint8_t Uart6_DMA_Buf[2048] = {0};
/** Private function prototypes ----------------------------------------------*/
static uint8_t Get_Uart_Index(USART_TypeDef *Instance);
static Uart_Dev_Handle_t *Create_Uart_Dev(UART_NUM_Typedef_t uart_num ,UART_HandleTypeDef *huart
                                        ,DMA_HandleTypeDef *hdma_rx, DMA_HandleTypeDef *hdma_tx, uint8_t *rx_temp_buf, uint16_t rx_temp_size
                                        ,uint32_t rxsize ,int work_mode
#if USE_FREERTOS
                                        ,osSemaphoreId *pRX_Sem
#endif
#if USE_USB_CDC
                                        ,int Is_USB_CDC_Mode
#endif
                                        );
/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/************************************************************
  * @brief   建立串口设备，为其建立双缓冲区-->使能串口空闲中断
  * @param   [in]uart_num 串口号.
  * @param   [in]huart    串口句柄.
  * @param   [in]hdma_rx  串口dma句柄.
  * @param   [in]rx_temp_buf  串口dma临时接收缓冲区.不使用malloc方式，可以指定内存区域防止Cache访问问题
  * @param   [in]rx_temp_size  串口临时缓存大小.
  * @param   [in]rxsize   串口接收缓存大小.
  * @param   [in]work_mode 串口工作模式.
  * @param   [in]pRX_Sem  串口接收信号量.
  * @param   [in]Is_USB_CDC_Mode CDC模式.
  * @return  串口操作句柄.
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
UNUSEDX static Uart_Dev_Handle_t *Create_Uart_Dev(UART_NUM_Typedef_t uart_num ,UART_HandleTypeDef *huart
                                        ,DMA_HandleTypeDef *hdma_rx, DMA_HandleTypeDef *hdma_tx, uint8_t *rx_temp_buf, uint16_t rx_temp_size
                                        ,uint32_t rxsize ,int work_mode
#if USE_FREERTOS
                                        ,osSemaphoreId *pRX_Sem
#endif
#if USE_USB_CDC
                                        ,int Is_USB_CDC_Mode
#endif
                                        )
{
  Uart_Dev_Handle_t *pUart_Dev = (Uart_Dev_Handle_t *)uartport_malloc(1, sizeof(Uart_Dev_Handle_t));
  if(pUart_Dev == NULL)
  {
    while(1);
  }
#if USE_USB_CDC
  pUart_Dev->Is_USB_CDC_Mode = Is_USB_CDC_Mode;
  if(Is_USB_CDC_Mode != 0)
  {
    pUart_Dev->phuart = NULL;
    pUart_Dev->cb = cb_create(rxsize);
    pUart_Dev->RX_Buff_Temp = rx_temp_buf;
    Uart_pDevice[uart_num] = pUart_Dev;
    return pUart_Dev;
  }
#endif ///<! USE_USB_CDC
  pUart_Dev->phuart = huart;
  pUart_Dev->phdma_rx = hdma_rx;
  pUart_Dev->phdma_tx = hdma_tx;
  pUart_Dev->cb = cb_create(rxsize);
  pUart_Dev->MAX_RX_Temp = rx_temp_size;
  pUart_Dev->RX_Buff_Temp = rx_temp_buf;
  if(NULL == pUart_Dev->RX_Buff_Temp)
  {
    return NULL;
  }
  pUart_Dev->Is_Half_Duplex = work_mode;
#if USE_FREERTOS
  pUart_Dev->pRX_Sem = pRX_Sem;
#endif
  Uart_pDevice[uart_num] = pUart_Dev;
#if USE_IDEL_CALLBACK
  //打开空闲中断
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  //使能DMA接收
  HAL_UART_Receive_DMA(huart, pUart_Dev->RX_Buff_Temp, pUart_Dev->MAX_RX_Temp);
#else
  HAL_UART_Receive_IT(huart, pUart_Dev->RX_Buff_Temp, 1);
#endif
  return pUart_Dev;
}

/************************************************************
  * @brief   获得当前串口信息索引
  * @param   [in]Instance 串口句柄.
  * @return  索引号.
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
static uint8_t Get_Uart_Index(USART_TypeDef *Instance)
{
  for(uint8_t index = 0; index < UART_MAX_NUM; index++)
  {
    if(Uart_pDevice[index] == NULL)
    {
      continue;
    }
    if(Uart_pDevice[index]->phuart->Instance == Instance)
    {
      return index;
    }
  }
  return UART_MAX_NUM;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/

/************************************************************
  * @brief   Rx Transfer IRQ
  * @param   huart UART handle.
  * @return  None
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
#if USE_IDEL_CALLBACK
  uint8_t index = Get_Uart_Index(huart->Instance);
  if(index != 0)
  {
    if((__HAL_UART_GET_FLAG(Uart_pDevice[index]->phuart ,UART_FLAG_IDLE) != RESET))
    {
      /*
      首先停止DMA传输，
      1.防止后面又有数据接收到，产生干扰，因为此时的数据还未处理。
      2.DMA需要重新配置。
      */
      HAL_UART_AbortReceive(Uart_pDevice[index]->phuart);
      /*清楚空闲中断标志，否则会一直不断进入中断*/
      __HAL_UART_CLEAR_IDLEFLAG(Uart_pDevice[index]->phuart);
      /*计算本次接收数据长度*/
      uint32_t data_length  = Uart_pDevice[index]->MAX_RX_Temp - __HAL_DMA_GET_COUNTER(Uart_pDevice[index]->phdma_rx);
      /*将数据记录至环形区*/
      CQ_putData(Uart_pDevice[index]->cb, Uart_pDevice[index]->RX_Buff_Temp, (uint32_t)data_length);
#if USE_LOOPBACK
      HAL_UART_Transmit(DEBUG_UART, (uint8_t *)Uart_pDevice[index]->RX_Buff_Temp, (uint16_t)data_length, 0xFFFF);
#endif
      /*清空临时缓冲区*/
      //memset(Uart_pDevice[index]->RX_Buff_Temp ,0 ,data_length);
      /*打开空闲中断*/
      __HAL_UART_ENABLE_IT(Uart_pDevice[index]->phuart, UART_IT_IDLE);
      /*重启开始DMA传输*/
      HAL_UART_Receive_DMA(Uart_pDevice[index]->phuart, Uart_pDevice[index]->RX_Buff_Temp ,Uart_pDevice[index]->MAX_RX_Temp);
    }
  }
#endif ///<! USE_IDEL_CALLBACK
}

/************************************************************
  * @brief   串口操作句柄获取
  * @param   [in]uart_opt_handle 串口句柄.
  * @param   [in]data 数据.
  * @param   [in]size 数据大小.
  * @return  true 发送成功.
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
bool Uart_Port_Transmit_Data(Uart_Dev_Handle_t *uart_opt_handle, uint8_t *data, uint16_t size, uint32_t BlockTime)
{
  if(uart_opt_handle == NULL)
  {
    return false;
  }
#if USE_USB_CDC
  if(uart_opt_handle->Is_USB_CDC_Mode != 0)
  {
    if(CDC_Transmit_FS(data, size) != USBD_OK)
    {
      return false;
    }
    return true;
  }
#endif

  /*优先使用中断发送，但是当阻塞时间不为0时使用阻塞发送*/
  if(BlockTime != 0)
  {
    if(HAL_UART_Transmit(uart_opt_handle->phuart, data, size, BlockTime) != HAL_OK)
    {
      return false;
    }
  }
  else
  {
#if USE_DMA_SEND_FIRST///<! DMA优先发送
    /* when this buf in dma buf. */
    if((uint32_t)data >= (uint32_t)DMA_Buf_Addr_Start && (uint32_t)data < (uint32_t)DMA_Buf_Addr_End)
    {
      /*如用等待DMA 空闲*/
      while(HAL_DMA_GetState(uart_opt_handle->phdma_tx) == HAL_DMA_STATE_BUSY);

      if(uart_opt_handle->phuart->gState != HAL_UART_STATE_READY)
      {
        HAL_UART_AbortTransmit(uart_opt_handle->phuart);
      }

      /* 关闭DMA */
      __HAL_DMA_DISABLE(uart_opt_handle->phdma_tx);

      if(HAL_UART_Transmit_DMA(uart_opt_handle->phuart, data, size) != HAL_OK)
      {
        return false;
      }
    }
    else
    {
      if(HAL_UART_Transmit_IT(uart_opt_handle->phuart, data, size) != HAL_OK)
      {
        return false;
      }
    }
#else
    if(HAL_UART_Transmit_IT(uart_opt_handle->phuart, data, size) != HAL_OK)
    {
      return false;
    }
#endif
  }
  return true;
}

/************************************************************
  * @brief   串口操作句柄获取
  * @param   [in]uart_num 串口号.
  * @return  串口句柄.
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
Uart_Dev_Handle_t *Uart_Port_Get_Handle(UART_NUM_Typedef_t uart_num)
{
  if(UART_MAX_NUM > uart_num)
  {
    return Uart_pDevice[uart_num];
  }
  else
  {
    return NULL;
  }
}

/************************************************************
  * @brief   串口接收中断
  * @param   [in]huart 串口句柄.
  * @return  None.
  * @author  aron566
  * @date    2021/2/3
  * @version v1.0
  * @note    @@
  ***********************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if !USE_IDEL_CALLBACK
  uint8_t index = Get_Uart_Index(huart->Instance);
  if(index != UART_MAX_NUM)
  {
    CQ_putData(Uart_pDevice[index]->cb, Uart_pDevice[index]->RX_Buff_Temp, 1);
  }
#if USE_LOOPBACK
  HAL_UART_Transmit(DEBUG_UART, (uint8_t *)Uart_pDevice[index]->RX_Buff_Temp, 1, 0xFF);
#endif
  HAL_UART_Receive_IT(huart, Uart_pDevice[index]->RX_Buff_Temp, 1);
#endif
}

/**
  ******************************************************************
  * @brief   数据接收回调
  * @param   [in]uart_num 串口设备号
  * @param   [in]data 数据
  * @param   [in]size 数据大小字节数
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-24
  ******************************************************************
  */
#if USE_USB_CDC
void Uart_Port_IRQHandler(UART_NUM_Typedef_t uart_num, const uint8_t *data, uint32_t size)
{
  if(uart_num >= UART_MAX_NUM)
  {
    return;
  }
  if(Uart_pDevice[uart_num] == NULL)
  {
    return;
  }
  if(Uart_pDevice[uart_num]->Is_USB_CDC_Mode != 0)
  {
    CQ_putData(Uart_pDevice[uart_num]->cb, Uart_pDevice[uart_num]->RX_Buff_Temp, size);
  }
}
#endif

/**
  ******************************************************************
  * @brief   设置通讯波特率
  * @param   [in]uart_num 串口设备号
  * @param   [in]BaudRate 波特率
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-01
  ******************************************************************
  */
void Uart_Port_Set_Baudrate(UART_NUM_Typedef_t uart_num, uint32_t BaudRate)
{
  if(uart_num >= UART_MAX_NUM)
  {
    return;
  }
  if(Uart_pDevice[uart_num] == NULL)
  {
    return;
  }
  if(Uart_pDevice[uart_num]->phuart->Init.BaudRate == BaudRate)
  {
    return;
  }
  /*等待空闲*/
  // while(UART_CheckIdleState(Uart_pDevice[uart_num]->phuart) != HAL_OK);
  // HAL_UART_Abort(Uart_pDevice[uart_num]->phuart);
  // Uart_pDevice[uart_num]->phuart->Init.BaudRate = BaudRate;
  // UART_SetConfig(Uart_pDevice[uart_num]->phuart);

  /*重置*/
  HAL_UART_Abort(Uart_pDevice[uart_num]->phuart);
  HAL_UART_DeInit(Uart_pDevice[uart_num]->phuart);
  Uart_pDevice[uart_num]->phuart->Init.BaudRate = BaudRate;
  HAL_UART_Init(Uart_pDevice[uart_num]->phuart);

  /*重新打开接收*/
#if USE_IDEL_CALLBACK
  /*打开空闲中断*/
  __HAL_UART_ENABLE_IT(Uart_pDevice[uart_num]->phuart, UART_IT_IDLE);
  /*使能DMA接收*/
  HAL_UART_Receive_DMA(Uart_pDevice[uart_num]->phuart, Uart_pDevice[uart_num]->RX_Buff_Temp, Uart_pDevice[uart_num]->MAX_RX_Temp);
#else
  HAL_UART_Receive_IT(Uart_pDevice[uart_num]->phuart, Uart_pDevice[uart_num]->RX_Buff_Temp, 1);
#endif
}

/**
  ******************************************************************
  * @brief   获取串口空闲状态
  * @param   [in]uart_opt_handle 串口句柄
  * @return  true 空闲中.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-11
  ******************************************************************
  */
UART_STATE_Typedef_t UART_Port_Get_Idel_State(Uart_Dev_Handle_t *uart_opt_handle)
{
  if(uart_opt_handle == NULL)
  {
    return UART_PORT_ERROR;
  }
  uint32_t State = (uint32_t)UART_PORT_READY;
#if USE_USB_CDC
  if(uart_opt_handle->Is_USB_CDC_Mode != 0)
  {
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
    if(hcdc->TxState == 0)
    {
      State |= UART_PORT_TX_IDEL;
    }
    else
    {
      State |= UART_PORT_TX_BUSY;
    }
    if(hcdc->RxState == 0)
    {
      State |= UART_PORT_RX_IDEL;
    }
    else
    {
      State |= UART_PORT_RX_BUSY;
    }
    return (UART_STATE_Typedef_t)State;
  }
#endif
  if(HAL_UART_GetState(uart_opt_handle->phuart) != HAL_UART_STATE_BUSY_TX)
  {
    State |= UART_PORT_TX_IDEL;
  }
  else
  {
    State |= UART_PORT_TX_BUSY;
  }
  if(HAL_UART_GetState(uart_opt_handle->phuart) != HAL_UART_STATE_BUSY_RX)
  {
    State |= UART_PORT_RX_IDEL;
  }
  else
  {
    State |= UART_PORT_RX_BUSY;
  }

  return (UART_STATE_Typedef_t)State;
}
/************************************************************
  * @brief   串口操作初始化
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
void Uart_Port_Init(void)
{
//   Create_Uart_Dev(UART_NUM_6, BLE_UART,
//                               BLE_RX,
//                               BLE_TX,
//                               (uint8_t *)Uart6_DMA_Buf,
//                               2048,
//                               (uint32_t)CQ_BUF_2KB, 0
// #if USE_FREERTOS
//                               ,NULL
// #endif
// #if USE_USB_CDC
//                               ,0
// #endif
// );
//   if(Uart_pDevice[UART_NUM_6] == NULL)
//   {
//     printf("create uart opt handle 6 faild.\r\n");
//   }

//   Create_Uart_Dev(UART_NUM_2, &huart2,
//                               &hdma_usart2_rx,
//                               &hdma_usart2_tx,
//                               (uint8_t *)Uart2_DMA_Buf,
//                               256,
//                               (uint32_t)CQ_BUF_256B, 0
// #if USE_FREERTOS
//                               ,NULL
// #endif
// #if USE_USB_CDC
//                               ,0
// #endif
//   );

//   if(Uart_pDevice[UART_NUM_2] == NULL)
//   {
//     printf("create uart opt handle 2 faild.\n");
//   }
#if USE_USB_CDC
  Create_Uart_Dev(UART_NUM_0, NULL,
                              NULL,
                              NULL,
                              UserRxBufferFS,
                              0,
                              (uint32_t)CQ_BUF_2KB, 0
#if USE_FREERTOS
                              ,NULL
#endif
                              ,1
  );

  if(Uart_pDevice[UART_NUM_0] == NULL)
  {
    printf("create uart opt handle 0 faild.\r\n");
  }
#endif
}

#if defined (__CC_ARM)
  #pragma import(__use_no_semihosting) // 确保没有从 C 库链接使用半主机的函数
#endif
struct __FILE // 标准库需要的支持函数
{
	int handle;
};
/* FILE is typedef ’ d in stdio.h. */
FILE __stdout;
void _ttywrch(int ch)
{
  ch = ch;
}

void _sys_exit(int x) //定义 _sys_exit() 以避免使用半主机模式
{
  (void)x;
}

#ifndef USE_SMALL_PRINTF
#include "stdio.h"
/************************************************************
  * @brief   重定向c库函数printf到HAL_UART_Transmit
  * @param   [in]ch 输出字符.
  * @param   [in]f 文件指针
  * @return  字符
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(DEBUG_UART, (uint8_t *)&ch, 1, 0xFF);
  return ch;
}

/************************************************************
  * @brief   重定向c库函数getchar,scanf
  * @param   [in]f 文件指针
  * @return  字符
  * @author  aron566
  * @date    2020/3/15
  * @version v1.0
  * @note    @@
  ***********************************************************/
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  while(HAL_UART_Receive(DEBUG_UART, (uint8_t *)&ch, 1, 0xFF) != HAL_OK);
  return ch;
}

#else
/*small printf */
#include "stdio.h"
int __io_putchar(int ch)
{
    HAL_UART_Transmit(DEBUG_UART, (uint8_t *)&ch ,1 ,0xFF);
    return ch;
}
int __write(int file, char *ptr, int len)
{
    int DataIdx;
    for(DataIdx = 0; DataIdx < len; DataIdx++)
    {
        __io_putchar(*ptr++);
    }
    return len;
}
#endif ///<！USE_SMALL_PRINTF

#ifdef __cplusplus //end extern c
}
#endif
