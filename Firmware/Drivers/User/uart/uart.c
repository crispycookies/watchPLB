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

#define UART_MODULE_COUNT 4

typedef enum {
	UART_1 = 0,
	UART_2 = 1,
	UART_4 = 2,
	UART_5 = 3
} UART_Modules;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart);
void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *dma);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart);

void USART1_IRQHandler();
void USART2_IRQHandler();
void USART4_5_IRQHandler();

static void startTransmit(UART_Instance* inst);
static void startReceive(UART_Instance* inst);

static UART_Instance* instances[UART_MODULE_COUNT];

void UART_Init(UART_Instance* inst, UART_Config* conf) {
	for (uint8_t i = 0; i < UART_MODULE_COUNT; i++) {
		instances[i] = 0;
	}

	memset(&(inst->txDma), 0, sizeof(DMA_HandleTypeDef));
	memset(&(inst->rxDma), 0, sizeof(DMA_HandleTypeDef));
	memset(&(inst->uart), 0, sizeof(UART_HandleTypeDef));

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

	__HAL_RCC_DMA1_CLK_ENABLE();

	uint8_t irq = 0;

	if (conf->uart == USART1) {
		__HAL_RCC_USART1_CLK_ENABLE();
		irq = USART1_IRQn;
		instances[UART_1] = inst;
	} else if (conf->uart == USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();
		irq = USART2_IRQn;
		instances[UART_2] = inst;
	} else if (conf->uart == USART4) {
		__HAL_RCC_USART4_CLK_ENABLE();
		irq = USART4_5_IRQn;
		instances[UART_4] = inst;
	} else if (conf->uart == USART5) {
		__HAL_RCC_USART5_CLK_ENABLE();
		irq = USART4_5_IRQn;
		instances[UART_5] = inst;
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

	inst->uart.Instance = conf->uart;
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
	inst->rxDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
	inst->rxDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	inst->rxDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	inst->rxDma.Init.MemInc = DMA_MINC_ENABLE;
	inst->rxDma.Init.PeriphInc = DMA_PINC_DISABLE;
	inst->rxDma.Init.Mode = DMA_NORMAL;
	HAL_DMA_Init(&(inst->rxDma));
	__HAL_LINKDMA((&inst->uart), hdmarx, inst->rxDma);
	DMA_RegisterInterrupt(&(inst->txDma));

    HAL_NVIC_EnableIRQ(irq);

	inst->rxCircHead = 0;
	inst->rxCircTail = 0;
	inst->rxCircWrap = FALSE;

	inst->txCircHead = 0;
	inst->txCircTail = 0;
	inst->txCircWrap = FALSE;

	startReceive(inst);
}

uint8_t UART_SendByte(UART_Instance* inst, uint8_t byte) {
	if (inst->txCircWrap && inst->txCircHead == inst->txCircTail) {
		return FALSE;
	}
	inst->txCircBuf[inst->txCircHead++] = byte;
	if (inst->txCircHead >= UART_TXBUFFER_SIZE) {
		inst->txCircHead = 0;
		inst->txCircWrap = TRUE;
	}

	startTransmit(inst);
	return TRUE;
}

uint8_t UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	if (len > UART_TXBUFFER_SIZE) {
		return FALSE;
	} else if (inst->txCircWrap) {
		if ((inst->txCircTail - inst->txCircHead) < len) {
			return FALSE;
		}
	} else if ((UART_TXBUFFER_SIZE - inst->txCircTail + inst->rxCircHead) < len) {
		return FALSE;
	}
	if (len > (UART_TXBUFFER_SIZE - inst->txCircHead)) {
		inst->txCircWrap = TRUE;
	}
	for (uint16_t i = 0; i < len; i++) {
		inst->txCircBuf[(inst->txCircHead+i)%UART_TXBUFFER_SIZE] = data[i];
	}
	inst->txCircHead = (inst->txCircHead+len)%UART_TXBUFFER_SIZE;

	startTransmit(inst);
	return TRUE;
}

uint8_t UART_SendString(UART_Instance* inst, uint8_t *byte) {
	return UART_SendData(inst, strlen((char*)byte), byte);
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
	if (HAL_UART_GetState(&(inst->uart)) == HAL_UART_STATE_BUSY_TX) {
		return;
	}
	inst->txCount = inst->txCircWrap == FALSE
			? inst->txCircHead - inst->txCircTail
			: UART_TXBUFFER_SIZE - inst->txCircTail;
	if (inst->txCount > 0)
		HAL_UART_Transmit_DMA(&(inst->uart), inst->txCircBuf + inst->txCircTail, inst->txCount);
}

