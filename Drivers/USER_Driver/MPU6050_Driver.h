/**
 *  @file MPU6050_Driver.h
 *
 *  @date 2021-03-14
 *
 *  @author aron566
 *
 *  @brief 陀螺仪驱动
 *
 *  @version V1.0
 */
#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/*MPU6050 structure*/
typedef struct
{
  int16_t Accel_X_RAW;
  int16_t Accel_Y_RAW;
  int16_t Accel_Z_RAW;
  double Ax;          /**< 加速度*/
  double Ay;
  double Az;

  int16_t Gyro_X_RAW;
  int16_t Gyro_Y_RAW;
  int16_t Gyro_Z_RAW;
  double Gx;
  double Gy;
  double Gz;

  float Temperature;  /**< 温度*/

  double KalmanAngleX;/**< 卡尔曼滤波后X轴角*/
  double KalmanAngleY;/**< 卡尔曼滤波后Y轴角*/
}MPU6050_Typedef_t;

/*Kalman structure*/
typedef struct
{
  double Q_angle;     /**< 加速度计的方差*/
  double Q_bias;      /**< 陀螺偏差的方差*/
  double R_measure;   /**< 测量噪声方差(测量噪声的方差)*/
  double angle;       /**< 卡尔曼滤波器计算的角度- 2x1状态向量的一部分*/
  double bias;        /**< 卡尔曼滤波器计算的陀螺偏置- 2x1状态矢量的一部分*/
  double P[2][2];     /**< 由速率和计算的偏差计算的无偏率-调用getAngle来更新速率*/
}KALMAN_Handle_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 初始化MPU*6050
 *
 * @return true 初始化成功
 * @return false
 */
bool MPU6050_Driver_Init(void);

/*读取陀螺仪数值*/
void MPU6050_Read_Gyro(MPU6050_Typedef_t *DataStruct);
/*读取温度数值*/
void MPU6050_Read_Temp(MPU6050_Typedef_t *DataStruct);
/*读取加速度*/
void MPU6050_Driver_Read_Accel(MPU6050_Typedef_t *DataStruct);
/*读取全部*/
void MPU6050_Read_All(MPU6050_Typedef_t *DataStruct);
/*卡尔曼滤波器*/
double Kalman_getAngle(KALMAN_Handle_Typedef_t *Kalman, double newAngle, double newRate, double dt);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
