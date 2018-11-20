/**
  ******************************************************************************
  * @file    ble_interface.c
  * @author  Tobias Egger
  * @version V0.9
  * @date    20-November-2018
  * @brief   WatchPLB BLE- Driver Interface File
  ******************************************************************************
  */
 
 /*Include Block*/
#include <stdlib.h>

#include "ble_interface.h"
#include "crc8.h"
#include "uart.h"

/*@brief Define Block*/
#define UART_START_SEQ 0xAA
/*@brief BLEDK3 CMDs*/
/*@brief COMMON 1*/
#define RL_INFO 0x01
#define RESET 0x02
#define R_BLEDK3 0x03
#define R_ADC_Value 0x04
#define INTO_SHTDWN_MODE 0x05
#define DEBUG_CMD 0x06
#define W_DEV_NAME 0x08
#define R_DEV_NAME 0x07
#define ERASE_ALL_PAIRED_INFO 0x09
#define R_PAIRING_MODE_STNG 0x0A
#define W_PAIRING_MODE_STNG 0x0B
#define R_ALL_PAIRED_INFO 0x0C
#define DELETE_PAIRED_DEV 0x0D
#define DIO_CTRL 0x0E
#define PWM_CTRL 0x0F

/*@brief GAP*/
#define R_RSSI_VALUE 0x10
#define W_ADV_DATA 0x11
#define W_SCN_RES_DATA 0x12
#define SET_ADVERTISING_PARAMETER 0x13
#define SET_SCAN_PARAMETER 0x15
#define SET_SCAN_ENABLE 0x16
#define LE_CREATE_CONN 0x17
#define LE_CREATE_CONN_CANCEL 0x18
#define CONN_PARAM_UPDATE_REQ 0x19
#define DC 0x1B
#define SET_ADV_ENABLE 0x1C
#define R_REMOTE_DEV_NAME 0x1F

/*@brief GATT_Client*/
#define DISCO_ALL_PRIM_SERVICES 0x30
#define DISCO_SPECIFIC_PRIM_SERVICE_CHARACTERISTICS 0x31
#define R_CHARACTERISTIC_VALUE 0x32
#define R_USING_CHARACTERISTIC_UID 0x33
#define W_CHARACTERISTIC_VALUE 0x34
#define ENABLE_TRANSPARENT 0x35

/*@brief GATT_Server*/
#define SEND_CHARACTERISTIC_VALUE 0x38
#define UPDATE_CHARACTERISTIC_VALUE 0x39
#define R_LOCAL_CHARACTERISTICS_VALUE0x3A
#define R_LOCAL_ALL_PRIM_SERVICES 0x3B
#define R_LOCAL_SPECIFIC_PRIMARY_SERVICE 0x3C
#define SEND_WRITE_RESPONSE 0x3D

/*@brief GATT_Transparent*/
#define SEND_TRANSPARENT_DATA 0x3F

/*@brief Pairing*/
#define PASSKYE_ENTRY_RES 0x40
#define USER_CNFRM_RES 0x41
#define PAIRING_REQUEST 0x42

/*@brief COMMON 2*/
#define LEAVE_CFG_MODE 0x52

/*@brief BLEDK3 ERROR*/
#define SUCCEED 0x00
#define FAIL_LWR 0x01
#define FAIL_UPR 0xFF

/*@brief ENABLE DISABLE*/
#define DEF_ENABLE_TRANSPARENT_PARAM 0x01
#define DEF_DISABLE_TRANSPARENT_PARAM 0x00

/*@brief Global Variables*/
static UART_Config conf;
static UART_Instance inst;

#define maxbuffer 255
static uint8_t * send_buffer = 0;
static uint8_t * rec_buffer = 0;
static uint8_t connhdl = 0x01;

/**
  * @brief Calculate CRC
  * @param data: The Data to use
  * @param data_len: Length of the Data to use
  * @retval Checksum
*/
static uint8_t ble_crc_hack(uint8_t * data, uint8_t data_length) {
	return __crc8(data, data_length);
}

/**
  * @brief Mask the Data for Sending
  * @param command: The Command to be sent
  * @param data: The Data to be sent
  * @param data_length: Length of the Data to use
  * @retval None
*/
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

	result[i] = ble_crc_hack(result, data_length + 3);
}

