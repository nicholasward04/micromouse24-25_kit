/* mm_systick.c
 *
 *  File for the implementation of functions to be placed in the SysTick_Handler in stm32f1xx_it.c
 */

#include <mm_systick.h>
#include "mm_commands.h"
#include "mm_vision.h"
#include "mm_supplemental.h"
#include "mm_motors.h"
#include "mm_encoders.h"
#include "mm_profiles.h"

extern uint8_t debugMode;
extern uint8_t debugCounter;
extern mouse_state_t mouse_state;
extern float SYSTICK_INTERVAL;

extern uint32_t global_time;

extern int32_t objective_L;
extern int32_t objective_R;

extern volatile uint16_t buzzerDelay;

extern profile_t forward_profile;
extern profile_t rotational_profile;

extern float steering_adjustment;

uint32_t time_last_updated_ms = 0;

void Systick() {
	global_time = HAL_GetTick();

	Debug_Mode();
	Buzzer_Check();

	if (global_time > time_last_updated_ms + (SYSTICK_INTERVAL * 1000)) {
		// Update distance traveled and angle turned
		Update_Encoders();
		// Update forward and rotational profile
		Update_Profile(&forward_profile);
		Update_Profile(&rotational_profile);
		// Update IR sensor readings and check for walls
		Poll_Sensors(&mouse_state);

		Wall_Front();
		Wall_Left();
		Wall_Right();

		Calculate_Error();
		// Update battery voltage
		mouse_state.battery_voltage = Read_Battery();
		// Update motor voltages
		Update_Motors(forward_profile.speed, rotational_profile.speed, steering_adjustment);

		time_last_updated_ms = global_time;
	}
}

void Debug_Mode() {
	if (debugMode) {
		debugCounter = (debugCounter + 1) % DEBUG_PERIOD;
		if (debugCounter == 0) {
			mouse_state.rpm.left_rpm = Calculate_RPM(objective_L, MOTOR_LEFT);
			mouse_state.rpm.right_rpm  = Calculate_RPM(objective_R, MOTOR_RIGHT);

			Debug_Packet_Send();
		}
	}
}

void Buzzer_Check() {
	if (buzzerDelay > 0) {
		buzzerDelay--;
	}
	else {
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
	}
}
