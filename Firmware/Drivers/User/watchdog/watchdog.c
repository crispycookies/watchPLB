/**
  **************************
  * @file    watchdog.c
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    12.06.2018
  * @brief   watchdog driver module
  **************************
  */

#include "watchdog.h"

	IWDG_HandleTypeDef *hiwdg;
	IWDG_TypeDef *iwdgInstance;

HAL_StatusTypeDef IWDG_Init(void){

	IWDG_InitTypeDef iwdgInit;
	iwdgInit.Prescaler = IWDG_PRESCALER_16;
	iwdgInit.Reload = RELOAD_VAL;
	iwdgInit.Window = IWDG_WINDOW_DISABLE;

	hiwdg->Init = iwdgInit;
	hiwdg->Instance = iwdgInstance;


	return HAL_IWDG_Init(hiwdg);
}



HAL_StatusTypeDef IWDG_Refresh(void){

	return HAL_IWDG_Refresh(hiwdg);
}
