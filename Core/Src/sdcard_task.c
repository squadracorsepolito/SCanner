#include "sdcard_task.h"
#include "usart.h"

typedef struct {
    CAN_NETWORK net;
    CAN_frame_t frame;
} SDCARD_msg;

uint8_t in_buffer[_MAX_SS] __attribute__((section(".fsSection")));
FATFS fs = {
    .win = in_buffer
};

uint8_t out_buffer[2][_MAX_SS] __attribute__((section(".fsSection")));
FIL f1 = {
    .buf = out_buffer[0]
};
FIL f2 = {
    .buf = out_buffer[1]
};

xQueueHandle sdcardQueue;

extern osEventFlagsId_t System_InitEventHandle;

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
void UART_Printf(const char* fmt, ...) {
  static char buff[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buff, sizeof(buff), fmt, args);
  HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
  va_end(args);
}

void sdcardInit() {
    sdcardQueue = xQueueCreate(32, sizeof(SDCARD_msg));
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

    res = f_open(&f1, "log1.txt", FA_OPEN_APPEND | FA_WRITE);
    if(res != FR_OK) {
        UART_Printf("f_open() failed, res = %d\r\n", res);
        return;
    }

    res = f_open(&f2, "log2.txt", FA_OPEN_APPEND | FA_WRITE);
    if(res != FR_OK) {
        UART_Printf("f_open() failed, res = %d\r\n", res);
        return;
    }

    osEventFlagsSet(System_InitEventHandle, SDCARD_Init_Done);
}

uint32_t sdcardAddMsgFromISR(CAN_frame_t *frame, CAN_NETWORK net, uint32_t *hptw) {
    SDCARD_msg msg = {
        .frame = *frame,
        .net = net
    };
    return xQueueSendToBackFromISR(sdcardQueue, &msg, hptw);
}

void sdcardTask(void *argument) {
    uint8_t buf[256];
    uint8_t data_s[17];
    SDCARD_msg msg;
    uint8_t btw;
    UINT bw;
    sdcardInit();
    osEventFlagsWait(System_InitEventHandle, SDCARD_Init_Done, osFlagsWaitAny, portMAX_DELAY);

    while(1) {
        data_s[0] = 0;
        buf[0] = 0;
        if(xQueueReceive(sdcardQueue, &msg, pdMS_TO_TICKS(100)) == pdPASS) {
            for(uint8_t i=0; i<msg.frame.dlc; ++i) {
                sprintf(data_s+strlen(data_s), "%2x", msg.frame.data[i]);
            }
            btw = sprintf(buf, "(%.6f) %s %03x#%s\r\n", (float)HAL_GetTick()/1000, msg.net == CAN_NET1 ? "mcb":"hvcb", msg.frame.id, data_s);

            if(msg.net == CAN_NET1) {
                f_write(&f1, buf, btw, &bw);
            } else {
                f_write(&f2, buf, btw, &bw);
            }
        } else {
            f_sync(&f1);
            f_sync(&f2);
        }
    }
}