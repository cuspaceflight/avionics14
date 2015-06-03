/* Mission Control Plan - Single Stage Rocket
 *
 */
 
 
#include <math.h>
#include "mission_control_single.h"
#include "state_estimation.h"
#include "pyro.h"
#include "config.h"
#include "datalogging.h"


#define AGREED_TIME_DELAY 1


/* function prototypes for the different states.
 * Changed names and added a couple of extra states 
 */

typedef single_state_t state_func_t(instance_data_t *data);

single_state_t run_state(single_state_t cur_state, instance_data_t *data);
static single_state_t do_state_standby(instance_data_t *data);
static single_state_t do_state_single_stage_fired(instance_data_t *data);
static single_state_t do_state_coasting(instance_data_t *data);
static single_state_t do_state_apogee(instance_data_t *data);
static single_state_t do_state_drogue_parachute_fired(instance_data_t *data);
static single_state_t do_state_main_parachute_fired_single(instance_data_t *data);
static single_state_t do_state_landed_single(instance_data_t *data);


state_func_t* const state_table[SINGLE_NUM_STATES] = 
{
    do_state_standby, do_state_single_stage_fired, do_state_coasting, 
    do_state_apogee, do_state_drogue_parachute_fired, 
    do_state_main_parachute_fired_single, do_state_landed_single
};


single_state_t run_state(single_state_t cur_state, instance_data_t *data) 
{
    return state_table[cur_state](data);
}

static single_state_t do_state_standby(instance_data_t *data)
{
    if(chTimeNow() < 10000)
        return SINGLE_STATE_STANDBY;
    else if(data->state.v > IGNITION_VELOCITY)
    {
        data->t_launch = chTimeNow() ;
        return SINGLE_STATE_FIRST_STAGE_FIRED;
    }
    else
        return SINGLE_STATE_STANDBY;
}


static single_state_t do_state_single_stage_fired(instance_data_t *data) 
{
       
    if(data->state.a < BURNOUT_ACCELERATION)
    {
        return SINGLE_STATE_COASTING;
    }
    
    else if(chTimeElapsedSince(data->t_launch) > (uint32_t)BURNOUT_TIMER) 
    {
        return SINGLE_STATE_COASTING;          
    }
    
    else
        return SINGLE_STATE_FIRST_STAGE_FIRED;
}

static single_state_t do_state_coasting(instance_data_t *data)
{
    if(data->state.v == 0 && data->state.a == 0 ) /* test for apogee */
        return SINGLE_STATE_APOGEE;
    else
        return SINGLE_STATE_COASTING;
}

static single_state_t do_state_apogee(instance_data_t *data)
{
    data->t_apogee = chTimeNow();
    pyro_fire_drogue(); 					
    return SINGLE_STATE_DROGUE_PARACHUTE_FIRED;
}

static single_state_t do_state_drogue_parachute_fired(instance_data_t *data)
{
    if(data->state.h < MAIN_DEPLOY_ALTITUDE) /* edit this constant */
    {
	pyro_fire_main();                     /* check this is functional */
        return SINGLE_STATE_MAIN_PARACHUTE_FIRED;
    }
    else if(chTimeElapsedSince(data->t_apogee) > (uint32_t)MAIN_DEPLOY_TIMER)
    {    
	pyro_fire_main();					   /* check this is functional */
	return SINGLE_STATE_MAIN_PARACHUTE_FIRED;
    }
    else
        return SINGLE_STATE_DROGUE_PARACHUTE_FIRED;
}

static single_state_t do_state_main_parachute_fired_single(instance_data_t *data)
{
    /* May be better to use an altitude/velocity check for this rather 
     * than a time check */
    if(chTimeElapsedSince(data->t_apogee) > (uint32_t)LANDED_TIMER) 
        return SINGLE_STATE_LANDED;
    else
        return SINGLE_STATE_MAIN_PARACHUTE_FIRED;
}


static single_state_t do_state_landed_single(instance_data_t *data)
{
    (void)data;
    return SINGLE_STATE_LANDED;
}

msg_t mission_thread(void* arg)
{
    (void)arg;
    single_state_t cur_state = SINGLE_STATE_STANDBY;
    single_state_t new_state;
    instance_data_t data;
    data.t_launch = -1;
    data.t_apogee = -1;
    data.t_separation = -1;

    chRegSetThreadName("Mission");

    while(1) {
        /* Run Kalman prediction step */
        data.state = state_estimation_get_state();

        /* Run state machine current state function */
        new_state = run_state(cur_state, &data);

        /* Log changes in state */
        if(new_state != cur_state) {
            log_s32(CHAN_SM_MISSION,
            (int32_t)cur_state, (int32_t)new_state);
            cur_state = new_state;
        }
        /* Tick the state machine about every millisecond */
        chThdSleepMilliseconds(1);
    }
}
 
 
