/**
  ******************************************************************************
  * @file    battery.c
  * @author  Tobias Egger
  * @author  Michael Neuhofer
  * @author  Samuel Hick
  * @version V1.1
  * @date    24-January-2019
  * @brief   WatchPLB Battery Driver
  ******************************************************************************
  */

/*Includes*/
#include "battery.h"
#include "adc.h"

/*Module Internal Constats*/
const uint32_t timeout = 10000;
const uint32_t V_0 = 1656;
const float conv_fact = 4.82;

/**
  * @brief Initializes ADC and Battery
  * @param  none
  * @retval none
*/
void battery_init() {
	Adc_Init();
}

/**
  * @brief Converts the Value
  * @param  value: Raw ADC Value
  * @retval Percent
*/
uint8_t battery_convert(const int32_t value) {
	if(value < V_0){
		return (uint8_t) (0);
	}
	return (uint8_t) ((value-V_0)/conv_fact);
}

/**
  * @brief Fetches, Converts and Returns the Battery Loading State in Percent
  * @param  none
  * @retval Percent
*/
uint8_t battery_status() {
	return battery_convert(Adc_GetValue(timeout));
}
