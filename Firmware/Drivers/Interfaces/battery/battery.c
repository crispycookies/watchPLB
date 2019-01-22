/*
 * battery.c
 *
 *  Created on: 15.01.2019
 *      Author: tobi
 */

#include "battery.h"
#include "adc.h"


const uint32_t timeout = 10000;
const uint32_t V_0 = 1656;
const float conv_fact = 4.82;

void battery_init() {
	Adc_Init();
}

uint8_t battery_convert(const int32_t value) {
	if(value < V_0){
		return (uint8_t) (0);
	}
	return (uint8_t) ((value-V_0)/conv_fact);
}

uint8_t battery_status() {
	return battery_convert(Adc_GetValue(timeout));
}
