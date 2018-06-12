/*
 * key.c
 *
 *  Created on: 15.05.2018
 *      Author: Michael
 */

#include "key.h"


#define KEY_COUNT 4


void KEY_Init(void){

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpioB;
	GPIO_InitTypeDef gpioC;

	gpioB.Pin = BTN_1;
	gpioB.Mode = GPIO_MODE_INPUT;
	gpioB.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOB, &gpioB);


	gpioC.Pin |= BTN_2 | BTN_3 | BTN_4;// | BTN_TEST;
	gpioC.Mode = GPIO_MODE_INPUT;
	gpioC.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOC, &gpioC);
}

GPIO_PinState KEY_Get(BTN_Pins btn){

	if(btn < BTN_2)
	{
		return HAL_GPIO_ReadPin (GPIOB, btn);
	}
	else
	{
		return HAL_GPIO_ReadPin (GPIOC, btn);
	}
}
