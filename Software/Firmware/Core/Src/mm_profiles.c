/*
 * mm_profiles.c
 *
 * File for the implementation of profiles to manage acceleration and velocity
 */

#include "mm_profiles.h"

extern float SYSTICK_INTERVAL;

extern profile_t forward_profile;
extern profile_t rotational_profile;

extern bool adjust_steering;
extern float STEERING_ADJUSTMENT_LIMIT;

extern float mouse_position;
extern float mouse_angle;

float on_completion_error_forward = 0;
float on_completion_error_rotational = 0;

extern uint16_t wall_front_thresh;
extern uint16_t wall_left_thresh;
extern uint16_t wall_right_thresh;

void Clear_Profile(profile_t* profile) {
	bzero(profile, sizeof(profile_t));
	profile->state = IDLE; // Should be handled above, just make sure it's IDLE
	profile->direction = 1;
}

void Start_Profile(param_t parameters, profile_t* profile) {
	if (parameters.distance < 0) {
		profile->direction = -1;
		parameters.distance *= -1;
	}
	if (parameters.distance < 1.0) {
		profile->state = COMPLETE;
		return;
	}
	if (parameters.end_speed > parameters.max_speed) {
		parameters.end_speed = parameters.max_speed;
	}

	profile->position = 0;
	profile->parameters.distance += profile == &forward_profile ? on_completion_error_forward : 0;
	parameters.max_speed = profile->direction * fabsf(parameters.max_speed);
	parameters.end_speed = profile->direction * fabsf(parameters.end_speed);
	parameters.acceleration = fabsf(parameters.acceleration);
	memcpy(&(profile->parameters), &parameters, sizeof(param_t));
	profile->inverse_acceleration = parameters.acceleration >= 1 ? (1.0f / parameters.acceleration) : 1.0;

	profile->state = ACCELERATING;
}

void Profile_Container(param_t parameters, profile_t* profile) {
	float original_position = mouse_position;
	Start_Profile(parameters, profile);
	while (profile->state != COMPLETE);
	float delta_position = mouse_position - original_position;
	on_completion_error_forward = parameters.distance - delta_position;
}

void Turn_Container(param_t fwd_parameters, param_t rot_parameters, profile_t* fwd_profile, profile_t* rot_profile) {
    Profile_Container(fwd_parameters, fwd_profile);
    adjust_steering = false;
    STEERING_ADJUSTMENT_LIMIT = 0.1;
    Clear_Profile(fwd_profile);
    Clear_Profile(rot_profile);
    Profile_Container(rot_parameters, rot_profile);
    Clear_Profile(rot_profile);
    fwd_parameters.end_speed = fwd_parameters.max_speed;
    adjust_steering = true;
    Profile_Container(fwd_parameters, fwd_profile);
    STEERING_ADJUSTMENT_LIMIT = 0.5;
}

void Smooth_Turn_Container(param_t fwd_parameters, param_t rot_parameters, profile_t* fwd_profile, profile_t* rot_profile) {
	adjust_steering = false;
	rot_parameters.acceleration *= 2;
	Clear_Profile(rot_profile);
	Start_Profile(fwd_parameters, fwd_profile);
	Start_Profile(rot_parameters, rot_profile);
	while (rot_profile->state != COMPLETE);
	Clear_Profile(rot_profile);
	adjust_steering = true;
	fwd_parameters.distance = 15;
	Profile_Container(fwd_parameters, fwd_profile);
}

void About_Face_Container(param_t fwd_parameters, param_t rev_parameters, param_t rot_parameters, profile_t* fwd_profile, profile_t* rot_profile, bool wall_realignment) {
	if (wall_realignment) { // Back up into existing wall
		Profile_Container(fwd_parameters, fwd_profile);
		adjust_steering = false;
		Profile_Container(rot_parameters, rot_profile);
		Clear_Profile(rot_profile);
		Profile_Container(rev_parameters, fwd_profile);
		fwd_profile->direction *= -1;
		fwd_profile->position = 0;
		HAL_Delay(500);
		adjust_steering = true;
	}
	else { // Turn in place
		adjust_steering = false;
		Profile_Container(rot_parameters, rot_profile);
		Clear_Profile(rot_profile);
		HAL_Delay(500);
		adjust_steering = true;
	}
}

param_t Parameter_Packer(float distance, float max_speed, float end_speed, float acceleration, bool forward) {
	param_t return_parameters = {
			.distance = distance,
			.max_speed = max_speed,
			.end_speed = end_speed,
			.acceleration = acceleration,
			.forward = forward
	};

	return return_parameters;
}

float Calculate_Braking_Distance(float current_speed, float end_speed, float inverse_acceleration) {
	return fabsf(current_speed * current_speed - end_speed * end_speed) * 0.5 * inverse_acceleration;
}

void Update_Profile(profile_t* profile) {
	if (profile->state == IDLE || profile->state == COMPLETE) {
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
