/**
  ******************************************************************************
  * @file    battery.h
  * @author  Tobias Egger
  * @author  Michael Neuhofer
  * @author  Samuel Hick
  * @version V1.1
  * @date    24-January-2019
  * @brief   WatchPLB Battery Driver
  ******************************************************************************
  */

#ifndef INTERFACES_BATTERY_BATTERY_H_
#define INTERFACES_BATTERY_BATTERY_H_

/**
  * @brief Initializes ADC and Battery
  * @param  none
  * @retval none
*/
void battery_init(void);

/**
  * @brief Fetches, Converts and Returns the Battery Loading State in Percent
  * @param  none
  * @retval Percent
*/
uint8_t battery_status(void);

#endif /* INTERFACES_BATTERY_BATTERY_H_ */
