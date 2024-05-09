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
#include "sdcard_task.h"
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
uint32_t defaultTaskBuffer[ 512 ];
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
  IP4_ADDR(&(cnl1_handle.Init.addr), 192, 168, 10, 255);
  IP4_ADDR(&(cnl2_handle.Init.addr), 192, 168, 10, 255);
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

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
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
// #include <string.h>
// #include <stdarg.h>
// #include <stdio.h>
// void UART_Printf(const char* fmt, ...) {
//   static char buff[256];
//   va_list args;
//   va_start(args, fmt);
//   vsnprintf(buff, sizeof(buff), fmt, args);
//   HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
//   va_end(args);
// }

// void dioca() {
//     fs.win = in_buffer;
//     FRESULT res;
//     UART_Printf("Ready!\r\n");

//     // mount the default drive
//     res = f_mount(&fs, "", 0);
//     if(res != FR_OK) {
//         UART_Printf("f_mount() failed, res = %d\r\n", res);
//         return;
//     }

//     UART_Printf("f_mount() done!\r\n");

//     uint32_t freeClust;
//     FATFS* fs_ptr = &fs;
//     res = f_getfree("", &freeClust, &fs_ptr); // Warning! This fills fs.n_fatent and fs.csize!
//     if(res != FR_OK) {
//         UART_Printf("f_getfree() failed, res = %d\r\n", res);
//         return;
//     }

//     UART_Printf("f_getfree() done!\r\n");

//     uint32_t totalBlocks = (fs.n_fatent - 2) * fs.csize;
//     uint32_t freeBlocks = freeClust * fs.csize;

//     UART_Printf("Total blocks: %lu (%lu Mb)\r\n", totalBlocks, totalBlocks / 2000);
//     UART_Printf("Free blocks: %lu (%lu Mb)\r\n", freeBlocks, freeBlocks / 2000);

//     DIR dir;
//     res = f_opendir(&dir, "/");
//     if(res != FR_OK) {
//         UART_Printf("f_opendir() failed, res = %d\r\n", res);
//         return;
//     }

//     FILINFO fileInfo;
//     uint32_t totalFiles = 0;
//     uint32_t totalDirs = 0;
//     UART_Printf("--------\r\nRoot directory:\r\n");
//     for(;;) {
//         res = f_readdir(&dir, &fileInfo);
//         if((res != FR_OK) || (fileInfo.fname[0] == '\0')) {
//             break;
//         }
        
//         if(fileInfo.fattrib & AM_DIR) {
//             UART_Printf("  DIR  %s\r\n", fileInfo.fname);
//             totalDirs++;
//         } else {
//             UART_Printf("  FILE %s\r\n", fileInfo.fname);
//             totalFiles++;
//         }
//     }

//     UART_Printf("(total: %lu dirs, %lu files)\r\n--------\r\n", totalDirs, totalFiles);

//     res = f_closedir(&dir);
//     if(res != FR_OK) {
//         UART_Printf("f_closedir() failed, res = %d\r\n", res);
//         return;
//     }

//     UART_Printf("Writing to log.txt...\r\n");

//     char writeBuff[128];
//     snprintf(writeBuff, sizeof(writeBuff), "Total blocks: %lu (%lu Mb); Free blocks: %lu (%lu Mb)\r\n",
//         totalBlocks, totalBlocks / 2000,
//         freeBlocks, freeBlocks / 2000);

//     FIL logFile;
//     logFile.buf = out_buffer1;
//     res = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
//     if(res != FR_OK) {
//         UART_Printf("f_open() failed, res = %d\r\n", res);
//         return;
//     }

//     unsigned int bytesToWrite = strlen(writeBuff);
//     unsigned int bytesWritten;
//     res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);
//     if(res != FR_OK) {
//         UART_Printf("f_write() failed, res = %d\r\n", res);
//         return;
//     }

//     if(bytesWritten < bytesToWrite) {
//         UART_Printf("WARNING! Disk is full, bytesToWrite = %lu, bytesWritten = %lu\r\n", bytesToWrite, bytesWritten);
//     }

//     res = f_close(&logFile);
//     if(res != FR_OK) {
//         UART_Printf("f_close() failed, res = %d\r\n", res);
//         return;
//     }

//     UART_Printf("Reading file...\r\n");
//     FIL msgFile;
//     msgFile.buf = out_buffer2;
//     res = f_open(&msgFile, "log.txt", FA_READ);
//     if(res != FR_OK) {
//         UART_Printf("f_open() failed, res = %d\r\n", res);
//         return;
//     }

//     char readBuff[128];
//     unsigned int bytesRead;

//     UART_Printf("```\r\n");

//     do {
//       res = f_read(&msgFile, readBuff, sizeof(readBuff)-1, &bytesRead);
//       readBuff[bytesRead] = '\0';
//       UART_Printf("%s", readBuff);
//     } while (res == FR_OK && bytesRead != 0);

//     UART_Printf("\r\n```\r\n");

//     if(res != FR_OK) {
//         UART_Printf("f_read() failed, res = %d\r\n", res);
//         return;
//     }

//     res = f_close(&msgFile);
//     if(res != FR_OK) {
//         UART_Printf("f_close() failed, res = %d\r\n", res);
//         return;
//     }

//     // Unmount
//     res = f_mount(NULL, "", 0);
//     if(res != FR_OK) {
//         UART_Printf("Unmount failed, res = %d\r\n", res);
//         return;
//     }

//     UART_Printf("Done!\r\n");
//     }

/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  HAL_UART_Transmit(&huart1, "dioca\r\n", 7, 10);

  /* Infinite loop */
  for(;;)
  {
    HAL_UART_Transmit(&huart1, HAL_GPIO_ReadPin(POWER_SOURCE_GPIO_Port, POWER_SOURCE_Pin) ? "H\r" : "L\r", 2, 10);
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

