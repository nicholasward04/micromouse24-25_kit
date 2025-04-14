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

extern uint8_t debugMode;
extern uint8_t debugCounter;
extern mouse_state_t mouse_state;

extern uint32_t global_time;

extern int32_t objective_L;
extern int32_t objective_R;

extern volatile uint16_t buzzerDelay;

const uint8_t UPDATE_DELAY_MS = 100;
uint32_t prev_time = 0;

void Systick() {
	global_time = HAL_GetTick();

	if (global_time == prev_time + UPDATE_DELAY_MS) {
		Poll_Sensors(&mouse_state);
		mouse_state.battery_voltage = Read_Battery();
		mouse_state.motor_L_RPM = Calculate_RPM(objective_L, MOTOR_LEFT);
		mouse_state.motor_R_RPM = Calculate_RPM(objective_R, MOTOR_RIGHT);

		prev_time = global_time;
	}

	Debug_Mode();
	Buzzer_Check();
}

void Debug_Mode() {
	if (debugMode) {
		debugCounter = (debugCounter + 1) % DEBUG_PERIOD;
		if (debugCounter == 0) {
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
