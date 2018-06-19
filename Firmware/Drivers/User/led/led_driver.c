/**
  ******************************************************************************
  * @file    led_driver.c
  * @author  Tobias Egger
  * @version V1.1
  * @date    12-June-2018
  * @brief   WatchPLB LED- Driver Implementation File
  ******************************************************************************
  */

/* Include Block*/
#include "led_driver.h"
#include <assert.h>

/*Defines*/
/*Timer Defines*/
#define clkdivider 10000

/*Pin Defines*/
#define PA4 GPIO_PIN_4
#define PA5 GPIO_PIN_5
#define PA6 GPIO_PIN_6
#define PA7 GPIO_PIN_7
#define PC4 GPIO_PIN_4
#define PC5 GPIO_PIN_5
#define PB0 GPIO_PIN_0
#define PB1 GPIO_PIN_1
#define PB2 GPIO_PIN_2
#define PB10 GPIO_PIN_10
#define PB11 GPIO_PIN_11

/*Defines for LUT and REGSITERs*/
#define LUT_SIZE 11
#define REG_SIZE LUT_SIZE

/* Global Init Variables for LED*/
GPIO_InitTypeDef Led_Lights;

typedef struct {
	uint16_t real_pin;
	LED_PIN pseudo_pin;
} LUT;


LUT look_up[LUT_SIZE] = { { PA4, led_pa4 },		//1
		{ PA5, led_pa5 },						//2
		{ PA6, led_pa6 },						//3
		{ PA7, led_pa7 },						//4
		{ PC4, led_pc4 },						//5
		{ PC5, led_pc5 },						//6
		{ PB0, led_pb0 },						//7
		{ PB1, led_pb1 },						//8
		{ PB2, led_pb2 },						//9
		{ PB10, led_pb10 },						//10
		{ PB11, led_pb11 },						//11
		};

/* Led Init */

/**
  * @brief Initialize GPIOs for LEDs
  * @param  None
  * @retval None
*/
void led_init(void) {
	/* Clock Block*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*General Init*/
	Led_Lights.Mode = GPIO_MODE_OUTPUT_PP;
	Led_Lights.Pull = GPIO_PULLDOWN;
	Led_Lights.Speed = GPIO_SPEED_FREQ_LOW;

	/*Init GPIOA*/
	Led_Lights.Pin |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	HAL_GPIO_Init(GPIOA, &Led_Lights);

	/*Init GPIOC*/
	Led_Lights.Pin &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	Led_Lights.Pin |= (GPIO_PIN_4 | GPIO_PIN_5);
	HAL_GPIO_Init(GPIOC, &Led_Lights);

	/*Init GPIOB*/
	Led_Lights.Pin &= ~(GPIO_PIN_4 | GPIO_PIN_5);
	Led_Lights.Pin |= (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10
			| GPIO_PIN_11);
	HAL_GPIO_Init(GPIOC, &Led_Lights);
}

/**
  * @brief Turn on LED
  * @param  led: The LED to be turned on
  * @retval true if succeeded and false if not
*/
bool led_on(LED_PIN led) {
	if (led < led_pc4) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_WritePin(GPIOA, look_up[i].real_pin, GPIO_PIN_SET);
				break;
			}
		}
	} else if (led < led_pb0) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_WritePin(GPIOC, look_up[i].real_pin, GPIO_PIN_SET);
				break;
			}
		}
	} else if (led <= led_pb11) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_WritePin(GPIOB, look_up[i].real_pin, GPIO_PIN_SET);
				break;
			}
		}
	} else {
		return false;
	}
	return true;
}

/**
  * @brief Turn off LED
  * @param  led: The LED to be turned off
  * @retval true if succeeded and false if not
*/
bool led_off(LED_PIN led) {
	if (led < led_pc4) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_WritePin(GPIOA, look_up[i].real_pin, GPIO_PIN_RESET);
				break;
			}
		}
	} else if (led < led_pb0) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_WritePin(GPIOC, look_up[i].real_pin, GPIO_PIN_RESET);
				break;
			}
		}
	} else if (led <= led_pb11) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_WritePin(GPIOB, look_up[i].real_pin, GPIO_PIN_RESET);
				break;
			}
		}
		if (i == (LUT_SIZE - 1)) {
			return false;											// Not Found
		}
	} else {
		return false;
	}
	return true;
}

