/**
 *  @file ADC_Port.c
 *
 *  @date 2021-07-01
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief ADC采样
 *
 *  @details 1、16bit分辨率表示1/3(分压电阻)*(3~4.2v)，REF+为3.3v
 *           2、转换时间 = SampleTime采样时间 + 12.5个时钟周期
 *           3、ADC最大的转换速率为1MHz,也就是说最快转换时间为1us
 *           4、T = 采样时间 + 12.5个周期，其中1周期为1/ADCCLK
 *           5、当ADCCLK=14Mhz 的时候，并设置 1.5 个周期的采样时间，则得到Tcovn=1.5+12.5=14个周期=1us。
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
#include "ADC_Port.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
/** Private macros -----------------------------------------------------------*/
#define BAT_MEAS_HANDLE   &hadc1  /**< 电量计测量句柄*/
#define RATIO_FACTOR      3.f     /**< 除以1/3分压，比值系数*/
/** Private constants --------------------------------------------------------*/
static const float Ref_Voltage_Factor = 3.3f/(float)USHRT_MAX;/**< 满量程下的与ADC值系数关系*/
/** Private variables --------------------------------------------------------*/
static float Bat_Voltage = 0.0f;
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
  ******************************************************************
  * @brief   ADC转换完成回调接口
  * @param   [in]hadc adc句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-01
  ******************************************************************
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if(hadc == BAT_MEAS_HANDLE)
  {
    uint32_t val = HAL_ADC_GetValue(hadc);
    Bat_Voltage = Ref_Voltage_Factor * val * RATIO_FACTOR;
  }
}

/**
  ******************************************************************
  * @brief   ADC接口获取当前电量数据
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-01
  ******************************************************************
  */
uint16_t ADC_Port_Get_Battery(void)
{
  /*查电压-电量对照表返回最接近值*/
  return (uint16_t)Bat_Voltage;
}

/**
  ******************************************************************
  * @brief   ADC接口启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-01
  ******************************************************************
  */
void ADC_Port_Start(void)
{
  HAL_ADC_Start_IT(BAT_MEAS_HANDLE);
}

/**
  ******************************************************************
  * @brief   ADC接口初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-07-01
  ******************************************************************
  */
void ADC_Port_Init(void)
{
  HAL_ADC_Start_IT(BAT_MEAS_HANDLE);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
