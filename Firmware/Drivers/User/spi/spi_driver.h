/*
 * spi_driver.h
 *
 *  Created on: 29.05.2018
 *      Author: tobilinux
 */

#ifndef USER_SPI_SPI_DRIVER_H_
#define USER_SPI_SPI_DRIVER_H_

typedef uint8_t GPIO_PinType;

typedef enum {
	SPI_RET_NOK = 0,
	SPI_RET_OK = 1,
	SPI_RET_INVALID_PARAM = 2,
	SPI_RET_TIMEOUT = 3,
	SPI_RET_INVALID_BANK = 4,
	SPI_RET_FAILED_INIT = 5,
	SPI_RET_OP_FAILED = 6
} SPI_RetType;

typedef struct{
	GPIO_PinType pin;
	GPIO_TypeDef * bank;
} SPI_GPIO_Pair;

typedef struct {
	SPI_GPIO_Pair MOSI;
	SPI_GPIO_Pair MISO;
	SPI_GPIO_Pair CS;
	SPI_GPIO_Pair SCLK;
	SPI_HandleTypeDef * SPI;
} SPI_Init_Struct;

SPI_RetType SPI_Init(SPI_Init_Struct * spi_init);
SPI_RetType SPI_SendData(SPI_Init_Struct * spi_init, void * tx_buffer,uint8_t timeout);
SPI_RetType SPI_ReadData(SPI_Init_Struct * spi_init, void * rx_buffer,uint8_t timeout);
SPI_RetType SPI_DeInit(SPI_Init_Struct * spi_init);

void SPI_CS_Enable(const SPI_GPIO_Pair gp);
void SPI_CS_Disable(const SPI_GPIO_Pair gp);

#endif /* USER_SPI_SPI_DRIVER_H_ */
