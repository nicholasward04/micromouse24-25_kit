/* mm_motors.c
 *
 * File for the implementation of motor control
 */

#ifndef INC_MM_MOTORS_H_
#define INC_MM_MOTORS_H_

#include "main.h"

typedef enum {
	MOTOR_LEFT = 0,
	MOTOR_RIGHT = 1
} motor_t;

typedef enum {
	FORWARD = 0,
	REVERSE = 1,
	BREAK = 2
} motor_direction_t;

void Set_PWM(motor_t motor, uint16_t counter_period);
void Set_Direction(motor_t motor, motor_direction_t direction);

#endif /* INC_MM_MOTORS_H_ */
