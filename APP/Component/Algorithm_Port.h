/**
 *  @file Algorithm_Port.h
 *
 *  @date 2022-03-09
 *
 *  @author aron566
 *
 *  @brief 算法调用接口
 *
 *  @version v0.1.5
 */
#ifndef ALGORITHM_PORT_H
#define ALGORITHM_PORT_H
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
#define BUILD_STATIC_LIB      0    /**< 是否编译静态库 */
#define BUILD_LIB_ENBALE_KEY  0    /**< 是否启用Key校验 */
#define ENABLE_EXPORT_FUNC    0    /**< 使能音频导出功能 */
#define ENBALE_EXPORT_CRC     0    /**< 使能音频导出校验 */
#define AUDIO_CHANNEL_NUM     2U   /**< 音频导出通道数2 */
/** Exported constants -------------------------------------------------------*/
extern const uint16_t AIDS_COMP_Gain_Q7[]; /**< 压缩表 */
extern const uint16_t AIDS_AMP_Gain_Q7[];  /**< 放大表 */
/** Exported macros-----------------------------------------------------------*/
#define AUDIO_SAMPLE_FS             (32000)               /**< 音频采样率32K */
#define MIC_CHANNEL_NUM             (4U)                  /**< 通道总数 */
#define MONO_FRAME_SIZE             (64)                 /**< 单声道帧大小 */
#define STEREO_FRAME_SIZE           (MONO_FRAME_SIZE*2)   /**< 立体声帧大小 */

#define ALGORITHM_FREQ_BAND_NUM     4                     /**< 算法子带数量 */

#ifndef ALGORITHM_MONO_FRAME_SIZE
  #define ALGORITHM_MONO_FRAME_SIZE       MONO_FRAME_SIZE
#endif
#ifndef ALGORITHM_AUDIO_SAMPLE_FS
  #define ALGORITHM_AUDIO_SAMPLE_FS       AUDIO_SAMPLE_FS
#endif

#define ALGORITHM_WINDOW_LEN             (ALGORITHM_MONO_FRAME_SIZE*2)/**< 窗长度 */

/* 每点表示频率 */
#define ALGORITHM_ONE_POINT_FREQ        (ALGORITHM_AUDIO_SAMPLE_FS / ALGORITHM_WINDOW_LEN)

/**
 * @brief 求频率所需点数
 * @param Freqency 频率
 */
#define ALGORITHM_FREQ_INDEX(Frequency)  (Frequency / ALGORITHM_ONE_POINT_FREQ * 2U)

/**
 * @brief 求ms延时所需帧数
 * @param DelayMS 延时ms数
 */
#define ALGORITHM_GET_DELAY_FRAME_NUM(DelayMS)  ((DelayMS * (AUDIO_SAMPLE_FS / 1000)) / ALGORITHM_MONO_FRAME_SIZE)

#define ALGORITHM_MIC_SENSITIVITY_LEVEL  (-26)            /**< 麦克风灵敏级 */
#define ALGORITHM_REFP                   94               /**< 空气中参考声压级 20*log10(1/2^(10^-5))=94 */
#define ALGORITHM_SPL_MAX                (120)            /**< 120dB最大值 */

/* 调试模式开启将：
  1、实时更新时域能量
  2、实时更新频域能量
  3、使用PE5作为算法耗时测试点（输出IO）
  */
#define ALGORITHM_ENABLE_DEBUG           1                   /**< 启动调试模式 */

/** Exported typedefines -----------------------------------------------------*/
/* 算法及任务功能分类 */
typedef enum
{
  BF_FUNC_SEL               = 0x00,/**< Beamforming波束形成 */
  DENOISE_FUNC_SEL          = 0x01,/**< 维纳降噪 */
  MASK_EMD_FUNC_SEL         = 0x02,/**< EMD方法 */
  EQ_FUNC_SEL               = 0X03,/**< EQ调节 */
  VOL_FUNC_SEL              = 0x04,/**< 音量控制 */
  WDRC_FUNC_SEL             = 0x05,/**< 宽动态控制 */
  AGC_FUNC_SEL              = 0x06,/**< 自动增益 */
  FUNC_MAX_SEL              = 0x07,/**< 算法全开 */
  NO_FUNC_SEL               = 0x08,/**< 原声输出 */
  TEST_FUNC_SEL             = 0x09,/**< 测听输出 */
  MUTE_FUNC_SEL             = 0x0A,/**< 静音输出 */
  FIR_FUNC_SEL              = 0x0B,/**< FIR滤波处理输出32阶1ms信号时延，暂不使用 */
  CALIBRATION_FUNC_SEL      = 0x0C,/**< 自主校准后测听输出 */
  AEC_FUNC_SEL              = 0x0D,/**< AEC输出 */
}ALGORITHM_FUNCTION_Typdef_t;

