/**
  **************************
  * @file    adc.h
  * @author  Michael Neuhofer
  * @version V1.0
  * @date    12.06.2018
  * @brief   adc driver module
  **************************
  */

#ifndef ADC_H
#define ADC_H


// if there are some problems with usage, first try to disable those two
#define ENABLE_AUTOPOWEROFF 	ENABLE;	//can be set to DISABLE to deset automatic poweroff/on at end/begin of conversation
#define ENABLE_AUTOPOWERWAIT	ENABLE; //can be set to DISABLE...


/**
 * @brief Initializes the adc using 12 bit resolution.
 * @param channel The Adc hardware channel that should be used in conversions.
 * @return HAL_OK if the given channel is valid else
 * 		   HAL_ERROR
 */
HAL_StatusTypeDef Adc_Init(uint8_t const channel);


/**
 * @brief Gets the adc value.
 * @param timeout Timeout to wait for the end of the conversion.
                  A value of 0 will cause infinite timeout.
 * @return The adc conversion value in mV or
 *         HAL_BUSY if the Adc is currently working or
 *         HAL_TIMEOUT if the conversion could not be completed
 *         within the given timeout.
 */
int32_t Adc_GetValue(uint32_t const timeout);


/**
 * @brief Selects the Adc hardware channel to use for conversion.
 * @param channel The Adc hardware channel that should be used in conversions.
 * for list of channels: stm32l0xx_hal_adc.h   @defgroup ADC_channels
 * @return HAL_OK if the given channel is valid else
 *         HAL_ERROR
 */
HAL_StatusTypeDef Adc_SetChannel(uint8_t const channel);


#endif //!ADC_H




