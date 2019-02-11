/**
 ******************************************************************************
 * @file    spi_driver.c
 * @author  Tobias Egger
 * @version V1.3
 * @date    02-February-2019
 * @brief   WatchPLB SPI- Driver Implementation File
 ******************************************************************************
 */
#define LUT_SIZE 26

#include "spi_driver.h"

/*Private Structs*/
/**
 * @brief Pin and its AF for use in the LUT
 */
typedef struct {
	SPI_GPIO_Pair gp;
	uint32_t AF;
} spi_init_lut_tdef;

/*Private Global Variables*/
/**
 * @brief A Look- Up- Table(LUT) to find out which PIN Corresponds to what AF.
 */
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

/**
 * @brief Initializes GPIO- Clock
 * @param  bank: The Bank to Initialize
 * @retval none
 */
static void SPI_GPIO_CLK_Enable(const GPIO_TypeDef * bank) {
	if (bank == GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	} else if (bank == GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
	} else if (bank == GPIOC) {
		__HAL_RCC_GPIOC_CLK_ENABLE();
	} else if (bank == GPIOD) {
		__HAL_RCC_GPIOD_CLK_ENABLE();
	} else if (bank == GPIOE) {
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}
}

/**
 * @brief Initializes PIN for AF
 * @param  gp: The Pins and Location to Initialize
 * @retval none
 */
static void SPI_AF_INIT(const SPI_GPIO_Pair gp) {

	SPI_GPIO_CLK_Enable(gp.bank);

	GPIO_InitTypeDef spi_init_def;
	spi_init_def.Pin = gp.pin;
	spi_init_def.Mode = GPIO_MODE_AF_PP;
	spi_init_def.Pull = GPIO_NOPULL;
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
/**
 * @brief Initializes PIN for CS
 * @param  gp: The Pin and Location to Initialize
 * @retval none
 */
static void SPI_Init_CS(const SPI_GPIO_Pair gp) {

	SPI_GPIO_CLK_Enable(gp.bank);



	GPIO_InitTypeDef spi_init_def;
	spi_init_def.Mode = GPIO_MODE_OUTPUT_PP;
	spi_init_def.Pin = gp.pin;
	spi_init_def.Pull = GPIO_NOPULL;
	spi_init_def.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(gp.bank, &spi_init_def);
}
/**
 * @brief Enable CS
 * @param  gp: The Pin and the Location of the Pin to use
 * @retval none
 */
void SPI_CS_Enable(SPI_Init_Struct * spi_init) {
	HAL_GPIO_WritePin(spi_init->CS.bank, spi_init->CS.pin, GPIO_PIN_RESET);
}
/**
 * @brief Disable CS
 * @param  gp: The Pin and the Location of the Pin to use
 * @retval none
 */
void SPI_CS_Disable(SPI_Init_Struct * spi_init) {
	HAL_GPIO_WritePin(spi_init->CS.bank, spi_init->CS.pin, GPIO_PIN_SET);
}
/**
 * @brief Initialize SPI and GPIOs; Enables CS
 * @param  spi_init: The Pins and SPI to initialize
 * @retval Result of Operation
 */
SPI_RetType SPI_Init(SPI_Init_Struct * spi_init) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	if (spi_init->SPI.Instance == SPI1) {
		__HAL_RCC_SPI1_CLK_ENABLE();
	} else if (spi_init->SPI.Instance == SPI2) {
		__HAL_RCC_SPI2_CLK_ENABLE();
	} else {
		return SPI_RET_INVALID_PARAM;
	}

	//SPI_AF_INIT(spi_init->CS);
	SPI_AF_INIT(spi_init->MOSI);
	SPI_AF_INIT(spi_init->MISO);
	SPI_AF_INIT(spi_init->SCLK);
	SPI_Init_CS(spi_init->CS);
	SPI_CS_Disable(spi_init);

	__HAL_SPI_DISABLE(&spi_init->SPI);

	if (HAL_SPI_Init(&spi_init->SPI) != HAL_OK) {
		return SPI_RET_FAILED_INIT;
	}

	__HAL_SPI_ENABLE(&spi_init->SPI);

	return SPI_RET_OK;
}
/**
 * @brief Send Data via given SPI
 * @param spi_init: The Pins and SPI to use
 * @param tx_buffer: The Data to send
 * @param tx_buffer_size: The Length of the Buffer to be used
 * @param timeout: Timeout
 * @retval Result of Operation
 */
SPI_RetType SPI_SendData(SPI_Init_Struct * spi_init, uint8_t * tx_buffer,
		uint8_t tx_buffer_size, uint8_t timeout) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	if (HAL_SPI_Transmit(&spi_init->SPI,  tx_buffer, tx_buffer_size,
			timeout) != HAL_OK) {
		return SPI_RET_OP_FAILED;
	}


	return SPI_RET_OK;
}

/**
 * @brief Send and Receive Data via given SPI
 * @param spi_init: The Pins and SPI to use
 * @param tx_byte: the byte to send
 * @param rx_byte: the byte to receive
 * @param timout: the timeout
 * @retval Result of Operation
 */
SPI_RetType SPI_WriteRead(SPI_Init_Struct * spi_init, uint8_t tx_byte, 
		uint8_t * rx_byte, uint8_t timeout) {
	
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	if (HAL_SPI_TransmitReceive(&spi_init->SPI, &tx_byte, rx_byte, 1, timeout) != HAL_OK) {
		return SPI_RET_OP_FAILED;
	}

	return SPI_RET_OK;
}

/**
 * @brief Receive Data via given SPI
 * @param spi_init: The Pins and SPI to use
 * @param rx_buffer: The Data to Receive
 * @param rx_buffer_size: The Length of the Buffer to be used
 * @param timeout: Timeout
 * @retval Result of Operation
 */
SPI_RetType SPI_ReadData(SPI_Init_Struct * spi_init, uint8_t * rx_buffer,
		uint8_t rx_buffer_size, uint8_t timeout) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}

	if (HAL_SPI_Receive(&spi_init->SPI, (uint8_t*) rx_buffer, rx_buffer_size,
			timeout) != HAL_OK) {
		return SPI_RET_OP_FAILED;
	}

	return SPI_RET_OK;
}

/**
 * @brief Deinitialize SPI and GPIOs; Disables CS
 * @param spi_init: The Pins and SPI to deinitialize
 * @retval Result of Operation
 */
SPI_RetType SPI_DeInit(SPI_Init_Struct * spi_init) {
	if (spi_init == 0) {
		return SPI_RET_INVALID_PARAM;
	}
	SPI_CS_Disable(spi_init);
	HAL_GPIO_DeInit(spi_init->CS.bank, spi_init->CS.pin);
	HAL_GPIO_DeInit(spi_init->SCLK.bank, spi_init->SCLK.pin);
	HAL_GPIO_DeInit(spi_init->MOSI.bank, spi_init->MOSI.pin);
	HAL_GPIO_DeInit(spi_init->MISO.bank, spi_init->MISO.pin);

	if (HAL_SPI_DeInit(&spi_init->SPI) != HAL_OK) {
		return SPI_RET_FAILED_INIT;
	}

	return SPI_RET_OK;
}

