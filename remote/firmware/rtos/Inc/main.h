/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

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
#define UART_RX_IND_Pin GPIO_PIN_15
#define UART_RX_IND_GPIO_Port GPIOC
#define BATT_ADC_Pin GPIO_PIN_1
#define BATT_ADC_GPIO_Port GPIOA
#define LRA_TRIG_Pin GPIO_PIN_3
#define LRA_TRIG_GPIO_Port GPIOA
#define LRA_EN_Pin GPIO_PIN_4
#define LRA_EN_GPIO_Port GPIOF
#define ACC_INT_Pin GPIO_PIN_0
#define ACC_INT_GPIO_Port GPIOB
#define ACC_INT_EXTI_IRQn EXTI0_IRQn
#define AUTO_DIM_Pin GPIO_PIN_7
#define AUTO_DIM_GPIO_Port GPIOC
#define AUTO_DIM_EXTI_IRQn EXTI9_5_IRQn
#define PWR_BUTTON_Pin GPIO_PIN_8
#define PWR_BUTTON_GPIO_Port GPIOC
#define PWR_BUTTON_EXTI_IRQn EXTI9_5_IRQn
#define MODE_BUTTON_Pin GPIO_PIN_9
#define MODE_BUTTON_GPIO_Port GPIOC
#define MODE_BUTTON_EXTI_IRQn EXTI9_5_IRQn
#define BUTTN_PRESS_LED_Pin GPIO_PIN_11
#define BUTTN_PRESS_LED_GPIO_Port GPIOA
#define LOW_BATT_LED_Pin GPIO_PIN_12
#define LOW_BATT_LED_GPIO_Port GPIOA
#define BLE_MODE_Pin GPIO_PIN_10
#define BLE_MODE_GPIO_Port GPIOC
#define BLE_STATUS1_IND_Pin GPIO_PIN_11
#define BLE_STATUS1_IND_GPIO_Port GPIOC
#define BLE_RST_N_Pin GPIO_PIN_8
#define BLE_RST_N_GPIO_Port GPIOB
#define BLE_STATUS2_IND_Pin GPIO_PIN_9
#define BLE_STATUS2_IND_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define Haptic_Driver_Address_7BIT 0x5A
#define Haptic_Driver_Address ((0x5A) << 1)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
