/* mm_motors.c
 *
 * File for the implementation of motor control
 */

#include "mm_motors.h"
#include "mm_supplemental.h"
#include "mm_systick.h"
#include "mm_profiles.h"

//#define FEEDFORWARD_ENABLE

extern float delta_position_forward;
extern float delta_position_rotational;

extern profile_t rotational_profile;
extern bool motor_controller_enabled;

extern const float MOUSE_RADIUS;
extern mouse_state_t mouse_state;

const float RADIANS_PER_DEGREE = 2 * M_PI / 360.0;

const float MAX_MOTOR_VOLTAGE = 6.0;
const uint16_t MAX_PWM = 2047;
const uint16_t PWM_LIMIT = 0.72 * MAX_PWM;

const float SYSTICK_FREQUENCY = 500.0;
const float SYSTICK_INTERVAL = (1.0 / SYSTICK_FREQUENCY);

const float FWD_KM = 257.0; // slope mm/s/volt
const float FWD_TM = 0.1; // time constant

const float ROT_KM = FWD_KM * 1.3; // slope dg/s/volt 1.3
const float ROT_TM = 0.1; // time constant

const float FWD_ZETA = 0.707; // sqrt(1/2) smaller is more agressive, larger is slower
const float FWD_TD = FWD_TM;  // fwd_tm
const float FWD_KP = 16 * FWD_TM / (FWD_KM * FWD_ZETA * FWD_ZETA * FWD_TD * FWD_TD);
const float FWD_KD = SYSTICK_FREQUENCY * (8 * FWD_TM - FWD_TD) / (FWD_KM * FWD_TD);

const float ROT_ZETA = 0.707;
const float ROT_TD = ROT_TM;
const float ROT_KP = 16 * ROT_TM / (ROT_KM * ROT_ZETA * ROT_ZETA * ROT_TD * ROT_TD); // 16
const float ROT_KD = SYSTICK_FREQUENCY * (8 * ROT_TM - ROT_TD) / (ROT_KM * ROT_TD); // 8

float forward_error = 0;
float previous_forward_error = 0;
float rotational_error = 0;
float previous_rotational_error = 0;

float motor_left_previous_speed = 0;
float motor_right_previous_speed = 0;

bool motor_controller_enabled = false;

void Set_Motor_Volts(motor_t motor, float voltage_to_translate) {
	voltage_to_translate = voltage_to_translate > MAX_MOTOR_VOLTAGE ? MAX_MOTOR_VOLTAGE : voltage_to_translate;
	voltage_to_translate = voltage_to_translate < -MAX_MOTOR_VOLTAGE ? -MAX_MOTOR_VOLTAGE : voltage_to_translate;

	int counter_period = MAX_PWM * (voltage_to_translate + Calculate_Battery_Bias(voltage_to_translate)) / mouse_state.battery_voltage;
	if (counter_period < 0) {
		Set_Direction(motor, REVERSE);
		counter_period = -counter_period;
	}
	else if (counter_period > 0) {
		Set_Direction(motor, FORWARD);
	}

	Set_PWM(motor, counter_period);
}

void Set_PWM(motor_t motor, uint16_t counter_period) {
	// Software limit for motor voltage ~6V
	counter_period = counter_period > PWM_LIMIT ? PWM_LIMIT: counter_period;
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
				case BRAKE:
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
				case BRAKE:
					HAL_GPIO_WritePin(MR_FWD_GPIO_Port,  MR_FWD_Pin, 0);
					HAL_GPIO_WritePin(MR_BWD_GPIO_Port, MR_BWD_Pin, 0);
					break;
			}
			break;
	}
}

void Complete_Stop() {
	Set_Direction(MOTOR_LEFT, BRAKE);
	Set_Direction(MOTOR_RIGHT, BRAKE);

	Set_PWM(MOTOR_LEFT, 0);
	Set_PWM(MOTOR_RIGHT, 0);

	motor_controller_enabled = false;
}

float Position_Controller(float velocity) {
	float increment = velocity * SYSTICK_INTERVAL;
	forward_error += increment - delta_position_forward;
	float forward_error_difference = forward_error - previous_forward_error;
	previous_forward_error = forward_error;

	return FWD_KP * forward_error + FWD_KD * forward_error_difference;
}

float Rotational_Controller(float steering_adjustment, float omega) {
	float increment = omega * SYSTICK_INTERVAL;
	rotational_error += increment - delta_position_rotational;
	rotational_error += steering_adjustment;
	float rotational_error_difference = rotational_error - previous_rotational_error;
	previous_rotational_error = rotational_error;

	return ROT_KP * rotational_error + ROT_KD * rotational_error_difference;
}

void Update_Motors(float velocity, float omega, float steering_adjustment) {
	steering_adjustment = rotational_profile.state == IDLE || rotational_profile.state == COMPLETE ? steering_adjustment : 0;

	float position_output = Position_Controller(velocity);
	float rotational_output = Rotational_Controller(steering_adjustment, omega);

	float motor_left_voltage = 0;
	float motor_right_voltage = 0;
	motor_left_voltage = position_output - rotational_output;
	motor_right_voltage = position_output + rotational_output;

	if (motor_controller_enabled) {
		Set_Motor_Volts(MOTOR_LEFT, motor_left_voltage);
		Set_Motor_Volts(MOTOR_RIGHT, motor_right_voltage);
	}
}
