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
	BAUD_9600,
	BAUD_19200,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200
} UART_BaudRate;

typedef struct {
	USART_TypeDef* usart;

	uint8_t  rxBuf[UART_RXBUFFER_SIZE];
	uint16_t rxHead;
	uint16_t rxTail;
	uint8_t  rxWrap;

	uint8_t  txBuf[UART_RXBUFFER_SIZE];
	uint16_t txHead;
	uint16_t txTail;
	uint8_t  txWrap;
} UART_Instance;

typedef struct {
	USART_TypeDef* usart;
	GPIO_TypeDef* txBoard;
	GPIO_TypeDef* rxBoard;
	uint16_t      txPin;
	uint16_t      rxPin;
	UART_BaudRate baud;
} UART_Config;

void UART_Init(UART_Instance* inst, UART_Config* conf);

void UART_SendByte(UART_Instance* inst, uint8_t byte);

void UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data);

void UART_SendString(UART_Instance* inst, uint8_t *byte);

uint16_t UART_GetAvailableBytes(UART_Instance* inst);

uint8_t UART_GetByte(UART_Instance* inst);

uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data);


#endif /* UART_H */
