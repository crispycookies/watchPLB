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

	IWDG_HandleTypeDef hiwdg;

HAL_StatusTypeDef IWDG_Init(void){

	// fill watchdog init structure
	hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
	hiwdg.Init.Reload = RELOAD_VAL;
	hiwdg.Init.Window = IWDG_WINDOW_DISABLE;

	return HAL_IWDG_Init(&hiwdg);
}


// refreshes the watchdog counter (=set to 0)
HAL_StatusTypeDef IWDG_Refresh(void){

	return HAL_IWDG_Refresh(&hiwdg);
}

// starts the Watchdog
void IWDG_Start(void){

	__HAL_IWDG_START(&hiwdg);
}
