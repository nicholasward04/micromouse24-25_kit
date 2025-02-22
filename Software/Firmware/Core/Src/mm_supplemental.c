/*	mm_supplemental.h
 *
 *	File for implementation of basic functions
 */

#include "mm_supplemental.h"
#include "main.h"

void LED_Power_Init() {
	HAL_GPIO_TogglePin(LED_Power_GPIO_Port, LED_Power_Pin);
}


