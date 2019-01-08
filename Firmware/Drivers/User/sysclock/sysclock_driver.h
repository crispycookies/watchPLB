/**
  ******************************************************************************
  * @file    sysclock_driver.h
  * @author  Tobias Egger
  * @version V0.9
  * @date    18-December-2018
  * @brief   WatchPLB Sysclock Driver
  ******************************************************************************
  */


#ifndef USER_SYSCLOCK_SYSCLOCK_DRIVER_H_
#define USER_SYSCLOCK_SYSCLOCK_DRIVER_H_

/**
  * @brief Error Handler
  * @param file: unused
  * @param line: unused
  * @retval None
*/
void _Error_Handler(char *file, int line);


/**
  * @brief Sysclock Configuration Function - Full Speed
  * @param None
  * @retval None
*/
void SystemClock_Config(void);

/**
  * @brief Sysclock Configuration Function - Stand By
  * @param None
  * @retval None
*/
void SystemClock_SleepMode_Config(void);

/**
  * @brief Sysclock Configuration Function - Stand By
  * @param None
  * @retval None
*/
void SystemClock_UnSleepMode_Config(void);

#endif /* USER_SYSCLOCK_SYSCLOCK_DRIVER_H_ */
