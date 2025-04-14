/*	mm_supplemental.h
 *
 *	File for implementation of basic functions
 */

#include "main.h"

#ifndef INC_MM_SUPPLEMENTAL_H_
#define INC_MM_SUPPLEMENTAL_H_

void LED_Power_Init();
void Reset_Buffers(uint8_t rxBuff, uint8_t txBuff);
void Pulse_Buzzer(uint16_t delay);
void LED_Red_Toggle();
void LED_Green_Toggle();
void LED_Blue_Toggle();

double Read_Battery();

void ARM_Button();
void RACE_Button();
void LOADMAZE_Button();

#endif
