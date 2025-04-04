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
extern int32_t prev_obj_L;
extern int32_t prev_obj_R;

const uint16_t TICKS_PER_ROTATION = 360;
const uint16_t MS_PER_SEC = 1000;
const uint16_t SEC_PER_MIN = 60;

uint16_t counter_L = 0;
uint16_t counter_R = 0;

uint32_t last_calculated_time[2] = { 0, 0 }; // { last calculated time left, last calculated time right }
uint32_t last_tick_count[2] = { 0, 0 };

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	uint16_t current_count = __HAL_TIM_GET_COUNTER(htim);
	if (htim == &htim4) { // Motor right
		prev_obj_R = objective_R;
		objective_R += (int16_t)(current_count - counter_R);
		counter_R = current_count;
	}
	if (htim == &htim3) { // Motor left
		prev_obj_L = objective_L;
		objective_L += (int16_t)(current_count - counter_L);
		counter_L = current_count;
	}
}

uint16_t Calculate_RPM(int32_t current_ticks, uint8_t motor) {
	uint32_t time_difference = global_time - last_calculated_time[motor];
	last_calculated_time[motor] = global_time;

	int32_t tick_difference = abs(current_ticks - last_tick_count[motor]);
	last_tick_count[motor] = current_ticks;

	return ((MS_PER_SEC * SEC_PER_MIN * tick_difference) / (TICKS_PER_ROTATION * time_difference));
}
