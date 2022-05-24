/**
 *  @file TP_Driver.c
 *
 *  @date 2022-4-1
 *
 *  @author PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief  触摸屏驱动（泰斯强厂商）
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "TP_Driver.h"
#include "main.h"
#include "lv_app.h"
#include "lv_app_test.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c3;
#define TP_I2C_HANDLE           &hi2c3/**< I2C句柄*/

#define Delay                   HAL_Delay
#define DEV_ADDR                0x2A

#define DEV_ADDR_WRITE          0x2A
#define DEV_ADDR_READ           0x2B

#define TP_START_REG            0x00
#define TP_ID_REG               0xA7
#define TP_VERSION_REG          0xA9
#define TP_MODE_REG             0xFA
#define TP_BD_FRE_REG           0xEE
#define TP_LOW_POWER_REG        0xFE

#define BD_FRE_NUM              0x02
#define BD_MODE                 0x60

#define OPEN_LOW_POWER          0x00
#define STOP_LOW_POWER          0x01

/* 按下状态 */
#define TP_PRES_DOWN            0x80
#define TP_PRES_UP              0x00

#define TP_RESET_CLR  do \
                      {HAL_GPIO_WritePin(TP_RESET_GPIO_Port, TP_RESET_Pin, GPIO_PIN_RESET);}while(0)

#define TP_RESET_SET  do \
                      {HAL_GPIO_WritePin(TP_RESET_GPIO_Port, TP_RESET_Pin, GPIO_PIN_SET);}while(0)
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/
/* 触摸驱动句柄 */
static TP_HANDLE_Typedef_t *TP_Dev;
                      
static lv_obj_t* label_tp_x = NULL;
static lv_obj_t* label_tp_y = NULL;                      
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
 * @brief 触摸屏复位
 */
void TP_Driver_Reset(void)
{
  TP_RESET_CLR;
  Delay(10);
  TP_RESET_SET;
  Delay(50);
}

/**
 * @brief 获取触摸屏ID
 */
void TP_Driver_Get_ID(void)
{
  uint8_t reg = TP_ID_REG;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, &reg, 1, 100);
  HAL_I2C_Master_Receive(TP_I2C_HANDLE, DEV_ADDR_READ,  &TP_Dev->ID, 1, 100);
}

/**
 * @brief 获取触摸屏版本
 */
void TP_Driver_Get_Version(void)
{
  uint8_t reg = TP_VERSION_REG;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, &reg, 1, 100);
  HAL_I2C_Master_Receive(TP_I2C_HANDLE, DEV_ADDR_READ,  &TP_Dev->Version, 1, 100);
}

void TP_Driver_Set_BD_Fre(uint8_t num)
{
  uint8_t data[2];
  data[0]= TP_BD_FRE_REG;
  data[1]= num;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, data, 2, 100);
}

void TP_Driver_Set_BD_MODE(void)
{
  uint8_t data[2];
  data[0]= TP_MODE_REG;
  data[1]= BD_MODE;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, data, 2, 100);
}

void TP_Driver_Open_Low_Power(void)
{
  uint8_t data[2];
  data[0]= TP_LOW_POWER_REG;
  data[1]= OPEN_LOW_POWER;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, data, 2, 100);
}

void TP_Driver_Stop_Low_Power(void)
{
  uint8_t data[2];
  data[0]= TP_LOW_POWER_REG;
  data[1]= STOP_LOW_POWER;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, data, 2, 100);
}

/**
 * @brief 触摸产生中断时调取该函数，获取相关信息（获取X,Y坐标点）
 *
 */
void TP_Driver_Coordinate_Update(void)
{
  uint8_t tmp[7];
  uint8_t reg = TP_START_REG;
  HAL_I2C_Master_Transmit(TP_I2C_HANDLE, DEV_ADDR_WRITE, &reg, 1, 100);
  HAL_I2C_Master_Receive(TP_I2C_HANDLE, DEV_ADDR_READ,  tmp, 7, 100);
  TP_Dev->x = ((uint16_t)(tmp[3] & 0x0F)<<8) + (uint16_t)tmp[4];
  TP_Dev->y = ((uint16_t)(tmp[5] & 0x0F)<<8) + (uint16_t)tmp[6];
  //TP_Dev->x = 360 - (((uint16_t)(tmp[3] & 0x0F)<<8) + (uint16_t)tmp[4]);
  //TP_Dev->y = 360 - (((uint16_t)(tmp[5] & 0x0F)<<8) + (uint16_t)tmp[6]);
  
  //printf("TP_Dev->x = %d, TP_Dev->y = %d\r\n", TP_Dev->x, TP_Dev->y);

  /* 判断模式，tmp[1] == 0x00 报点模式 */
  if(tmp[1] == 0x00)
  {
    /* 状态码 */
    uint8_t tp_type = tmp[3] >> 4;
#if 0
		if(tp_type == 0x00)
		{
			if(TP_Dev->sta == TP_PRES_UP)
			{
				TP_Dev->sta = TP_PRES_DOWN;
			}
		}
		else if(tp_type == 0x04)
		{
			if(TP_Dev->sta == TP_PRES_DOWN)
			{
				TP_Dev->sta = TP_PRES_UP;
			}
		}
#else
    if(tp_type == 0x00)
    {
      TP_Dev->sta = TP_PRES_DOWN;
    }
#endif
  }
  if(Current_UI_Inf->ui_idx == TEST_TP_IDX)
  {
    if(label_tp_x != NULL || label_tp_y != NULL)
      {
        lv_obj_del(label_tp_x);
        lv_obj_del(label_tp_y);
        label_tp_x = NULL;
        label_tp_y = NULL;      
      }
      /* 创建一个 label 标签来显示当前的TP的X坐标值 */
      label_tp_x = lv_label_create(cont_tp,NULL);
      lv_obj_align(label_tp_x,cont_tp,LV_ALIGN_CENTER,0,30);//设置与 lmeter1 居中对齐
      //使能自动对齐功能,当文本长度发生变化时,它会自动对齐的
      lv_obj_set_auto_realign(label_tp_x,true);
      char buff_x[10];
      sprintf(buff_x,"X:%d",TP_Dev->x);
      lv_label_set_text(label_tp_x,buff_x);//设置文本
      
      /* 创建一个 label 标签来显示当前的TP的Y坐标值 */
      label_tp_y = lv_label_create(cont_tp,NULL);
      lv_obj_align(label_tp_y,cont_tp,LV_ALIGN_CENTER,0,60);//设置与 lmeter1 居中对齐
      //使能自动对齐功能,当文本长度发生变化时,它会自动对齐的
      lv_obj_set_auto_realign(label_tp_y,true);
      char buff_y[10];
      sprintf(buff_y,"Y:%d",TP_Dev->y);
      lv_label_set_text(label_tp_y,buff_y);//设置文本
  }
}
/**
 * @brief 触摸屏初始化
 *
 */
void TP_Driver_Init(TP_HANDLE_Typedef_t *TP_Handle)
{
  /* 重置驱动 */
  TP_Driver_Reset();

  /* 初始化句柄 */
  TP_Dev = TP_Handle;

  /* 获取ID */
  TP_Driver_Get_ID();
  printf("TP ID: 0x%02X\r\n", TP_Dev->ID);

  /* 设置模式 */
  TP_Driver_Set_BD_MODE();

  /* 设置频率 */
  TP_Driver_Set_BD_Fre(BD_FRE_NUM);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