/**
  * @brief Toggle LED
  * @param  led: The LED to be toggled
  * @retval true if succeeded and false if not
*/
bool led_toggle(LED_PIN led) {
	if (led < led_pc4) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_TogglePin(GPIOA, look_up[i].real_pin);
				break;
			}
		}
	} else if (led < led_pb0) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_TogglePin(GPIOC, look_up[i].real_pin);
				break;
			}
		}
	} else if (led <= led_pb11) {
		int i = 0;
		for (; i < LUT_SIZE; i++) {
			if (look_up[i].pseudo_pin == led) {
				HAL_GPIO_TogglePin(GPIOB, look_up[i].real_pin);
				break;
			}
		}
		if (i == (LUT_SIZE - 1)) {
			return false;											// Not Found
		}
	} else {
		return false;
	}
	return true;
}

/**
  * @brief Deinitializes GPIOs and turns of Clocks
  * @warning Only use this Function if you are sure about the Consequences
  * @param  none
  * @retval none
*/
void led_deinit(void) {

#warning "Do not Call if you are unsure"

	/*Init GPIOA*/
	HAL_GPIO_DeInit(GPIOA, (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7));

	/*Init GPIOC*/
	HAL_GPIO_DeInit(GPIOC, (GPIO_PIN_4 | GPIO_PIN_5));

	/*Init GPIOB*/
	HAL_GPIO_DeInit(GPIOB,
			(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10 | GPIO_PIN_11));

	/* Clock Block*/
	//__HAL_RCC_GPIOA_CLK_DISABLE();
	//__HAL_RCC_GPIOB_CLK_DISABLE();
	//__HAL_RCC_GPIOC_CLK_DISABLE();
}

/* Timer*/

/* Global Init Variables for Timer*/

TIM_Base_InitTypeDef led_tim7_base;
TIM_HandleTypeDef led_tim7_hdl;

/* Global Init Variables for Timer*/
REGISTER tim_shift_reg[REG_SIZE] = {0};

/**
  * @brief Interrupt Handler for Timer 7
  * @brief Shifts Registers and Enables/Disables LED
  * @param  none
  * @retval none
*/
void TIM7_IRQHandler(){
	/* Shifter */
	static const REGISTER n_bit= 1<<(sizeof(REGISTER)*8-1);
	for(int i = 0; i < REG_SIZE; i++){
		if((tim_shift_reg[i] & 1)){
			led_on(i);
			tim_shift_reg[i]=n_bit |tim_shift_reg[i]>>1;

		}else{
			led_off(i);
			tim_shift_reg[i]= tim_shift_reg[i]>>1;
		}

	}
	TIM7->SR = 0;
}

/**
  * @brief Initializes the Timer and sets the Interval in which the Timer throws an interrupt
  * @brief led_init must be called first
  * @param  time_intervall: The LED to be turned on
  * @retval none
*/
void led_timer_init(TIME time_intervall) {
	__HAL_RCC_TIM7_CLK_ENABLE();

	SystemCoreClockUpdate();
	const double factor = 3;

	led_tim7_base.Prescaler = SystemCoreClock/(clkdivider)*4;
	led_tim7_base.CounterMode = TIM_COUNTERMODE_UP;
	led_tim7_base.Period = time_intervall*factor;
	led_tim7_base.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	led_tim7_hdl.Init = led_tim7_base;
	led_tim7_hdl.Instance = TIM7;

	HAL_TIM_Base_Init(&led_tim7_hdl);
}

/**
  * @brief Starts the Timer and its Interrupt
  * @param  none
  * @retval true if succeeded and false if not
*/
bool led_timer_start(void) {
	NVIC_EnableIRQ(TIM7_IRQn);
	return HAL_TIM_Base_Start_IT(&led_tim7_hdl);
}

/**
  * @brief Stops the Timer and its Interrupt
  * @param  none
  * @retval true if succeeded and false if not
*/
bool led_timer_stop(void) {
	NVIC_DisableIRQ(TIM7_IRQn);
	return HAL_TIM_Base_Stop_IT(&led_tim7_hdl);
}

/**
  * @brief Stops the Timer and its Interrupt
  * @param led: the LED for which the REGISTER value has to be set
  * @param value: the Register Value. Interrupt shifts this Value by 1 and checks if Bit is set. If Set, it enables LED, otherwise it disables.
  * @retval true if succeeded and false if not
*/
void led_action_time(LED_PIN led, REGISTER value ) {
	tim_shift_reg[led]  = value;
}
