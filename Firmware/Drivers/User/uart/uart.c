/*
 * uart.c
 *
 *  Created on: 24.04.2018
 *      Author: Paul
 */


#include "uart.h"
#include "dma.h"
#include <string.h>
#include "stm32l0xx_hal_conf.h"

#define TRUE 1
#define FALSE 0

#define UART_MODULE_COUNT 4

typedef enum {
	UART_1 = 0,
	UART_2 = 1,
	UART_4 = 2,
	UART_5 = 3
} UART_Modules;

/**
 * @brief Receive complete callback function
 * 
 * @param uart uart handle
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart);

/**
 * @brief DMA receive abort callback function
 * 
 * @param dma dma handle
 */
void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *dma);

/**
 * @brief Transmit complete callback function
 * 
 * @param uart uart handle
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart);

/**
 * @brief USART 1 interrupt handler
 * 
 */
void USART1_IRQHandler();

/**
 * @brief USART 2 interrupt handler
 * 
 */
void USART2_IRQHandler();

/**
 * @brief USART 4 and 5 interrupt handler
 * 
 */
void USART4_5_IRQHandler();

/**
 * @brief start transmission
 * 
 * @param inst UART instance
 */
static void startTransmit(UART_Instance* inst);

/**
 * @brief start reception
 * 
 * @param inst UART instance
 */
static void startReceive(UART_Instance* inst);

static uint8_t init = FALSE;	//module init flag
static UART_Instance* instances[UART_MODULE_COUNT];	//uart instances table (needed for interrupts)

