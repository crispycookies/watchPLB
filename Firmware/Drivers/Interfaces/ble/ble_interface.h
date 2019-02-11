/**
  ******************************************************************************
  * @file    ble_interface.h
  * @author  Tobias Egger
  * @version V0.9
  * @date    20-November-2018
  * @brief   WatchPLB LED- Driver Interface File
  ******************************************************************************
  */

#ifndef INTERFACE_BLE_INTERFACE_H
#define INTERFACE_BLE_INTERFACE_H
  
#include <stdbool.h>

/**
  * @brief Initialize the BLE- Module
  * @param  None
  * @retval None
*/
void ble_interface_init();

//void ble_enable(){};
//void ble_disable(){};

//void ble_enable_pairing_auto(){};
//void ble_enable_pairing(){};

//void ble_accept_pairing_request(){};
/**
  * @brief Send Data in Transparent Mode
  * @param tx_buffer: pointer to tx buffer
  * @param tx_buffer_length: length of tx buffer
  * @retval None
*/
void ble_interface_send(uint8_t * tx_buffer, uint8_t tx_buffer_length);

/**
  * @brief Get Length of Received Data
  * @param None
  * @retval Length of Data
*/
uint16_t ble_interface_get_buffer_length();

/**
  * @brief Get Received Data
  * @param rx_buffer: Pointer to where the Data shall be stored  
  * @param rx_buffer_length: Length of the Buffer
  * @retval Remaining Bytes
*/
uint16_t ble_interface_get_buffer(uint8_t *rx_buffer, uint8_t rx_buffer_length);

/**
  * @brief Free Bytes of Internal Buffers
  * @param None
  * @retval None
*/
void ble_interface_deinit();

/**
  * @brief Connect - Empty
  * @param None
  * @retval None
*/
void ble_interface_connect(bool rd_addr, uint64_t address);

/**
  * @brief Disconnect
  * @param None
  * @retval None
*/
void ble_interface_disconnect();

/**
  * @brief Set Name of Device for Advertizing
  * @param ble_name: Name of the Device
  * @param ble_name_len: Length of the Device Name
  * @retval None
*/
void ble_interface_set_name(const uint8_t * ble_name, const uint8_t ble_name_len);

/**
  * @brief Advertize Device
  * @param ble_advertize: Advertize if true
  * @retval None
*/
void ble_interface_advertize(const bool ble_advertize);

#endif /*INTERFACE_BLE_INTERFACE_H*/
