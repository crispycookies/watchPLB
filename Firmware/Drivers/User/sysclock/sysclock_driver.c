/**
  ******************************************************************************
  * @file    sysclock_driver.c
  * @author  Tobias Egger
  * @version V0.9
  * @date    18-December-2018
  * @brief   WatchPLB Sysclock Driver
  ******************************************************************************
  */

#include "sysclock_driver.h"

/**
  * @brief Error Handler
  * @param file: unused
  * @param line: unused
  * @retval None
*/
void _Error_Handler(char *file, int line)
{
	while(1) {}
}

/**
  * @brief Sysclock Configuration Function - Full Speed
  * @param None
  * @retval None
*/
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	//Configure the main internal regulator output voltage
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	//Initializes the CPU, AHB and APB busses clocks
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_12;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	//Initializes the CPU, AHB and APB busses clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
									| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_USART1;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	//Configure the Systick interrupt time
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	//Configure the Systick
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	//SysTick_IRQn interrupt configuration
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief Sysclock Configuration Function - Stand By
  * @param None
  * @retval None
*/
void SystemClock_SleepMode_Config(void){
		RCC_OscInitTypeDef RCC_OscInitStruct;
		RCC_ClkInitTypeDef RCC_ClkInitStruct;
		RCC_PeriphCLKInitTypeDef PeriphClkInit;

		//Configure the main internal regulator output voltage
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		//Initializes the CPU, AHB and APB busses clocks
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
		RCC_OscInitStruct.HSEState = RCC_HSE_ON;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
		RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
		RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_4;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		//Initializes the CPU, AHB and APB busses clocks
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
										| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_USART1;
		PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
		PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		//Configure the Systick interrupt time
		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

		//Configure the Systick
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

		//SysTick_IRQn interrupt configuration
		HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