void UART_Init(UART_Instance* inst, UART_Config* conf) {
	if (!init) {
		//if not already initialized clear instance table
		for (uint8_t i = 0; i < UART_MODULE_COUNT; i++) {
			instances[i] = 0;
		}
		init = TRUE;
	}

	//no valid parameter given
	if ((!inst) || (!conf)) {
		return;
	}

	//reset HAL handler
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

	//enable dma clock
	__HAL_RCC_DMA1_CLK_ENABLE();

	uint8_t irq = 0;
	uint32_t txRequest = 0;
	uint32_t rxRequest = 0;

	//enable uart clock and retrieve dma request
	if (conf->uart == USART1) {
		__HAL_RCC_USART1_CLK_ENABLE();
		irq = USART1_IRQn;
		instances[UART_1] = inst;
		if ((conf->txDmaChannel == DMA1_Channel2) || ((conf->txDmaChannel == DMA1_Channel4))) {
			txRequest = DMA_REQUEST_3;
		}
		if ((conf->rxDmaChannel == DMA1_Channel3) || ((conf->rxDmaChannel == DMA1_Channel5))) {
			rxRequest = DMA_REQUEST_3;
		}
	} else if (conf->uart == USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();
		irq = USART2_IRQn;
		instances[UART_2] = inst;
		if ((conf->txDmaChannel == DMA1_Channel4) || ((conf->txDmaChannel == DMA1_Channel7))) {
			txRequest = DMA_REQUEST_4;
		}
		if ((conf->rxDmaChannel == DMA1_Channel5) || ((conf->rxDmaChannel == DMA1_Channel6))) {
			rxRequest = DMA_REQUEST_4;
		}
	} else if (conf->uart == USART4) {
		__HAL_RCC_USART4_CLK_ENABLE();
		irq = USART4_5_IRQn;
		instances[UART_4] = inst;
		if ((conf->txDmaChannel == DMA1_Channel3) || ((conf->txDmaChannel == DMA1_Channel7))) {
			txRequest = DMA_REQUEST_4;
		}
		if ((conf->rxDmaChannel == DMA1_Channel4) || ((conf->rxDmaChannel == DMA1_Channel6))) {
			rxRequest = DMA_REQUEST_4;
		}
	} else if (conf->uart == USART5) {
		__HAL_RCC_USART5_CLK_ENABLE();
		irq = USART4_5_IRQn;
		instances[UART_5] = inst;
		if ((conf->txDmaChannel == DMA1_Channel3) || ((conf->txDmaChannel == DMA1_Channel7))) {
			txRequest = DMA_REQUEST_4;
		}
		if ((conf->rxDmaChannel == DMA1_Channel4) || ((conf->rxDmaChannel == DMA1_Channel6))) {
			rxRequest = DMA_REQUEST_4;
		}
	}

	//config rx pin
	GPIO_InitTypeDef gpio;
	gpio.Pin = conf->rxPin;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	gpio.Alternate = conf->rxAF;
	HAL_GPIO_Init(conf->rxBoard, &gpio);

	//config tx pin
	gpio.Pin = conf->txPin;
	gpio.Alternate = conf->txAF;
	HAL_GPIO_Init(conf->txBoard, &gpio);

	//configure HAL uart module
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

	//configure DMA for transmission
	inst->txDma.Instance = conf->txDmaChannel;
	inst->txDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
	inst->txDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	inst->txDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	inst->txDma.Init.MemInc = DMA_MINC_ENABLE;
	inst->txDma.Init.PeriphInc = DMA_PINC_DISABLE;
	inst->txDma.Init.Mode = DMA_NORMAL;
	inst->txDma.Init.Request = txRequest;
	HAL_DMA_Init(&(inst->txDma));
	__HAL_LINKDMA(&(inst->uart), hdmatx, inst->txDma);
	DMA_RegisterInterrupt(&(inst->txDma));

	//configure DMA for reception
	inst->rxDma.Instance = conf->rxDmaChannel;
	inst->rxDma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	inst->rxDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	inst->rxDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	inst->rxDma.Init.MemInc = DMA_MINC_ENABLE;
	inst->rxDma.Init.PeriphInc = DMA_PINC_DISABLE;
	inst->rxDma.Init.Mode = DMA_NORMAL;
	inst->rxDma.Init.Request = rxRequest;
	HAL_DMA_Init(&(inst->rxDma));
	__HAL_LINKDMA(&(inst->uart), hdmarx, inst->rxDma);
	DMA_RegisterInterrupt(&(inst->rxDma));

	//enable uart interrupt
    HAL_NVIC_EnableIRQ(irq);

	//reset receive buffer
	inst->rxCircHead = 0;
	inst->rxCircTail = 0;
	inst->rxCircWrap = FALSE;

	//reset transmit buffer
	inst->txCircHead = 0;
	inst->txCircTail = 0;
	inst->txCircWrap = FALSE;

	//start reception
	startReceive(inst);
}

uint8_t UART_SendByte(UART_Instance* inst, uint8_t byte) {
	//invalid parameter
	if (!inst) {
		return FALSE;
	}

	//return false on full buffer
	if (inst->txCircWrap && inst->txCircHead == inst->txCircTail) {
		return FALSE;
	}

	//load byte into buffer
	inst->txCircBuf[inst->txCircHead++] = byte;

	//if end of buffer reached wrap head
	if (inst->txCircHead >= UART_TXBUFFER_SIZE) {
		inst->txCircHead = 0;
		inst->txCircWrap = TRUE;
	}

	//start transmission
	startTransmit(inst);
	return TRUE;
}

uint8_t UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	//invalid parameter
	if ((!inst) || (!data) || (!len)) {
		return FALSE;
	}

	//return false if length of data is larger than the actual size of the buffer
	if (len > UART_TXBUFFER_SIZE) {
		return FALSE;
	//return false if length of data is larger than the available space
	} else if (inst->txCircWrap) {
		if ((inst->txCircTail - inst->txCircHead) < len) {
			return FALSE;
		}
	} else if ((UART_TXBUFFER_SIZE - inst->txCircTail + inst->txCircHead) < len) {
		return FALSE;
	}

	//check if wrap is needed
	if (len > (UART_TXBUFFER_SIZE - inst->txCircHead)) {
		inst->txCircWrap = TRUE;
	}

	//copy data in buffer
	for (uint16_t i = 0; i < len; i++) {
		inst->txCircBuf[(inst->txCircHead+i)%UART_TXBUFFER_SIZE] = data[i];
	}

	//adjust buffer head position
	inst->txCircHead = (inst->txCircHead+len)%UART_TXBUFFER_SIZE;

	//start transmission
	startTransmit(inst);
	return TRUE;
}

