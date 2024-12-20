#include "cannelloni_task.h"
#include "can_task.h"
#include <memory.h>
#include "usart.h"
#include "lwip.h"

#define CNL_BUF_SIZE 32

void cnl_can_rx(cannelloni_handle_t *const handle);
bool cnl_can_tx(cannelloni_handle_t *const handle, struct canfd_frame *const frame);

struct canfd_frame cnl1_tx_buf[CNL_BUF_SIZE];
struct canfd_frame cnl1_rx_buf[CNL_BUF_SIZE];

struct canfd_frame cnl2_tx_buf[CNL_BUF_SIZE];
struct canfd_frame cnl2_rx_buf[CNL_BUF_SIZE];

extern canNetwork_t can1_network;
extern canNetwork_t can2_network;

cannelloni_handle_t cnl1_handle = {
    .Init = {
        .can_buf_size = CNL_BUF_SIZE,
        .can_rx_buf = cnl1_rx_buf,
        .can_rx_fn = cnl_can_rx,
        .can_tx_buf = cnl1_tx_buf,
        .can_tx_fn = cnl_can_tx,
        .port = 20000,
        .remote_port = 20000,
        .user_data = &can1_network
    }
};
cannelloni_handle_t cnl2_handle = {
    .Init = {
        .can_buf_size = CNL_BUF_SIZE,
        .can_rx_buf = cnl2_rx_buf,
        .can_rx_fn = cnl_can_rx,
        .can_tx_buf = cnl2_tx_buf,
        .can_tx_fn = cnl_can_tx,
        .port = 20001,
        .remote_port = 20001,
        .user_data = &can2_network
    }
};

extern osEventFlagsId_t System_InitEventHandle;

void cnl_can_rx(cannelloni_handle_t *const handle) {
    canNetwork_t *can_network = handle->Init.user_data;

    while(canIsFramePending(can_network)) {
        CAN_frame_t msg;
        canRecv(&msg, &can1_network, 0);
		struct canfd_frame *frame = get_can_rx_frame(handle);
		if (frame) {
			frame->can_id = msg.id;
			frame->len = msg.dlc;
            memcpy(frame->data, msg.data, msg.dlc);
		}
    }
}

bool cnl_can_tx(cannelloni_handle_t *const handle, struct canfd_frame *const frame) {
    CAN_frame_t msg;
    canNetwork_t *can_network = handle->Init.user_data;

    msg.id = frame->can_id;
    msg.dlc = frame->len;
    memcpy(msg.data, frame->data, frame->len);

    return canSend(&msg, can_network, 0);
}

void cannelloniTask(const void * argument) {
    cannelloni_handle_t *const handle = (cannelloni_handle_t *const)argument;

    osEventFlagsWait(System_InitEventHandle, LWIP_Init_Done, osFlagsWaitAny, portMAX_DELAY);

    LOCK_TCPIP_CORE();
    init_cannelloni(handle);
    UNLOCK_TCPIP_CORE();

    while(1) {
        LOCK_TCPIP_CORE();
        run_cannelloni(handle);
        UNLOCK_TCPIP_CORE();
        vTaskDelay(10);
    }
}