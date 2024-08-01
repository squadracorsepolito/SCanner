#ifndef SDCARD_TASK_H
#define SDCARD_TASK_H

#include "fatfs.h"
#include "can_task.h"

void sdcardInit();
uint32_t sdcardAddMsgFromISR(CAN_frame_t *frame, CAN_NETWORK net, uint32_t timestamp, BaseType_t *hptw);
void sdcardTask(void *argument);
void sdcardForceSync(void);

#endif // SDCARD_TASK_H