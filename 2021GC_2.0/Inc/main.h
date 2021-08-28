/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

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
#define heat_IMU_Pin GPIO_PIN_5
#define heat_IMU_GPIO_Port GPIOB
#define J1__Pin GPIO_PIN_5
#define J1__GPIO_Port GPIOE
#define relay_control___Pin GPIO_PIN_6
#define relay_control___GPIO_Port GPIOE
#define I2__Pin GPIO_PIN_0
#define I2__GPIO_Port GPIOF
#define I1_Pin GPIO_PIN_1
#define I1_GPIO_Port GPIOF
#define Micro_down_NO_Pin GPIO_PIN_1
#define Micro_down_NO_GPIO_Port GPIOC
#define Moto__3_3_Pin GPIO_PIN_2
#define Moto__3_3_GPIO_Port GPIOC
#define Moto_IN1_Pin GPIO_PIN_3
#define Moto_IN1_GPIO_Port GPIOC
#define Micro_up_NO_Pin GPIO_PIN_4
#define Micro_up_NO_GPIO_Port GPIOA
#define Moto_IN2_Pin GPIO_PIN_4
#define Moto_IN2_GPIO_Port GPIOC
#define Micro_up_C_Pin GPIO_PIN_5
#define Micro_up_C_GPIO_Port GPIOA
#define Micro_down_C_Pin GPIO_PIN_5
#define Micro_down_C_GPIO_Port GPIOC
#define Moto_IN1B1_Pin GPIO_PIN_1
#define Moto_IN1B1_GPIO_Port GPIOB
#define relay_control____Pin GPIO_PIN_12
#define relay_control____GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
