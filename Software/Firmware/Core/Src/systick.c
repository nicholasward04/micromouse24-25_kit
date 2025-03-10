/* systick.c
 *
 *  File for the implementation of functions to be placed in the SysTick_Handler in stm32f1xx_it.c
 */

#include "systick.h"
#include "mm_commands.h"

extern uint8_t debugMode;
extern uint8_t debugCounter;

extern volatile uint16_t buzzerDelay;

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
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
	}
}
