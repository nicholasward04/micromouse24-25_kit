/* mm_systick.h
 *
 *  File for the implementation of functions to be placed in the SysTick_Handler in stm32f1xx_it.c
 */

#ifndef INC_MM_SYSTICK_H_
#define INC_MM_SYSTICK_H_

#define DEBUG_PERIOD 50

#include "main.h"

typedef struct {
	uint16_t front_left;
	uint16_t left;
	uint16_t right;
	uint16_t front_right;
} raw_ir_t;

typedef struct {
	uint16_t front_left;
	uint16_t left;
	uint16_t right;
	uint16_t front_right;
} cal_ir_t;

typedef struct {
	uint16_t left_rpm;
	uint16_t right_rpm;
} rpm_t;

typedef struct {
	uint8_t current_cell;
	uint8_t mouse_position[2];
	uint8_t mouse_direction;

	rpm_t rpm;

	raw_ir_t raw;
	cal_ir_t cal;

	double battery_voltage;
} mouse_state_t;

void Systick();
void Debug_Mode();
void Buzzer_Check();

#endif /* INC_MM_SYSTICK_H_ */
