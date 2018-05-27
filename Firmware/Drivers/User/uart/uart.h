/*
 * uart.h
 *
 *  Created on: 24.04.2018
 *      Author: Paul
 */

#ifndef UART_H
#define UART_H

#define UART_TXBUFFER_SIZE 256
#define UART_RXBUFFER_SIZE 256

typedef enum {
	BAUD_9600 = 9600,
	BAUD_19200 = 19200,
	BAUD_38400 = 38400,
	BAUD_57600 = 57600,
	BAUD_115200 = 115200
} UART_BaudRate;

typedef struct {
	UART_HandleTypeDef uart;

	DMA_HandleTypeDef rxDma;
	uint8_t  rxCircBuf[UART_RXBUFFER_SIZE];
	uint16_t rxCircHead;
	uint16_t rxCircTail;
	uint8_t  rxCircWrap;
	uint16_t rxCount;

	DMA_HandleTypeDef txDma;
	uint8_t  txCircBuf[UART_RXBUFFER_SIZE];
	uint16_t txCircHead;
	uint16_t txCircTail;
	uint8_t  txCircWrap;
	uint16_t txCount;
} UART_Instance;

typedef struct {
	USART_TypeDef*			uart;
	DMA_Channel_TypeDef*	txDmaChannel;
	DMA_Channel_TypeDef*	rxDmaChannel;
	GPIO_TypeDef* 			txBoard;
	GPIO_TypeDef* 			rxBoard;
	uint16_t      			txPin;
	uint16_t      			rxPin;
	uint32_t				txAF;
	uint32_t				rxAF;
	UART_BaudRate 			baud;
} UART_Config;

void UART_Init(UART_Instance* inst, UART_Config* conf);

void UART_SendByte(UART_Instance* inst, uint8_t byte);

void UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data);

void UART_SendString(UART_Instance* inst, uint8_t *byte);

uint16_t UART_GetAvailableBytes(UART_Instance* inst);

uint8_t UART_GetByte(UART_Instance* inst);

uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data);


#endif /* UART_H */
