/*
 * spi_driver.h
 *
 *  Created on: 29.05.2018
 *      Author: tobilinux
 */

#ifndef USER_SPI_SPI_DRIVER_H_
#define USER_SPI_SPI_DRIVER_H_

typedef uint8_t GPIO_PinType;
typedef uint32_t SPI_BaudType;

typedef enum {
	SPI_RET_OK = 0, SPI_RET_NOK = 1, SPI_RET_INVALID_PARAM = 2, SPI_RET_TIMEOUT = 3, SPI_RET_INVALID_BANK = 4
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
	SPI_TypeDef * SPI;
	SPI_BaudType BAUDRATE;
} SPI_Init_Struct;

SPI_RetType SPI_Init(SPI_Init_Struct * spi_init);
SPI_RetType SPI_SendData(SPI_Init_Struct * spi_init);
SPI_RetType SPI_ReadData(SPI_Init_Struct * spi_init);
SPI_RetType SPI_DeInit(SPI_Init_Struct * spi_init);

#endif /* USER_SPI_SPI_DRIVER_H_ */
