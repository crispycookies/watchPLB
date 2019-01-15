/*
 * key.c
 *
 *  Created on: 15.05.2018
 *      Author: Michael
 */

#include "key.h"


#define KEY_COUNT 4

typedef struct {
	GPIO_TypeDef* board;
	uint16_t      pin;
} pin_typedef;

static pin_typedef keys[KEY_COUNT] = {
	{GPIOB,GPIO_PIN_12},
	{GPIOC,GPIO_PIN_7},
	{GPIOC,GPIO_PIN_8},
	{GPIOC,GPIO_PIN_9},
};

void KEY_Init(void){

	// activate clocks
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpio;

	//set gpio modes
	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	
	for(uint8_t i = 0; i < KEY_COUNT; i++)
	{
		gpio.Pin = keys[i].pin;
		HAL_GPIO_Init(keys[i].board, &gpio);
	}
}


// gives you the state of the requested button
GPIO_PinState KEY_Get(BTN_Pins btn){
	if (btn >= 0 && btn < KEY_COUNT) {
		return HAL_GPIO_ReadPin(keys[btn].board, keys[btn].pin);
	}
	return 0;
}
