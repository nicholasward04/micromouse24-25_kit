/* mm_encoders.c
 *
 *  File for the implementation of encoder count tracking
 */

#include "mm_encoders.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern uint32_t global_time;
extern int32_t objective_L;
extern int32_t objective_R;

const float GEAR_RATIO = 29.86f;
const float ENCODER_TICKS_PER_ROTATION = 12.0f;
const float WHEEL_DIAMETER = 28.32;
const float MOUSE_RADIUS = 39.08; // Distance from center point of motors to center point of wheels
const float ROTATION_BIAS = -0.05; // Start with 0, manual correction for inconsistent wheel diameters

const float TICKS_PER_ROTATION = ENCODER_TICKS_PER_ROTATION * GEAR_RATIO;
const uint16_t MS_PER_SEC = 1000;
const uint16_t SEC_PER_MIN = 60;

const float MM_PER_TICK = M_PI * WHEEL_DIAMETER / TICKS_PER_ROTATION;
const float MM_PER_TICK_LEFT = (1 - ROTATION_BIAS) * MM_PER_TICK;
const float MM_PER_TICK_RIGHT = (1 + ROTATION_BIAS) * MM_PER_TICK;
const float DEG_PER_MM_DIFFERENCE = (180.0 / (2 * MOUSE_RADIUS * M_PI));

uint16_t counter_L = 0;
uint16_t counter_R = 0;

float delta_position_forward = 0;
float delta_position_rotational = 0;

float mouse_angle = 0;
float mouse_position = 0;

uint32_t last_calculated_time[2] = { 0, 0 }; // { last calculated time left, last calculated time right }
uint32_t last_tick_count[2] = { 0, 0 };

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	uint16_t current_count = __HAL_TIM_GET_COUNTER(htim);
	if (htim == &htim4) { // Motor right
		objective_R -= (int16_t)(current_count - counter_R);
		counter_R = current_count;
	}
	if (htim == &htim3) { // Motor left
		objective_L -= (int16_t)(current_count - counter_L);
		counter_L = current_count;
	}
}

// LIKELY WILL HAVE TO CHANGE
uint16_t Calculate_RPM(int32_t current_ticks, uint8_t motor) {
	uint32_t time_difference = global_time - last_calculated_time[motor];
	last_calculated_time[motor] = global_time;

	int32_t tick_difference = abs(current_ticks - last_tick_count[motor]);
	last_tick_count[motor] = current_ticks;

	return ((MS_PER_SEC * SEC_PER_MIN * tick_difference) / (TICKS_PER_ROTATION * time_difference));
}

void Update_Encoders() {
	int32_t delta_tick_right = objective_R;
	int32_t delta_tick_left = objective_L;
	objective_R = 0;
	objective_L = 0;

	float delta_position_right = delta_tick_right * MM_PER_TICK_LEFT;
	float delta_position_left = delta_tick_left * MM_PER_TICK_RIGHT;
	delta_position_forward = (delta_position_right + delta_position_left) / 2; // average change in position
	mouse_position += delta_position_forward;
	delta_position_rotational = (delta_position_right - delta_position_left) * DEG_PER_MM_DIFFERENCE;
	mouse_angle += delta_position_rotational;
}
