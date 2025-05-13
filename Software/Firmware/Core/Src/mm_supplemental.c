/*	mm_supplemental.c
 *
 *	File for implementation of basic functions
 */

#include "mm_supplemental.h"

extern ADC_HandleTypeDef hadc2;
extern bool searching;
extern bool armed;

volatile uint16_t buzzerDelay = 0;

const float BATTERY_BIAS_SLOPE = 0.0757;
const float BATTERY_BIAS_OFFSET = 0.0229;
const float BATTERY_RATIO = 0.913;

const float DIVIDER_RESISTOR_1 = 20000.0;
const float DIVIDER_RESISTOR_2 = 10000.0;
const float DIVIDER_RATIO = (DIVIDER_RESISTOR_2) / (DIVIDER_RESISTOR_1 + DIVIDER_RESISTOR_2);
const float ADC_MAX_READING = 4096.0;
const float ADC_REFERENCE_V = 3.3;

const float BATTERY_MULTIPLIER = (ADC_REFERENCE_V / ADC_MAX_READING / DIVIDER_RATIO);

void LED_Power_Init() {
	HAL_GPIO_TogglePin(LED_Power_GPIO_Port, LED_Power_Pin);
}

void Reset_Buffers(uint8_t rxBuff, uint8_t txBuff) {
	rxBuff = 0;
	txBuff = 0;
}

void Pulse_Buzzer(uint16_t delay) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
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

float Calculate_Battery_Bias(float commanded_voltage) {
	return (BATTERY_BIAS_SLOPE * commanded_voltage + BATTERY_BIAS_OFFSET) / BATTERY_RATIO;
}

double Read_Battery() {
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	uint16_t raw_adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);

	return raw_adc * BATTERY_MULTIPLIER + 1.0;
}

void ARM_Button() {
	for (uint8_t i=0; i < 10; i++) {
		LED_Red_Toggle();
		HAL_Delay(500);
	}
	armed = true;
}

void RACE_Button() {
	searching = false;
	LED_Blue_Toggle();
	HAL_Delay(1000);
}

void LOADMAZE_Button() {
	// TODO: Implement load from flash
}