/**
  * @brief Zeros the Buffer
  * @param None
  * @retval None
*/
static void zero_arr(){
	for(int i = 0; i < maxbuffer; i++){
		send_buffer[i] = 0;
	}
}

/**
  * @brief Sends the Data
  * @param command: The Command to be sent
  * @param data: The Data to use
  * @param data_length: Length of the Data to use
  * @retval Result of Operation
*/
static bool ble_write(uint8_t command, uint8_t * data, uint8_t data_length){
	zero_arr();
	ble_mask(command, data, data_length, send_buffer, maxbuffer);
	UART_SendData(&inst,data_length+4,send_buffer);
	//TODO Check if it Returns only Byte
	UART_GetByte(&inst);
	if(UART_GetByte(&inst)!=SUCCEED) return false;
	return true;;
}

/**
  * @brief Initialize the BLE- Module
  * @param  None
  * @retval None
*/
void ble_interface_init() {

	//READY
	send_buffer= malloc(maxbuffer);
	rec_buffer = malloc(maxbuffer);

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

/**
  * @brief Send Data in Transparent Mode
  * @param tx_buffer: pointer to tx buffer
  * @param tx_buffer_length: length of tx buffer
  * @retval None
*/
void ble_interface_send(uint8_t * tx_buffer, uint8_t tx_buffer_length) {
	//TODO Get To Transparent Mode
	uint8_t cmd_assembled[3] = {connhdl, DEF_ENABLE_TRANSPARENT_PARAM, 0};
	ble_write(ENABLE_TRANSPARENT, cmd_assembled, 3);
	//TODO Send Data
	ble_write(SEND_TRANSPARENT_DATA, tx_buffer, tx_buffer_length);
	//TODO Get To Transparent Mode
	cmd_assembled[1] = DEF_DISABLE_TRANSPARENT_PARAM;
	ble_write(ENABLE_TRANSPARENT, cmd_assembled, 3 );
}

/**
  * @brief Get Length of Received Data
  * @param None
  * @retval Length of Data
*/
uint16_t ble_interface_get_buffer_length(){
	uint16_t bytes = 0;
	if((bytes=UART_GetAvailableBytes(&inst))==0 || bytes <= 3) return 0;
	return bytes - 3;
}

/**
  * @brief Get Received Data
  * @param rx_buffer: Pointer to where the Data shall be stored  
  * @param rx_buffer_length: Length of the Buffer
  * @retval Remaining Bytes
*/
uint16_t ble_interface_get_buffer(uint8_t *rx_buffer, uint8_t rx_buffer_length){
	if(rx_buffer_length +3 >= maxbuffer) return 0;
	uint16_t len = UART_GetData(&inst, rx_buffer_length+3, rec_buffer)-3;
	for(int i = 2; i < rx_buffer_length; i++){
		rx_buffer[i] = rec_buffer[i];
	}
	return len;
}

/**
  * @brief Connect - Empty
  * @param None
  * @retval None
*/
void ble_interface_connect(){
	//---
}

/**
  * @brief Disconnect
  * @param None
  * @retval None
*/
void ble_interface_disconnect(){
	uint8_t buffer = 0;
	ble_write(DC, &buffer, 1);
	
}

/**
  * @brief Free Bytes of Internal Buffers
  * @param None
  * @retval None
*/
void ble_interface_deinit(){
	free(send_buffer);
	free(rec_buffer);
}

/**
  * @brief Set Name of Device for Advertizing
  * @param ble_name: Name of the Device
  * @param ble_name_len: Length of the Device Name
  * @retval None
*/
void ble_interface_set_name(const uint8_t * ble_name, const uint8_t ble_name_len){
	ble_write(W_DEV_NAME, ble_name, ble_name_len);
}

/**
  * @brief Advertize Device
  * @param ble_advertize: Advertize if true
  * @retval None
*/
void ble_interface_advertize(const bool ble_advertize){
	if(!ble_advertize){
		uint8_t adv_ = 0x00;
		ble_write(SET_ADV_ENABLE, &adv,1);
	}
	else{
		uint8_t adv_ = 0x81;
		ble_write(SET_ADV_ENABLE, &adv,1);
	}
}