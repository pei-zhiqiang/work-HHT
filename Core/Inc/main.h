/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TP_EINT_Pin GPIO_PIN_13
#define TP_EINT_GPIO_Port GPIOC
#define TP_EINT_EXTI_IRQn EXTI15_10_IRQn
#define DEBUG_USART2_TX_Pin GPIO_PIN_2
#define DEBUG_USART2_TX_GPIO_Port GPIOA
#define DEBUG_USART2_RX_Pin GPIO_PIN_3
#define DEBUG_USART2_RX_GPIO_Port GPIOA
#define LCD_SPI_CS_Pin GPIO_PIN_4
#define LCD_SPI_CS_GPIO_Port GPIOA
#define LCD_SPI_SCK_Pin GPIO_PIN_5
#define LCD_SPI_SCK_GPIO_Port GPIOA
#define LCD_SPI_MISO_Pin GPIO_PIN_6
#define LCD_SPI_MISO_GPIO_Port GPIOA
#define LCD_SPI_MOSI_Pin GPIO_PIN_7
#define LCD_SPI_MOSI_GPIO_Port GPIOA
#define LCD_RESRT_Pin GPIO_PIN_1
#define LCD_RESRT_GPIO_Port GPIOB
#define TP_RESET_Pin GPIO_PIN_2
#define TP_RESET_GPIO_Port GPIOB
#define LCD_D_C_Pin GPIO_PIN_10
#define LCD_D_C_GPIO_Port GPIOE
#define TP_SDA_Pin GPIO_PIN_9
#define TP_SDA_GPIO_Port GPIOC
#define TP_SCL_Pin GPIO_PIN_8
#define TP_SCL_GPIO_Port GPIOA
#define LCD_BL_EN_Pin GPIO_PIN_11
#define LCD_BL_EN_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */
#define  LCD_CS_CLR   		HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_Port, LCD_SPI_CS_Pin, GPIO_PIN_RESET)
#define  LCD_CS_SET   		HAL_GPIO_WritePin(LCD_SPI_CS_GPIO_Port, LCD_SPI_CS_Pin, GPIO_PIN_SET)

#define	LCD_DC_CLR	  		HAL_GPIO_WritePin(LCD_D_C_GPIO_Port, LCD_D_C_Pin, GPIO_PIN_RESET)
#define	LCD_DC_SET	      HAL_GPIO_WritePin(LCD_D_C_GPIO_Port, LCD_D_C_Pin, GPIO_PIN_SET)

#define  LCD_RESRT_CLR     HAL_GPIO_WritePin(LCD_RESRT_GPIO_Port, LCD_RESRT_Pin, GPIO_PIN_RESET)
#define  LCD_RESRT_SET     HAL_GPIO_WritePin(LCD_RESRT_GPIO_Port, LCD_RESRT_Pin, GPIO_PIN_SET)

#define  TP_RESET_CLR      HAL_GPIO_WritePin(TP_RESET_GPIO_Port, TP_RESET_Pin, GPIO_PIN_RESET)
#define  TP_RESET_SET      HAL_GPIO_WritePin(TP_RESET_GPIO_Port, TP_RESET_Pin, GPIO_PIN_SET)

#define  LCD_BL_LOW        HAL_GPIO_WritePin(LCD_BL_EN_GPIO_Port, LCD_BL_EN_Pin, GPIO_PIN_RESET)   /**< ???? */
#define  LCD_BL_HIGH       HAL_GPIO_WritePin(LCD_BL_EN_GPIO_Port, LCD_BL_EN_Pin, GPIO_PIN_SET)     /**< ???? */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