/* 算法通道 */
typedef enum
{
  ALGORITHM_CHANNEL_CH1     = 0x00,/**< 算法通道1 */
  ALGORITHM_CHANNEL_CH2     = 0x01,/**< 算法通道2 */
}ALGORITHM_CHANNEL_Typedef_t;

/* 重置参数回调 */
typedef void (*USER_PAR_RESET_CALLBACK_Typedef_t)(void);
/** Exported variables -------------------------------------------------------*/
/* 当前时域及频域能量，存储方式[时域，子带1、2、3、4、5] */
extern uint8_t Algorithm_Current_dB[];
/** Exported functions prototypes --------------------------------------------*/
/* 调试数据获取接口 */
#if ALGORITHM_ENABLE_DEBUG
/**
 * @brief 获取时域能量.
 *
 * @param dB 能量存储区.
 */
void Algorithm_Port_Get_Time_Domain_Energy(uint8_t *dB);

/**
 * @brief 获取频域能量.
 *
 * @param dB_buf 子带能量.
 * @param Subband_Num 子带数量.
 * @example 获取频域能量.
 * @code    uint8_t *dB_Ptr = NULL; uint8_t Subband_Num;
 *          Algorithm_Port_Get_Freqency_Domain_Energy(&dB_Ptr, &Subband_Num);
 *          for(uint8_t i = 0; i < Subband_Num; i++)
 *          {
 *            printf("dB_Ptr[%d] = %d\n", i, dB_Ptr[i]);
 *          }
 */
void Algorithm_Port_Get_Freqency_Domain_Energy(uint8_t **dB_buf, uint8_t *Subband_Num);
#endif

/**
 * @brief 获取算法库版本号.
 *
 * @return uint16_t 版本号 0x0001表示版本v0.0.1, v1.0.0用0x0100表示.
 */
uint16_t Algorithm_Port_Get_Lib_Version(void);

/**
 * @brief 算法初始化.
 *
 * @return true 初始化成功或者校验成功.
 * @return false 密钥校验失败.
 */
bool Algorithm_Port_Init(void);

/**
 * @brief 算法启动.
 *
 * @param Source_Data MIC_CHANNEL_NUM * 32bit原始数据.
 * @param pOut_CH1 通道1算法输出数据地址存储区 32bit.
 * @param pOut_CH2 通道2算法输出数据地址存储区 32bit.
 * @return None.
 */
void Algorithm_Port_Start(const int32_t Source_Data[][MONO_FRAME_SIZE], \
                          int32_t **pOut_CH1, int32_t **pOut_CH2);

/**
 * @brief 设置MIC灵敏级 + AD参考电压补偿10*log(vREF^2)模拟麦克.
 *
 * @param Level 灵敏级[-60, 0].
 */
bool Algorithm_Port_Set_MIC_Sensitivity_Level(int8_t Level);
/**
 * @brief 获取MIC灵敏级.
 *
 * @param Level 灵敏级存储区.
 */
void Algorithm_Port_Get_MIC_Sensitivity_Level(int8_t *Level);

/**
 * @brief 设置MIC补偿值，用于能量计算还原 ADC + NdB -> Src -> Result_dB - NdB.
 *
 * @param dB 增益值[0 - 60].
 * @return true 设置成功.
 */
bool Algorithm_Port_Set_MIC_Compensation_Val(uint8_t dB);
/**
 * @brief 获取MIC补偿值.
 *
 * @param dB 获取补偿值存储区.
 */
void Algorithm_Port_Get_MIC_Compensation_Val(uint8_t *dB);

/**
 * @brief 设置助听功能，当设置非FUNC_MAX_SEL时，强制使能单算法.
 *
 * @param Func_Name 功能数值.
 */
void Algorithm_Port_Set_Function(ALGORITHM_FUNCTION_Typdef_t Func_Name);
/**
 * @brief 获取当前助听功能.
 *
 * @param Func_Name 功能存储区.
 */
void Algorithm_Port_Get_Function(ALGORITHM_FUNCTION_Typdef_t *Func_Name);

/**
 * @brief 设置助听器算法使能,只有算法功能为FUNC_MAX_SEL设置有效.
 *
 * @param Func_Name 算法功能名，数值范围:[DENOISE_FUNC_SEL,EQ_FUNC_SEL,WDRC_FUNC_SEL,VOL_FUNC_SEL,AGC_FUNC_SEL,FIR_FUNC_SEL].
 * @param State 设置状态，数值范围[0, 1], 0表示禁用，1表示使能.
 * @return true 设置成功.
 */
