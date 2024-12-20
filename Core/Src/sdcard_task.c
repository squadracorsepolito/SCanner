#include "sdcard_task.h"
#include "usart.h"
#include "fatfs.h"
#include "sdcard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    CAN_NETWORK net;
    CAN_frame_t frame;
    uint32_t timestamp;
} SDCARD_msg;

xQueueHandle sdcardQueue;

FIL *f1, *f2;

char dirname[16];

extern osEventFlagsId_t System_InitEventHandle;

void sdcardInit() {
    sdcardQueue = xQueueCreate(128, sizeof(SDCARD_msg));
    FRESULT res;

    DIR dir;
    uint16_t max = 0;
    uint16_t current;
    char filename1[32], filename2[32];

    res = f_opendir(&dir, "/");
    if(res != FR_OK) {
        return ;
    }

    FILINFO fileInfo;
    for(;;) {
        res = f_readdir(&dir, &fileInfo);
        if((res != FR_OK) || (fileInfo.fname[0] == '\0')) {
            break;
        }

        if(fileInfo.fattrib & AM_DIR) {
            current = atol(fileInfo.fname);
            if(current > max)
                max = current;
        }
    }
    f_closedir(&dir);

    snprintf(dirname, 16, "%u", max+1);
    f_mkdir(dirname);
    
    snprintf(filename1, sizeof(filename1), "%s/mcb.log", dirname);
    snprintf(filename2, sizeof(filename2), "%s/hvcb.log", dirname);

    res = sdcard_fopen_logger(&f1, filename1, FA_OPEN_APPEND | FA_WRITE, &f2, filename2, FA_OPEN_APPEND | FA_WRITE);
    if(res != FR_OK) {
        return;
    }
}

uint32_t sdcardAddMsgFromISR(CAN_frame_t *frame, CAN_NETWORK net, uint32_t timestamp, BaseType_t *hptw) {
    SDCARD_msg msg = {
        .frame = *frame,
        .net = net,
        .timestamp = timestamp
    };
    return xQueueSendToBackFromISR(sdcardQueue, &msg, hptw);
}

void sdcardForceSync(void) {
    f_sync(f1);
    f_sync(f2);
}

void sdcardTask(void *argument) {
    char buf[256];
    char data_s[17];
    SDCARD_msg msg;
    uint8_t btw;
    UINT bw;
    osEventFlagsWait(System_InitEventHandle, SDCARD_Init_Done, osFlagsWaitAny, portMAX_DELAY);
    sdcardInit();
    uint32_t now = osKernelGetTickCount();

    while(1) {
        data_s[0] = 0;
        buf[0] = 0;
        if(xQueueReceive(sdcardQueue, &msg, pdMS_TO_TICKS(1000)) == pdPASS) {
            for(uint8_t i=0; i<msg.frame.dlc; ++i) {
                sprintf(data_s+2*i, "%02X", msg.frame.data[i]);
            }
            btw = sprintf(buf, "(%.6f) %s %03x#%s\n", (float)msg.timestamp/100000, msg.net == CAN_NET1 ? "mcb":"hvcb", msg.frame.id, data_s);

            if(msg.net == CAN_NET1) {
                f_write(f1, buf, btw, &bw);
            } else {
                f_write(f2, buf, btw, &bw);
            }

            if(osKernelGetSysTimerCount() - now > 1000) {
                now = osKernelGetTickCount();
                f_sync(f1);
                f_sync(f2);
            }
        } else {
            now = osKernelGetTickCount();
            f_sync(f1);
            f_sync(f2);
        }
    }
}