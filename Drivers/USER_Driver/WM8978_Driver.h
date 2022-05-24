/**
 *  @file WM8978_Driver.h
 *
 *  @date 2021-01-01
 *
 *  @author zgl
 *
 *  @brief WM8978驱动
 *
 *  @version V1.0
 */
#ifndef WM8978_DRIVER_H
#define WM8978_DRIVER_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< nedd definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/*通道类型*/
typedef enum
{
  LEFT_CHANNEL = 0,
  RIGHT_CHANNEL
}CHANNEL_NUM_Typedef_t;

/*高通滤波器*/
typedef enum
{
  ADC_HI_PASS_16KHZ_OFF = 0,
  ADC_HI_PASS_16KHZ_82HZ,
  ADC_HI_PASS_16KHZ_102HZ,
  ADC_HI_PASS_16KHZ_131HZ,
  ADC_HI_PASS_16KHZ_163HZ,
  ADC_HI_PASS_16KHZ_204HZ,
  ADC_HI_PASS_16KHZ_261HZ,
  ADC_HI_PASS_16KHZ_327HZ,
  ADC_HI_PASS_16KHZ_408HZ,
}ADC_HIGH_PASS_FILTER_Typedef_t;

/*采样率*/
typedef enum
{
  SAMP_RATE_8KHZ  = 0,
  SAMP_RATE_12KHZ,
  SAMP_RATE_16KHZ,
  SAMP_RATE_24KHZ,
  SAMP_RATE_32KHZ,
  SAMP_RATE_48KHZ,
}WM8978_SAMP_RATE_Typedef_t;

/*AGC参数*/
typedef enum
{
  LIMATK_US_94     = 0x0000,
  LIMATK_US_188    = 0x0001,
  LIMATK_US_375    = 0x0010,
  LIMATK_US_750    = 0x0011,
  LIMATK_MS_1_5    = 0x0100,
  LIMATK_MS_3      = 0x0101,
  LIMATK_MS_6      = 0x0110,
  LIMATK_MS_12     = 0x0111,
  LIMATK_MS_24     = 0x1000,
  LIMATK_MS_48     = 0x1001,
  LIMATK_MS_96     = 0x1010,
  LIMATK_MS_192    = 0x1011,
}WM8978_LIMATK_Typedef_t;

typedef enum
{
  LIMDCY_US_750    = 0x0000,
  LIMDCY_MS_1_5    = 0x0001,
  LIMDCY_MS_3      = 0x0010,
  LIMDCY_MS_6      = 0x0011,
  LIMDCY_MS_12     = 0x0100,
  LIMDCY_MS_24     = 0x0101,
  LIMDCY_MS_48     = 0x0110,
  LIMDCY_MS_96     = 0x0111,
  LIMDCY_MS_192    = 0x1000,
  LIMDCY_MS_384    = 0x1001,
  LIMDCY_MS_768    = 0x1010,
  LIMDCY_SEC_1_536 = 0x1011,
}WM8978_LIMDCY_Typedef_t;
typedef struct
{
  WM8978_LIMATK_Typedef_t atk;
  WM8978_LIMDCY_Typedef_t dcy;
  uint8_t boost;              /**< Limiter volume boost (can be used as a stand alone volume boost when LIMEN=0)*/
  uint8_t level_l;            /**< determines level at which the limiter starts to operate*/
}WM8978_AGC_PAR_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/

/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*配置WM8978并启动*/
void WM8978_Init_Start(void);
/*关闭WM8978*/
void WM8978_DeInit(void);
/*设置WM8978前置放大*/
void WM8978_Set_Preamp_Gain(CHANNEL_NUM_Typedef_t Channel, float Gain);
/*设置WM8978左右耳增益*/
void WM8978_Set_Vol_Gain(CHANNEL_NUM_Typedef_t Channel, float Gain);
/*设置WM8978高通滤波器*/
void WM8978_Set_HIPASS(ADC_HIGH_PASS_FILTER_Typedef_t High_Pass_Hz);
/*设置WM8978 EQ 5波段中心频率及增益*/
void WM8978_Set_EQ_Band_Gain(const uint16_t *fq, const uint8_t *Gain_dB, uint16_t Sig_Path);
/*设置WM8978采样率*/
void WM8978_Set_SampRate(WM8978_SAMP_RATE_Typedef_t Sample_Rate);
/*设置WM8978软件静音*/
void WM8978_Set_Soft_Mute(bool Enable);
/*设置WM8978 DAC AGC参数*/
void WM8978_Set_AGC_Gain(WM8978_AGC_PAR_Typedef_t *AGC_Par);

/**
 * @brief 设置WM8978输出1使能状态
 *
 * @param State true打开
 */
void WM8978_Set_LR_OUT1_On_Off(bool State);

/**
 * @brief 设置WM8978输出2使能状态
 *
 * @param State true打开
 */
void WM8978_Set_LR_OUT2_On_Off(bool State);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
