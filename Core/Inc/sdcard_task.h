#ifndef SDCARD_TASK_H
#define SDCARD_TASK_H

#include "fatfs.h"
#include "can_task.h"

void sdcardInit();
uint32_t sdcardAddMsgFromISR(CAN_frame_t *frame, CAN_NETWORK net, uint32_t *hptw);
void sdcardTask(void *argument);

#endif // SDCARD_TASK_H