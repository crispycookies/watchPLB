/**
 * @file location.c
 * @author Paul Götzinger
 * @brief Location module
 * @version 1.0
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "location.h"
#include "nmea.h"
#include "ubx.h"
#include "uart.h"
#include <string.h>

#define BUF_LEN 32

typedef enum {
    No,
    InProgress,
    Yes
} Configured;

static NMEA_Instance nmea;
static UBX_Instance ubx;
static UART_Instance uart;

static POS_Position position;
static Configured cfgState;

static uint8_t buf[BUF_LEN];

/**
 * @brief Callback function for received position
 * 
 * @param pos position
 */
static void positionCallback(POS_Position *pos);

/**
 * @brief callback function for other nmea messages
 * 
 * @param type nmea message type
 * @param data message payload
 * @param len payload length
 */
static void unknownCallback(NMEA_Type type, uint8_t* data, uint16_t len);

/**
 * @brief UBX acknowledge callback function
 * 
 * @param msgClass message class from message to be acknowledge
 * @param id message id from message to be acknowledge
 * @param ack acknowledge/not acknowledge
 */
static void ackCallback(UBX_Class msgClass, uint8_t id, UBX_Id_Ack ack);

void LOC_Init() {
    position.valid = POS_Valid_Flag_Invalid;
    cfgState = No;

    //configure uart
    UART_Config uart_conf;
    
    uart_conf.uart = USART4;
    uart_conf.baud = UART_BaudRate_9600;
    uart_conf.rxDmaChannel = DMA1_Channel6;
    uart_conf.rxBoard = GPIOC;
    uart_conf.rxPin = GPIO_PIN_11;
    uart_conf.rxAF = GPIO_AF6_USART4;
    uart_conf.txDmaChannel = DMA1_Channel7;
    uart_conf.txBoard = GPIOC;
    uart_conf.txPin = GPIO_PIN_10;
    uart_conf.txAF = GPIO_AF6_USART4;
    
    UART_Init(&uart, &uart_conf);

    //configure nmea interface
    NMEA_Init(&nmea);
    NMEA_SetPositionCallback(&nmea, positionCallback);
    NMEA_SetUnknownCallback(&nmea, unknownCallback);

    //configure ubx interface
    UBX_Init(&ubx);
    UBX_SetAckCallback(&ubx, ackCallback, UBX_Class_CFG, 0x17);
}

void LOC_Process() {
    while (UART_GetAvailableBytes(&uart) > 0) {
        uint8_t byte = UART_GetByte(&uart);
        NMEA_Process(&nmea, byte);
        UBX_Process(&ubx, byte);
    }
}

uint8_t LOC_PositionAvailable() {
    return position.valid;
}

POS_Position* LOC_GetLastPosition() {
    return &position;
}

void LOC_InjectPosition(POS_Position* pos) {
    if (pos != 0) {
        memcpy(&position, pos, sizeof(POS_Position));
        LOG("\n[LOC] Position injected\n");
    }
}

static void positionCallback(POS_Position *pos) {
    if (pos != 0 && pos->valid != 0) {
        memcpy(&position, pos, sizeof(POS_Position));
    }
}

static void unknownCallback(NMEA_Type type, uint8_t* data, uint16_t len) {
    if (cfgState == No) {
        LOG("\n[LOC] Configure NMEA\n");
        cfgState = InProgress;

        //configure gps module 
        uint16_t cnt = UBX_CreateNMEAConfigFrame(&ubx, buf, BUF_LEN);
        UART_SendData(&uart, cnt, buf);
    }
}

static void ackCallback(UBX_Class msgClass, uint8_t id, UBX_Id_Ack ack) {
    switch (msgClass) {
        case UBX_Class_CFG:
            if (id == 0x17) {
                if (ack == UBX_Id_Ack_Ack) {
                    cfgState = Yes;
                    LOG("\n[LOC] NMEA config ACK\n");
                } else {
                    LOG("\n[LOC] NMEA config NAK\n");
                }
                
            }
            break;
        default:
            break;
    }
}
