/**
  **************************
  * @file    watchdog.h
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    12.06.2018
  * @brief   watchdog driver module
  **************************
  */
#ifndef IWDG_H
#define IWDG_H

#define RELOAD_VAL 0x09C4; //watchdog is set to 1 s  (clock is 40.000 / 16 = 2500)

// freeze and unfreeze watchdog with __HAL_DBGMCU_FREEZE_IWDG() and __HAL_DBGMCU_UNFREEZE_IWDG() for debug purposes

/**
 * @brief initializes the watchdog
 * @param none
 * @return 	HAL_OK if everything went fine
 * 			HAL_ERROR if something unexpected happened
 */
HAL_StatusTypeDef IWDG_Init(void);


/**
 * @brief refreshes the Watchdog
 * @param
 * @return HAL_OK if everything went fine
 * 		   HAL_ERROR if something unexpected happened
 */
HAL_StatusTypeDef IWDG_Refresh(void);


/**
 * @brief start the watchdog (check if HAL_StatusTypeDef return value of Init function is HAL_OK
 * @param none
 * @return none
 */
void IWDG_Start(void);



#endif //!IWDG_H
