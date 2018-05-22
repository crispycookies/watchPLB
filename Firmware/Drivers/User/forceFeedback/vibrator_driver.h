/*
 * vibratordriver.h
 *
 *  Created on: 22.05.2018
 *      Author: tobilinux
 */

#ifndef USER_FORCEFEEDBACK_VIBRATOR_DRIVER_H_
#define USER_FORCEFEEDBACK_VIBRATOR_DRIVER_H_

typedef uint8_t GPIO_PinType;

uint8_t vibrator_init(GPIO_TypeDef BANK, GPIO_PinType PIN);

void vibrator_on(void);

void vibrator_off(void);

void vibrator_deinit(void);

#endif /* USER_FORCEFEEDBACK_VIBRATOR_DRIVER_H_ */