static void startReceive(UART_Instance* inst) {
	if (HAL_UART_GetState(&(inst->uart)) == HAL_UART_STATE_BUSY_RX) {
		return;
	}
	inst->rxCount = inst->rxCircWrap == FALSE
			? UART_RXBUFFER_SIZE - inst->rxCircHead
			: inst->rxCircTail - inst->rxCircHead;

	if (inst->rxCount > 0)
		HAL_UART_Receive_DMA(&(inst->uart), inst->rxCircBuf + inst->rxCircHead, inst->rxCount);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart) {
	UART_Instance *inst = 0;
	if (uart->Instance == USART1) {
		inst = instances[UART_1];
	} else if (uart->Instance == USART2) {
		inst = instances[UART_2];
	} else if (uart->Instance == USART4) {
		inst = instances[UART_4];
	} else if (uart->Instance == USART5) {
		inst = instances[UART_5];
	} else {
		return;
	}

	uint16_t cnt = inst->rxCount - __HAL_DMA_GET_COUNTER(&(inst->rxDma));

	inst->rxCircHead += cnt;
	if (inst->rxCircHead >= UART_RXBUFFER_SIZE) {
		inst->rxCircHead = 0;
		inst->rxCircHead = TRUE;
	}

	startReceive(inst);
}

void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *dma) {
	HAL_UART_RxCpltCallback(dma->Parent);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart) {
	UART_Instance *inst = 0;
	if (uart->Instance == USART1) {
		inst = instances[UART_1];
	} else if (uart->Instance == USART2) {
		inst = instances[UART_2];
	} else if (uart->Instance == USART4) {
		inst = instances[UART_4];
	} else if (uart->Instance == USART5) {
		inst = instances[UART_5];
	} else {
		return;
	}

	uint16_t cnt = inst->txCount - __HAL_DMA_GET_COUNTER(&(inst->txDma));

	inst->txCircTail += cnt;
	if (inst->txCircTail >= UART_TXBUFFER_SIZE) {
		inst->txCircTail = 0;
		inst->txCircWrap = FALSE;
	}

	startTransmit(inst);
}

void USART1_IRQHandler() {
	if (instances[UART_1] != 0) {
		if (__HAL_UART_GET_IT(&(instances[UART_1]->uart),UART_IT_IDLE) != FALSE) {
			__HAL_UART_CLEAR_IT(&(instances[UART_1]->uart),UART_IT_IDLE);
			HAL_DMA_Abort_IT(&(instances[UART_1]->rxDma));
		} else {
			HAL_UART_IRQHandler(&(instances[UART_1]->uart));
		}
	}
}

void USART2_IRQHandler() {
	if (instances[UART_2] != 0) {
		if (__HAL_UART_GET_IT(&(instances[UART_2]->uart),UART_IT_IDLE) != FALSE) {
			__HAL_UART_CLEAR_IT(&(instances[UART_2]->uart),UART_IT_IDLE);
			HAL_DMA_Abort_IT(&(instances[UART_2]->rxDma));
		} else {
			HAL_UART_IRQHandler(&(instances[UART_2]->uart));
		}
	}
}

void USART4_5_IRQHandler() {
	if (instances[UART_4] != 0) {
		if (__HAL_UART_GET_IT(&(instances[UART_4]->uart),UART_IT_IDLE) != FALSE) {
			__HAL_UART_CLEAR_IT(&(instances[UART_4]->uart),UART_IT_IDLE);
			HAL_DMA_Abort_IT(&(instances[UART_4]->rxDma));
		} else {
			HAL_UART_IRQHandler(&(instances[UART_4]->uart));
		}
	}
	if (instances[UART_5] != 0) {
		if (__HAL_UART_GET_IT(&(instances[UART_5]->uart),UART_IT_IDLE) != FALSE) {
			__HAL_UART_CLEAR_IT(&(instances[UART_5]->uart),UART_IT_IDLE);
			HAL_DMA_Abort_IT(&(instances[UART_5]->rxDma));
		} else {
			HAL_UART_IRQHandler(&(instances[UART_5]->uart));
		}
	}
}
