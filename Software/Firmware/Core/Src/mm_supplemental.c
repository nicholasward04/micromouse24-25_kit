/*	mm_supplemental.c
 *
 *	File for implementation of basic functions
 */

#include "mm_supplemental.h"

volatile uint16_t buzzerDelay = 0;

extern ADC_HandleTypeDef hadc2;

void LED_Power_Init() {
	HAL_GPIO_TogglePin(LED_Power_GPIO_Port, LED_Power_Pin);
}

void Reset_Buffers(uint8_t rxBuff, uint8_t txBuff) {
	rxBuff = 0;
	txBuff = 0;
}

void Pulse_Buzzer(uint16_t delay) {
	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
	buzzerDelay = delay;
}

void LED_Red_Toggle() {
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
}

void LED_Green_Toggle() {
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
}

void LED_Blue_Toggle() {
	HAL_GPIO_TogglePin(LED_Blue_GPIO_Port, LED_Blue_Pin);
}

double Read_Battery() {
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	uint16_t raw_adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);

	return ((raw_adc / 4095.0) * 3.3) * 3.5;
}

