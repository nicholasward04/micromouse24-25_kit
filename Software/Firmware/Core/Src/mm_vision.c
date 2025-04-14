/* mm_vision.c
 *
 * File for the implementation of infrared emit and receive functions using 12-bit on-board ADC
 */

#include "mm_vision.h"
#include "mm_systick.h"

extern ADC_HandleTypeDef hadc1;

extern mouse_state_t mouse_state;

extern uint16_t cal_FL;
extern uint16_t cal_L;
extern uint16_t cal_R;
extern uint16_t cal_FR;

// Calibration Values (determined before runtime)
uint16_t translation_FL = 0;
uint16_t translation_L = 0;
uint16_t translation_R = 0;
uint16_t translation_FR = 0;

// Wall Thresholds
uint16_t wall_front_thresh = 100;
uint16_t wall_left_thresh = 100;
uint16_t wall_right_thresh = 100;

bool wall_front = false;
bool wall_left = false;
bool wall_right = false;

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

void Calibrate_Readings(mouse_state_t* mouse_state) { // Calibrate raw IR values
	cal_FL = 200 / translation_FL;
	cal_L = 100 / translation_L;
	cal_R = 100 / translation_R;
	cal_FR = 200 / translation_FR;
}

void Poll_Sensors(mouse_state_t* mouse_state){ // Gather all raw IR values
	mouse_state->raw_FL = Measure_Dist(DIST_FL);
	mouse_state->raw_L = Measure_Dist(DIST_L);
	mouse_state->raw_R = Measure_Dist(DIST_R);
	mouse_state->raw_FR = Measure_Dist(DIST_FR);

	Calibrate_Readings(mouse_state);
}

bool Wall_Front() {
	Poll_Sensors(&mouse_state);
	uint16_t front_avg = (cal_FL + cal_FR) / 2;
	if (front_avg > wall_front_thresh) {
		wall_front = true;
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, 0);

	}
	else {
		wall_front = false;
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, 1);
	}
	return wall_front;
}

bool Wall_Left() {
	Poll_Sensors(&mouse_state);
	if (cal_L > wall_left_thresh) {
		wall_left = true;
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, 0);
	}
	else {
		wall_left = false;
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, 1);
	}
	return wall_left;
}

bool Wall_Right() {
	Poll_Sensors(&mouse_state);
	if (cal_R > wall_right_thresh) {
		wall_right = true;
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, 0);

	}
	else {
		wall_right = false;
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, 1);
	}
	return wall_right;
}
