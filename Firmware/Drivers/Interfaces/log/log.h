/**
  **************************
  * @file    log.h
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    13.11.2016
  * @brief   usb driver module
  **************************
  */

#ifndef LOG_H
#define LOG_H

#define LOG_NONE 0
#define LOG_USB  1
#define LOG_UART 2
#define LOG_GPS  3

#define LOG_DEST LOG_GPS

// Initializes all needed components (USB)
void LOG_Init();

// Writes formated Data via USB
void LOG_Log(const char * format, ...);

void LOG_BitArray(uint8_t *array, uint16_t len);

#endif //LOG_H