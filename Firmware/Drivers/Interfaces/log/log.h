/**
  **************************
  * @file    log.h
  * @author  Michael Neuhofer
  * @author  Paul Götzinger
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

#define LOG_DEST LOG_USB

/**
 * @brief Initializes all needed components
 * 
 */
void LOG_Init();

/**
 * @brief Logs a printf formatted message
 * 
 * @param format format of data (printf style)
 * @param ... data
 */
void LOG_Log(const char * format, ...);

/**
 * @brief Log a bitarray
 * 
 * @param array bit array
 * @param len length of bit array
 */
void LOG_BitArray(uint8_t *array, uint16_t len);

#endif //LOG_H