/**
  **************************
  * @file    usb.h
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    12.06.2018
  * @brief   usb driver module
  **************************
  */

// Initializes all components needed to send data via USB to Virtual COM Port
HAL_StatusTypeDef USB_Init();

// Sends unsigned char array to vcom
HAL_StatusTypeDef USB_SendData(uint8_t* Buf, uint16_t Len);
