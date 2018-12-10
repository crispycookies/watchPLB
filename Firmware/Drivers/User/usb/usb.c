/**
  **************************
  * @file    usb.c
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    12.06.2018
  * @brief   usb driver module
  **************************
  */
#include "usb.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

HAL_StatusTypeDef USB_Init()
{
	//enable clocks
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  MX_USB_DEVICE_Init();

	  return HAL_OK;
}

HAL_StatusTypeDef USB_SendData(uint8_t* Buf, uint16_t Len)
{
	//CDC_Transmit_FS: Data to send over USB IN endpoint are sent over CDC interface through this function.
	if(CDC_Transmit_FS(Buf, Len) == USBD_FAIL)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}
