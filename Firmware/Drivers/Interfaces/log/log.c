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

#define BUFFER_LEN 256

static uint8_t buffer[BUFFER_LEN];

static uint8_t init = 0;

void LOG_Init()
{
	USB_Init();
	init = 1;
	return;
}


void LOG_Log(const char * format, ...)
{
	if (init == 0) {
		return;
	}

	va_list args;
	va_start (args, format);
	
	uint16_t len = vsnprintf ((char*)buffer, BUFFER_LEN, format, args);
	USB_SendData (buffer, len);

	va_end (args); 
	return;
}
