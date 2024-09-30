/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "usart.h"
#include "cannelloni_task.h"
#include "can_task.h"
#include "fatfs.h"
#include "sdcard.h"
#include "fs.h"
#include "sdcard_task.h"
#include "http_server.h"
#include "tftp_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;
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
extern cannelloni_handle_t cnl1_handle;
extern cannelloni_handle_t cnl2_handle;

extern canNetwork_t can1_network;
extern canNetwork_t can2_network;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 2048 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for can1TaskName */
osThreadId_t can1TaskNameHandle;
uint32_t can1TaskNameBuffer[ 256 ];
osStaticThreadDef_t can1TaskNameControlBlock;
const osThreadAttr_t can1TaskName_attributes = {
  .name = "can1TaskName",
  .cb_mem = &can1TaskNameControlBlock,
  .cb_size = sizeof(can1TaskNameControlBlock),
  .stack_mem = &can1TaskNameBuffer[0],
  .stack_size = sizeof(can1TaskNameBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for can2TaskName */
osThreadId_t can2TaskNameHandle;
uint32_t can2TaskNameBuffer[ 256 ];
osStaticThreadDef_t can2TaskNameControlBlock;
const osThreadAttr_t can2TaskName_attributes = {
  .name = "can2TaskName",
  .cb_mem = &can2TaskNameControlBlock,
  .cb_size = sizeof(can2TaskNameControlBlock),
  .stack_mem = &can2TaskNameBuffer[0],
  .stack_size = sizeof(can2TaskNameBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for cnl1TaskName */
osThreadId_t cnl1TaskNameHandle;
uint32_t cnl1TaskNameBuffer[ 512 ];
osStaticThreadDef_t cnl1TaskNameControlBlock;
const osThreadAttr_t cnl1TaskName_attributes = {
  .name = "cnl1TaskName",
  .cb_mem = &cnl1TaskNameControlBlock,
  .cb_size = sizeof(cnl1TaskNameControlBlock),
  .stack_mem = &cnl1TaskNameBuffer[0],
  .stack_size = sizeof(cnl1TaskNameBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for cnl2TaskName */
osThreadId_t cnl2TaskNameHandle;
uint32_t cnl2TaskNameBuffer[ 512 ];
osStaticThreadDef_t cnl2TaskNameControlBlock;
const osThreadAttr_t cnl2TaskName_attributes = {
  .name = "cnl2TaskName",
  .cb_mem = &cnl2TaskNameControlBlock,
  .cb_size = sizeof(cnl2TaskNameControlBlock),
  .stack_mem = &cnl2TaskNameBuffer[0],
  .stack_size = sizeof(cnl2TaskNameBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sdcardTaskName */
osThreadId_t sdcardTaskNameHandle;
uint32_t sdcardTaskNameBuffer[ 512 ];
osStaticThreadDef_t sdcardTaskNameControlBlock;
const osThreadAttr_t sdcardTaskName_attributes = {
  .name = "sdcardTaskName",
  .cb_mem = &sdcardTaskNameControlBlock,
  .cb_size = sizeof(sdcardTaskNameControlBlock),
  .stack_mem = &sdcardTaskNameBuffer[0],
  .stack_size = sizeof(sdcardTaskNameBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for InitTaskName */
osThreadId_t InitTaskNameHandle;
uint32_t InitTaskNameBuffer[ 512 ];
osStaticThreadDef_t InitTaskNameControlBlock;
const osThreadAttr_t InitTaskName_attributes = {
  .name = "InitTaskName",
  .cb_mem = &InitTaskNameControlBlock,
  .cb_size = sizeof(InitTaskNameControlBlock),
  .stack_mem = &InitTaskNameBuffer[0],
  .stack_size = sizeof(InitTaskNameBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for SDCARD_FILE_LOCK */
osMutexId_t SDCARD_FILE_LOCKHandle;
const osMutexAttr_t SDCARD_FILE_LOCK_attributes = {
  .name = "SDCARD_FILE_LOCK"
};
/* Definitions for SDCARD_DIR_LOCK */
osMutexId_t SDCARD_DIR_LOCKHandle;
const osMutexAttr_t SDCARD_DIR_LOCK_attributes = {
  .name = "SDCARD_DIR_LOCK"
};
/* Definitions for System_InitEvent */
osEventFlagsId_t System_InitEventHandle;
osStaticEventGroupDef_t System_InitEventControlBlock;
const osEventFlagsAttr_t System_InitEvent_attributes = {
  .name = "System_InitEvent",
  .cb_mem = &System_InitEventControlBlock,
  .cb_size = sizeof(System_InitEventControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void canTask(void *argument);
extern void cannelloniTask(void *argument);
extern void sdcardTask(void *argument);
void InitTask(void *argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* ETH_CODE: add breakpoint when stack oveflow is detected by FreeRTOS.
    * Useful for debugging issues.
    */
   __BKPT(0);
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  HAL_GPIO_WritePin(nRST_PHY_GPIO_OUT_GPIO_Port, nRST_PHY_GPIO_OUT_Pin, GPIO_PIN_RESET);

  IP4_ADDR(&(cnl1_handle.Init.addr), 192, 168, 10, 255);
  IP4_ADDR(&(cnl2_handle.Init.addr), 192, 168, 10, 255);

  HAL_GPIO_WritePin(nRST_PHY_GPIO_OUT_GPIO_Port, nRST_PHY_GPIO_OUT_Pin, GPIO_PIN_SET);
  
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of SDCARD_FILE_LOCK */
  SDCARD_FILE_LOCKHandle = osMutexNew(&SDCARD_FILE_LOCK_attributes);

  /* creation of SDCARD_DIR_LOCK */
  SDCARD_DIR_LOCKHandle = osMutexNew(&SDCARD_DIR_LOCK_attributes);

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of can1TaskName */
  can1TaskNameHandle = osThreadNew(canTask, (void*) &can1_network, &can1TaskName_attributes);

  /* creation of can2TaskName */
  can2TaskNameHandle = osThreadNew(canTask, (void*) &can2_network, &can2TaskName_attributes);

  /* creation of cnl1TaskName */
  cnl1TaskNameHandle = osThreadNew(cannelloniTask, (void*) &cnl1_handle, &cnl1TaskName_attributes);

  /* creation of cnl2TaskName */
  cnl2TaskNameHandle = osThreadNew(cannelloniTask, (void*) &cnl2_handle, &cnl2TaskName_attributes);

  /* creation of sdcardTaskName */
  sdcardTaskNameHandle = osThreadNew(sdcardTask, NULL, &sdcardTaskName_attributes);

  /* creation of InitTaskName */
  InitTaskNameHandle = osThreadNew(InitTask, NULL, &InitTaskName_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of System_InitEvent */
  System_InitEventHandle = osEventFlagsNew(&System_InitEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  HAL_UART_Transmit(&huart1, (uint8_t*)"***SCanner***\r\n", 15, 10);
  /* Infinite loop */
  osThreadExit();
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_InitTask */
/**
* @brief Function implementing the InitTaskName thread.
* @param argument: Not used
* @retval None
*/
extern char dirname[16];
/* USER CODE END Header_InitTask */
void InitTask(void *argument)
{
  /* USER CODE BEGIN InitTask */
  sdcard_init();

  // osDelay(1000);

  osEventFlagsWait(System_InitEventHandle, LWIP_Init_Done, osFlagsWaitAny, portMAX_DELAY);

  http_server_init();
  tftp_if_init();

  canEnable(can1_network.Init.hfdcan, can1_network.Init.ActiveITs);
  canEnable(can2_network.Init.hfdcan, can2_network.Init.ActiveITs);

  uint32_t not_val;
  char new_dirname[32];

  while(1) {
    if(xTaskNotifyWait(0, 0x1, &not_val, portMAX_DELAY)) {
      // HAL_UART_Transmit(&huart1, "starting self-kill procedure\r\n", 30, 10);

      canDisable(can1_network.Init.hfdcan, can1_network.Init.ActiveITs);
      canDisable(can2_network.Init.hfdcan, can2_network.Init.ActiveITs);
      
      sdcardForceSync();
      sdcard_deinit();

      // sprintf(new_dirname, "%s_%lus", dirname, osKernelGetTickCount()/osKernelGetTickFreq());
      // volatile FRESULT res = f_rename(dirname, new_dirname);

      f_mount(NULL, "", 0);

      HAL_GPIO_WritePin(SELF_KILL_GPIO_Port, SELF_KILL_Pin, GPIO_PIN_SET); // self-kill
    }
  }
  /* USER CODE END InitTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  BaseType_t hptw;
  if(GPIO_Pin == 64) { //power source switched to backup battery
      xTaskNotifyFromISR(InitTaskNameHandle, 0x1, eSetValueWithOverwrite, &hptw);
      portYIELD_FROM_ISR(hptw);
  }
}

/* USER CODE END Application */

