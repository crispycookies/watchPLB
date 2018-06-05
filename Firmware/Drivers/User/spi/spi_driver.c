/*
 * spidriver.cpp
 *
 *  Created on: 29.05.2018
 *      Author: tobilinux
 */

#include "spi_driver.h"

static SPI_RetType SPI_GPIO_Init(GPIO_PinType MOSI, GPIO_PinType MISO, GPIO_PinType SCLK, GPIO_PinType CS, GPIO_TypeDef * BANK){
	GPIO_InitTypeDef spi_init_def;
	if(BANK == GPIOA){
		spi_init_def.Alternate = GPIO_AF0_SPI1;
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
	else if(BANK == GPIOB){
		spi_init_def.Alternate = GPIO_AF0_SPI1;
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	else if(BANK==GPIOC){
		return SPI_RET_INVALID_BANK;
	}
	else if(BANK==GPIOD){
		spi_init_def.Alternate = GPIO_AF1_SPI2|GPIO_AF2_SPI2;
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
	else if(BANK == GPIOE){
		spi_init_def.Alternate = GPIO_AF2_SPI1;
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}
	else{
		return SPI_RET_INVALID_BANK;
	}
	spi_init_def.Mode = GPIO_MODE_AF_PP;
	spi_init_def.Pull = GPIO_PULLDOWN;
	spi_init_def.Speed = GPIO_SPEED_FREQ_HIGH;
	spi_init_def.Pin = (MOSI|MISO|SCLK|CS);

	// Irrelevant which Define to use - i Chose SPI1
	// Works with all SPIs, if they are hooked on AF0


	HAL_GPIO_Init(BANK, &spi_init_def);
	return SPI_RET_OK;
}

SPI_RetType SPI_Init(SPI_Init_Struct * spi_init){
	if(spi_init == 0){
		return SPI_RET_INVALID_PARAM;
	}

	//SPI_RetType rt_gpio_init = SPI_GPIO_Init(spi_init->MOSI, spi_init->MISO, spi_init->SCLK, spi_init->CS, spi_init->BANK);
	/*if(rt_gpio_init!=SPI_RET_OK){
		return rt_gpio_init;
	}*/

	return SPI_RET_OK;
}
SPI_RetType SPI_SendData(SPI_Init_Struct * spi_init){
	if(spi_init == 0){
		return SPI_RET_INVALID_PARAM;
	}

	return SPI_RET_OK;
}
SPI_RetType SPI_ReadData(SPI_Init_Struct * spi_init){
	if(spi_init == 0){
		return SPI_RET_INVALID_PARAM;
	}
	return SPI_RET_OK;
}
SPI_RetType SPI_DeInit(SPI_Init_Struct * spi_init){
	if(spi_init == 0){
		return SPI_RET_INVALID_PARAM;
	}
	return SPI_RET_OK;
}

