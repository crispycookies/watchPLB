/**
  ******************************************************************************
  * @file           : usbd_desc.h
  * @version        :
  * @brief          : Header for usbd_conf.c file.
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_DESC__H__
#define __USBD_DESC__H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/** @defgroup USBD_DESC_Exported_Variables USBD_DESC_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** Descriptor for the Usb device. */
extern USBD_DescriptorsTypeDef FS_Desc;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC__H__ */
