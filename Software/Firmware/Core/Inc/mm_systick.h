/* mm_systick.h
 *
 *  File for the implementation of functions to be placed in the SysTick_Handler in stm32f1xx_it.c
 */

#ifndef INC_MM_SYSTICK_H_
#define INC_MM_SYSTICK_H_

#define DEBUG_PERIOD 50

#include "main.h"

typedef struct {
	uint8_t current_cell;
	uint8_t mouse_position[2];
	uint8_t mouse_direction;

	uint16_t motor_L_RPM;
	uint16_t motor_R_RPM;

	uint16_t raw_FL;
	uint16_t raw_L;
	uint16_t raw_R;
	uint16_t raw_FR;

	int64_t encoder_count_L;
	int64_t encoder_count_R;

	double battery_voltage;
} mouse_state_t;

void Systick();
void Debug_Mode();
void Buzzer_Check();

#endif /* INC_MM_SYSTICK_H_ */
