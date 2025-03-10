/* systick.h
 *
 *  File for the implementation of functions to be placed in the SysTick_Handler in stm32f1xx_it.c
 */

#ifndef INC_SYSTICK_H_
#define INC_SYSTICK_H_

#define DEBUG_PERIOD 50

void Debug_Mode();
void Buzzer_Check();

#endif /* INC_SYSTICK_H_ */
