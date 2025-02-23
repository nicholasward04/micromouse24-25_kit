/*	mm_supplemental.h
 *
 *	File for implementation of basic functions
 */

#include "main.h"
#include "mm_supplemental.h"

void LED_Power_Init() {
	HAL_GPIO_TogglePin(LED_Power_GPIO_Port, LED_Power_Pin);
}

void Reset_Buffers(uint8_t rxBuff, uint8_t *txBuff) {
	bzero(&rxBuff, sizeof(rxBuff));
	bzero (txBuff, sizeof(txBuff));
}

