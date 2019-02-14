/**
  **************************
  * @file    log.c
  * @author  Michael Neuhofer
  * @author  Paul Götzinger
  * @version V1.0
  * @date    13.11.2016
  * @brief   usb driver module
  **************************
  */
#include "log.h"
#include "usb.h"
#include <stdio.h>
#include <stdarg.h>

#if LOG_DEST == LOG_UART || LOG_DEST == LOG_GPS
#include "uart.h"
#endif

#define BUFFER_LEN 256

static uint8_t buffer[BUFFER_LEN];

static uint8_t init = 0;

#if LOG_DEST == LOG_UART || LOG_DEST == LOG_GPS
UART_Instance uart;
#endif

void LOG_Init()
{
#if LOG_DEST == LOG_NONE
	//no logging, no init
	return;
#elif LOG_DEST == LOG_USB
	USB_Init();
#elif LOG_DEST == LOG_UART
	//init uart for logging
    UART_Config uart_conf;
    
    uart_conf.uart = USART2;
    uart_conf.baud = UART_BaudRate_9600;
    uart_conf.rxDmaChannel = DMA1_Channel5;
    uart_conf.rxBoard = GPIOA;
    uart_conf.rxPin = GPIO_PIN_2;
    uart_conf.rxAF = GPIO_AF4_USART2;
    uart_conf.txDmaChannel = DMA1_Channel4;
    uart_conf.txBoard = GPIOA;
    uart_conf.txPin = GPIO_PIN_3;
    uart_conf.txAF = GPIO_AF4_USART2;
    
    UART_Init(&uart, &uart_conf);
#elif LOG_DEST == LOG_GPS
	//init uart for logging
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
#endif
	init = 1;
}


void LOG_Log(const char * format, ...)
{
	if (init == 0) {
		return;
	}

	va_list args;
	va_start (args, format);
	
	uint16_t len = vsnprintf ((char*)buffer, BUFFER_LEN, format, args);

#if LOG_DEST == LOG_NONE
	//no logging, should skipped becaus no init
#elif LOG_DEST == LOG_USB
	USB_SendData (buffer, len);
	HAL_Delay(2);
#elif LOG_DEST == LOG_UART || LOG_DEST == LOG_GPS
	//send data  via uart
	UART_SendData(&uart, len, buffer);
#endif

	va_end (args); 
}

void LOG_BitArray(uint8_t *array, uint16_t len) {
	if (array != 0 && len >= BUFFER_LEN-1) {
		return;
	}
	for (uint16_t i = 0; i < len; i++) {
		buffer[i] = array[i] > 0 ? '1' : '0';
	}
	buffer[len] = '\n';
	
#if LOG_DEST == LOG_NONE
	//no logging, should skipped becaus no init
#elif LOG_DEST == LOG_USB
	USB_SendData(buffer, len+1);
	HAL_Delay(2);
#elif LOG_DEST == LOG_UART || LOG_DEST == LOG_GPS
	//send data  via uart
	UART_SendData(&uart, len, buffer);
#endif

}
