/*
 * key.h
 *
 *  Created on: 15.05.2018
 *      Author: Michael
 */
#ifndef KEY_H
#define KEY_H


typedef enum{
	BTN_1 = GPIO_PIN_12, 		//PB12
	BTN_2 = GPIO_PIN_7, 		//PC7
	BTN_3 = GPIO_PIN_8,			//PC8
	BTN_4 = GPIO_PIN_9			//PC9
	//BTN_TEST = GPIO_PIN_13		//PC13 on TESTBOARD
}BTN_Pins;

// initializes all Keys used
void KEY_Init(void);

// Returns the State of the given Button
GPIO_PinState KEY_Get(BTN_Pins btn);

#endif //KEY_H
