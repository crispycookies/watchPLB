/*
 * uart.c
 *
 *  Created on: 24.04.2018
 *      Author: Paul
 */


#include "uart.h"



void UART_Init(UART_Instance* inst, UART_Config* conf) {
	if (conf->txBoard == GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	} else if (conf->txBoard == GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
	} else if (conf->txBoard == GPIOC) {
		__HAL_RCC_GPIOC_CLK_ENABLE();
	} else if (conf->txBoard == GPIOH) {
		__HAL_RCC_GPIOH_CLK_ENABLE();
	}

	if (conf->rxBoard != conf->txBoard) {
		if (conf->rxBoard == GPIOA) {
			__HAL_RCC_GPIOA_CLK_ENABLE();
		} else if (conf->rxBoard == GPIOB) {
			__HAL_RCC_GPIOB_CLK_ENABLE();
		} else if (conf->rxBoard == GPIOC) {
			__HAL_RCC_GPIOC_CLK_ENABLE();
		} else if (conf->rxBoard == GPIOH) {
			__HAL_RCC_GPIOH_CLK_ENABLE();
		}
	}

	if (conf->usart == USART3) {
		__HAL_RCC
	}
}

void UART_SendByte(UART_Instance* inst, uint8_t byte) {

}

void UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data) {

}

void UART_SendString(UART_Instance* inst, uint8_t *byte) {

}

uint16_t UART_GetAvailableBytes(UART_Instance* inst) {

}

uint8_t UART_GetByte(UART_Instance* inst) {

}

uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data) {

}