uint8_t UART_SendString(UART_Instance* inst, uint8_t *byte) {
	return UART_SendData(inst, strlen((char*)byte), byte);
}

uint16_t UART_GetAvailableBytes(UART_Instance* inst) {
	//invalid parameter
	if (!inst) {
		return 0;
	}

	if (inst->rxCircWrap) {
		return UART_RXBUFFER_SIZE - inst->rxCircTail + inst->rxCircHead;
	} else {
		return inst->rxCircHead - inst->rxCircTail;
	}
}

uint8_t UART_GetByte(UART_Instance* inst) {
	//invalid parameter
	if (!inst) {
		return 0;
	}

	//check if bytes are available
	if (UART_GetAvailableBytes(inst) < 1) {
		return 0;
	}
	//retrieve byte
	uint8_t byte = inst->rxCircBuf[inst->rxCircTail++];
	//wrap tail if necessary
	if (inst->rxCircTail >= UART_RXBUFFER_SIZE) {
		inst->rxCircWrap = FALSE;
		inst->rxCircTail = 0;
	}
	return byte;
}

uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data) {
	//invalid parameter
	if ((!inst) || (!data)) {
		return FALSE;
	}
	
	//check if enough bytes are available
	//else adjust length to retreive
	if (UART_GetAvailableBytes(inst) < len) {
		len = UART_GetAvailableBytes(inst);
	}

	//retreive data from buffer
	for (uint16_t i = 0; i < len; i++) {
		data[i] = inst->rxCircBuf[(inst->rxCircTail+i)%UART_RXBUFFER_SIZE];
	}
	//handle wrap
	if ((inst->rxCircTail + len) >= UART_RXBUFFER_SIZE) {
		inst->rxCircWrap = FALSE;
	}
	//avance tail index
	inst->rxCircTail = (inst->rxCircTail + len) % UART_RXBUFFER_SIZE;
	
	//start reception
	startReceive(inst);
	return len;
}

static void startTransmit(UART_Instance* inst) {
	//check if UART ready to transmit
	HAL_UART_StateTypeDef state = HAL_UART_GetState(&(inst->uart));
	if ((state == HAL_UART_STATE_BUSY_TX) || (state == HAL_UART_STATE_BUSY_TX_RX)) {
		return;
	}
	//calculate count of byte to transmit
	inst->txCount = inst->txCircWrap == FALSE
			? inst->txCircHead - inst->txCircTail
			: UART_TXBUFFER_SIZE - inst->txCircTail;

	//transmit data if count > 0
	if (inst->txCount > 0) {
		HAL_UART_Transmit_DMA(&(inst->uart), inst->txCircBuf + inst->txCircTail, inst->txCount);
	}
}

