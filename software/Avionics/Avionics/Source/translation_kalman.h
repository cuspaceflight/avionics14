/*
* Translational State estimation and sensor fusion
* Avionics14
* 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
*/

#ifndef TRANSLATION_KALMAN_H
#define TRANSLATION_KALMAN_H

#include <stdint.h>
#include <state_estimate.h>

void translation_kalman_prediction_step(state_estimate_t* state, float dt);

void translation_kalman_new_pressure(float pressure);

// accel should correctly scaled and with the gravity vector removed
void translation_kalman_new_accel(const float* accel);

float state_estimation_pressure_to_altitude(float pressure);

extern volatile uint8_t translation_kalman_trust_barometer;



#endif /* TRANSLATION_KALMAN_H */