/*
 * Lights.h
 *
 *  Created on: 10.02.2018
 *      Author: tobilinux
 */

#ifndef LIGHTS_H_
#define LIGHTS_H_

#include "stm32f3xx.h"
#include "stm32f303xe.h"
#include "../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.h"
#include "../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.h"

#include "Object.h"

extern "C" void TIM7_IRQHandler();

class Lights: public Object{
	friend void TIM7_IRQHandler();
private:
	const int clkdivider =  100000;
	const int tim7wavelen = 20;
	const int tim7intlen = clkdivider/tim7wavelen;

	void EnableCollisionTimer7();
	void DisableCollisionTimer7();

	GPIO_InitTypeDef CollisionLights;
public:
	Lights();

	void EnableAllLights();
	void DisableAllLights();

	void EnableCollisionLights();
	void DisableCollisionLights();

	void EnableNavigationLights();
	void DisableNavigationLights();

	void EnableLandingLight();
	void DisableLandingLight();

	void EnableArmedLight();
	void DisableArmedLight();

	~Lights();
};

#endif /* LIGHTS_H_ */
