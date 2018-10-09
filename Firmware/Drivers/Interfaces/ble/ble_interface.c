/*
 * ble_interface.c
 *
 *  Created on: 02.10.2018
 *      Author: tobi
 */

#include "ble_interface.h"

#include "uart.h"

#define UART_START_SEQ 0xAA

static uint8_t ble_crc_hack(uint8_t * data, uint8_t data_len) {
	return 0;
}

static void ble_mask(uint8_t command, uint8_t * data, uint8_t data_length,
		uint8_t * result, uint8_t result_length) {
	if (result_length != data_length + 4) {
		return;
	}
	result[0] = UART_START_SEQ;
	result[1] = data_length;
	result[2] = command;
	int i = 0;
	for (; i < data_length; i++) {
		result[i + 3] = data[i];
	}

	//TODO CRC for CS
	result[i] = ble_crc_hack(result, data_length+3);
}

void ble_interface_init() {

	//READY

	UART_Config conf;
	UART_Instance inst;

	conf.baud = BAUD_115200;
	conf.uart = USART1;
	conf.rxBoard = GPIOA;
	conf.rxPin = GPIO_PIN_9;
	conf.rxAF = GPIO_AF4_USART1;
	conf.txBoard = GPIOA;
	conf.txPin = GPIO_PIN_10;
	conf.txAF = GPIO_AF4_USART1;

	UART_Init(&inst, &conf);
}
void ble_interface_send(uint8_t * tx_buffer, uint8_t tx_buffer_length) {

}
uint16_t ble_interface_get_buffer_length() {
	return 0;
}
uint16_t ble_interface_get_buffer(uint8_t *rx_buffer, uint8_t rx_buffer_length) {
	return 0;
}
void ble_interface_deinit() {

}

void ble_interface_sleep() {

}
void ble_interface_awake() {

}

void ble_interface_connect(const ble_interface_dev_id * dev_id) {

}
void ble_interface_disconnect(const ble_interface_dev_id * dev_id) {

}

void ble_interface_allow_connection() {

}
bool ble_interface_is_connection_availiable() {
	return false;
}
ble_interface_dev_id * ble_interface_get_availiable_conections() {

}
