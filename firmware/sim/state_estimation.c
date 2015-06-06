#include "state_estimation.h"
volatile uint8_t state_estimation_trust_barometer;
state_estimate_t state_estimation_get_state()
{
    state_estimate_t s = {0.0f, 0.0f, 0.0f};
    return s;
}
