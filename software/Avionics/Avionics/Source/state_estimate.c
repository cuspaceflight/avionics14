#include <state_estimate.h>
#include <logging.h>

void print_state_estimate(const state_estimate_t* estimate) {
	PRINT("State estimate { Position: [");
	for (int i = 0; i < 3; ++i)
		PRINT(" %f", estimate->pos[i]);
	PRINT(" ] Velocity: [");
	for (int i = 0; i < 3; ++i)
		PRINT(" %f", estimate->vel[i]);
	PRINT(" ] Acceleration: [");
	for (int i = 0; i < 3; ++i)
		PRINT(" %f", estimate->accel[i]);
	PRINT(" ]}\n");
}