/*
 * uart.c
 *
 *  Created on: 24.04.2018
 *      Author: Paul
 */


#include "uart.h"
#include "dma.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

static void startTransmit(UART_Instance* inst);

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
	inst->txDma.Instance = conf->txDmaChannel;
	inst->txDma.Parent = inst;
	inst->txDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
	inst->txDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	inst->txDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	inst->txDma.Init.MemInc = DMA_MINC_ENABLE;
	inst->txDma.Init.PeriphInc = DMA_PINC_DISABLE;
	inst->txDma.Init.Mode = DMA_NORMAL;
	HAL_DMA_Init(&(inst->txDma));
	__HAL_LINKDMA((&inst->uart), hdmatx, inst->txDma);
	DMA_RegisterInterrupt(&(inst->txDma));

	//RX
	inst->rxDma.Instance = conf->txDmaChannel;
	inst->rxDma.Parent = inst;
	inst->rxDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
	inst->rxDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	inst->rxDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	inst->rxDma.Init.MemInc = DMA_MINC_ENABLE;
	inst->rxDma.Init.PeriphInc = DMA_PINC_DISABLE;
	inst->rxDma.Init.Mode = DMA_NORMAL;
	HAL_DMA_Init(&(inst->rxDma));
	__HAL_LINKDMA((&inst->uart), hdmarx, inst->rxDma);
	DMA_RegisterInterrupt(&(inst->txDma));



}

void UART_SendByte(UART_Instance* inst, uint8_t byte) {
	if (inst->txCircWrap && inst->txCircHead == inst->txCircTail) {
		return;	//TODO: Do ERROR (e.g return false)
	}
	inst->txCircBuf[inst->txCircHead++] = byte;
	if (inst->txCircHead >= UART_TXBUFFER_SIZE) {
		inst->txCircHead = 0;
		inst->txCircWrap = TRUE;
	}

	//TODO: if no transmission is in progress start transmission
}

void UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	if (len > UART_TXBUFFER_SIZE) {
		return; //TODO: Do ERROR (e.g return false)
	} else if (inst->txCircWrap) {
		if ((inst->txCircTail - inst->txCircHead) < len) {
			return; //TODO: Do ERROR (e.g return false)
		}
	} else if ((UART_TXBUFFER_SIZE - inst->txCircTail + inst->rxCircHead) < len) {
		return; //TODO: Do ERROR (e.g return false)
	}
	if (len > (UART_TXBUFFER_SIZE - inst->txCircHead)) {
		inst->txCircWrap = TRUE;
	}
	for (uint16_t i = 0; i < len; i++) {
		inst->txCircBuf[(inst->txCircHead+i)%UART_TXBUFFER_SIZE] = data[i];
	}
	inst->txCircHead = (inst->txCircHead+len)%UART_TXBUFFER_SIZE;

	//TODO: if no transmission is in progress start transmission
}

void UART_SendString(UART_Instance* inst, uint8_t *byte) {
	UART_SendData(inst, strlen((char*)byte), byte);
}

uint16_t UART_GetAvailableBytes(UART_Instance* inst) {
	if (inst->rxCircWrap) {
		return UART_RXBUFFER_SIZE - inst->rxCircTail + inst->rxCircHead;
	} else {
		return inst->rxCircHead - inst->rxCircTail;
	}
}

uint8_t UART_GetByte(UART_Instance* inst) {
	if (UART_GetAvailableBytes(inst) < 1) {
		return 0;	//ERROR
	}
	uint8_t byte = inst->rxCircBuf[inst->rxCircTail++];
	if (inst->rxCircTail >= UART_RXBUFFER_SIZE) {
		inst->rxCircWrap = FALSE;
		inst->rxCircTail = 0;
	}
	return byte;
}

uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	if (UART_GetAvailableBytes(inst) < len) {
		len = UART_GetAvailableBytes(inst);
	}
	for (uint16_t i = 0; i < len; i++) {
		data[i] = inst->rxCircBuf[(inst->rxCircTail+i)%UART_RXBUFFER_SIZE];
	}
	if ((UART_RXBUFFER_SIZE - inst->rxCircTail) < len) {
		inst->rxCircWrap = FALSE;
	}
	inst->rxCircTail = (inst->rxCircTail + len) % UART_RXBUFFER_SIZE;
	return len;
}

static void startTransmit(UART_Instance* inst) {
	if ()
}
