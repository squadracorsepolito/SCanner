#ifndef SDCARD_H
#define SDCARD_H

#include "fatfs.h"

FRESULT sdcard_init(void);
void sdcard_deinit(void);
FIL* sdcard_fopen(const TCHAR *path, BYTE mode, uint32_t timeout);
FRESULT sdcard_fopen_logger(FIL** f1, const TCHAR *path1, BYTE mode1, FIL** f2, const TCHAR *path2, BYTE mode2);
FRESULT sdcard_fclose(FIL *fp);
DIR* sdcard_opendir(const TCHAR *path, uint32_t timeout);
FRESULT sdcard_closedir(DIR *dir);

#endif // SDCARD_H