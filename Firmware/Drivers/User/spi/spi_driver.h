/**
 ******************************************************************************
 * @file    spi_driver.h
 * @author  Tobias Egger
 * @version V1.3
 * @date    02-February-2019
 * @brief   WatchPLB SPI- Driver Interface File
 ******************************************************************************
 */

#ifndef USER_SPI_SPI_DRIVER_H_
#define USER_SPI_SPI_DRIVER_H_

#include "stm32l0xx_hal_spi.h"

/*Global Typedefs*/
typedef uint16_t GPIO_PinType;

/*Global Enums*/

/*
 * */

/**
 * @brief Public Enum for Return Values
 */
typedef enum {
	SPI_RET_NOK = 0,
	SPI_RET_OK = 1,
	SPI_RET_INVALID_PARAM = 2,
	SPI_RET_TIMEOUT = 3,
	SPI_RET_INVALID_BANK = 4,
	SPI_RET_FAILED_INIT = 5,
	SPI_RET_OP_FAILED = 6
} SPI_RetType;

/*Global Structs*/
/**
 * @brief Public Struct for GPIO PIN/BANK Pair
 */
typedef struct {
	GPIO_PinType pin;
	GPIO_TypeDef * bank;
} SPI_GPIO_Pair;

/*
 * @brief Public Struct for Pins to Use and SPI to Initialize
 * */
typedef struct {
	SPI_GPIO_Pair MOSI;
	SPI_GPIO_Pair MISO;
	SPI_GPIO_Pair CS;
	SPI_GPIO_Pair SCLK;
	SPI_HandleTypeDef SPI;
} SPI_Init_Struct;

/*Basic LED- Driver Block*/
/**
 * @brief Initialize SPI and GPIOs; Enables CS
 * @param  spi_init: The Pins and SPI to initialize
 * @retval Result of Operation
 */
SPI_RetType SPI_Init(SPI_Init_Struct * spi_init);

/**
 * @brief Send Data via given SPI
 * @param spi_init: The Pins and SPI to use
 * @param tx_buffer: The Data to send
 * @param tx_buffer_size: The Length of the Buffer to be used
 * @param timeout: Timeout
 * @retval Result of Operation
 */
SPI_RetType SPI_SendData(SPI_Init_Struct * spi_init, uint8_t * tx_buffer,
		uint8_t tx_buffer_size, uint8_t timeout);

/**
 * @brief Receive Data via given SPI
 * @param spi_init: The Pins and SPI to use
 * @param rx_buffer: The Data to Receive
 * @param timeout: Timeout
 * @param rx_buffer_size: The Length of the Buffer to be used
 * @retval Result of Operation
 */
SPI_RetType SPI_ReadData(SPI_Init_Struct * spi_init, uint8_t * rx_buffer,
		uint8_t rx_buffer_size, uint8_t timeout);

/**
 * @brief Send and receive one byte via SPI
 * 
 * @param spi_init The Pins and SPI to use
 * @param tx_byte byte to send
 * @param rx_byte byte to receive
 * @param timeout timeout
 * @return Result of operation
 */
SPI_RetType SPI_WriteRead(SPI_Init_Struct * spi_init, uint8_t tx_byte, 
		uint8_t * rx_byte, uint8_t timeout);

/**
 * @brief Send and Receive Data via given SPI
 * @param spi_init: The Pins and SPI to use
 * @param tx_byte: the byte to send
 * @param rx_byte: the byte to receive
 * @param timout: the timeout
 * @retval Result of Operation
 */
SPI_RetType SPI_WriteRead(SPI_Init_Struct * spi_init, uint8_t tx_byte, 
		uint8_t * rx_byte, uint8_t timeout);

/**
 * @brief Deinitialize SPI and GPIOs; Disables CS
 * @param spi_init: The Pins and SPI to deinitialize
 * @retval Result of Operation
 */
SPI_RetType SPI_DeInit(SPI_Init_Struct * spi_init);

/**
 * @brief Enable CS
 * @param  gp: The Pin and the Location of the Pin to use
 * @retval none
 */
void SPI_CS_Enable(SPI_Init_Struct * spi_init);

/**
 * @brief Disable CS
 * @param  gp: The Pin and the Location of the Pin to use
 * @retval none
 */
void SPI_CS_Disable(SPI_Init_Struct * spi_init);

#endif /* USER_SPI_SPI_DRIVER_H_ */
