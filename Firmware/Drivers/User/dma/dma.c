/*
 * dma.c
 *
 *  Created on: 27.05.2018
 *      Author: Paul
 */


#include "dma.h"

#define TRUE  1
#define FALSE 0

#define DMA_CHANNELS 7

static uint8_t init = FALSE;
static DMA_HandleTypeDef* dma_handles[DMA_CHANNELS];

void DMA_RegisterInterrupt(DMA_HandleTypeDef *dma) {
	if (init == FALSE) {
		for (uint8_t i = 0; i < DMA_CHANNELS; i++)
			dma_handles[i] = 0;
		init = TRUE;
	}

	if (dma->Instance == DMA1_Channel1) {
		dma_handles[0] = dma;
	    NVIC_SetPriority(DMA1_Channel1_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	} else if (dma->Instance == DMA1_Channel2) {
		dma_handles[1] = dma;
	    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	} else if (dma->Instance == DMA1_Channel3) {
		dma_handles[2] = dma;
	    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	} else if (dma->Instance == DMA1_Channel4) {
		dma_handles[3] = dma;
	    NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
	} else if (dma->Instance == DMA1_Channel5) {
		dma_handles[4] = dma;
	    NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
	} else if (dma->Instance == DMA1_Channel6) {
		dma_handles[5] = dma;
	    NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
	} else if (dma->Instance == DMA1_Channel7) {
		dma_handles[6] = dma;
	    NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0x01);
	    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
	}
}

/**
* @brief This function handles DMA1 channel 1 interrupt.
*/
void DMA1_Channel1_IRQHandler(void) {
	if (dma_handles[0] != 0)
		HAL_DMA_IRQHandler(dma_handles[0]);
}

/**
* @brief This function handles DMA1 channel 2 and channel 3 interrupts.
*/
void DMA1_Channel2_3_IRQHandler(void) {
	if (dma_handles[1] != 0)
		HAL_DMA_IRQHandler(dma_handles[1]);
	if (dma_handles[2] != 0)
		HAL_DMA_IRQHandler(dma_handles[2]);
}

/**
* @brief This function handles DMA1 channel 4-7 interrupts.
*/
void DMA1_Channel4_5_6_7_IRQHandler(void) {
	if (dma_handles[3] != 0)
		HAL_DMA_IRQHandler(dma_handles[3]);
	if (dma_handles[4] != 0)
		HAL_DMA_IRQHandler(dma_handles[4]);
	if (dma_handles[5] != 0)
		HAL_DMA_IRQHandler(dma_handles[5]);
	if (dma_handles[6] != 0)
		HAL_DMA_IRQHandler(dma_handles[6]);
}
