/**
 *  @file MPU6050_Driver.c
 *
 *  @date 2021-03-14
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 陀螺仪驱动
 *
 *  @details 1、YAW偏航角(左右) ROLL横滚角(沿Y轴旋转即倾斜角) PITCH俯仰角(沿X轴旋转)
 *           2、正方向的定义符合笛卡尔右手系规则：右手大姆指指向对应座标轴正向(箭头),其余四指环绕方向是绕该轴转动的正方向
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <math.h>
#include "arm_math.h"
/* Private includes ----------------------------------------------------------*/
#include "MPU6050_Driver.h"
#include "main.h"
/** Private typedef ----------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef    hi2c4;
/** Private macros -----------------------------------------------------------*/
#define ENABLE_MPU9250_DEV      0   /**< 启用MPU9250设备*/
#define ENABLE_LP_CYCLE_MODE    1   /**< 启用低功耗周期唤醒模式*/
/*MPU6050设备地址*/
#define MPU6050_ADDR            0xD0

/*MPU6050寄存器地址*/
#define SELF_TEST_REG           0x0D/**< MPU6050自检寄存器0x0D-0x10*/
#define SELF_TEST_END_REG       0x10

#define SMPLRT_DIV_REG          0x19/**< 采样频率分频 采样频率=陀螺仪输出频率/(1+SMPLRT_DIV)*/
#define GYRO_CONFIG_REG         0x1B/**< 陀螺仪配置*/
#define ACCEL_CONFIG_REG        0x1C/**< 加速计配置*/

/*X:15-8bit X:7-0bit Y:15-8bit Y:7-0bit Z:15-8bit Z:7-0bit*/
#define ACCEL_XOUT_H_REG        0x3B/**< 加速度计测量值，依据采样频率更新，满量程定义在 ACCEL_FS（寄存器0x1C)*/

#define TEMP_OUT_H_REG          0x41/**< 温度测量值16bit*/

/*X:15-8bit X:7-0bit Y:15-8bit Y:7-0bit Z:15-8bit Z:7-0bit*/
#define GYRO_XOUT_H_REG         0x43/**< 陀螺仪测量值，依据采样频率更新，满量程定义在 FS_SEL（寄存器0x1B)*/

/*BIT7:1 RESET,BIT6:1 SLEEP BIT5: 1 Cycle模式(bit6需为0)依据LP_WAKE_CTRL循环 BIT3: 1关闭温度传感器 BIT2-0:设置时钟源 0为内部*/
#define PWR_MGMT_1_REG          0x6B/**< 电源管理,配置电源模式和时钟源,复位整个设备和禁用温度传感器*/
/*只让MPU-60X0的加速度计进入低功耗模式的步骤：(PWR_MGMT_1_REG)CYCLE:1 SLEEP:1 关闭温度传感器 (PWR_MGMT_2_REG)STBY_XG,STBY_YG,STBY_ZG位为1*/
/*BIT7-6: 唤醒的频率 0为1.25Hz 1为5Hz 2为20Hz 3为40Hz BIT5/4/3分别为：写1加速度计的X/Y/Z轴进入待机模式 BIT2-0分别为：写1陀螺仪的X/Y/Z轴进入待机模式*/
#define PWR_MGMT_2_REG          0x6C/**< 电源管理,配置低功耗下唤醒频率，开关个别轴*/

#define WHO_AM_I_REG            0x75/**< 标识设备的身份,默认值为0x68*/

#define RAD_TO_DEG              57.295779513082320876798154814105L
#define I2C_TIMEOUT_MS          2U  /**< I2C超时时间*/
#define ACCEL_Z_CORRECTOR       14418.0L

#define MPU6050_I2C_HANDLE      &hi2c4/**< I2C句柄*/
/** Private constants --------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static uint32_t timer = 0;

/*卡尔曼滤波器参数*/
static KALMAN_Handle_Typedef_t KalmanX =
{
  .Q_angle    = 0.001f,
  .Q_bias     = 0.003f,
  .R_measure  = 0.03f
};

static KALMAN_Handle_Typedef_t KalmanY =
{
  .Q_angle    = 0.001f,
  .Q_bias     = 0.003f,
  .R_measure  = 0.03f,
};
/** Private function prototypes ----------------------------------------------*/
static void MPU6050_Cycle_Wakeup(void);
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
  * @brief   MPU6050陀螺仪周期唤醒
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-09
  ******************************************************************
  */
