/*
 * battery.c
 *
 *  Created on: 15.01.2019
 *      Author: tobi
 */

#include "battery.h"
#include "adc.h"

const uint8_t channel = 1;
const uint32_t pin = GPIO_PIN_1;
GPIO_TypeDef * bank = GPIOA;

void battery_init() {
	GPIO_InitTypeDef adc_init_def;
	adc_init_def.Mode = GPIO_MODE_ANALOG;
	adc_init_def.Pin = pin;
	adc_init_def.Pull = GPIO_NOPULL;
	adc_init_def.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(bank, &adc_init_def);

	Adc_Init(channel);
}

uint8_t battery_convert(const int32_t value) {
	const uint16_t divider = 330;
	return (uint8_t) (value/divider);
}

uint8_t battery_status() {
	Adc_SetChannel(channel);
	return battery_convert(Adc_GetValue(channel));
}