bool Algorithm_Port_Set_Function_State(ALGORITHM_FUNCTION_Typdef_t Func_Name, uint8_t State);
/**
 * @brief 获取助听器算法使能状态.
 *
 * @param Func_Name 算法功能名，数值范围:[DENOISE_FUNC_SEL,EQ_FUNC_SEL,WDRC_FUNC_SEL,VOL_FUNC_SEL,AGC_FUNC_SEL,FIR_FUNC_SEL].
 * @param State 状态存储区.
 */
void Algorithm_Port_Get_Function_State(ALGORITHM_FUNCTION_Typdef_t Func_Name, uint8_t *State);

/**
 * @brief 重置测听音频数据,切出测听模式时调用即可，否则下次进入测听模式时会是上次设置的值.
 *
 */
void Algorithm_Port_Reset_Test_Wave_Data(void);

/**
 * @brief 更新测听音频参数.
 *
 * @param Hz 设置发声频率 250 ~ 500 1K 2K 4K 8KHz.
 * @param dB 设置发声dB值 数值范围0-100.
 * @param Channel 设置所属通道 [ ALGORITHM_CHANNEL_CH1 - ALGORITHM_CHANNEL_CH2].
 * @return true 成功.
 */
bool Algorithm_Port_Update_Test_Par(uint16_t Hz, uint8_t dB, ALGORITHM_CHANNEL_Typedef_t Channel);

/**
 * @brief 设置音频采样率.
 *
 * @param Sample_Fs 音频采样率 [16000 - 96000].
 * @return true 设置成功.
 */
bool Algorithm_Port_Set_SAMPLE_FS(uint32_t Sample_Fs);

/**
 * @brief 设置校准时间.
 *
 * @param Time_s 时间(单位:秒)，数值范围[3，60]，手持耳机贴近MIC校准时间，时间越久，校准越精确.
 * @return true 设置成功.
 */
bool Algorithm_Port_Set_Calibration_Time(uint32_t Time_s);
/**
 * @brief 获取校准时间.
 *
 * @param Time_s 时间存储区.
 */
void Algorithm_Port_Get_Calibration_Time(uint32_t *Time_s);

/**
 * @brief 设置测听Speaker补偿参数.
 *
 * @param Hz 测听频率 250 ~ 500 1K 2K 4K 8KHz.
 * @param Set_dB 测听的dB数值，数值范围[1，50].
 * @param Out_dB 真实测得输出dB数值，数值范围[0，120].
 * @return true 设置成功.
 */
bool Algorithm_Port_Set_Test_Speaker_Par(uint16_t Hz, int8_t Set_dB, int8_t Out_dB);
/**
 * @brief 获取测听Speaker补偿参数.
 *
 * @param Hz 获取补偿参数频率值 250 ~ 500 1K 2K 4K 8KHz.
 * @param Diff_dB 补偿值dB存储区.
 */
void Algorithm_Port_Get_Test_Speaker_Par(uint16_t Hz, int8_t *Diff_dB);

/**
 * @brief 设置算法平滑系数.
 *
 * @param Coeff 平滑系数数值4段，数值需放大100倍传入，数值范围[1 - 256].
 * @param Func_Name 当前系数用于的算法 数值范围[EQ_FUNC_SEL or WDRC_FUNC_SEL].
 * @return true 设置成功.
 */
bool Algorithm_Port_Set_Smooth_Coeff(const int16_t *Coeff, ALGORITHM_FUNCTION_Typdef_t Func_Name);
/**
 * @brief 获取算法平滑系数.
 *
 * @param Coeff 平滑系数值存储区.
 * @param Func_Name 算法名 数值范围[EQ_FUNC_SEL or WDRC_FUNC_SEL].
 */
void Algorithm_Port_Get_Smooth_Coeff(int16_t *Coeff, ALGORITHM_FUNCTION_Typdef_t Func_Name);

/**
 * @brief 设置BF角度.
 *
 * @param Angle 角度，数值范围[0 - 359].
 *
 * @return true 成功.
 */
bool Algorithm_Port_Set_BF_Angle(uint16_t Angle);
/**
 * @brief 获取BF角度.
 *
 * @param Angle 角度信息存储区.
 */
void Algorithm_Port_Get_BF_Angle(uint16_t *Angle);

/**
 * @brief 设置DENOISE算法参数.
 *
 * @param Level 设置降噪等级，数值范围[1 - 3].
 * @return true 更新成功.
 */
