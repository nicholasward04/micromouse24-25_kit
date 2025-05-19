/*
 * mm_profiles.h
 *
 * File for the implementation of profiles to manage acceleration and velocity
 */

#ifndef INC_MM_PROFILES_H_
#define INC_MM_PROFILES_H_

#include "main.h"

typedef struct {
	float distance; 					// mm             degrees
	float max_speed;					// mm/sec         degrees/sec
	float end_speed;					// mm/sec         degrees/sec
	float acceleration;					// mm/(sec^2)     degrees/(sec^2)
} param_t;

typedef enum {
	IDLE = 0,
	ACCELERATING = 1,
	BRAKING = 2,
	COMPLETE = 3
} state_t;

typedef struct {
	param_t parameters;
	state_t state;

	float speed;
	float position;
	float inverse_acceleration;
	int direction; // 1: forward -1: reverse
} profile_t;

void Clear_Profile(profile_t* profile);
void Start_Profile(param_t parameters, profile_t* profile);
void Profile_Container(param_t parameters, profile_t* profile);
void Smooth_Turn_Container(param_t fwd_parameters, param_t rot_parameters, profile_t* fwd_profile, profile_t* rot_profile);
float Calculate_Braking_Distance(float current_speed, float end_speed, float inverse_acceleration);
void Update_Profile(profile_t* profile);

#endif /* INC_MM_PROFILES_H_ */
