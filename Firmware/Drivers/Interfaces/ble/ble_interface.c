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
#include "../CRC/crc8.h"
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
#define R_LOCAL_CHARACTERISTICS_VALUE 0x3A
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
#define ENTER_CFG_MODE 0x0B

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
static uint8_t send_buffer[maxbuffer] = {0};
static uint8_t rec_buffer[maxbuffer] = {0};
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
  * @brief Zeros the Buffer
  * @param None
  * @retval None
*/
static void zero_arr(){
	for(int i = 0; i < maxbuffer; i++){
		send_buffer[i] = 0;
	}
}

void ble_receive(){
	if(!ble_interface_get_buffer_length()){
		return;
	}
	uint8_t buf[3] = {0};
	ble_interface_get_buffer_length(&buf, 3);

	if(buf[2] != SUCCEED){
		return;
	}
}
/**
  * @brief Sends the Data
  * @param command: The Command to be sent
  * @param data: The Data to use
  * @param data_length: Length of the Data to use
  * @retval Result of Operation
*/
static bool ble_write(const uint8_t command, const uint8_t * data, const uint8_t data_length){
		zero_arr();

		if((int16_t)(data_length+5) >= (int16_t)maxbuffer)
			return false;

		uint8_t * to_send = send_buffer;

	//https://github.com/Iclario/BM70-BLEDK3/blob/master/BM70.cpp
		uint8_t lengthH  = (uint8_t) ((1 + data_length) >> 8);
		uint8_t lengthL  = (uint8_t) (1 + data_length);
		uint8_t checksum = 0 - lengthH - lengthL - command;


		for (uint16_t i = 0; i < data_length; i++)
			checksum -= data[i];

		to_send[0] = UART_START_SEQ;
		to_send[1] = lengthH;
		to_send[2] = lengthL;
		to_send[3] = command;

		uint16_t i = 0;
		for(;i < data_length; i++){
			to_send[i+4] = data[i];
		}

		to_send[++i] = checksum;

		UART_SendData(&inst, data_length+3, to_send);

		return true;
}

/**
  * @brief Initialize the BLE- Module
  * @param  None
  * @retval None
*/
void ble_interface_init() {
	//READY
	conf.baud = UART_BaudRate_115200;
	conf.uart = USART1;
	conf.rxBoard = GPIOA;
	conf.rxPin = GPIO_PIN_9;
	conf.rxAF = GPIO_AF4_USART1;
	conf.rxDmaChannel = DMA1_Channel3;
	conf.txBoard = GPIOA;
	conf.txPin = GPIO_PIN_10;
	conf.txAF = GPIO_AF4_USART1;
	conf.txDmaChannel = DMA1_Channel2;

	UART_Init(&inst, &conf);

	ble_write(ENTER_CFG_MODE, 0, 0);
	ble_receive();
	ble_write(RESET, 0, 0);
	ble_receive();
}
/**
  * @brief Send Data in Transparent Mode
  * @param tx_buffer: pointer to tx buffer
  * @param tx_buffer_length: length of tx buffer
  * @retval None
*/
void ble_interface_send(uint8_t * tx_buffer, uint8_t tx_buffer_length) {
	if(tx_buffer_length >= 50 || tx_buffer_length == 0){
		return;
	}

	uint8_t to_write[maxbuffer] = {0};
	to_write[0] = connhdl;

	for(int i = 0; i < tx_buffer_length;i++){
		to_write[i+1] = tx_buffer[i];
	}

	ble_receive();
	ble_write(SEND_TRANSPARENT_DATA, to_write, tx_buffer_length+1);
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
void ble_interface_connect(bool rd_addr, uint64_t address){
	ble_receive();

	uint8_t * to_send = malloc(8);

	to_send[0] = 0;
	to_send[1] = (uint8_t)rd_addr;

	for(int i = 0; i < 6; i++){
		to_send[i+2] = (uint8_t)(address >> (8*i));
	}

	ble_write(0x17, to_send, 8);
	ble_receive();
}

/**
  * @brief Disconnect
  * @param None
  * @retval None
*/
void ble_interface_disconnect(){
	uint8_t buffer = 0;
	ble_receive();
	ble_write(DC, &buffer, 1);
}

/**
  * @brief Free Bytes of Internal Buffers
  * @param None
  * @retval None
*/
void ble_interface_deinit(){

}

/**
  * @brief Set Name of Device for Advertizing
  * @param ble_name: Name of the Device
  * @param ble_name_len: Length of the Device Name
  * @retval None
*/
void ble_interface_set_name(const uint8_t * ble_name, const uint8_t ble_name_len){
	if(ble_name_len > 16){
		return;
	}

	ble_write(ENTER_CFG_MODE, 0, 0);

	uint8_t to_write[maxbuffer] = {0};

	to_write[0] = 0;
	for(int i = 0; i < ble_name_len-1; i++){
		to_write[i+1] = ble_name[i];
	}

	ble_write(W_DEV_NAME, to_write, ble_name_len);
	ble_write(LEAVE_CFG_MODE, 0, 0);
}

/**
  * @brief Advertize Device
  * @param ble_advertize: Advertize if true
  * @retval None
*/
void ble_interface_advertize(const bool ble_advertize){

	ble_write(ENTER_CFG_MODE, 0, 0);
	if(ble_advertize){
		uint8_t adv = 0x01;
		ble_write(SET_ADV_ENABLE, &adv,1);
	}
	else{
		uint8_t adv = 0x00;
		ble_write(SET_ADV_ENABLE, &adv,1);
	}
	ble_write(LEAVE_CFG_MODE, 0, 0);
}
