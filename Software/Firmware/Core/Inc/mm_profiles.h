/*
 * mm_profiles.h
 *
 * File for the implementation of profiles to manage acceleration and velocity
 */

#ifndef INC_MM_PROFILES_H_
#define INC_MM_PROFILES_H_

#include "main.h"

const float MOUSE_BACK_TO_CENTER_MM = 32.467;

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

param_t SEARCH_BACK_TO_WALL_FWD = { .distance = 150 - MOUSE_BACK_TO_CENTER_MM,
								    .max_speed = 500,
								    .end_speed = 500,
								    .acceleration = 2500 };

param_t SEARCH_FWD = { .distance = 180,
					   .max_speed = 500,
					   .end_speed = 500,
					   .acceleration = 2500 };

param_t SEARCH_TURN_FWD = { .distance = 10,
					   .max_speed = 500,
					   .end_speed = 500,
					   .acceleration = 2500 };

param_t SEARCH_STOP_FWD = { .distance = 30,
						    .max_speed = 500,
						    .end_speed = 0,
						    .acceleration = 2500 };

param_t SEARCH_REVERSE_FWD = { .distance = -150,
								.max_speed = 500,
								.end_speed = 0,
								.acceleration = 2500 };

param_t SEARCH_ROT_RIGHT = { .distance = -90,
							 .max_speed = 500,
						     .end_speed = 500,
							 .acceleration = 2500 };

param_t SEARCH_ROT_LEFT = { .distance = 90,
							.max_speed = 500,
						    .end_speed = 500,
							.acceleration = 2500 };

param_t ROT_ABOUT = { .distance = 180,
					  .max_speed = 500,
					  .end_speed = 0,
					  .acceleration = 2500 };

void Clear_Profile(profile_t* profile);
void Start_Profile(param_t parameters, profile_t* profile);
void Profile_Container(param_t parameters, profile_t* profile);
void Smooth_Turn_Container(param_t fwd_parameters, param_t rot_parameters, profile_t* fwd_profile, profile_t* rot_profile);
float Calculate_Braking_Distance(float current_speed, float end_speed, float inverse_acceleration);
void Update_Profile(profile_t* profile);

#endif /* INC_MM_PROFILES_H_ */
