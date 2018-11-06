#include "location.h"
#include "nmea.h"
#include "ubx.h"
#include "uart.h"
#include <string.h>

static NMEA_Instance nmea;
static UBX_Instance ubx;
static UART_Instance uart;

static POS_Position position;
static uint8_t posAvailable;

static void positionCallback(POS_Position *pos);

void LOC_Init() {
    posAvailable = 0;

    UART_Config uart_conf;
    /* Needs to be configured
    uart_conf.uart = ;
    uart_conf.baud = UART_BaudRate_9600;
    uart_conf.rxDmaChannel = ;
    uart_conf.rxBoard = ;
    uart_conf.rxPin = ;
    uart_conf.rxAF = ;
    uart_conf.txDmaChannel ;
    uart_conf.txBoard = ;
    uart_conf.txPin = ;
    uart_conf.txAF = ;
    */
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
    return posAvailable;
}

POS_Position* LOC_GetLastPosition() {
    return &position;
}

static void positionCallback(POS_Position *pos) {
    if (pos != 0) {
        memcpy(&position, pos, sizeof(POS_Position));
        posAvailable = 1;
    }
}