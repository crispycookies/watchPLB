/**
 **************************
 * @file    ui.c
 * @author  Michael Neuhofer
 * @version V1.0
 * @date    18.12.2018
 * @brief   user interface module
 **************************
 */

#include <stdbool.h>

#include "ui.h"
#include "led_driver.h"
#include "sysclock_driver.h"
#include "vibrator_driver.h"
#include "key.h"
#include "emergencyCall.h"
#include "battery.h"

typedef enum {
	UI_Idle, UI_Waiting, UI_SendLocation, UI_Sleepmode
} UIstates;

UIstates UIstate = UI_Idle;
static bool isSleepmode = false;
static const TIME ledtimercd = 10;
static REGISTER ledreg = 0x0F;

const uint8_t battery_100 = 80;
const uint8_t battery_80 = 60;
const uint8_t battery_60 = 40;
const uint8_t battery_40 = 20;
const uint8_t battery_20 = 0;

const uint8_t limiter = 10;

void UI_UpdateBattery() {
	// Check Battery state via ADC
	uint8_t battery = battery_status();

	if (battery > battery_100 ) {
		led_on(led_pa4);
		led_on(led_pa5);
		led_on(led_pa6);
		led_on(led_pa7);
		led_on(led_pc4);
	} else if (battery > battery_80) {
		led_off(led_pa4);
		led_on(led_pa5);
		led_on(led_pa6);
		led_on(led_pa7);
		led_on(led_pc4);
	} else if (battery > battery_60) {
		led_off(led_pa4);
		led_off(led_pa5);
		led_on(led_pa6);
		led_on(led_pa7);
		led_on(led_pc4);
	} else if (battery > battery_40) {
		led_off(led_pa4);
		led_off(led_pa5);
		led_off(led_pa6);
		led_on(led_pa7);
		led_on(led_pc4);
	} else if (battery >= battery_20) {
		led_off(led_pa4);
		led_off(led_pa5);
		led_off(led_pa6);
		led_off(led_pa7);
		led_on(led_pc4);
	} else {
		_Error_Handler("statebatteryerror", 222);
	}

}

void UI_TurnLEDsOff() {
	led_off(led_pa4);
	led_off(led_pa5);
	led_off(led_pa6);
	led_off(led_pa7);
	led_off(led_pc4);
	led_off(led_pc5);
	led_off(led_pb0);
	led_off(led_pb1);
	led_off(led_pb2);
}

void UI_CrazyLEDs() {
	led_timer_start();
	led_action_time(led_pa4, ledreg);
	led_action_time(led_pa5, ledreg);
	led_action_time(led_pa6, ledreg);
	led_action_time(led_pa7, ledreg);
	led_action_time(led_pc4, ledreg);
//led_action_time(led_pc5,ledreg);
	led_action_time(led_pb0, ledreg);
	led_action_time(led_pb1, ledreg);
	led_action_time(led_pb2, ledreg);
//led_timer_stop();
}

HAL_StatusTypeDef UI_Init() {
	SystemClock_Config();

	KEY_Init();

	led_init();
	battery_init();

	led_timer_init(ledtimercd);
//vibrator_init(GPIOA,15);
	UIstate = UI_Idle;

	return HAL_OK;
}

HAL_StatusTypeDef UI_Update() {

	static uint8_t counter = 0;
	if(counter == limiter){
		UI_UpdateBattery();
		counter = 0;
	}
	counter++;


	switch (UIstate) {
	case UI_Idle: {

		UIstate = UI_Waiting;
		break;
	}
	case UI_Waiting: {

		led_on(led_pb1);

		if (KEY_Get(BTN_1) == GPIO_PIN_SET) {
			// Sleepmode
			UIstate = UI_Sleepmode;
		} else if (KEY_Get(BTN_2) == GPIO_PIN_SET) {
			// activate Bluetooth here ////////////////////////////////////////////////////////////////
			led_on(led_pb0); /////////////////////////////TEST PURPOSE
			UIstate = UI_Waiting;

		} else if (KEY_Get(BTN_3) == GPIO_PIN_SET) {
			if (KEY_Get(BTN_4) == GPIO_PIN_SET) {
				//SOS
				UIstate = UI_SendLocation;
			}
		}

		// show the battery status via LEDs
		UI_UpdateBattery();

		break;
	}
	case UI_SendLocation: {
		//vibrator_on();
		// call SOS function

		EMC_SetEmergency(EMC_State_Emergency);

		//vibrator_off();
		led_on(led_pb2); /////////////////////////////TEST PURPOSE

		UIstate = UI_Waiting;
		break;
	}
	case UI_Sleepmode: {
		if (isSleepmode != true) {
			UI_TurnLEDsOff();
			isSleepmode = true;
			SystemClock_SleepMode_Config();
			// indicates Sleepmode
			led_on(led_pc5);
		}

		if (KEY_Get(BTN_2) == GPIO_PIN_SET || KEY_Get(BTN_3) == GPIO_PIN_SET
				|| KEY_Get(BTN_4) == GPIO_PIN_SET) {
			SystemClock_UnSleepMode_Config();
			UIstate = UI_Waiting;
			isSleepmode = false;
			led_off(led_pc5);
		}
		break;
	}
	default: {
		//errorhandler makes all LEDs go crazy
		_Error_Handler("stateerror", 222);
	}
	}

	return HAL_OK;
}
