/**
 *  @file Gyroscope_Port.c
 *
 *  @date 2021-03-25
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 陀螺仪应用层接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Gyroscope_Port.h"
#include "MPU6050_Driver.h"
#include "Timer_Port.h"
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static MPU6050_Typedef_t MPU6050_Handle;
static bool Is_Vertical_State = false;

static uint32_t Last_Update_Time_Sec = 0;
static uint32_t Time_Sec_Cnt = 0;
static bool Device_Is_Ok = false;
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
  * @brief   更新陀螺仪设备状态
  * @param   [in]None
  * @return  true 是垂直状态
  * @author  aron566
  * @version V1.0
  * @date    2021-07-09
  ******************************************************************
  */
static inline void Gyroscope_Port_Update_State(void)
{
  if(Last_Update_Time_Sec == Timer_Port_Get_Current_Time(TIMER_SEC))
  {
    return;
  }
  Last_Update_Time_Sec = Timer_Port_Get_Current_Time(TIMER_SEC);

  /*水平状态*/
  if(MPU6050_Handle.KalmanAngleY < 30.l)
  {
    /*已是水平*/
    if(Is_Vertical_State == false)
    {
      Time_Sec_Cnt = 0;
      return;
    }
    if(Time_Sec_Cnt > 2)
    {
      Is_Vertical_State = false;
    }
  }
  /*垂直状态*/
  else
  {
    /*已是垂直*/
    if(Is_Vertical_State == true)
    {
      Time_Sec_Cnt = 0;
      return;
    }
    if(Time_Sec_Cnt > 2)
    {
      Is_Vertical_State = true;
    }
  }
  Time_Sec_Cnt++;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/

/**
 * @brief 获取设备工作状态
 *
 * @return true
 * @return false
 */
bool Gyroscope_Port_Get_Work_State(void)
{
  return Device_Is_Ok;
}

/**
  ******************************************************************
  * @brief   读取陀螺仪设备X方向角度数值
  * @param   [in]None
  * @return  角度
  * @author  aron566
  * @version V1.0
  * @date    2021-03-25
  ******************************************************************
  */
double Gyroscope_Port_Get_X_Angle(void)
{
  return MPU6050_Handle.KalmanAngleX;
}

/**
  ******************************************************************
  * @brief   读取陀螺仪设备Y方向角度数值
  * @param   [in]None
  * @return  角度
  * @author  aron566
  * @version V1.0
  * @date    2021-03-25
  ******************************************************************
  */
double Gyroscope_Port_Get_Y_Angle(void)
{
  return MPU6050_Handle.KalmanAngleY;
}

/**
  ******************************************************************
  * @brief   陀螺仪设备是否是垂直状态
  * @param   [in]None
  * @return  true 是垂直状态
  * @author  aron566
  * @version V1.0
  * @date    2021-07-09
  ******************************************************************
  */
bool Gyroscope_Port_Is_Vertical_State(void)
{
  return false;//Is_Vertical_State;
}

/**
  ******************************************************************
  * @brief   启动读取陀螺仪设备数值
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-25
  ******************************************************************
  */
void Gyroscope_Port_Start(void)
{
  MPU6050_Read_All(&MPU6050_Handle);

  Gyroscope_Port_Update_State();
  //printf("current x angle:%lf,current y angle:%lf current temperature:%.1f\r\n", MPU6050_Handle.KalmanAngleX, MPU6050_Handle.KalmanAngleY, MPU6050_Handle.Temperature);
}

/**
  ******************************************************************
  * @brief   初始化陀螺仪设备
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-25
  ******************************************************************
  */
void Gyroscope_Port_Init(void)
{
  Device_Is_Ok = MPU6050_Driver_Init();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
