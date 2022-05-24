/**
 *  @file BT8829_Signal_Port.h
 *
 *  @date 2021-01-25
 *
 *  @author zgl
 *
 *  @brief
 *
 *  @version V1.0
 */
#ifndef _BT8829_SIGNAL__
#define _BT8829_SIGNAL__
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
#define POWER_KEY_HIGH  do \
                        {HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);}while(0)

#define POWER_KEY_LOW   do \
                        {HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);}while(0)
/** Exported typedefines -----------------------------------------------------*/
/** 数据结构体*/
typedef enum
{
	Elec_STOP = 0,
	Elec_Fst_LOW,
	Elec_Fst_HIGH,
	Elec_Sec_LOW,
}Elec_Station_Typedef_t;

typedef struct Electric_opt
{
	Elec_Station_Typedef_t Elec_Stat;
	int Elec_Delay;
}Elec_Opt_Typedef_t;

typedef enum
{
  Ble_Disconnected,
	Ble_Connected,
}Ble_Connect_Stat_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
void elec_signal_switch(void);
void elec_signal_start(void);
Ble_Connect_Stat_Typedef_t Ble_Connect_Check(void);

extern Ble_Connect_Stat_Typedef_t Ble_Connect_Stat;
extern Ble_Connect_Stat_Typedef_t Pre_Ble_Connect_Stat;
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
