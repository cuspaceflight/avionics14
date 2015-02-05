/*
* Translational State estimation and sensor fusion
* Avionics14
* 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
*/

#ifndef TRANSLATION_KALMAN_H
#define TRANSLATION_KALMAN_H

#include <stdint.h>

typedef struct translational_state_t {
	float pos[3];
	float vel[3];
	float accel[3];
} translational_state_t;

extern volatile uint8_t translation_kalman_trust_barometer;

void translation_kalman_prediction_step(translational_state_t* state);

void translation_kalman_new_pressure(float pressure);

void translation_kalman_new_accel(const float* accel);

#endif /* TRANSLATION_KALMAN_H */