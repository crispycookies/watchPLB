/**
  **************************
  * @file    log.h
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    13.11.2016
  * @brief   usb driver module
  **************************
  */

// Initializes all needed components (USB)
HAL_StatusTypeDef LOG_Init();

// Writes formated Data via USB
HAL_StatusTypeDef LOG_Log(const uint8_t * format, ...);
