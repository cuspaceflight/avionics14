/*
 * Mission Control Single Stage: Header
 */

#ifndef MISSION_CONTROL_SINGLE_H
#define MISSION_CONTROL_SINGLE_H

#include "ch.h"
#include "state_estimation.h"

typedef enum {
    SINGLE_STATE_STANDBY = 0, SINGLE_STATE_FIRST_STAGE_FIRED, 
    SINGLE_STATE_COASTING,SINGLE_STATE_APOGEE, 
    SINGLE_STATE_DROGUE_PARACHUTE_FIRED, SINGLE_STATE_MAIN_PARACHUTE_FIRED, 
    SINGLE_STATE_LANDED, SINGLE_NUM_STATES
} single_state_t;


struct instance_data {
    int32_t t_launch;
    int32_t t_apogee;
    int32_t t_separation; /* added */
    state_estimate_t state; 
};

typedef struct instance_data instance_data_t;

msg_t mission_thread(void *arg);

#endif /* MISSION_CONTROL_BOTTOM_H */
