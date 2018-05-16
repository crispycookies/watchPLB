/*
 * led_driver.h
 *
 *  Created on: 15.05.2018
 *      Author: tobilinux
 */

#ifndef USER_LED_LED_DRIVER_H_
#define USER_LED_LED_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

enum LED_ACTION{
	led_enum_on  = 0,
	led_enum_off = 1,
	led_enum_toggle = 2
};
typedef enum {
	led_pa4 = 0,
	led_pa5 = 1,
	led_pa6 = 2,
	led_pa7 = 3,
	led_pc4 = 4,
	led_pc5 = 5,
	led_pb0 = 6,
	led_pb1 = 7,
	led_pb2 = 8,
	led_pb10 = 9,
	led_pb11 = 10
} LED_PIN;

// Led Init

void led_init(void);

bool led_on(LED_PIN led);

bool led_off(LED_PIN led);

bool led_toggle(LED_PIN led);

void led_deinit(void);

// Timer

typedef uint16_t REGISTER;

void led_timer_init(uint8_t time_intervall);

bool led_timer_start(void);

bool led_timer_stop(void);

void led_action_time(LED_PIN led, REGISTER value);

#endif /* USER_LED_LED_DRIVER_H_ */
