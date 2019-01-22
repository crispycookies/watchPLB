/*
 * adc.c
 *
 *  Created on: 22.05.2018
 *      Author: Michael
 */

#include "adc.h"

	ADC_HandleTypeDef hadc;


HAL_StatusTypeDef Adc_Init()
{

	GPIO_InitTypeDef GPIO_InitStruct;

	//clock init
	__HAL_RCC_ADC1_CLK_ENABLE();

    /**ADC GPIO Configuration
    PA1     ------> ADC_IN1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



	ADC_ChannelConfTypeDef sConfig;

	    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	    */
	  hadc.Instance = ADC1;

	  hadc.Init.OversamplingMode = DISABLE;
	  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	  hadc.Init.Resolution = ADC_RESOLUTION_12B;
	  hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  hadc.Init.ContinuousConvMode = DISABLE;
	  hadc.Init.DiscontinuousConvMode = DISABLE;
	  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  hadc.Init.DMAContinuousRequests = DISABLE;
	  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	  hadc.Init.LowPowerAutoWait = DISABLE;
	  hadc.Init.LowPowerFrequencyMode = DISABLE;
	  hadc.Init.LowPowerAutoPowerOff = DISABLE;
	  if (HAL_ADC_Init(&hadc) != HAL_OK)
	  {
		  return HAL_ERROR;
	  }

	    /**Configure for the selected ADC regular channel to be converted.
	    */
	  sConfig.Channel = ADC_CHANNEL_1;
	  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	  {
		  return HAL_ERROR;
	  }

	//return Adc_SetChannel(channel);
	return HAL_OK;
}


// gives you the value of the adc
int32_t Adc_GetValue(uint32_t const timeout)
{
	uint32_t adcvalue = 0;

   /*Start the conversion process*/
   if (HAL_ADC_Start(&hadc) != HAL_OK)
   {
	  //return HAL_ERROR;
   }

	if (HAL_ADC_PollForConversion(&hadc, timeout) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	else
	{
		adcvalue = HAL_ADC_GetValue(&hadc);
	}
	return adcvalue;
}

// change/ set the hardware channel used for conversion
HAL_StatusTypeDef Adc_SetChannel(uint8_t const channel)
{
	/*chconfig.Channel = channel;
	chconfig.Rank =  ADC_RANK_CHANNEL_NUMBER;

	if (HAL_ADC_ConfigChannel(&hadc, &chconfig) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;*/
}
