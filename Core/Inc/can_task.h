#ifndef CAN_TASK_H
#define CAN_TASK_H

#include "fdcan.h"
#include "FreeRTOS.h"
#include "queue.h"

#define CAN_QUEUE_SIZE 32

typedef enum {
    CAN_NET1,
    CAN_NET2
} CAN_NETWORK;

typedef struct {
    uint16_t id;
    uint8_t dlc;
    uint8_t data[8];
} CAN_frame_t;

typedef struct {
    struct {
        FDCAN_HandleTypeDef *const hfdcan;
        FDCAN_FilterTypeDef *const sFilterConfig;
        uint8_t sFilterConfigN;
        uint32_t ActiveITs;
    } Init;
    QueueHandle_t rx_queue;
    QueueHandle_t tx_queue;

    uint8_t rx_buf[CAN_QUEUE_SIZE * sizeof(CAN_frame_t)];
    uint8_t tx_buf[CAN_QUEUE_SIZE * sizeof(CAN_frame_t)];

    StaticQueue_t rx_static_queue;
    StaticQueue_t tx_static_queue;
} canNetwork_t;

uint8_t canSend(CAN_frame_t *const frame, canNetwork_t *const network, uint32_t ticksToWait);
uint8_t canRecv(CAN_frame_t *const frame, canNetwork_t *const network, uint32_t ticksToWait);
uint8_t canIsFramePending(canNetwork_t *const network);
void can_task_inc_wac(void);

#endif // CAN_TASK_H
