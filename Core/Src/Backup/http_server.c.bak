#include "http_server.h"

#include "sdcard.h"
#include "fatfs.h"
#include "fs.h"
#include "httpd.h"

#include <string.h>

typedef enum { URI_DIR, URI_FIL } http_uri_type;

typedef struct {
    http_uri_type t;
    void *p;
    char name[256];
} http_uri;

void http_server_init(void) {
  LOCK_TCPIP_CORE();
  httpd_init();
  UNLOCK_TCPIP_CORE();
}

int fs_open_custom(struct fs_file *file, const char *name) {
    FRESULT res;
    FILINFO fno;
    http_uri *uri;

    uint8_t is_root = !strcmp(name, "/");

    if(!is_root) {
        res = f_stat(name, &fno);
        if(res != FR_OK && !is_root) {
            return 0;
        }
    }

    uri = malloc(sizeof(http_uri));
    if(!uri) {
        return 0;
    }

    strncpy(uri->name, name+1, sizeof(uri->name));

    if(is_root || fno.fattrib & AM_DIR) {
        uri->t = URI_DIR;
        uri->p = sdcard_opendir(name, portMAX_DELAY);
        file->len = 256;
        strcat(name, ".html");
    } else {
        uri->t = URI_FIL;
        uri->p = sdcard_fopen(name, FA_READ, portMAX_DELAY);
        file->len = fno.fsize;
    }

    if(!uri->p) {
        free(uri);
        return 0;
    }

    file->pextension = uri;
    file->index = 0;
    
    return 1;
}

void fs_close_custom(struct fs_file *file) {
    http_uri *uri = (http_uri*)file->pextension;
    
    if(uri->t == URI_DIR) {
        sdcard_closedir(uri->p);
    } else {
        sdcard_fclose(uri->p);
    }

    free(uri);
}

int fs_read_custom(struct fs_file *file, char *buffer, int count) {
    FRESULT res;
    UINT br = 0;
    UINT btr;
    http_uri *uri = (http_uri*)file->pextension;

    buffer[0] = 0;
    
    if(uri->t == URI_DIR) {
        FILINFO fno;
        btr = count;
        if(file->index == 0) {
            br += snprintf(buffer, btr-br, "<html><body>");
            br += snprintf(buffer+br, btr-br, "<h1>sdcard/%s</h1>", uri->name);
            file->index++;
        }
        while(br < btr - 128) {
            res = f_readdir(uri->p, &fno);
            if((res != FR_OK) || (fno.fname[0] == '\0')) {
                br += snprintf(buffer+br, btr-br, "</body></html>");
                file->index = file->len;
                break;
            }
            
            if(fno.fattrib & AM_DIR) {
                br += snprintf(buffer+br, btr - br, "<a href=\"%s/%s\">D - %s</a><br>", uri->name, fno.fname, fno.fname);
            } else {
                br += snprintf(buffer+br, btr - br, "<a href=\"%s/%s\">F - %s - %luB</a><br>", uri->name, fno.fname, fno.fname, fno.fsize);
            }
        }
        return br;
    } else {
        UINT brtot = 0;

        while(brtot < count) {
            btr = count - brtot < 256 ? count - brtot : 256;
            res = f_read(uri->p, buffer+brtot, btr, &br);
            if(res != FR_OK) {
                return 0;
            }
            brtot += br;
        }

        file->index += brtot;
        return brtot;
    }
}