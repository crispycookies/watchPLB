/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "usb.h"
#include "log.h"
#include "location.h"
#include "emergencyCall.h"

/* Private variables ---------------------------------------------------------*/
#define POS_CNT 7
#define DELAY 1000

POS_Position positions[POS_CNT] = {
	{
		//empty config
		.time = {
			.hour = 0,
			.minute = 0,
			.second = 0,
			.split = 0
		},
		.latitude = {
			.direction = POS_Latitude_Flag_S,
			.degree = 0,
			.minute = 0.0
		},
		.longitude = {
			.direction = POS_Longitude_Flag_W,
			.degree = 0,
			.minute = 0.0
		},
		.valid = POS_Valid_Flag_Valid //has to be valid
	}, {
		//test config
		.time = {
			.hour = 1,
			.minute = 2,
			.second = 3,
			.split = 4
		},
		.latitude = {
			.direction = POS_Latitude_Flag_N,
			.degree = 23,
			.minute = 45.234
		},
		.longitude = {
			.direction = POS_Longitude_Flag_W,
			.degree = 10,
			.minute = 38.923
		},
		.valid = POS_Valid_Flag_Valid
	}, {
		//test config (Hagenberg)
		.time = {
			.hour = 1,
			.minute = 2,
			.second = 4,
			.split = 2
		},
		.latitude = {
			.direction = POS_Latitude_Flag_N,
			.degree = 48,
			.minute = 22.082
		},
		.longitude = {
			.direction = POS_Longitude_Flag_E,
			.degree = 14,
			.minute = 30.782
		},
		.valid = POS_Valid_Flag_Valid
	}, {
		//test config (New York, US)
		.time = {
			.hour = 2,
			.minute = 3,
			.second = 4,
			.split = 5
		},
		.latitude = {
			.direction = POS_Latitude_Flag_N,
			.degree = 40,
			.minute = 43.650
		},
		.longitude = {
			.direction = POS_Longitude_Flag_W,
			.degree = 73,
			.minute = 59.328
		},
		.valid = POS_Valid_Flag_Valid
	}, {
		//test config(Melbourne, Australia)
		.time = {
			.hour = 12,
			.minute = 13,
			.second = 14,
			.split = 15
		},
		.latitude = {
			.direction = POS_Latitude_Flag_S,
			.degree = 37,
			.minute = 48.858
		},
		.longitude = {
			.direction = POS_Longitude_Flag_E,
			.degree = 144,
			.minute = 57.804
		},
		.valid = POS_Valid_Flag_Valid
	}, {
		//test config (should be neglected because of old timestamp)
		.time = {
			.hour = 10,
			.minute = 0,
			.second = 0,
			.split = 0
		},
		.latitude = {
			.direction = POS_Latitude_Flag_S,
			.degree = 0,
			.minute = 0.0
		},
		.longitude = {
			.direction = POS_Longitude_Flag_W,
			.degree = 0,
			.minute = 0.0
		},
		.valid = POS_Valid_Flag_Valid
	}, {
		//test config (should be neglected because invalid)
		.time = {
			.hour = 14,
			.minute = 0,
			.second = 0,
			.split = 0
		},
		.latitude = {
			.direction = POS_Latitude_Flag_S,
			.degree = 0,
			.minute = 0.0
		},
		.longitude = {
			.direction = POS_Longitude_Flag_W,
			.degree = 0,
			.minute = 0.0
		},
		.valid = POS_Valid_Flag_Invalid
	}, 
};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void _Error_Handler(char *file, int line);

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void) {
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	/* System interrupt init*/
	/* SVC_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SVC_IRQn, 0, 0);
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	HAL_Init();
	SystemClock_Config();
	LOG_Init();
	LOC_Init();
	EMC_Init();
	
	LOG("System initialized\n");

	uint32_t next = HAL_GetTick() + DELAY;
	uint8_t i = 0;

	while (1) {
		if (HAL_GetTick()  > next) {
			next = HAL_GetTick() + DELAY;
			LOC_InjectPosition(&positions[i++]);
		}

		LOC_Process();
		EMC_Process();
	}
}

/**
  * @brief  System Clock Configuration
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
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
	while(1) {}
}