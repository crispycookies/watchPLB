/**
  **************************
  * @file    log.c
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    13.11.2016
  * @brief   usb driver module
  **************************
  */
#include "log.h"
#include "usb.h"
#include <stdio.h>
#include <stdarg.h>

static uint8_t buffer[256];

HAL_StatusTypeDef LOG_Init()
{
	USB_Init();
	return HAL_OK;
}


HAL_StatusTypeDef LOG_Log(const uint8_t * format, ...)
{
  va_list args;
  va_start (args, format);
  uint16_t len = vsprintf ((char*)buffer,(char*)format, args);
  if(USB_SendData (buffer, len) != HAL_OK)
  {
	  return HAL_ERROR;
  }
  va_end (args);

  return HAL_OK;
}
