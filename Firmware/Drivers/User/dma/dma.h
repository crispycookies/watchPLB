/**
 * @file dma.h
 * @author Paul Götzinger
 * @brief DMA interrupt module
 * @version 0.1
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef DMA_H
#define DMA_H

/**
 * @brief Register dma handler for interrupt
 * 
 * @param dma dma handle
 */
void DMA_RegisterInterrupt(DMA_HandleTypeDef *dma);

#endif /* DMA_H */
