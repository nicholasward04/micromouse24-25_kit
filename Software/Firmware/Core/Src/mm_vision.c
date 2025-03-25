/* mm_vision.c
 *
 * File for the implementation of infrared emit and receive functions using 12-bit on-board ADC
 */

#include "mm_vision.h"

extern ADC_HandleTypeDef hadc1;

extern uint16_t raw_FL;
extern uint16_t raw_L;
extern uint16_t raw_R;
extern uint16_t raw_FR;

static void ADC1_Select_CH9(void) {
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_9;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	if (HAL_ADC_ConfigChannel (&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

static void ADC1_Select_CH8(void) {
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

static void ADC1_Select_CH5(void) {
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

static void ADC1_Select_CH4(void) {
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	if (HAL_ADC_ConfigChannel (&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

uint16_t Measure_Dist(dist_t dist) { // Poll raw IR sensors
	GPIO_TypeDef* emitter_port;
	uint16_t emitter_pin;

	switch(dist) {
	case DIST_FL:
		emitter_port = EMIT_FL_GPIO_Port;
		emitter_pin = EMIT_FL_Pin;
		ADC1_Select_CH9();
		break;
	case DIST_L:
		emitter_port = EMIT_L_GPIO_Port;
		emitter_pin = EMIT_L_Pin;
		ADC1_Select_CH8();
		break;
	case DIST_R:
		emitter_port = EMIT_R_GPIO_Port;
		emitter_pin = EMIT_R_Pin;
		ADC1_Select_CH5();
		break;
	case DIST_FR:
		emitter_port = EMIT_FR_GPIO_Port;
		emitter_pin = EMIT_FR_Pin;
		ADC1_Select_CH4();
		break;
	default:
		break;
	}
	HAL_GPIO_WritePin(emitter_port, emitter_pin, GPIO_PIN_SET);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	uint16_t adc_val = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	HAL_GPIO_WritePin(emitter_port, emitter_pin, GPIO_PIN_RESET);

	return adc_val;
}

void Poll_Sensors(){ // Gather all raw IR values
	raw_FL = Measure_Dist(DIST_FL);
	raw_L = Measure_Dist(DIST_L);
	raw_R = Measure_Dist(DIST_R);
	raw_FR = Measure_Dist(DIST_FR);
}