bool Algorithm_Port_Set_DENOISE_Par(uint8_t Level);
/**
 * @brief 获取DENOISE算法参数.
 *
 * @param Level 降噪等级存储区.
 */
void Algorithm_Port_Get_DENOISE_Par(uint8_t *Level);

/**
 * @brief 设置EQ算法参数.
 *
 * @param dB 设置增益数值 [-30 ~ 30].
 * @param SubBand 设置所属频带 [ 1:1500 ~ 2:3000 - 3:6000 - 4:8000].
 * @param Channel 设置所属通道 [ ALGORITHM_CHANNEL_CH1 - ALGORITHM_CHANNEL_CH2].
 * @return true 更新成功.
 */
bool Algorithm_Port_Set_EQ_Par(int8_t dB, uint8_t SubBand, ALGORITHM_CHANNEL_Typedef_t Channel);
/**
 * @brief 获取EQ算法参数.
 *
 * @param dB 增益数值存储区.
 * @param SubBand 设置读取频带 [ 1:1500 ~ 2:3000 - 3:6000 - 4:8000].
 * @param Channel 设置读取通道 [ ALGORITHM_CHANNEL_CH1 - ALGORITHM_CHANNEL_CH2].
 */
void Algorithm_Port_Get_EQ_Par(int8_t *dB, uint8_t SubBand, ALGORITHM_CHANNEL_Typedef_t Channel);

/**
 * @brief 设置WDRC算法参数.
 *
 * @param dB 设置增益数值 4组[0]:mute数值范围0 - 60 [1]:amp数值范围1 - 100 [2]:cmp数值范围2 - 119 [3]limit数值范围3 - 120.
 * @param L_gain 设置低增益数值 [0 ~ 50].
 * @param H_gain 设置高增益数值 [0 ~ 50].
 * @param SubBand 设置所属频带 [ 1:1500 ~ 2:3000 - 3:6000 - 4:8000].
 * @param Channel 设置所属通道 [ ALGORITHM_CHANNEL_CH1 - ALGORITHM_CHANNEL_CH2].
 * @return true 更新成功.
 */
bool Algorithm_Port_Set_WDRC_Par(const uint8_t *dB, uint8_t L_gain, uint8_t H_gain, \
                                 uint8_t SubBand, ALGORITHM_CHANNEL_Typedef_t Channel);
/**
 * @brief 获取WDRC算法参数.
 *
 * @param dB 增益数值存储区4Bytes.
 * @param L_gain 低增益数值.
 * @param H_gain 高增益数值.
 * @param SubBand 设置读取频带 [ 1:1500 ~ 2:3000 - 3:6000 - 4:8000].
 * @param Channel 设置读取通道 [ ALGORITHM_CHANNEL_CH1 - ALGORITHM_CHANNEL_CH2].
 */
void Algorithm_Port_Get_WDRC_Par(uint8_t *dB, uint8_t *L_gain, uint8_t *H_gain, \
                                 uint8_t SubBand, ALGORITHM_CHANNEL_Typedef_t Channel);
/**
 * @brief 获取WDRC的Atk Rsl参数.
 *
 * @param Atk Atk存储区.
 * @param Rsl Rsl存储区.
 */
void Algorithm_Port_Get_WDRC_Atk_Rsl_Par(uint16_t *Atk, uint16_t *Rsl);

/**
 * @brief 设置音量等级.
 *
 * @param Level 音量等级[-7 , 7]，对应增益值 [0, ±3, ±6, ±9, ±12, ±15, ±18, ±21]dB.
 * @param Channel 设置所属通道.
 * @return true 成功.
 */
bool Algorithm_Port_Set_VC_Level(int8_t Level, ALGORITHM_CHANNEL_Typedef_t Channel);
/**
 * @brief 获取音量等级.
 *
 * @param Level 音量等级存储区.
 * @param Channel 设置获取通道.
 */
void Algorithm_Port_Get_VC_Level(int8_t *Level, ALGORITHM_CHANNEL_Typedef_t Channel);

/**
 * @brief 设置AGC自动增益参数.
 *
 * @param UP_dB 压缩阈值，高于此值时压缩限幅 [90 - 110].
 * @param DOWN_dB (静音阈值，低于此值时静音输出[1 - 10]，处于高低之间不做处理).
 */
bool Algorithm_Port_Set_AGC_Par(uint8_t UP_dB, uint8_t DOWN_dB);
/**
 * @brief 获取AGC算法参数.
 *
 * @param UP_dB 压缩阈值存储区.
 * @param DOWN_dB 静音阈值存储区.
 */
