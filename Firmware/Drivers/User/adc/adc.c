/*
 * adc.c
 *
 *  Created on: 22.05.2018
 *      Author: Michael
 */

#include "adc.h"

	ADC_HandleTypeDef hadc;
	ADC_ChannelConfTypeDef chconfig;


HAL_StatusTypeDef Adc_Init(uint8_t const channel)
{
	ADC_InitTypeDef adcinit;


	//clock init
	__HAL_RCC_ADC1_CLK_ENABLE();

	/* Initialize Adc configuration structure */
	 adcinit.OversamplingMode = DISABLE;
	 adcinit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	 adcinit.Resolution = ADC_RESOLUTION_12B;
	 adcinit.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	 adcinit.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	 adcinit.DataAlign = ADC_DATAALIGN_RIGHT;
	 adcinit.ContinuousConvMode = DISABLE;
	 adcinit.DiscontinuousConvMode = DISABLE;
	 adcinit.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	 adcinit.ExternalTrigConv = ADC_SOFTWARE_START;
	 adcinit.DMAContinuousRequests = DISABLE;
	 adcinit.EOCSelection = ADC_EOC_SINGLE_CONV;
	 adcinit.Overrun = ADC_OVR_DATA_PRESERVED;
	 adcinit.LowPowerAutoWait = DISABLE;
	 adcinit.LowPowerFrequencyMode = DISABLE;
	 adcinit.LowPowerAutoPowerOff = DISABLE;


	hadc.Init = adcinit;


	if(HAL_ADC_Init(&hadc) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return Adc_SetChannel(channel);
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

	chconfig.Channel = channel;
	chconfig.Rank =  ADC_RANK_CHANNEL_NUMBER;

	if (HAL_ADC_ConfigChannel(&hadc, &chconfig) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}