static void MPU6050_Cycle_Wakeup(void)
{
  uint8_t Data = 0;

  /*失能BIT6 SLEEP 0,使能BIT5 CYCLE 1,关闭温度计BIT3 TEMP_DIS 1*/
  Data = 0x18;
  HAL_I2C_Mem_Write(MPU6050_I2C_HANDLE, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, I2C_TIMEOUT_MS);

  /*设置周期唤醒频率,关闭Z轴加速 陀螺仪*/
  Data = 0x09;
  HAL_I2C_Mem_Write(MPU6050_I2C_HANDLE, MPU6050_ADDR, PWR_MGMT_2_REG, 1, &Data, 1, I2C_TIMEOUT_MS);
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
  * @brief   读取MPU6050陀螺仪测量值
  * @param   [in]DataStruct 句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-14
  ******************************************************************
  */
void MPU6050_Read_Gyro(MPU6050_Typedef_t *DataStruct)
{
	uint8_t Rec_Data[6];

	/*Read 6 BYTES of data staring from GYRO_XOUT_H register*/
	HAL_I2C_Mem_Read(MPU6050_I2C_HANDLE, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, I2C_TIMEOUT_MS);

	DataStruct->Gyro_X_RAW = (int16_t )(Rec_Data [0] << 8 | Rec_Data [1]);
	DataStruct->Gyro_Y_RAW = (int16_t )(Rec_Data [2] << 8 | Rec_Data [3]);
	DataStruct->Gyro_Z_RAW = (int16_t )(Rec_Data [4] << 8 | Rec_Data [5]);

	DataStruct->Gx = DataStruct->Gyro_X_RAW/131.0;
	DataStruct->Gy = DataStruct->Gyro_Y_RAW/131.0;
	DataStruct->Gz = DataStruct->Gyro_Z_RAW/131.0;
}

/**
  ******************************************************************
  * @brief   读取MPU6050温度测量值
  * @param   [in]DataStruct 句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-14
  ******************************************************************
  */
void MPU6050_Read_Temp(MPU6050_Typedef_t *DataStruct)
{
	uint8_t Rec_Data[2];
	int16_t temp;
  /*Read 2 BYTES of data starting from TEMP_OUT_H_REG register*/
	HAL_I2C_Mem_Read(MPU6050_I2C_HANDLE, MPU6050_ADDR, TEMP_OUT_H_REG, 1, Rec_Data, 2, I2C_TIMEOUT_MS);

  temp = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
  DataStruct->Temperature = (float)((int16_t)temp / (float)340.0 + (float)36.53);
}

/**
  ******************************************************************
  * @brief   读取MPU6050加速度测量值
  * @param   [in]DataStruct 句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-14
  ******************************************************************
  */
void MPU6050_Driver_Read_Accel(MPU6050_Typedef_t *DataStruct)
{
	uint8_t Rec_Data[6];

	/*Read 6 BYTES of data starting from ACCEL_XOUT_H register*/
	HAL_I2C_Mem_Read(MPU6050_I2C_HANDLE, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, I2C_TIMEOUT_MS);

  DataStruct->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
  DataStruct->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
  DataStruct->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

  /*** convert the RAW values into dps (m/s)
       we have to divide according to the Full scale value set in FS_SEL
       I have configured FS_SEL = 0. So I am dividing by 131.0
       for more details check GYRO_CONFIG Register              ****/
	DataStruct->Ax = DataStruct->Accel_X_RAW / 16384.0;
	DataStruct->Ay = DataStruct->Accel_Y_RAW / 16384.0;
	DataStruct->Az = DataStruct->Accel_Z_RAW / 16384.0;
}

/**
  ******************************************************************
  * @brief   读取MPU6050全部数据
  * @param   [in]DataStruct 句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-14
  ******************************************************************
  */
void MPU6050_Read_All(MPU6050_Typedef_t *DataStruct)
{
  uint8_t Rec_Data[14];
  int16_t temp;

  /*Read 14 BYTES of data starting from ACCEL_XOUT_H register*/

  if(HAL_I2C_Mem_Read(MPU6050_I2C_HANDLE, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 14, I2C_TIMEOUT_MS) != HAL_OK)
  {
    printf("Error Read MPU6050 Faild.\r\n");
    return;
  }

  DataStruct->Accel_X_RAW = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
  DataStruct->Accel_Y_RAW = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
  DataStruct->Accel_Z_RAW = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
  temp = (int16_t) (Rec_Data[6] << 8 | Rec_Data[7]);
  DataStruct->Gyro_X_RAW = (int16_t) (Rec_Data[8] << 8 | Rec_Data[9]);
  DataStruct->Gyro_Y_RAW = (int16_t) (Rec_Data[10] << 8 | Rec_Data[11]);
  DataStruct->Gyro_Z_RAW = (int16_t) (Rec_Data[12] << 8 | Rec_Data[13]);

  DataStruct->Ax = DataStruct->Accel_X_RAW / 16384.0;
  DataStruct->Ay = DataStruct->Accel_Y_RAW / 16384.0;
  DataStruct->Az = DataStruct->Accel_Z_RAW / ACCEL_Z_CORRECTOR;
  DataStruct->Temperature = (float) ((int16_t) temp / (float) 340.0 + (float) 36.53);
  DataStruct->Gx = DataStruct->Gyro_X_RAW / 131.0;
  DataStruct->Gy = DataStruct->Gyro_Y_RAW / 131.0;
  DataStruct->Gz = DataStruct->Gyro_Z_RAW / 131.0;

  /*Kalman angle solve*/
  double dt = (double) (HAL_GetTick() - timer) / 1000;
  timer = HAL_GetTick();
  double roll;

  /*勾股算出重力加速度在X-Z平面的投影长度*/
//  double roll_sqrt = sqrt(
//          DataStruct->Accel_X_RAW * DataStruct->Accel_X_RAW + DataStruct->Accel_Z_RAW * DataStruct->Accel_Z_RAW);
  /*DSP加速*/
  double roll_sqrt = 0.0;
  float32_t roll_ret = 0.0;
  arm_sqrt_f32(DataStruct->Accel_X_RAW * DataStruct->Accel_X_RAW + DataStruct->Accel_Z_RAW * DataStruct->Accel_Z_RAW, &roll_ret);
  roll_sqrt = (double)roll_ret;
  if(roll_sqrt != 0.0)
  {
    roll = atan(DataStruct->Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
  }
  else
  {
    roll = 0.0;
  }
  double pitch = atan2(-DataStruct->Accel_X_RAW, DataStruct->Accel_Z_RAW) * RAD_TO_DEG;
  if((pitch < -90 && DataStruct->KalmanAngleY > 90) || (pitch > 90 && DataStruct->KalmanAngleY < -90))
  {
    KalmanY.angle = pitch;
    DataStruct->KalmanAngleY = pitch;
  }
  else
  {
    DataStruct->KalmanAngleY = Kalman_getAngle(&KalmanY, pitch, DataStruct->Gy, dt);
  }
  if(fabs(DataStruct->KalmanAngleY) > 90)
  {
    DataStruct->Gx = -DataStruct->Gx;
  }
  DataStruct->KalmanAngleX = Kalman_getAngle(&KalmanX, roll, DataStruct->Gy, dt);
}

/**
  ******************************************************************
  * @brief   卡尔曼滤波
  * @param   [in]Kalman 句柄
  * @param   [in]newAngle 新角度
  * @param   [in]dt 时间
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-14
  ******************************************************************
  */
double Kalman_getAngle(KALMAN_Handle_Typedef_t *Kalman, double newAngle, double newRate, double dt)
{
  double rate = newRate - Kalman->bias;
  Kalman->angle += dt * rate;

  Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
  Kalman->P[0][1] -= dt * Kalman->P[1][1];
  Kalman->P[1][0] -= dt * Kalman->P[1][1];
  Kalman->P[1][1] += Kalman->Q_bias * dt;

  double S = Kalman->P[0][0] + Kalman->R_measure;
  double K[2];
  K[0] = Kalman->P[0][0] / S;
  K[1] = Kalman->P[1][0] / S;

  double y = newAngle - Kalman->angle;
  Kalman->angle += K[0] * y;
  Kalman->bias += K[1] * y;

  double P00_temp = Kalman->P[0][0];
  double P01_temp = Kalman->P[0][1];

  Kalman->P[0][0] -= K[0] * P00_temp;
  Kalman->P[0][1] -= K[0] * P01_temp;
  Kalman->P[1][0] -= K[1] * P00_temp;
  Kalman->P[1][1] -= K[1] * P01_temp;

  return Kalman->angle;
};

/**
  ******************************************************************
  * @brief   MPU6050初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-03-14
  ******************************************************************
  */
bool MPU6050_Driver_Init(void)
{
  uint8_t check, Data;

  /*check device ID WHO_AM_I*/
  HAL_StatusTypeDef State = HAL_I2C_Mem_Read(MPU6050_I2C_HANDLE, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, I2C_TIMEOUT_MS);
  if(State != HAL_OK)
  {
    return false;
  }
#if ENABLE_MPU9250_DEV
  if(check == 0x71)
#else
  if(check == 0x68)/**< 0x68 will be returned by the sensor if everything goes well*/
#endif
  {
    /*power management register 0x6B we should write all 0's to wake the sensor up*/
    Data = 0;
    HAL_I2C_Mem_Write(MPU6050_I2C_HANDLE, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, I2C_TIMEOUT_MS);

    /*Set DATA RATE of 1KHz by writing SMPLRT_DIV register*/
    Data = 0x07;
    HAL_I2C_Mem_Write(MPU6050_I2C_HANDLE, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, I2C_TIMEOUT_MS);

    /*Set accelerometer configuration in ACCEL_CONFIG Register*/
    /*XA_ST=0,YA_ST=0,ZA_ST=0,  FS_SEL=0 ->±2g*/
    Data = 0x00;
    HAL_I2C_Mem_Write(MPU6050_I2C_HANDLE, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, I2C_TIMEOUT_MS);

    /*Set Gyroscopic configuration in GYRO_CONFIG Register*/
    /*XG_ST=0,YG_ST=0,  FS_SEL=0 ->± 250 °/s*/
    Data = 0x00;
    HAL_I2C_Mem_Write(MPU6050_I2C_HANDLE, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, I2C_TIMEOUT_MS);

    /*低功耗周期唤醒设置*/
#if ENABLE_LP_CYCLE_MODE
    MPU6050_Cycle_Wakeup();
#endif
  }
  return true;
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
