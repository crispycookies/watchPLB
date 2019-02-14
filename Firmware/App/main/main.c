
/**
 * @file main.c
 * @author Paul Götzinger
 * @brief 
 * @version 0.1
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
 
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "log.h"
#include "ui.h"
#include "emergencyCall.h"
#include "location.h"
#include "sysclock_driver.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	LOG_Init();
	
	HAL_Delay(1000);
	
	LOC_Init();
	EMC_Init();
  	UI_Init();

	while (1) {
		LOC_Process();
		EMC_Process();
	 	UI_Update();
	}
}





#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
