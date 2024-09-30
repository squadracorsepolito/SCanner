#include "sdcard.h"
#include "FreeRTOS.h"
#include "semphr.h"

uint8_t fs_buffer[_MAX_SS] __attribute__((section(".fsSection")));
FATFS fs = {
    .win = fs_buffer
};

uint8_t file_buffer[3][_MAX_SS] __attribute__((section(".fsSection")));
FIL log_f1 = {
    .buf = file_buffer[0]
};
FIL log_f2 = {
    .buf = file_buffer[1]
};
FIL f3 = {
    .buf = file_buffer[2]
};
DIR d;

extern osMutexId_t SDCARD_FILE_LOCKHandle;
extern osMutexId_t SDCARD_DIR_LOCKHandle;
extern osEventFlagsId_t System_InitEventHandle;

FRESULT sdcard_init(void) {
    FRESULT res = f_mount(&fs, "", 1);
    if(res == FR_OK)
        osEventFlagsSet(System_InitEventHandle, SDCARD_Init_Done);
    return res;
}

void sdcard_deinit(void) {
    f_close(&log_f1);
    f_close(&log_f2);
    f_close(&f3);
    f_closedir(&d);
}

FIL* sdcard_fopen(const TCHAR *path, BYTE mode, uint32_t timeout) {
    osMutexAcquire(SDCARD_FILE_LOCKHandle, timeout);
    FRESULT res = f_open(&f3, path, mode);
    if(res != FR_OK) {
        osMutexRelease(SDCARD_FILE_LOCKHandle);
        return NULL;
    }
    return &f3;
}

FRESULT sdcard_fopen_logger(FIL** f1, const TCHAR *path1, BYTE mode1, FIL** f2, const TCHAR *path2, BYTE mode2) {
    FRESULT res = f_open(&log_f1, path1, mode1);
    if(res != FR_OK)
        return res;
    *f1 = &log_f1;
    
    res = f_open(&log_f2, path2, mode2);
    if(res != FR_OK)
        return res;
    *f2 = &log_f2;

    return res;
}

FRESULT sdcard_fclose(FIL *fp) {
    FRESULT res;
    res = f_close(fp);
    if(fp == &f3) {
        osMutexRelease(SDCARD_FILE_LOCKHandle);
    }
    return res;
}

DIR* sdcard_opendir(const TCHAR *path, uint32_t timeout) {
    osMutexAcquire(SDCARD_DIR_LOCKHandle, timeout);
    FRESULT res = f_opendir(&d, path);
    if(res != FR_OK) {
        osMutexRelease(SDCARD_DIR_LOCKHandle);
        return NULL;
    }
    return &d;
}

FRESULT sdcard_closedir(DIR *dir) {
    if(dir == &d)
        osMutexRelease(SDCARD_DIR_LOCKHandle);
    return f_closedir(dir);
}
