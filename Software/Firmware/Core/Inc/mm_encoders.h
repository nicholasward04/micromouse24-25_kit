/* mm_encoders.h
 *
 *  File for the implementation of encoder count tracking
 */

#ifndef INC_MM_ENCODERS_H_
#define INC_MM_ENCODERS_H_

#include "main.h"

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
uint16_t Calculate_RPM(int32_t current_ticks, uint8_t motor);
void Update_Encoders();

#endif /* INC_MM_ENCODERS_H_ */
