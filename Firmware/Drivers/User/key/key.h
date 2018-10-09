/*
  **************************
  * @file    key.h
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    15.05.2018
  * @brief   key driver module
  **************************
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


/**
 * @brief initializes all Keys used
 * @param none
 */
void KEY_Init(void);

/**
 * @brief initializes all Keys used
 * @param BTN_Pins enum, to declare which Key is meant
 * @return GPIO_PIN_RESET = 0U, if the key is pressed
  	  	   GPIO_PIN_SET		  	if the key is not pressed
 */
GPIO_PinState KEY_Get(BTN_Pins btn);

#endif //KEY_H
