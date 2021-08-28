/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Centre.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId ChassisHandle;
osThreadId GetHandle;
osThreadId OtherHandle;
osThreadId KeyHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Chassis_Task(void const * argument);
void Get_task(void const * argument);
void Other_task(void const * argument);
void Key_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Chassis */
  osThreadDef(Chassis, Chassis_Task, osPriorityNormal, 0, 128);
  ChassisHandle = osThreadCreate(osThread(Chassis), NULL);

  /* definition and creation of Get */
  osThreadDef(Get, Get_task, osPriorityNormal, 0, 128);
  GetHandle = osThreadCreate(osThread(Get), NULL);

  /* definition and creation of Other */
  osThreadDef(Other, Other_task, osPriorityNormal, 0, 128);
  OtherHandle = osThreadCreate(osThread(Other), NULL);

  /* definition and creation of Key */
  osThreadDef(Key, Key_Task, osPriorityNormal, 0, 128);
  KeyHandle = osThreadCreate(osThread(Key), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Chassis_Task */
/**
  * @brief  Function implementing the Chassis thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Chassis_Task */
void Chassis_Task(void const * argument)
{
  /* USER CODE BEGIN Chassis_Task */
  /* Infinite loop */
  for(;;)
  {
	chassis_all_task();
    osDelay(1);
  }
  /* USER CODE END Chassis_Task */
}

/* USER CODE BEGIN Header_Get_task */
/**
* @brief Function implementing the Get thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Get_task */
void Get_task(void const * argument)
{
  /* USER CODE BEGIN Get_task */
  /* Infinite loop */
  for(;;)
  {
	Get_all_task();
    osDelay(1);
  }
  /* USER CODE END Get_task */
}

/* USER CODE BEGIN Header_Other_task */
/**
* @brief Function implementing the Other thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Other_task */
void Other_task(void const * argument)
{
  /* USER CODE BEGIN Other_task */
  /* Infinite loop */
  for(;;)
  {
	Other_all_task();
    osDelay(1);
  }
  /* USER CODE END Other_task */
}

/* USER CODE BEGIN Header_Key_Task */
/**
* @brief Function implementing the Key thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key_Task */
void Key_Task(void const * argument)
{
  /* USER CODE BEGIN Key_Task */
  /* Infinite loop */
  for(;;)
  {
	key_all_task();
    osDelay(1);
  }
  /* USER CODE END Key_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
