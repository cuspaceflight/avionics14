#ifndef STATE_ESTIMATE_H
#define STATE_ESTIMATE_H

typedef struct state_estimate_t {
	float pos[3];
	float vel[3];
	float accel[3];

	float orientation[3];
} state_estimate_t;

void print_state_estimate(const state_estimate_t* estimate);

#endif /* STATE_ESTIMATE_H */