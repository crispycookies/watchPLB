/*
 * Lights.cpp
 *
 *  Created on: 10.02.2018
 *      Author: tobilinux
 */

#include "Lights.h"

#include "../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.h"
#include "core_cm4.h"

//Interrupt Block

int tim7count = 0;
int tim7extwavelen = 0;


extern "C" void TIM7_IRQHandler(){
	if(tim7count==0){
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
		tim7count++;
	}else{
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
		if(tim7count == tim7extwavelen/2){
			tim7count= 0;
		}else{
			tim7count++;
		}
	}
	TIM7->SR = 0;
	TIM7->CNT = 0;
}
//Constructor Destructor Block

Lights::Lights(){
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	CollisionLights.Mode = GPIO_MODE_OUTPUT_PP;
	CollisionLights.Pin |= (GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_14);
	CollisionLights.Pull = GPIO_PULLDOWN;
	CollisionLights.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &CollisionLights);
}

Lights::~Lights(){
	DisableAllLights();
	RCC->AHBENR &= ~RCC_AHBENR_GPIOEEN;
	HAL_GPIO_DeInit(GPIOB, (GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_14));
}

//Disable Lights Block

void Lights::DisableAllLights(){
	DisableArmedLight();
	DisableCollisionLights();
	DisableNavigationLights();
	DisableLandingLight();
}
void Lights::DisableArmedLight(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
}
void Lights::DisableCollisionLights(){
	DisableCollisionTimer7();
}
void Lights::DisableLandingLight(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}
void Lights::DisableNavigationLights(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}

//Private Disable Block

void Lights::DisableCollisionTimer7(){
	TIM7->CR1 &= ~1;
	NVIC_DisableIRQ(TIM7_IRQn);
}

//Enable Lights Block

void Lights::EnableAllLights(){
	EnableArmedLight();
	EnableLandingLight();
	EnableNavigationLights();
	EnableCollisionLights();
}
void Lights::EnableArmedLight(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
}
void Lights::EnableCollisionLights(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

	tim7count = 0;
	tim7extwavelen = tim7wavelen;

	EnableCollisionTimer7();
}
void Lights::EnableLandingLight(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
}
void Lights::EnableNavigationLights(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
}

//Private Enable Block
void Lights::EnableCollisionTimer7(){
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->PSC = SystemCoreClock/clkdivider-1;
	TIM7->ARR = tim7intlen;
	TIM7->SR = 0;
	TIM7->DIER = TIM_DIER_UIE;
	TIM7->CNT = 0;
	TIM7->CR1 &= ~1;
	TIM7->CR1 = 1;
	NVIC_EnableIRQ(TIM7_IRQn);
}