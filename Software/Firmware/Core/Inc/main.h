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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Power_Pin GPIO_PIN_13
#define LED_Power_GPIO_Port GPIOC
#define RACE_SW2_Pin GPIO_PIN_14
#define RACE_SW2_GPIO_Port GPIOC
#define RACE_SW2_EXTI_IRQn EXTI15_10_IRQn
#define V_METER_Pin GPIO_PIN_1
#define V_METER_GPIO_Port GPIOA
#define ML_PWM_Pin GPIO_PIN_2
#define ML_PWM_GPIO_Port GPIOA
#define MR_PWM_Pin GPIO_PIN_3
#define MR_PWM_GPIO_Port GPIOA
#define RECIV_FL_Pin GPIO_PIN_4
#define RECIV_FL_GPIO_Port GPIOA
#define RECIV_L_Pin GPIO_PIN_5
#define RECIV_L_GPIO_Port GPIOA
#define ML_ENC_A_Pin GPIO_PIN_6
#define ML_ENC_A_GPIO_Port GPIOA
#define ML_ENC_B_Pin GPIO_PIN_7
#define ML_ENC_B_GPIO_Port GPIOA
#define RECIV_R_Pin GPIO_PIN_0
#define RECIV_R_GPIO_Port GPIOB
#define RECIV_FR_Pin GPIO_PIN_1
#define RECIV_FR_GPIO_Port GPIOB
#define EMIT_L_Pin GPIO_PIN_10
#define EMIT_L_GPIO_Port GPIOB
#define EMIT_R_Pin GPIO_PIN_11
#define EMIT_R_GPIO_Port GPIOB
#define EMIT_FR_Pin GPIO_PIN_12
#define EMIT_FR_GPIO_Port GPIOB
#define MR_FWD_Pin GPIO_PIN_13
#define MR_FWD_GPIO_Port GPIOB
#define ML_FWD_Pin GPIO_PIN_14
#define ML_FWD_GPIO_Port GPIOB
#define MR_BWD_Pin GPIO_PIN_15
#define MR_BWD_GPIO_Port GPIOB
#define ML_BWD_Pin GPIO_PIN_8
#define ML_BWD_GPIO_Port GPIOA
#define LED_Red_Pin GPIO_PIN_11
#define LED_Red_GPIO_Port GPIOA
#define LED_Blue_Pin GPIO_PIN_12
#define LED_Blue_GPIO_Port GPIOA
#define LED_Green_Pin GPIO_PIN_15
#define LED_Green_GPIO_Port GPIOA
#define ARM_SW1_Pin GPIO_PIN_4
#define ARM_SW1_GPIO_Port GPIOB
#define ARM_SW1_EXTI_IRQn EXTI4_IRQn
#define EMIT_FL_Pin GPIO_PIN_5
#define EMIT_FL_GPIO_Port GPIOB
#define MR_ENC_B_Pin GPIO_PIN_6
#define MR_ENC_B_GPIO_Port GPIOB
#define MR_ENC_A_Pin GPIO_PIN_7
#define MR_ENC_A_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOB
#define LOADMAZE_SW3_Pin GPIO_PIN_9
#define LOADMAZE_SW3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
