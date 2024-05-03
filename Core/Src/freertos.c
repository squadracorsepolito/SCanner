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
/* Definitions for fatfsTaskName */
osThreadId_t fatfsTaskNameHandle;
uint32_t fatfsBuffer[ 1024 ];
osStaticThreadDef_t fatfsControlBlock;
const osThreadAttr_t fatfsTaskName_attributes = {
  .name = "fatfsTaskName",
  .cb_mem = &fatfsControlBlock,
  .cb_size = sizeof(fatfsControlBlock),
  .stack_mem = &fatfsBuffer[0],
  .stack_size = sizeof(fatfsBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LWIP_InitEvent */
osEventFlagsId_t LWIP_InitEventHandle;
osStaticEventGroupDef_t LWIP_InitEventControlBlock;
const osEventFlagsAttr_t LWIP_InitEvent_attributes = {
  .name = "LWIP_InitEvent",
  .cb_mem = &LWIP_InitEventControlBlock,
  .cb_size = sizeof(LWIP_InitEventControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void canTask(void *argument);
extern void cannelloniTask(void *argument);
void fatfsTask(void *argument);

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

  /* creation of fatfsTaskName */
  fatfsTaskNameHandle = osThreadNew(fatfsTask, NULL, &fatfsTaskName_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of LWIP_InitEvent */
  LWIP_InitEventHandle = osEventFlagsNew(&LWIP_InitEvent_attributes);

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
#include <string.h>
#include <stdarg.h>
void UART_Printf(const char* fmt, ...) {
  char buff[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buff, sizeof(buff), fmt, args);
  HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
  va_end(args);
}

FATFS fs;
uint8_t buffer[_MAX_SS] __attribute__((section(".fsSection")));

void dioca() {
  fs.win = buffer;
    FRESULT res;
    UART_Printf("Ready!\r\n");

    // mount the default drive
    res = f_mount(&fs, "", 0);
    if(res != FR_OK) {
        UART_Printf("f_mount() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("f_mount() done!\r\n");

    uint32_t freeClust;
    FATFS* fs_ptr = &fs;
    res = f_getfree("", &freeClust, &fs_ptr); // Warning! This fills fs.n_fatent and fs.csize!
    if(res != FR_OK) {
        UART_Printf("f_getfree() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("f_getfree() done!\r\n");

    uint32_t totalBlocks = (fs.n_fatent - 2) * fs.csize;
    uint32_t freeBlocks = freeClust * fs.csize;

    UART_Printf("Total blocks: %lu (%lu Mb)\r\n", totalBlocks, totalBlocks / 2000);
    UART_Printf("Free blocks: %lu (%lu Mb)\r\n", freeBlocks, freeBlocks / 2000);

    DIR dir;
    res = f_opendir(&dir, "/");
    if(res != FR_OK) {
        UART_Printf("f_opendir() failed, res = %d\r\n", res);
        return;
    }

    FILINFO fileInfo;
    uint32_t totalFiles = 0;
    uint32_t totalDirs = 0;
    UART_Printf("--------\r\nRoot directory:\r\n");
    for(;;) {
        res = f_readdir(&dir, &fileInfo);
        if((res != FR_OK) || (fileInfo.fname[0] == '\0')) {
            break;
        }
        
        if(fileInfo.fattrib & AM_DIR) {
            UART_Printf("  DIR  %s\r\n", fileInfo.fname);
            totalDirs++;
        } else {
            UART_Printf("  FILE %s\r\n", fileInfo.fname);
            totalFiles++;
        }
    }

    UART_Printf("(total: %lu dirs, %lu files)\r\n--------\r\n", totalDirs, totalFiles);

    res = f_closedir(&dir);
    if(res != FR_OK) {
        UART_Printf("f_closedir() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("Writing to log.txt...\r\n");

    char writeBuff[128];
    snprintf(writeBuff, sizeof(writeBuff), "Total blocks: %lu (%lu Mb); Free blocks: %lu (%lu Mb)\r\n",
        totalBlocks, totalBlocks / 2000,
        freeBlocks, freeBlocks / 2000);

    FIL logFile;
    res = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
    if(res != FR_OK) {
        UART_Printf("f_open() failed, res = %d\r\n", res);
        return;
    }

    unsigned int bytesToWrite = strlen(writeBuff);
    unsigned int bytesWritten;
    res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);
    if(res != FR_OK) {
        UART_Printf("f_write() failed, res = %d\r\n", res);
        return;
    }

    if(bytesWritten < bytesToWrite) {
        UART_Printf("WARNING! Disk is full, bytesToWrite = %lu, bytesWritten = %lu\r\n", bytesToWrite, bytesWritten);
    }

    res = f_close(&logFile);
    if(res != FR_OK) {
        UART_Printf("f_close() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("Reading file...\r\n");
    FIL msgFile;
    res = f_open(&msgFile, "log.txt", FA_READ);
    if(res != FR_OK) {
        UART_Printf("f_open() failed, res = %d\r\n", res);
        return;
    }

    char readBuff[128];
    unsigned int bytesRead;
    res = f_read(&msgFile, readBuff, sizeof(readBuff)-1, &bytesRead);
    if(res != FR_OK) {
        UART_Printf("f_read() failed, res = %d\r\n", res);
        return;
    }

    readBuff[bytesRead] = '\0';
    UART_Printf("```\r\n%s\r\n```\r\n", readBuff);

    res = f_close(&msgFile);
    if(res != FR_OK) {
        UART_Printf("f_close() failed, res = %d\r\n", res);
        return;
    }

    // Unmount
    res = f_mount(NULL, "", 0);
    if(res != FR_OK) {
        UART_Printf("Unmount failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("Done!\r\n");
    }

/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  HAL_UART_Transmit(&huart1, "dioca\r\n", 7, 10);

  // dioca();

  /* Infinite loop */
  for(;;)
  {
    HAL_UART_Transmit(&huart1, HAL_GPIO_ReadPin(POWER_SOURCE_GPIO_Port, POWER_SOURCE_Pin) ? "H\r" : "L\r", 2, 10);
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_fatfsTask */
/**
* @brief Function implementing the fatfsTaskName thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fatfsTask */
__weak void fatfsTask(void *argument)
{
  /* USER CODE BEGIN fatfsTask */
  dioca();
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END fatfsTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

