/*
 * mm_profiles.c
 *
 * File for the implementation of profiles to manage acceleration and velocity
 */

#include "mm_profiles.h"

extern float SYSTICK_INTERVAL;

extern profile_t forward_profile;
extern profile_t rotational_profile;

extern float mouse_position;

float on_completion_error_forward = 0;
float on_completion_error_rotational = 0;

void Clear_Profile(profile_t* profile) {
	bzero(profile, sizeof(profile_t));
	profile->state = IDLE; // Should be handled above, just make sure it's IDLE
	profile->direction = 1;
}

void Start_Profile(param_t* parameters, profile_t* profile) {
	if (parameters->distance < 0) {
		profile->direction = -1;
		parameters->distance *= -1;
	}
	if (parameters->distance < 1.0) {
		profile->state = COMPLETE;
		return;
	}
	if (parameters->end_speed > parameters->max_speed) {
		parameters->end_speed = parameters->max_speed;
	}

	profile->position = 0;
	profile->parameters.distance += profile == &forward_profile ? on_completion_error_forward : 0;
	parameters->max_speed = profile->direction * fabsf(parameters->max_speed);
	parameters->end_speed = profile->direction * fabsf(parameters->end_speed);
	parameters->acceleration = fabsf(parameters->acceleration);
	memcpy(&(profile->parameters), parameters, sizeof(param_t));
	profile->inverse_acceleration = parameters->acceleration >= 1 ? (1.0f / parameters->acceleration) : 1.0;

	profile->state = ACCELERATING;
}

void Profile_Container(param_t* parameters, profile_t* profile) {
	float original_position = mouse_position;
	Start_Profile(parameters, profile);
	while (profile->state != COMPLETE) {
		HAL_Delay(2);
	}
	float delta_position = mouse_position - original_position;
	on_completion_error_forward = parameters->distance - delta_position;
}

float Calculate_Braking_Distance(float current_speed, float end_speed, float inverse_acceleration) {
	return fabsf(current_speed * current_speed - end_speed * end_speed) * 0.5 * inverse_acceleration;
}

void Update_Profile(profile_t* profile) {
	if (profile->state == IDLE) {
		return;
	}

	float delta_v = profile->parameters.acceleration * SYSTICK_INTERVAL;
	float remaining_distance = fabsf(profile->parameters.distance) - fabsf(profile->position);
	if (profile->state == ACCELERATING) {
		float braking_distance = Calculate_Braking_Distance(profile->speed, profile->parameters.end_speed, profile->inverse_acceleration);
		if (remaining_distance < braking_distance) {
			profile->state = BRAKING;
			if (profile->parameters.end_speed == 0) {
				profile->parameters.max_speed = profile->direction * 5.0f;
			}
			else {
				profile->parameters.max_speed = profile->parameters.end_speed;
			}
		}
	}

	if (profile->speed < profile->parameters.max_speed) {
		profile->speed += delta_v;
		if (profile->speed > profile->parameters.max_speed) {
			profile->speed = profile->parameters.max_speed;
		}
	}
	if (profile->speed > profile->parameters.max_speed) {
		profile->speed -= delta_v;
		if (profile->speed < profile->parameters.max_speed) {
			profile->speed = profile->parameters.max_speed;
		}
	}

	profile->position += profile->speed * SYSTICK_INTERVAL;
	if (profile->state != COMPLETE && remaining_distance < 0.125) {
		profile->state = COMPLETE;
		profile->parameters.max_speed = profile->parameters.end_speed;
	}
}
