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
#include "stm32f1xx_hal.h"

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
#define R_D_Pin GPIO_PIN_4
#define R_D_GPIO_Port GPIOA
#define R_C_Pin GPIO_PIN_5
#define R_C_GPIO_Port GPIOA
#define R_B_Pin GPIO_PIN_6
#define R_B_GPIO_Port GPIOA
#define R_A_Pin GPIO_PIN_7
#define R_A_GPIO_Port GPIOA
#define Battery_Pin GPIO_PIN_0
#define Battery_GPIO_Port GPIOB
#define Tongxin_Pin GPIO_PIN_15
#define Tongxin_GPIO_Port GPIOB
#define Run_Pin GPIO_PIN_8
#define Run_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_3
#define OLED_CS_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_4
#define OLED_DC_GPIO_Port GPIOB
#define OLED_RES_Pin GPIO_PIN_5
#define OLED_RES_GPIO_Port GPIOB
#define OLED_D1_Pin GPIO_PIN_6
#define OLED_D1_GPIO_Port GPIOB
#define OLED_D0_Pin GPIO_PIN_7
#define OLED_D0_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
