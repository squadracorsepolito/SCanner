#include "tftp_if.h"

#include "FreeRTOS.h"
#include "lwip.h"
#include "lwip/apps/tftp_server.h"
#include "fatfs.h"
#include "sdcard.h"

#include <string.h>

extern osEventFlagsId_t System_InitEventHandle;

void* open(const char* fname, const char* mode, u8_t write);
void close(void* handle);
int read(void* handle, void* buf, int bytes);
int write(void* handle, struct pbuf* p);

char buffer[64];

struct tftp_context ctx = {
    .open = open,
    .close = close,
    .read = read,
    .write = write
};

void tftp_if_init() {
    LOCK_TCPIP_CORE();
    tftp_init(&ctx);
    UNLOCK_TCPIP_CORE();
}

void* open(const char* fname, const char* mode, u8_t write) {
    char *idp = strstr(fname, "index");
    if(idp) {
        buffer[0] = 0;
        strncpy(buffer, fname, idp-fname);
        if(idp > fname && *(idp-1) == '/')
            buffer[idp-fname-1] = 0;
        return buffer;
    }

    BYTE m = write ? FA_WRITE : FA_READ;

    return sdcard_fopen(fname, m, portMAX_DELAY);
}

void close(void* handle) {
    if(handle == buffer) {
        return;
    }

    sdcard_fclose(handle);
}

int read(void* handle, void* buf, int bytes) {
    FRESULT res;
    uint32_t br, brtot;

    if(handle == buffer) {
        char *cur = buf;
        DIR dir;
        res = f_opendir(&dir, (char*)buffer);
        if(res != FR_OK) {
            return 0;
        }

        FILINFO fileInfo;
        for(;;) {
            res = f_readdir(&dir, &fileInfo);
            if((res != FR_OK) || (fileInfo.fname[0] == '\0')) {
                break;
            }

            if(fileInfo.fattrib & AM_DIR) {
                cur += snprintf(cur, (char*)buf+bytes-cur, "D - %s\r\n", fileInfo.fname);
            } else {
                cur += snprintf(cur, (char*)buf+bytes-cur, "F - %s\r\n", fileInfo.fname);
            }
        }
        f_closedir(&dir);

        return cur-(char*)buf;
    }
    
    res = f_read(handle, buf, bytes/2, (UINT*)&br);
    if(br == bytes/2) {
        brtot = br;
        res = f_read(handle, buf+bytes/2, bytes-bytes/2, (UINT*)&br);
        brtot += br;
    }
    if(res != FR_OK) {
        return 0;
    }
    return brtot;
}

int write(void* handle, struct pbuf* p) {
    FRESULT res;
    uint32_t tot_bw, bw;
    if(handle == buffer) {
        return 0;
    }

    do {
        res = f_write(handle, p->payload, p->len, (UINT*)&bw);
        if(res != FR_OK) {
            return tot_bw;
        }
        tot_bw += bw;
    } while(p->next && p->tot_len > p->len);

    return tot_bw;
}