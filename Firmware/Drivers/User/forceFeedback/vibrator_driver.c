/*
 * vibratordriver.c
 *
 *  Created on: 22.05.2018
 *      Author: tobilinux
 */

#include "vibrator_driver.h"

#define true 1
#define false 0

// Global Init Variables for LED
GPIO_InitTypeDef Vibrator_Oli;
GPIO_TypeDef  *bank;
GPIO_PinType pin;

uint8_t vibrator_init(GPIO_TypeDef BANK, GPIO_PinType PIN){
	bank = bank;
	pin = pin;

	if(bank == GPIOA){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}else if(bank == GPIOB){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}else if(bank == GPIOC){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}else if(bank == GPIOD){
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}else{
		return false;
	}

	//General Init
	Vibrator_Oli.Mode = GPIO_MODE_OUTPUT_PP;
	Vibrator_Oli.Pull = GPIO_PULLDOWN;
	Vibrator_Oli.Speed = GPIO_SPEED_FREQ_LOW;
	Vibrator_Oli.Pin |= PIN;
	HAL_GPIO_Init(bank, &Vibrator_Oli);

	return true;
}

void vibrator_on(){
	HAL_GPIO_WritePin(bank, pin,  GPIO_PIN_SET);
}

void vibrator_off(){
	HAL_GPIO_WritePin(bank, pin,  GPIO_PIN_RESET);
}

void vibrator_deinit(){
	HAL_GPIO_DeInit(bank, pin);

	if(bank == GPIOA){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}else if(bank == GPIOB){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}else if(bank == GPIOC){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}else if(bank == GPIOD){
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
}
