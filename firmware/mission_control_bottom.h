/*
 * Mission Control Bottom: Header
 */

#ifndef MISSION_H
#define MISSION_H

#include "ch.h"
#include "state_estimation.h"

typedef enum {
    STATE_STANDBY = 0, STATE_FIRST_STAGE_FIRED, STATE_SEPARATED, STATE_TIME_DELAY, 
    STATE_MAIN_PARACHUTE_FIRED_BOTTOM, STATE_LANDED_BOTTOM, NUM_STATES  
} state_t;


struct instance_data {
    int32_t t_launch;
    int32_t t_apogee;
    int32_t t_separation; /* added */
    state_estimate_t state; 
};

typedef struct instance_data instance_data_t;

msg_t mission_thread(void *arg);

#endif /* MISSION_H */
