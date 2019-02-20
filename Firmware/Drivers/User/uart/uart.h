/**
 * @file uart.h
 * @author Paul Götzinger
 * @brief 
 * @version 1.0
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef UART_H
#define UART_H

#define UART_TXBUFFER_SIZE 256
#define UART_RXBUFFER_SIZE 256

/**
 * @brief Baud rate definitons
 * 
 */
typedef enum {
	UART_BaudRate_9600 = 9600,
	UART_BaudRate_19200 = 19200,
	UART_BaudRate_38400 = 38400,
	UART_BaudRate_57600 = 57600,
	UART_BaudRate_115200 = 115200
} UART_BaudRate;

/**
 * @brief UART Instance structure
 * 
 */
typedef struct {
	UART_HandleTypeDef uart;	//HAL driver UART handle

	DMA_HandleTypeDef rxDma;	//HAL driver DMA handle for receiving
	uint8_t  rxCircBuf[UART_RXBUFFER_SIZE];	//receive buffer
	uint16_t rxCircHead;	//receive buffer head
	uint16_t rxCircTail;	//receive buffer tail
	uint8_t  rxCircWrap;	//Flag if Head got wrapped around
	uint16_t rxCount;		//count of bytes to receive

	DMA_HandleTypeDef txDma;	//HAL driver DMA handle for transmitting
	uint8_t  txCircBuf[UART_TXBUFFER_SIZE];	//transmit buffer
	uint16_t txCircHead;	//transmit buffer head
	uint16_t txCircTail;	//transmit buffer tail
	uint8_t  txCircWrap;	//Flag if Head got wrapped around
	uint16_t txCount;		//count of bytes to transmit
} UART_Instance;

/**
 * @brief UART Configuration structure
 * 
 */
typedef struct {
	USART_TypeDef*			uart;		//USART module to configure
	DMA_Channel_TypeDef*	txDmaChannel;	//DMA channel used for transmitting
	DMA_Channel_TypeDef*	rxDmaChannel;	//DMA channel used for receiving
	GPIO_TypeDef* 			txBoard;	//GPIO board for transmition
	GPIO_TypeDef* 			rxBoard;	//GPIO board for reception
	uint16_t      			txPin;		//GPIO pin for transmition
	uint16_t      			rxPin;		//GPIO pin for reception
	uint32_t				txAF;		//Alternate function for transmition 
	uint32_t				rxAF;		//Alternate function for reception
	UART_BaudRate 			baud;		//baud rate
} UART_Config;

/**
 * @brief UART initialisation
 * 
 * @param inst empty UART instance
 * @param conf Configration
 */
void UART_Init(UART_Instance* inst, UART_Config* conf);

/**
 * @brief Send single byte
 * 
 * @param inst UART instance
 * @param byte byte to send
 * @return uint8_t 1 on success, 0 on failure
 */
uint8_t UART_SendByte(UART_Instance* inst, uint8_t byte);

/**
 * @brief Send array of bytes
 * 
 * @param inst UART instance
 * @param len length of data (count of bytes)
 * @param data data to send
 * @return uint8_t 1 on success, 0 on failure
 */
uint8_t UART_SendData(UART_Instance* inst, uint16_t len, uint8_t *data);

/**
 * @brief Send 0-terminated string
 * 
 * @param inst UART instance
 * @param byte string (must be 0-terminated)
 * @return uint8_t 1 on success, 0 on failure
 */
uint8_t UART_SendString(UART_Instance* inst, uint8_t *byte);

/**
 * @brief Get number of received bytes in buffer
 * 
 * @param inst UART instance
 * @return uint16_t number of bytes available
 */
uint16_t UART_GetAvailableBytes(UART_Instance* inst);

/**
 * @brief Retreive 1 byte from receive buffer
 * 
 * @param inst UART instance
 * @return uint8_t retreived byte
 */
uint8_t UART_GetByte(UART_Instance* inst);

/**
 * @brief Retreive data from receive buffer
 * 
 * @param inst UART instance
 * @param len maximal length of data to retreive
 * @param data array to store data
 * @return uint16_t count of retreived bytes
 */
uint16_t UART_GetData(UART_Instance* inst, uint16_t len, uint8_t *data);


#endif /* UART_H */
