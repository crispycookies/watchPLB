/**
  ******************************************************************************
  * @file    led_driver.h
  * @author  Tobias Egger
  * @version V1.1
  * @date    12-June-2018
  * @brief   WatchPLB LED- Driver Interface File
  ******************************************************************************
  */

#ifndef USER_LED_LED_DRIVER_H_
#define USER_LED_LED_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

/*public enums*/
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

/*Basic LED- Driver Block*/
/**
  * @brief Initialize GPIOs for LEDs
  * @param  None
  * @retval None
*/
void led_init(void);

/**
  * @brief Turn on LED
  * @param  led: The LED to be turned on
  * @retval true if succeeded and false if not
*/
bool led_on(LED_PIN led);

/**
  * @brief Turn off LED
  * @param  led: The LED to be turned off
  * @retval true if succeeded and false if not
*/
bool led_off(LED_PIN led);

/**
  * @brief Toggle LED
  * @param  led: The LED to be toggled
  * @retval true if succeeded and false if not
*/
bool led_toggle(LED_PIN led);

/**
  * @brief Deinitializes GPIOs and turns of Clocks
  * @warning Only use this Function if you are sure about the Consequences
  * @param  none
  * @retval none
*/
void led_deinit(void);


/*Advanced LED- Driver Block*/
/*Global Typedefs*/
typedef uint16_t REGISTER;
typedef uint16_t TIME;

/**
  * @brief Initializes the Timer and sets the Interval in which the Timer throws an interrupt
  * @brief led_init must be called first
  * @param  time_intervall: The LED to be turned on
  * @retval none
*/
void led_timer_init(TIME time_intervall);

/**
  * @brief Starts the Timer and its Interrupt
  * @param  none
  * @retval true if succeeded and false if not
*/
bool led_timer_start(void);

/**
  * @brief Stops the Timer and its Interrupt
  * @param  none
  * @retval true if succeeded and false if not
*/
bool led_timer_stop(void);

/**
  * @brief Stops the Timer and its Interrupt
  * @param led: the LED for which the REGISTER value has to be set
  * @param value: the Register Value. Interrupt shifts this Value by 1 and checks if Bit is set. If Set, it enables LED, otherwise it disables.
  * @retval true if succeeded and false if not
*/
void led_action_time(LED_PIN led, REGISTER value);

#endif /* USER_LED_LED_DRIVER_H_ */
