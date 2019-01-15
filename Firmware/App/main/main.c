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
#include "usb.h"
#include "log.h"
#include "location.h"
#include "emergencyCall.h"
#include "sysclock_driver.h"

/* Private variables ---------------------------------------------------------*/
#define POS_CNT 7
#define DELAY 2000

POS_Position positions[POS_CNT] = {
	{
		//empty config
		.time = {
			.hour = 1,
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
	
	HAL_Delay(1000);
	
	LOC_Init();
	LOC_InjectPosition(&(positions[2]));

	EMC_Init();
	EMC_SetEmergency(EMC_State_Emergency);

	LOG("System initialized\n");

	uint32_t next = HAL_GetTick() + DELAY;
	uint8_t i = 0;

	while (1) {
		//if (HAL_GetTick()  > next && i < POS_CNT) {
		//	next = HAL_GetTick() + DELAY;
		//	LOG("[MAIN] Next Position: %u Tick: %lu\n", i, HAL_GetTick());
		//	//LOC_InjectPosition(&positions[i++]);
		//}

		//LOC_Process();
		EMC_Process();
	}
}