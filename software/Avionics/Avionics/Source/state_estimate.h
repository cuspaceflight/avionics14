#ifndef STATE_ESTIMATE_H
#define STATE_ESTIMATE_H
#include <stdint.h>

typedef struct state_estimate_t {
	float pos[3];
	float vel[3];
	float accel[3];

	float orientation_q[4];
	float orientation_euler[3];
} state_estimate_t;

void print_state_estimate(const state_estimate_t* estimate);

void state_estimate_new_accel_raw(const int16_t accel[3]);

void state_estimate_new_pressure_raw(int pressure);

void state_estimate_new_magnetometer_raw(const int16_t mag[3]);

void state_estimate_compute_next(state_estimate_t* next_estimate, float dt);

#endif /* STATE_ESTIMATE_H */