static void startReceive(UART_Instance* inst) {
	//check if uart ready to receive
	HAL_UART_StateTypeDef state = HAL_UART_GetState(&(inst->uart));
	if ((state == HAL_UART_STATE_BUSY_RX) || (state == HAL_UART_STATE_BUSY_TX_RX)) {
		return;
	}
	//calculate maximal count of bytes to receive
	inst->rxCount = inst->rxCircWrap == FALSE
			? UART_RXBUFFER_SIZE - inst->rxCircHead
			: inst->rxCircTail - inst->rxCircHead;

	if (inst->rxCount > 0) {
		//start reception
		HAL_UART_Receive_DMA(&(inst->uart), inst->rxCircBuf + inst->rxCircHead, inst->rxCount);
		//clear IDLE interrupt
		__HAL_UART_CLEAR_IT(&(inst->uart),UART_CLEAR_IDLEF);
		//enable IDLE interrupt
		__HAL_UART_ENABLE_IT(&(inst->uart), UART_IT_IDLE);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart) {
	UART_Instance *inst = 0;
	//get instance from table
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

	//get count of received byte
	uint16_t cnt = inst->rxCount - __HAL_DMA_GET_COUNTER(&(inst->rxDma));

	//advance head index and wrap if necessary
	inst->rxCircHead += cnt;
	if (inst->rxCircHead >= UART_RXBUFFER_SIZE) {
		inst->rxCircHead = 0;
		inst->rxCircWrap = TRUE;
	}

	//start new reception
	startReceive(inst);
}

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *uart) {
	//call uart receive complete callback
	HAL_UART_RxCpltCallback(uart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart) {
	UART_Instance *inst = 0;
	//get instance from table
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

	//get count of transmitted byte
	uint16_t cnt = inst->txCount - __HAL_DMA_GET_COUNTER(&(inst->txDma));

	//advance tail index and wrap if necessary
	inst->txCircTail += cnt;
	if (inst->txCircTail >= UART_TXBUFFER_SIZE) {
		inst->txCircTail = 0;
		inst->txCircWrap = FALSE;
	}

	//start new transmission
	startTransmit(inst);
}

void USART1_IRQHandler() {
	//if instance for uart1 is configured
	if (instances[UART_1] != 0) {
		//check for IDLE interrupt
		if (__HAL_UART_GET_IT(&(instances[UART_1]->uart),UART_IT_IDLE) != FALSE) {
			//clear interrupt and abort reception
			__HAL_UART_CLEAR_IT(&(instances[UART_1]->uart),UART_CLEAR_IDLEF);
			HAL_UART_AbortReceive_IT(&(instances[UART_1]->uart));
		}
		//call HAL interrupt handler
		HAL_UART_IRQHandler(&(instances[UART_1]->uart));
	}
}

void USART2_IRQHandler() {
	//if instance for uart2 is configured
	if (instances[UART_2] != 0) {
		//check for IDLE interrupt
		if (__HAL_UART_GET_IT(&(instances[UART_2]->uart),UART_IT_IDLE) != FALSE) {
			//clear interrupt and abort reception
			__HAL_UART_CLEAR_IT(&(instances[UART_2]->uart),UART_CLEAR_IDLEF);
			HAL_UART_AbortReceive_IT(&(instances[UART_2]->uart));
		}
		//call HAL interrupt handler
		HAL_UART_IRQHandler(&(instances[UART_2]->uart));
	}
}

void USART4_5_IRQHandler() {
	//if instance for uart4 is configured
	if (instances[UART_4] != 0) {
		//check for IDLE interrupt
		if (__HAL_UART_GET_IT(&(instances[UART_4]->uart),UART_IT_IDLE) != FALSE) {
			//clear interrupt and abort reception
			__HAL_UART_CLEAR_IT(&(instances[UART_4]->uart),UART_CLEAR_IDLEF);
			HAL_UART_AbortReceive_IT(&(instances[UART_4]->uart));
		}
		//call HAL interrupt handler
		HAL_UART_IRQHandler(&(instances[UART_4]->uart));
	}
	//if instance for uart5 is configured
	if (instances[UART_5] != 0) {
		//check for IDLE interrupt
		if (__HAL_UART_GET_IT(&(instances[UART_5]->uart),UART_IT_IDLE) != FALSE) {
			//clear interrupt and abort reception
			__HAL_UART_CLEAR_IT(&(instances[UART_5]->uart),UART_CLEAR_IDLEF);
			HAL_UART_AbortReceive_IT(&(instances[UART_5]->uart));
		}
		//call HAL interrupt handler
		HAL_UART_IRQHandler(&(instances[UART_5]->uart));
	}
}
