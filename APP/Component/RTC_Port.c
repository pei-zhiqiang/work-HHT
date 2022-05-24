/**
 *  @file RTC_Port.c
 *
 *  @date 2021-04-14
 *
 *  @author PZQ
 *
 *  @copyright None.
 *
 *  @brief  RTC操作接口，用于时间显示
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "RTC_Port.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
RTC_DateTypeDef sdatestructure;   /**< 用于获取年月日*/
RTC_TimeTypeDef stimestructure;   /**< 用于获取时分秒*/
/** Private variables --------------------------------------------------------*/
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
void Get_Time(void)
{
  HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);  
//  printf("%02d/%02d/%02d\r\n",2000 + sdatestructure.Year, sdatestructure.Month, sdatestructure.Date);
//	printf("%02d:%02d:%02d\r\n",stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
//  printf("%d\r\n", sdatestructure.Date);
}

//void Set_Time(void)
//{
//  sdatestructure.Year = 22;
//  sdatestructure.Month = 4;
//  sdatestructure.Date = 15;
//  
//  stimestructure.Hours = 14;
//  stimestructure.Minutes = 7;
//  stimestructure.Seconds = 30;
//  
//  HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
//  HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
//}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
