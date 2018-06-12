/*
 * uart.c
 *
 *  Created on: 24.04.2018
 *      Author: Paul
 */


#include "uart.h"
#include <string.h>

#define TRUE 1;
#define FALSE 0;

void UART_Init(UART_Instance* inst, UART_Config* conf) {
	//enable GPIO clock
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

	if (conf->uart == USART1) {
		__HAL_RCC_USART1_CLK_ENABLE();
	} else if (conf->uart == USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();
	} else if (conf->uart == USART4) {
		__HAL_RCC_USART4_CLK_ENABLE();
	} else if (conf->uart == USART5) {
		__HAL_RCC_USART5_CLK_ENABLE();
	}

	//config rx pin
	GPIO_InitTypeDef gpio;
	gpio.Pin = conf->rxPin;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	gpio.Alternate = conf->rxAF;
	HAL_GPIO_Init(conf->rxBoard, &gpio);

	gpio.Pin = conf->txPin;
	gpio.Alternate = conf->txAF;
	HAL_GPIO_Init(conf->txBoard, &gpio);

	inst->uart.Init.BaudRate = conf->baud;
	inst->uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	inst->uart.Init.Mode = UART_MODE_TX_RX;
	inst->uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	inst->uart.Init.OverSampling = UART_OVERSAMPLING_16;
	inst->uart.Init.Parity = UART_PARITY_NONE;
	inst->uart.Init.StopBits = UART_STOPBITS_1;
	inst->uart.Init.WordLength = UART_WORDLENGTH_8B;

	HAL_UART_Init(&(inst->uart));


	//TX
	inst->dma.Init.Direction = DMA_MEMORY_TO_PERIPH;
	inst->dma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	inst->dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	inst->dma.Init.MemInc = DMA_MINC_ENABLE;
	inst->dma.Init.PeriphInc = DMA_PINC_DISABLE;
	HAL_DMA_Init(&(inst->dma));

	//TODO: DMA
	//TODO: Interrupt
}

void UART_SendByte(UART_Instance* inst, uint8_t byte) {
	if (inst->txWrap && inst->txHead == inst->txTail) {
		return;	//TODO: Do ERROR (e.g return false)
	}
	inst->txBuf[inst->txHead++] = byte;
	if (inst->txHead >= UART_TXBUFFER_SIZE) {
		inst->txHead = 0;
		inst->txWrap = TRUE;
	}

	//TODO: if no transmission is in progress start transmission
}

void UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	if (len > UART_TXBUFFER_SIZE) {
		return; //TODO: Do ERROR (e.g return false)
	} else if (inst->txWrap) {
		if ((inst->txTail - inst->txHead) < len) {
			return; //TODO: Do ERROR (e.g return false)
		}
	} else if ((UART_TXBUFFER_SIZE - inst->txTail + inst->rxHead) < len) {
		return; //TODO: Do ERROR (e.g return false)
	}
	if (len > (UART_TXBUFFER_SIZE - inst->txHead)) {
		inst->txWrap = TRUE;
	}
	for (uint16_t i = 0; i < len; i++) {
		inst->txBuf[(inst->txHead+i)%UART_TXBUFFER_SIZE] = data[i];
	}
	inst->txHead = (inst->txHead+len)%UART_TXBUFFER_SIZE;

	//TODO: if no transmission is in progress start transmission
}

void UART_SendString(UART_Instance* inst, uint8_t *byte) {
	UART_SendData(inst, strlen((char*)byte), byte);
}

uint16_t UART_GetAvailableBytes(UART_Instance* inst) {
	if (inst->rxWrap) {
		return UART_RXBUFFER_SIZE - inst->rxTail + inst->rxHead;
	} else {
		return inst->rxHead - inst->rxTail;
	}
}

uint8_t UART_GetByte(UART_Instance* inst) {
	if (UART_GetAvailableBytes(inst) < 1) {
		return 0;	//ERROR
	}
	uint8_t byte = inst->rxBuf[inst->rxTail++];
	if (inst->rxTail >= UART_RXBUFFER_SIZE) {
		inst->rxWrap = FALSE;
		inst->rxTail = 0;
	}
	return byte;
}

uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	if (UART_GetAvailableBytes(inst) < len) {
		len = UART_GetAvailableBytes(inst);
	}
	for (uint16_t i = 0; i < len; i++) {
		data[i] = inst->rxBuf[(inst->rxTail+i)%UART_RXBUFFER_SIZE];
	}
	if ((UART_RXBUFFER_SIZE - inst->rxTail) < len) {
		inst->rxWrap = FALSE;
	}
	inst->rxTail = (inst->rxTail + len) % UART_RXBUFFER_SIZE;
	return len;
}
