/* Mission Control Plan - BOTTOM STAGE	
 *
 * The states that are being used are for the top stage are as follows: 
 * standby, first_stage_fired, separated, time_delay, second_stage_fired 
 * (which occurs after a delay of one second following separation),
 * coasting, apogee, drogue_parachute_fired, main_parachute_fired_top, 
 * landed_top
 *
 * The states for the bottom stage are as follows:
 * standby, first_stage_rocket, separated, main_parachute_fired_bottom, 
 * landed_bottom
 *
 */
 
 
/* Header files below are just lifted from m2 code, but obviously this depends 
 * on how we're implementing everything.
 * Most likely quite similar though... 
 * Will need to modify symbolic constants and some functions (eg pyro stuff) 
 */
 
#include <math.h>
#include "mission_control_bottom.h"
#include "state_estimation.h"
#include "pyro.h"
#include "microsd.h"
#include "config.h"
#include "sbp_io.h"

/* #define AGREED_TIME_DELAY = 1 second ?? NEEDS CLARIFICATION */


/* function prototypes for the different states.
 * Changed names and added a couple of extra states 
 */

typedef state_t state_func_t(instance_data_t *data);

state_t run_state(state_t cur_state, instance_data_t *data);
static state_t do_state_standby(instance_data_t *data);
static state_t do_state_first_stage_fired(instance_data_t *data);
static state_t do_state_separated(instance_data_t *data);
static state_t do_state_time_delay(instance_data_t *data);
static state_t do_state_main_parachute_fired_bottom(instance_data_t *data);
static state_t do_state_landed_bottom(instance_data_t *data);


state_func_t* const state_table[NUM_STATES] = 
{
    do_state_standby, do_state_first_stage_fired, do_state_separated, 
    do_state_time_delay, do_state_main_parachute_fired_bottom, 
    do_state_landed_bottom
};


state_t run_state(state_t cur_state, instance_data_t *data) 
{
    return state_table[cur_state](data);
}

static state_t do_state_standby(instance_data_t *data)
{
    state_estimation_trust_barometer = 1;
    if(chTimeNow() < 10000)
        return STATE_STANDBY;
    else if(data->state.v > IGNITION_VELOCITY)
    {
        data->t_launch = chTimeNow() ;
        return STATE_FIRST_STAGE_FIRED;
    }
    else
        return STATE_STANDBY;
}

/* Adam had a separate state in here which was effectively in between the 
 * standby and first stage being fired. 
 * It read in the current system time and stored this before moving to the next
 * state.
 * I've put this in the previous body of code as it didn't seem like this 
 * intermediate state was doing anything else.
 */


static state_t do_state_first_stage_fired(instance_data_t *data) 
{
       
    state_estimation_trust_barometer = 0;
    if(data->state.a < BURNOUT_ACCELERATION)
    {
	pyro_fire_separation(); /* THIS FUNCTION IS NOT YET DEFINED  */
	
	/* to sort out the time delay  */
        data->t_separation = chTimeNow() ; 
        return STATE_SEPARATED;
    }
       /* need the time mentioned earlier in this step */
    else if(chTimeElapsedSince(data->t_launch) > BURNOUT_TIMER) 
    {
        pyro_fire_separation();  /* THIS FUNCTION IS NOT YET DEFINED */
        
        /* to sort out the time delay */
        data->t_separation = chTimeNow() ; 
        return STATE_SEPARATED;
    }
    else
        return STATE_FIRST_STAGE_FIRED;
}

static state_t do_state_separated(instance_data_t *data)
{
    return STATE_TIME_DELAY;
}


/* Delay before launching parachute */
static state_t do_state_time_delay(instance_data_t *data)
{
	/* AGREED_TIME_DELAY will need defining properly. Included at top of file.*/
    if(chTimeElapsedSince(data->t_separation) > AGREED_TIME_DELAY) 
    {
        pyro_fire_main();						/* check this is functional */
	return STATE_MAIN_PARACHUTE_FIRED_BOTTOM;
    }
    else if (data->state.h < MAIN_DEPLOY_ALTITUDE) /* edit this constant */
    {
	pyro_fire_main();                     /* check this is functional */
        return STATE_MAIN_PARACHUTE_FIRED_BOTTOM
    }
    else
        return STATE_TIME_DELAY;
}


static state_t do_state_main_parachute_fired_bottom(instance_data_t *data)
{
    state_estimation_trust_barometer = 1;
    /* May be better to use an altitude/velocity check for this rather 
     * than a time check */
    if(chTimeElapsedSince(data->t_separation) > LANDED_TIMER) 
        return STATE_LANDED_BOTTOM;
    else
        return STATE_MAIN_PARACHUTE_FIRED_BOTTOM;
}


static state_t do_state_landed_bottom(instance_data_t *data)
{
    state_estimation_trust_barometer = 1;
    (void)data;
    return STATE_LANDED_BOTTOM;
}

msg_t mission_thread(void* arg)
{
    (void)arg;
    state_t cur_state = STATE_STANDBY;
    state_t new_state;
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
            microsd_log_s32(CHAN_SM_MISSION,
            (int32_t)cur_state, (int32_t)new_state);
            cur_state = new_state;
            SBP_SEND(0x30, new_state);
        }

        /* Tick the state machine about every millisecond */
        chThdSleepMilliseconds(1);
    }
}
 
 
