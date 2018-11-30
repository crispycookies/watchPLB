/**
  ******************************************************************************
  * @file           : usb_device.h
  * @version        :
  * @brief          : Header for usb_device.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "usbd_def.h"


/** USB device core handle. */
extern USBD_HandleTypeDef hUsbDeviceFS;


/** USB Device initialization function. */
void MX_USB_DEVICE_Init(void);


#ifdef __cplusplus
}
#endif

#endif
