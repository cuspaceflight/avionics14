/* Final Mission Control
 * Both top and bottom stages unified
 */


#ifndef MISSION_CONTROL_FINAL_H
#define MISSION_CONTROL_FINAL_H

#include "ch.h"
#include "state_estimation.h"

typedef enum {
     TOP_STATE_STANDBY = 0, TOP_STATE_FIRST_STAGE_FIRED, TOP_STATE_SEPARATED, TOP_STATE_TIME_DELAY, 
     TOP_STATE_SECOND_STAGE_FIRED, TOP_STATE_COASTING, TOP_STATE_APOGEE, TOP_STATE_DROGUE_PARACHUTE_FIRED,
     TOP_STATE_MAIN_PARACHUTE_FIRED, TOP_STATE_LANDED, TOP_NUM_STATES  
 } top_state_t;

typedef enum {
    BOTTOM_STATE_STANDBY = 0, BOTTOM_STATE_FIRST_STAGE_FIRED, BOTTOM_STATE_SEPARATED, BOTTOM_STATE_TIME_DELAY, 
    BOTTOM_STATE_MAIN_PARACHUTE_FIRED, BOTTOM_STATE_LANDED, BOTTOM_NUM_STATES  
} bottom_state_t;


struct instance_data {
    int32_t t_launch;
    int32_t t_apogee;
    int32_t t_separation; /* added */
    state_estimate_t state; 
};

typedef struct instance_data instance_data_t;

int board_location;

msg_t mission_thread(void *arg);

#endif /* MISSION_CONTROL_FINAL_H */
