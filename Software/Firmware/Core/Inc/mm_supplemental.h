/*	mm_supplemental.h
 *
 *	File for implementation of basic functions
 */

#include "main.h"

#ifndef INC_MM_SUPPLEMENTAL_H_
#define INC_MM_SUPPLEMENTAL_H_

typedef enum {
	SEARCHING = 0,
	RACING = 1,
	TEST = 2,
} mouse_mode_t;

typedef enum {
	FORWARD_DRIVE = 0,
	RIGHT_TURN = 1,
	LEFT_TURN = 2,
	ABOUT_FACE = 3,
	NONE = 4
} prev_action_t;

void LED_Power_Init();
void Reset_Buffers(uint8_t rxBuff, uint8_t txBuff);
void Pulse_Buzzer(uint16_t delay);
void LED_Red_Toggle();
void LED_Green_Toggle();
void LED_Blue_Toggle();

float Calculate_Battery_Bias(float commanded_voltage);
double Read_Battery();

void ARM_Button();
void RACE_Button();
void LOADMAZE_Button();

#endif
