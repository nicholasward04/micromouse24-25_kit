/* mm_vision.c
 *
 * File for the implementation of infrared emit and receive functions using 12-bit on-board ADC
 */

#include "mm_vision.h"
#include "mm_systick.h"
#include "mm_supplemental.h"
#include "mm_profiles.h"

extern ADC_HandleTypeDef hadc1;
extern profile_t rotational_profile;

extern mouse_state_t mouse_state;

extern float SYSTICK_FREQUENCY;

const uint8_t FRONT_WALL_LIMIT = 100; // If front wall too close, disable IR correction due to increase in reflection
const float STEERING_ADJUSTMENT_LIMIT = 5.0f; // In degrees

const float IR_KP = 0.002f;

// Translation Values (determined before runtime)
uint16_t translation_FL = 143;
uint16_t translation_L = 787;
uint16_t translation_R = 606;
uint16_t translation_FR = 134;

// Wall Thresholds
uint16_t wall_front_thresh = 60;
uint16_t wall_left_thresh = 20;
uint16_t wall_right_thresh = 20;

bool wall_front = false;
bool wall_left = false;
bool wall_right = false;

bool adjust_steering = false;
float steering_adjustment = 0;

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
		ADC1_Select_CH4();
		break;
	case DIST_L:
		emitter_port = EMIT_L_GPIO_Port;
		emitter_pin = EMIT_L_Pin;
		ADC1_Select_CH5();
		break;
	case DIST_R:
		emitter_port = EMIT_R_GPIO_Port;
		emitter_pin = EMIT_R_Pin;
		ADC1_Select_CH8();
		break;
	case DIST_FR:
		emitter_port = EMIT_FR_GPIO_Port;
		emitter_pin = EMIT_FR_Pin;
		ADC1_Select_CH9();
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

void Calibrate_Readings(mouse_state_t* mouse_state) { // Normalize raw IR values
	mouse_state->cal.front_left  = 200 * ((float)mouse_state->raw.front_left / (float)translation_FL);
	mouse_state->cal.left        = 100 * ((float)mouse_state->raw.left / (float)translation_L);
	mouse_state->cal.right       = 100 * ((float)mouse_state->raw.right / (float)translation_R);
	mouse_state->cal.front_right = 200 * ((float)mouse_state->raw.front_right / (float)translation_FR);
}

void Poll_Sensors(mouse_state_t* mouse_state){ // Gather all raw IR values
	mouse_state->raw.front_left = Measure_Dist(DIST_FL);
	mouse_state->raw.left = Measure_Dist(DIST_L);
	mouse_state->raw.right = Measure_Dist(DIST_R);
	mouse_state->raw.front_right = Measure_Dist(DIST_FR);

	Calibrate_Readings(mouse_state);
}

bool Wall_Front() {
	Poll_Sensors(&mouse_state);
	uint16_t front_avg = (mouse_state.cal.front_left + mouse_state.cal.front_right) / 2;
	if (front_avg > wall_front_thresh) {
		wall_front = true;
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);

	}
	else {
		wall_front = false;
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
	}
	return wall_front;
}

bool Wall_Left() {
	Poll_Sensors(&mouse_state);
	if (mouse_state.cal.left > wall_left_thresh) {
		wall_left = true;
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);
	}
	else {
		wall_left = false;
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);
	}
	return wall_left;
}

bool Wall_Right() {
	Poll_Sensors(&mouse_state);
	if (mouse_state.cal.right > wall_right_thresh) {
		wall_right = true;
		HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, GPIO_PIN_RESET);

	}
	else {
		wall_right = false;
		HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, GPIO_PIN_SET);
	}
	return wall_right;
}

void Calculate_Steering_Adjustment(int error) {
	float adjustment = IR_KP * error;

	adjustment = adjustment > STEERING_ADJUSTMENT_LIMIT ? STEERING_ADJUSTMENT_LIMIT: adjustment;
	adjustment = adjustment < -STEERING_ADJUSTMENT_LIMIT ? -STEERING_ADJUSTMENT_LIMIT: adjustment;

	steering_adjustment = rotational_profile.state == COMPLETE || rotational_profile.state == IDLE ? adjustment : 0;
}

void Calculate_Error() {
	int error = 0;
	int right_error = 100 - mouse_state.cal.right;
	int left_error = 100 - mouse_state.cal.left;

	if (adjust_steering) {
		if (wall_left && wall_right) {
			error = right_error - left_error;
		}
		else if (wall_left) {
			error = -2 * left_error;
		}
		else if (wall_right) {
			error = 2 * right_error;
		}
	}

	Calculate_Steering_Adjustment(error);
}
