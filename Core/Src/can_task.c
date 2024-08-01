#include "can_task.h"
#include "usart.h"
#include "sdcard_task.h"
#include "tim.h"

#include <string.h>
#include <stdio.h>

uint16_t wrapAroundCounter = 0;

FDCAN_FilterTypeDef can1_filters[] = {
    {
        .IdType = FDCAN_STANDARD_ID,
        .FilterIndex = 0,
        .FilterType = FDCAN_FILTER_RANGE,
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
        .FilterID1 = 0,
        .FilterID2 = 0x7FF,
        .RxBufferIndex = 0
    }
};

FDCAN_FilterTypeDef can2_filters[] = {
    {
        .IdType = FDCAN_STANDARD_ID,
        .FilterIndex = 0,
        .FilterType = FDCAN_FILTER_RANGE,
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO1,
        .FilterID1 = 0,
        .FilterID2 = 0x7FF,
        .RxBufferIndex = 0
    }
};

canNetwork_t can1_network = {
    .Init = {
        .hfdcan = &hfdcan1,
        .sFilterConfig = can1_filters,
        .sFilterConfigN = sizeof(can1_filters)/sizeof(can1_filters[0]),
        .ActiveITs = FDCAN_IT_RX_FIFO0_NEW_MESSAGE
    },
    .tx_queue = NULL,
    .rx_queue = NULL,
    .tx_static_queue = NULL,
    .rx_static_queue = NULL
};

canNetwork_t can2_network = {
    .Init = {
        .hfdcan = &hfdcan2,
        .sFilterConfig = can2_filters,
        .sFilterConfigN = sizeof(can2_filters)/sizeof(can2_filters[0]),
        .ActiveITs = FDCAN_IT_RX_FIFO1_NEW_MESSAGE
    },
    .tx_queue = NULL,
    .rx_queue = NULL,
    .tx_static_queue = NULL,
    .rx_static_queue = NULL
};

void canEnable(FDCAN_HandleTypeDef *const hfdcan, uint32_t ActiveITs) {
    // Activate the notification for new data in FIFO for FDCAN
    if (HAL_FDCAN_ActivateNotification(hfdcan, ActiveITs, 0) != HAL_OK)
    {
        /* Notification Error */
        Error_Handler();
    }
}

void canDisable(FDCAN_HandleTypeDef *const hfdcan, uint32_t ActiveITs) {
    // Activate the notification for new data in FIFO for FDCAN
    if (HAL_FDCAN_DeactivateNotification(hfdcan, ActiveITs) != HAL_OK)
    {
        /* Notification Error */
        Error_Handler();
    }
}

void canInit(FDCAN_HandleTypeDef *const hfdcan, FDCAN_FilterTypeDef *const sFilterConfig, uint8_t sFilterConfigN) {
    for (uint8_t i=0; i<sFilterConfigN; ++i) {
        if (HAL_FDCAN_ConfigFilter(hfdcan, sFilterConfig + i) != HAL_OK)
        {
            /* Filter configuration Error */
            Error_Handler();
        }
    }

    HAL_TIM_Base_Start_IT(&htim3);
    HAL_FDCAN_EnableTimestampCounter(hfdcan, FDCAN_TIMESTAMP_EXTERNAL);
    
    // Start FDCAN
    if(HAL_FDCAN_Start(hfdcan)!= HAL_OK)
    {
        Error_Handler();
    }
}

uint8_t canSend(CAN_frame_t *const frame, canNetwork_t *const network, uint32_t ticksToWait) {
    return network->tx_queue && (xQueueSendToBack(network->tx_queue, frame, ticksToWait) == pdTRUE);
}

uint8_t canRecv(CAN_frame_t *const frame, canNetwork_t *const network, uint32_t ticksToWait) {
    return network->rx_queue && (xQueueReceive(network->rx_queue, frame, ticksToWait) == pdTRUE);
}

uint8_t canIsFramePending(canNetwork_t *const network) {
    return network->rx_queue && uxQueueMessagesWaiting(network->rx_queue);
}

void canTask(const void * argument) {
    canNetwork_t *const network = (canNetwork_t *const)argument;
    FDCAN_TxHeaderTypeDef TxHeader;
    CAN_frame_t tx_frame;

    network->rx_queue = xQueueCreateStatic(CAN_QUEUE_SIZE, sizeof(CAN_frame_t), network->rx_buf, &network->rx_static_queue);
    network->tx_queue = xQueueCreateStatic(CAN_QUEUE_SIZE, sizeof(CAN_frame_t), network->tx_buf, &network->tx_static_queue);

    canInit(network->Init.hfdcan, network->Init.sFilterConfig, network->Init.sFilterConfigN);

    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    while(1) {
        if(xQueueReceive(network->tx_queue, &tx_frame, portMAX_DELAY)) {
            TxHeader.Identifier = tx_frame.id;
            TxHeader.DataLength = tx_frame.dlc;

            while(HAL_FDCAN_AddMessageToTxFifoQ(network->Init.hfdcan, &TxHeader, tx_frame.data) != HAL_OK 
                    && network->Init.hfdcan->ErrorCode == HAL_FDCAN_ERROR_FIFO_FULL);
        }
    }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    BaseType_t hptw = pdFALSE;
    CAN_frame_t frame;
    FDCAN_RxHeaderTypeDef rx_h;
    uint32_t exCounter = wrapAroundCounter << 16;

    if(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_h, frame.data);
        frame.id = rx_h.Identifier;
        frame.dlc = rx_h.DataLength;

        xQueueSendToBackFromISR(can1_network.rx_queue, &frame, &hptw);
        sdcardAddMsgFromISR(&frame, CAN_NET1, exCounter | rx_h.RxTimestamp, &hptw);
        portYIELD_FROM_ISR( hptw );
    }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
    BaseType_t hptw = pdFALSE;
    CAN_frame_t frame;
    FDCAN_RxHeaderTypeDef rx_h;
    uint32_t exCounter = wrapAroundCounter << 16;

    if(RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &rx_h, frame.data);
        frame.id = rx_h.Identifier;
        frame.dlc = rx_h.DataLength;

        xQueueSendToBackFromISR(can2_network.rx_queue, &frame, &hptw);
        sdcardAddMsgFromISR(&frame, CAN_NET2, exCounter | rx_h.RxTimestamp, &hptw);
        portYIELD_FROM_ISR( hptw );
    }
}

void can_task_inc_wac(void) {
    ++wrapAroundCounter;
}