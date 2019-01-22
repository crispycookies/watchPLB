/**
  ******************************************************************************
  * @file    vibrator_driver.h
  * @author  Tobias Egger
  * @version V1.1
  * @date    12-June-2018
  * @brief   WatchPLB Vibrator- Driver Interface File
  ******************************************************************************
  */

#ifndef USER_FORCEFEEDBACK_VIBRATOR_DRIVER_H_
#define USER_FORCEFEEDBACK_VIBRATOR_DRIVER_H_

#include <stdbool.h>

typedef uint32_t GPIO_PinType;

/**
  * @brief Initialize GPIOs for Vibrator
  * @param  BANK: The GPIO Bank to use
  * @param 	PIN: The Pin to use
  * @retval Returns True if Params are Valid, false if not
*/
bool vibrator_init(GPIO_TypeDef *BANK, GPIO_PinType PIN);

/**
  * @brief Turns on Vibrator
  * @param  None
  * @retval None
*/
void vibrator_on(void);

/**
  * @brief Turns off Vibrator
  * @param  None
  * @retval None
*/
void vibrator_off(void);

/**
  * @brief Deinitializes Vibrator
  * @warning Do not call if you are unsure
  * @param  None
  * @retval None
*/
void vibrator_deinit(void);

#endif /* USER_FORCEFEEDBACK_VIBRATOR_DRIVER_H_ */
