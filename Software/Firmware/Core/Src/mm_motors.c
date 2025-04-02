/* mm_motors.c
 *
 * File for the implementation of motor control
 */

#include "mm_motors.h"

void Set_PWM(motor_t motor, uint16_t counter_period) {
	// Software limit for motor voltage ~6V
	counter_period = counter_period > 1450 ? 1450: counter_period;
	switch(motor) {
		case MOTOR_LEFT:
			TIM2->CCR4 = counter_period;
			break;
		case MOTOR_RIGHT:
			TIM2->CCR3 = counter_period;
			break;
	}
}

void Set_Direction(motor_t motor, motor_direction_t direction) {
	switch(motor) {
		case MOTOR_LEFT:
			switch(direction) {
				case FORWARD:
					HAL_GPIO_WritePin(ML_FWD_GPIO_Port,  ML_FWD_Pin, 1);
					HAL_GPIO_WritePin(ML_BWD_GPIO_Port, ML_BWD_Pin, 0);
					break;
				case REVERSE:
					HAL_GPIO_WritePin(ML_FWD_GPIO_Port,  ML_FWD_Pin, 0);
					HAL_GPIO_WritePin(ML_BWD_GPIO_Port, ML_BWD_Pin, 1);
					break;
				case BREAK:
					HAL_GPIO_WritePin(ML_FWD_GPIO_Port,  ML_FWD_Pin, 0);
					HAL_GPIO_WritePin(ML_BWD_GPIO_Port, ML_BWD_Pin, 0);
					break;
			}
			break;
		case MOTOR_RIGHT:
			switch(direction) {
				case FORWARD:
					HAL_GPIO_WritePin(MR_FWD_GPIO_Port,  MR_FWD_Pin, 1);
					HAL_GPIO_WritePin(MR_BWD_GPIO_Port, MR_BWD_Pin, 0);
					break;
				case REVERSE:
					HAL_GPIO_WritePin(MR_FWD_GPIO_Port,  MR_FWD_Pin, 0);
					HAL_GPIO_WritePin(MR_BWD_GPIO_Port, MR_BWD_Pin, 1);
					break;
				case BREAK:
					HAL_GPIO_WritePin(MR_FWD_GPIO_Port,  MR_FWD_Pin, 0);
					HAL_GPIO_WritePin(MR_BWD_GPIO_Port, MR_BWD_Pin, 0);
					break;
			}
			break;
	}
}
