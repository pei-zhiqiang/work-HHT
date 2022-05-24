/**
 *  @file DAC_Port.h
 *
 *  @date 2021-03-11
 *
 *  @author aron566
 *
 *  @brief DAC操作接口
 *
 *  @version V1.0
 */
#ifndef DAC_PORT_H
#define DAC_PORT_H
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
#include "WM8978_Driver.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/*DAC通道编号*/
typedef enum
{
  DAC_CHANNEL_LEFT = 0,
  DAC_CHANNEL_RIGHT,
  DAC_CHANNEL_LR
}DAC_CHANNEL_NUM_Typedef_t;

/*DAC采样率*/
typedef enum
{
  DAC_8KHZ_SR  = 0,
  DAC_12KHZ_SR,
  DAC_16KHZ_SR,
  DAC_24KHZ_SR,
  DAC_32KHZ_SR,
  DAC_48KHZ_SR,
}DAC_SAMPLE_RATE_Typedef_t;

/*WM8978 AGC参数*/
typedef enum
{
  DAC_LIMATK_US_94     = 0x0000,
  DAC_LIMATK_US_188    = 0x0001,
  DAC_LIMATK_US_375    = 0x0010,
  DAC_LIMATK_US_750    = 0x0011,
  DAC_LIMATK_MS_1_5    = 0x0100,
  DAC_LIMATK_MS_3      = 0x0101,
  DAC_LIMATK_MS_6      = 0x0110,
  DAC_LIMATK_MS_12     = 0x0111,
  DAC_LIMATK_MS_24     = 0x1000,
  DAC_LIMATK_MS_48     = 0x1001,
  DAC_LIMATK_MS_96     = 0x1010,
  DAC_LIMATK_MS_192    = 0x1011,
}DAC_LIMATK_Typedef_t;

typedef enum
{
  DAC_LIMDCY_US_750    = 0x0000,
  DAC_LIMDCY_MS_1_5    = 0x0001,
  DAC_LIMDCY_MS_3      = 0x0010,
  DAC_LIMDCY_MS_6      = 0x0011,
  DAC_LIMDCY_MS_12     = 0x0100,
  DAC_LIMDCY_MS_24     = 0x0101,
  DAC_LIMDCY_MS_48     = 0x0110,
  DAC_LIMDCY_MS_96     = 0x0111,
  DAC_LIMDCY_MS_192    = 0x1000,
  DAC_LIMDCY_MS_384    = 0x1001,
  DAC_LIMDCY_MS_768    = 0x1010,
  DAC_LIMDCY_SEC_1_536 = 0x1011,
}DAC_LIMDCY_Typedef_t;
typedef struct
{
  DAC_LIMATK_Typedef_t atk;
  DAC_LIMDCY_Typedef_t dcy;
  uint8_t boost;              /**< Limiter volume boost (can be used as a stand alone volume boost when LIMEN=0)*/
  uint8_t level_l;            /**< determines level at which the limiter starts to operate*/
}DAC_AGC_PAR_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief DAC初始化
 *
 */
void DAC_Port_Init(void);

/**
 * @brief DAC反初始化
 *
 */
void DAC_Port_DeInit(void);

/**
 * @brief 更新DAC音频数据
 *
 * @param Audio_Data_CH1 通道1音频数据
 * @param Audio_Data_CH2 通道2音频数据
 * @param Send_Audio_Buf 输出缓冲区
 */
void DAC_Port_Play_Audio_Data(const int32_t *Audio_Data_CH1, const int32_t *Audio_Data_CH2, int32_t *Send_Audio_Buf);

/**
 * @brief 启动DAC数据更新
 *
 */
void DAC_Port_Play_Audio_Start(void);

/**
 * @brief 更新发送缓冲区地址
 *
 * @param Send_Buf_Ptr 缓冲区地址
 */
void DAC_Port_Update_Send_Buf(int32_t *Send_Buf_Ptr);

/*设置DAC采样率*/
void DAC_Port_Set_Sample_Rate(DAC_SAMPLE_RATE_Typedef_t sr);
/*调节前置放大*/
void DAC_Port_Set_Preamp(DAC_CHANNEL_NUM_Typedef_t Channel, float Gain);
/*设置左右耳增益*/
void DAC_Port_Set_Vol_Gain(DAC_CHANNEL_NUM_Typedef_t Channel, float Gain);
/*调节EQ5段中心频率及增益*/
void DAC_Port_Set_EQ_Center_Frequency(const uint16_t *fq, const uint8_t *Gain_dB);
/*调节听力曲线更新，自动验配*/
void DAC_Port_Set_Ref_Hear_Wave(const uint8_t *Left_Ref_dB_Min_Gain, const uint8_t *Right_Ref_dB_Min_Gain,
                                const uint8_t *Left_Ref_dB_Max_Gain, const uint8_t *Right_Ref_dB_Max_Gain);
/**
 * @brief 设置WM8978输出1使能状态
 *
 * @param State true打开
 */
void DAC_Port_Set_LR_OUT1_On_Off(bool State);

/**
 * @brief 设置WM8978输出2使能状态
 *
 * @param State true打开
 */
void DAC_Port_Set_LR_OUT2_On_Off(bool State);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