void Algorithm_Port_Get_AGC_Par(uint8_t *UP_dB, uint8_t *DOWN_dB);
/**
 * @brief 获取AGC的Atk Rsl参数.
 *
 * @param Atk Atk存储区.
 * @param Rsl Rsl存储区.
 */
void Algorithm_Port_Get_AGC_Atk_Rsl_Par(uint16_t *Atk, uint16_t *Rsl);

/**
 * @brief 设置FIR算法参数
 *
 * @param Coeff 系数数值，数值范围[-32768, 32767],系数数量 = 阶数 + 1.
 * @param Order 系数阶数，数值范围[1 - 64].
 * @return true 设置成功.
 * @return false 设置失败.
 * @example 设置32阶FIR系数.
 * @code  int16_t Coeff_Q10[33] = {0};
 *        Algorithm_Port_Set_FIR_Par(Coeff_Q10, 32);
 */
bool Algorithm_Port_Set_FIR_Par(const int16_t *Coeff_Q10, uint8_t Order);
/**
 * @brief 获取FIR算法参数
 *
 * @param Coeff 系数存储区.
 * @param Order 阶数存储区.
 * @example 获取FIR系数参数.
 * @code    int16_t Coeff_Q10[65] = {0}; uint16_t Order = 0;
 *          Algorithm_Port_Get_FIR_Par(Coeff_Q10, &Order);
 *          for(int i = 0; i < Order + 1; i++)
 *          {
 *            printf("Coeff_Q10[%d] = %d\n", i, Coeff_Q10[i]);
 *          }
 */
void Algorithm_Port_Get_FIR_Par(int16_t *Coeff_Q10, uint8_t *Order);

/**
 * @brief 设置BF角度.
 *
 * @param Angle 角度，数值范围[0 - 359].
 *
 * @return true 成功.
 */
bool Algorithm_Port_Set_BF_Angle(uint16_t Angle);
/**
 * @brief 获取BF角度.
 *
 * @param Angle 角度信息存储区.
 */
void Algorithm_Port_Get_BF_Angle(uint16_t *Angle);

/**
 * @brief 设置BF默认MIC，用于算法关闭状态下默认MIC通路.
 *
 * @param MIC_Channel_Num MIC编号[0 - 3].
 * @return true 设置成功.
 * @return false.
 */
bool Algorithm_Port_Set_BF_Default_MIC(uint8_t MIC_Channel_Num);
/**
 * @brief 获取BF默认MIC.
 *
 * @param MIC_Channel_Num MIC编号[0 - 3].
 */
void Algorithm_Port_Get_BF_Default_MIC(uint8_t *MIC_Channel_Num);

/**
 * @brief 设置AEC啸叫抑制参数
 *
 * @param Level 啸叫抑制等级 [0 - 320].
 * @param Order 啸叫抑制阶数 [1 - 64].
 */
bool Algorithm_Port_Set_AEC_Par(uint16_t Level, uint8_t Order);
/**
 * @brief 获取AEC算法参数
 *
 * @param Level 啸叫抑制等级存储区.
 * @param Order 啸叫抑制阶数存储区.
 */
void Algorithm_Port_Get_AEC_Par(uint16_t *Level, uint8_t *Order);

/**
 * @brief 重置所有算法参数.
 *
 */
void Algorithm_Port_RESET_Par(void);

/**
 * @brief 用户自定义算法参数重置回调.
 *
 * @param User_Reset_Exec 重置函数接口.
 */
void Algorithm_Port_Set_RESET_Callback(USER_PAR_RESET_CALLBACK_Typedef_t User_Reset_Exec);

/**
 * @brief 恢复所有算法参数.
 *
 * @param Flash_Par 参数数据.
 * @param Size 数据长度.
 * @return true 恢复成功.
 */
bool Algorithm_Port_RESTORE_ALL_Par(const uint8_t *Flash_Par, uint16_t Size);
/**
 * @brief 获取算法所有参数.
 *
 * @param Save_Buf 存储区域.
 * @param Size 参数总字节数存储区.
 * @param Buff_Size 存储区大小.
 * @return true 获取成功，当获取失败时说明Save_Buf过小，Size为参数总长度，Buff_Size >= Size.
 */
bool Algorithm_Port_Get_ALL_Par(uint8_t *Save_Buf, uint16_t *Size, uint16_t Buff_Size);

/**
 * @brief 获取增益系数.
 *
 * @param dB 增加dB或者减小dB的数值 范围[-37, 50].
 * @return uint16_t Q9.7系数值.
 */
uint16_t Algorithm_Port_Get_AMP_COMP_Q7Tab(int8_t dB);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
