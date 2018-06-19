/*
 * spidriver.cpp
 *
 *  Created on: 29.05.2018
 *      Author: tobilinux
 */

#define LUT_SIZE 26

#include "spi_driver.h"

typedef struct {
	SPI_GPIO_Pair gp;
	uint32_t AF;
} spi_init_lut_tdef;

spi_init_lut_tdef spi_init_lut[LUT_SIZE] = { { { GPIO_PIN_4, GPIOA },
		GPIO_AF0_SPI1 }, { { GPIO_PIN_5, GPIOA }, GPIO_AF0_SPI1 }, { {
		GPIO_PIN_6, GPIOA }, GPIO_AF0_SPI1 }, { { GPIO_PIN_7, GPIOA },
		GPIO_AF0_SPI1 }, { { GPIO_PIN_11, GPIOA }, GPIO_AF0_SPI1 }, { {
		GPIO_PIN_12, GPIOA }, GPIO_AF0_SPI1 }, { { GPIO_PIN_15, GPIOA },
		GPIO_AF0_SPI1 }, { { GPIO_PIN_9, GPIOB }, GPIO_AF5_SPI2 }, { {
		GPIO_PIN_10, GPIOB }, GPIO_AF5_SPI2 }, { { GPIO_PIN_3, GPIOB },
		GPIO_AF0_SPI1 }, { { GPIO_PIN_4, GPIOB }, GPIO_AF0_SPI1 }, { {
		GPIO_PIN_5, GPIOB }, GPIO_AF0_SPI1 }, { { GPIO_PIN_12, GPIOB },
		GPIO_AF0_SPI2 }, { { GPIO_PIN_13, GPIOB }, GPIO_AF0_SPI2 }, { {
		GPIO_PIN_14, GPIOB }, GPIO_AF0_SPI2 }, { { GPIO_PIN_15, GPIOB },
		GPIO_AF0_SPI2 }, { { GPIO_PIN_2, GPIOC }, GPIO_AF2_SPI2 }, { {
		GPIO_PIN_3, GPIOC }, GPIO_AF2_SPI2 }, { { GPIO_PIN_0, GPIOD },
		GPIO_AF1_SPI2 }, { { GPIO_PIN_1, GPIOD }, GPIO_AF1_SPI2 }, { {
		GPIO_PIN_3, GPIOD }, GPIO_AF2_SPI2 }, { { GPIO_PIN_4, GPIOD },
		GPIO_AF1_SPI2 }, { { GPIO_PIN_12, GPIOE }, GPIO_AF2_SPI1 }, { {
		GPIO_PIN_13, GPIOE }, GPIO_AF2_SPI1 }, { { GPIO_PIN_14, GPIOE },
		GPIO_AF2_SPI1 }, { { GPIO_PIN_15, GPIOE }, GPIO_AF2_SPI1 } };

static void SPI_AF_INIT(const SPI_GPIO_Pair gp) {
	if (gp.bank == GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	} else if (gp.bank == GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
	} else if (gp.bank == GPIOC) {
		__HAL_RCC_GPIOC_CLK_ENABLE();
	} else if (gp.bank == GPIOD) {
		__HAL_RCC_GPIOD_CLK_ENABLE();
	} else if (gp.bank == GPIOE) {
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}

	GPIO_InitTypeDef spi_init_def;
	spi_init_def.Pin = gp.pin;
	spi_init_def.Mode = GPIO_MODE_AF_PP;
	spi_init_def.Pull = GPIO_PULLDOWN;
	spi_init_def.Speed = GPIO_SPEED_FREQ_HIGH;

	for (int i = 0; i < LUT_SIZE; i++) {
		if (gp.pin == spi_init_lut[i].gp.pin
				&& gp.bank == spi_init_lut[i].gp.bank) {
			spi_init_def.Alternate = spi_init_lut[i].AF;
			break;
		}
	}

	HAL_GPIO_Init(gp.bank, &spi_init_def);

}
static void SPI_Init_CS(const SPI_GPIO_Pair gp) {
	GPIO_InitTypeDef spi_init_def;
	spi_init_def.Mode = GPIO_MODE_OUTPUT_PP;
	spi_init_def.Pin = gp.pin;
	spi_init_def.Pull = GPIO_PULLDOWN;
	spi_init_def.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(gp.bank, &spi_init_def);
}

void SPI_CS_Enable(const SPI_GPIO_Pair gp) {
	HAL_GPIO_WritePin(gp.bank, gp.pin, GPIO_PIN_SET);
}
void SPI_CS_Disable(const SPI_GPIO_Pair gp) {
	HAL_GPIO_WritePin(gp.bank, gp.pin, GPIO_PIN_RESET);
}

SPI_RetType SPI_Init(SPI_Init_Struct * spi_init) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	//SPI_AF_INIT(spi_init->CS);
	SPI_AF_INIT(spi_init->MOSI);
	SPI_AF_INIT(spi_init->MISO);
	SPI_AF_INIT(spi_init->SCLK);
	SPI_Init_CS(spi_init->CS);
	SPI_CS_Enable(spi_init->CS);

	if (HAL_SPI_Init(spi_init->SPI) != HAL_OK) {
		return SPI_RET_FAILED_INIT;
	}
	HAL_SPI_MspInit(spi_init->SPI);
	return SPI_RET_OK;
}
SPI_RetType SPI_SendData(SPI_Init_Struct * spi_init, void * tx_buffer,
		uint8_t timeout) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	if (HAL_SPI_Transmit(spi_init->SPI, (uint8_t*) tx_buffer, sizeof(tx_buffer),
			timeout) != HAL_OK) {
		return SPI_RET_OP_FAILED;
	}

	return SPI_RET_OK;
}
SPI_RetType SPI_ReadData(SPI_Init_Struct * spi_init, void * rx_buffer,
		uint8_t timeout) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	if (HAL_SPI_Receive(spi_init->SPI, (uint8_t*) rx_buffer, sizeof(rx_buffer),
			timeout) != HAL_OK) {
		return SPI_RET_OP_FAILED;
	}

	return SPI_RET_OK;
}
SPI_RetType SPI_DeInit(SPI_Init_Struct * spi_init) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}
	SPI_CS_Disable(spi_init->CS);
	HAL_GPIO_DeInit(spi_init->CS.bank, spi_init->CS.pin);
	HAL_GPIO_DeInit(spi_init->SCLK.bank, spi_init->SCLK.pin);
	HAL_GPIO_DeInit(spi_init->MOSI.bank, spi_init->MOSI.pin);
	HAL_GPIO_DeInit(spi_init->MISO.bank, spi_init->MISO.pin);

	if (HAL_SPI_DeInit(spi_init->SPI) != HAL_OK) {
		return SPI_RET_FAILED_INIT;
	}
	HAL_SPI_MspDeInit(spi_init->SPI);

	return SPI_RET_OK;
}

