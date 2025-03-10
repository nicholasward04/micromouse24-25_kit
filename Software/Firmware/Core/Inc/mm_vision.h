/* mm_vision.h
 *
 * File for the implementation of infrared emit and receive functions using 12-bit on-board ADC
 */

#ifndef INC_MM_VISION_H_
#define INC_MM_VISION_H_

#include "main.h"

typedef enum { // Enumeration for IR cases
	DIST_FL,
	DIST_L,
	DIST_R,
	DIST_FR
} dist_t;

uint16_t Measure_Distance(dist_t dist);
void Poll_Sensors();

#endif /* INC_MM_VISION_H_ */
