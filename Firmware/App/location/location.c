#include "location.h"
#include "nmea.h"
#include "ubx.h"
#include "uart.h"
#include <string.h>

static NMEA_Instance nmea;
static UBX_Instance ubx;
static UART_Instance uart;

static POS_Position position;

static void positionCallback(POS_Position *pos);

void LOC_Init() {
    position.valid = 0;

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

    NMEA_Init(&nmea);
    NMEA_SetPositionCallback(&nmea, positionCallback);

    UBX_Init(&ubx);
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

static void positionCallback(POS_Position *pos) {
    if (pos != 0 && pos->valid != 0) {
        memcpy(&position, pos, sizeof(POS_Position));
    }
}

void LOC_InjectPosition(POS_Position* pos) {
    if (pos != 0) {
        memcpy(&position, pos, sizeof(POS_Position));
        LOG("[LOC] Position injected\n");
    }
}