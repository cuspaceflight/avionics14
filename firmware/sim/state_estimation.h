#pragma once
#include <stdint.h>
typedef struct { float h, v, a; } state_estimate_t;
extern volatile uint8_t state_estimation_trust_barometer;
state_estimate_t state_estimation_get_state(void);
