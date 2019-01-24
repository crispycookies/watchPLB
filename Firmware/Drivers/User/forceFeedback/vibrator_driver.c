/**
  ******************************************************************************
  * @file    vibrator_driver.c
  * @author  Tobias Egger
  * @version V1.1
  * @date    12-June-2018
  * @brief   WatchPLB Vibrator- Driver Implementation File
  ******************************************************************************
  */


#include "vibrator_driver.h"

/* Global Init Variables for LED*/
GPIO_InitTypeDef vibrator;
GPIO_TypeDef  *bank;
GPIO_PinType pin;


/**
  * @brief Initialize GPIOs for Vibrator
  * @param  BANK: The GPIO Bank to use
  * @param 	PIN: The Pin to use
  * @retval Returns True if Params are Valid, false if not
*/
bool vibrator_init(GPIO_TypeDef *BANK, GPIO_PinType PIN){
	bank = BANK;
	pin = PIN;

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
	vibrator.Mode = GPIO_MODE_OUTPUT_PP;
	vibrator.Pull = GPIO_PULLDOWN;
	vibrator.Speed = GPIO_SPEED_FREQ_LOW;
	vibrator.Pin |= PIN;
	HAL_GPIO_Init(bank, &vibrator);

	return true;
}

/**
  * @brief Turns on Vibrator
  * @param  None
  * @retval None
*/
void vibrator_on(){
	HAL_GPIO_WritePin(bank, pin,  GPIO_PIN_SET);
}

/**
  * @brief Turns off Vibrator
  * @param  None
  * @retval None
*/
void vibrator_off(){
	HAL_GPIO_WritePin(bank, pin,  GPIO_PIN_RESET);
}

/**
  * @brief Deinitializes Vibrator
  * @warning Do not call if you are unsure
  * @param  None
  * @retval None
*/
void vibrator_deinit(){
#warning Do not call if you are unsure
	HAL_GPIO_DeInit(bank, pin);

	if(bank == GPIOA){
		__HAL_RCC_GPIOA_CLK_DISABLE();
	}else if(bank == GPIOB){
		__HAL_RCC_GPIOB_CLK_DISABLE();
	}else if(bank == GPIOC){
		__HAL_RCC_GPIOC_CLK_DISABLE();
	}else if(bank == GPIOD){
		__HAL_RCC_GPIOD_CLK_DISABLE();
	}
}
