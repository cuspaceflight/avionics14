 /* Mission Control Final
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
 * This state machine is designed to be used by both flight computers.
 * Must clarify in config whether the TOP_BOARD or BOTTOM_BOARD is being used
 *
 */
 
  
#include <math.h>
#include "mission_control_final.h"
#include "state_estimation.h"
#include "pyro.h"
#include "config.h"
#include "datalogging.h"

#define AGREED_TIME_DELAY 1

/* TOP_BOARD MISSION CONTROL */


#if board_location==TOP_BOARD

typedef top_state_t state_func_t(instance_data_t *data);

top_state_t run_state(top_state_t cur_state, instance_data_t *data);
static top_state_t do_state_standby(instance_data_t *data);
static top_state_t do_state_first_stage_fired(instance_data_t *data);
static top_state_t do_state_separated(instance_data_t *data);
static top_state_t do_state_time_delay(instance_data_t *data);
static top_state_t do_state_second_stage_fired(instance_data_t *data);
static top_state_t do_state_coasting(instance_data_t *data);
static top_state_t do_state_apogee(instance_data_t *data);
static top_state_t do_state_drogue_parachute_fired(instance_data_t *data);
static top_state_t do_state_main_parachute_fired_top(instance_data_t *data);
static top_state_t do_state_landed_top(instance_data_t *data);


state_func_t* const state_table[TOP_NUM_STATES] = 
{
    do_state_standby, do_state_first_stage_fired, do_state_separated, 
    do_state_time_delay, do_state_second_stage_fired, do_state_coasting, 
    do_state_apogee, do_state_drogue_parachute_fired, 
    do_state_main_parachute_fired_top, do_state_landed_top
};


top_state_t run_state(top_state_t cur_state, instance_data_t *data) 
{
    return state_table[cur_state](data);
}

static top_state_t do_state_standby(instance_data_t *data)
{
    if(chTimeNow() < 10000)
        return TOP_STATE_STANDBY;
    else if(data->state.v > IGNITION_VELOCITY)
    {
        data->t_launch = chTimeNow() ;
        return TOP_STATE_FIRST_STAGE_FIRED;
    }
    else
        return TOP_STATE_STANDBY;
}


static top_state_t do_state_first_stage_fired(instance_data_t *data)
{
    if(data->state.a < BURNOUT_ACCELERATION)
    {
        data->t_separation = chTimeNow() ; /* to sort out time delay later */
        return TOP_STATE_SEPARATED;
    }
    else if(chTimeElapsedSince(data->t_launch) > BURNOUT_TIMER) 
    {
        data->t_separation = chTimeNow() ; /* to sort out time delay later */
        return TOP_STATE_SEPARATED;
    }
    else
        return TOP_STATE_FIRST_STAGE_FIRED;

}

static top_state_t do_state_separated(instance_data_t *data)
{
    (void) data;
    return TOP_STATE_TIME_DELAY;
}

/* Delay before launching second stage */
static top_state_t do_state_time_delay(instance_data_t *data)
{
    /* AGREED_TIME_DELAY will need defining properly. Included at top of file.*/   
    if(chTimeElapsedSince(data->t_separation) > AGREED_TIME_DELAY) 
    {
	 pyro_fire_second_stage(); 
	 return TOP_STATE_SECOND_STAGE_FIRED;
    }
    else
        return TOP_STATE_TIME_DELAY;
}

static top_state_t do_state_second_stage_fired(instance_data_t *data)
{
    /* Will need to define GRAVITATIONAL_ACCELERATION and will need to 
     * free on error tolerance in its value */
    /* May be better to use a range of values and an inequality instead. */
    if((data->state.a) == GRAVITATIONAL_ACCELERATION ) 
	    return TOP_STATE_COASTING;						   
    else
	    return TOP_STATE_SECOND_STAGE_FIRED ;
}

static top_state_t do_state_coasting(instance_data_t *data)
{
    if(data->state.v == 0 && data->state.a == 0 ) /* test for apogee */
        return TOP_STATE_APOGEE;
    else
        return TOP_STATE_COASTING;
}

static top_state_t do_state_apogee(instance_data_t *data)
{
    data->t_apogee = chTimeNow();
    pyro_fire_drogue(); 					/* check this is functional */
    return TOP_STATE_DROGUE_PARACHUTE_FIRED;
}

static top_state_t do_state_drogue_parachute_fired(instance_data_t *data)
{
    if(data->state.h < MAIN_DEPLOY_ALTITUDE) /* edit this constant */
    {
	pyro_fire_main();                     /* check this is functional */
        return TOP_STATE_MAIN_PARACHUTE_FIRED;
    }
    else if(chTimeElapsedSince(data->t_apogee) > MAIN_DEPLOY_TIMER)
    {    
	pyro_fire_main();					   /* check this is functional */
	return TOP_STATE_MAIN_PARACHUTE_FIRED;
    }
    else
        return TOP_STATE_DROGUE_PARACHUTE_FIRED;
}

static top_state_t do_state_main_parachute_fired_top(instance_data_t *data)
{
    /* May be better to use an altitude/velocity check for this rather 
     * than a time check */
    if(chTimeElapsedSince(data->t_apogee) > LANDED_TIMER) 
        return TOP_STATE_LANDED;
    else
        return TOP_STATE_MAIN_PARACHUTE_FIRED;
}


static top_state_t do_state_landed_top(instance_data_t *data)
{
    (void)data;
    return TOP_STATE_LANDED;
}

msg_t mission_thread(void* arg)
{
    (void)arg;
    top_state_t cur_state = TOP_STATE_STANDBY;
    top_state_t new_state;
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

#endif /* TOP_BOARD MISSION CONTROL */




/* BOTTOM BOARD MISSION CONTROL */

#if board_location==BOTTOM_BOARD

typedef bottom_state_t state_func_t(instance_data_t *data);

bottom_state_t run_state(bottom_state_t cur_state, instance_data_t *data);
static bottom_state_t do_state_standby(instance_data_t *data);
static bottom_state_t do_state_first_stage_fired(instance_data_t *data);
static bottom_state_t do_state_separated(instance_data_t *data);
static bottom_state_t do_state_time_delay(instance_data_t *data);
static bottom_state_t do_state_main_parachute_fired_bottom(instance_data_t *data);
static bottom_state_t do_state_landed_bottom(instance_data_t *data);


state_func_t* const state_table[BOTTOM_NUM_STATES] = 
{
    do_state_standby, do_state_first_stage_fired, do_state_separated, 
    do_state_time_delay, do_state_main_parachute_fired_bottom, 
    do_state_landed_bottom
};


bottom_state_t run_state(bottom_state_t cur_state, instance_data_t *data) 
{
    return state_table[cur_state](data);
}

static bottom_state_t do_state_standby(instance_data_t *data)
{
    if(chTimeNow() < 10000)
        return BOTTOM_STATE_STANDBY;
    else if(data->state.v > IGNITION_VELOCITY)
    {
        data->t_launch = chTimeNow() ;
        return BOTTOM_STATE_FIRST_STAGE_FIRED;
    }
    else
        return BOTTOM_STATE_STANDBY;
}


static bottom_state_t do_state_first_stage_fired(instance_data_t *data) 
{
       
    if(data->state.a < BURNOUT_ACCELERATION)
    {
	pyro_fire_separation(); /* THIS FUNCTION IS NOT YET DEFINED  */
	
	/* to sort out the time delay  */
        data->t_separation = chTimeNow() ; 
        return BOTTOM_STATE_SEPARATED;
    }
       /* need the time mentioned earlier in this step */
    else if(chTimeElapsedSince(data->t_launch) > BURNOUT_TIMER) 
    {
        pyro_fire_separation();  /* THIS FUNCTION IS NOT YET DEFINED */
        
        /* to sort out the time delay */
        data->t_separation = chTimeNow() ; 
        return BOTTOM_STATE_SEPARATED;
    }
    else
        return BOTTOM_STATE_FIRST_STAGE_FIRED;
}

static bottom_state_t do_state_separated(instance_data_t *data)
{
    (void) data ;
    return BOTTOM_STATE_TIME_DELAY;
}


/* Delay before launching parachute */
static bottom_state_t do_state_time_delay(instance_data_t *data)
{
	/* AGREED_TIME_DELAY will need defining properly. Included at top of file.*/
    if(chTimeElapsedSince(data->t_separation) > AGREED_TIME_DELAY) 
    {
        pyro_fire_main();						/* check this is functional */
	return BOTTOM_STATE_MAIN_PARACHUTE_FIRED;
    }
    else if (data->state.h < MAIN_DEPLOY_ALTITUDE) /* edit this constant */
    {
	pyro_fire_main();                     /* check this is functional */
        return BOTTOM_STATE_MAIN_PARACHUTE_FIRED ;
    }
    else
        return BOTTOM_STATE_TIME_DELAY;
}


static bottom_state_t do_state_main_parachute_fired_bottom(instance_data_t *data)
{
    /* May be better to use an altitude/velocity check for this rather 
     * than a time check */
    if(chTimeElapsedSince(data->t_separation) > LANDED_TIMER) 
        return BOTTOM_STATE_LANDED;
    else
        return BOTTOM_STATE_MAIN_PARACHUTE_FIRED;
}


static bottom_state_t do_state_landed_bottom(instance_data_t *data)
{
    (void)data;
    return BOTTOM_STATE_LANDED;
}

msg_t mission_thread(void* arg)
{
    (void)arg;
    bottom_state_t cur_state = BOTTOM_STATE_STANDBY;
    bottom_state_t new_state;
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


#endif /* BOTTOM_BOARD MISSION CONTROL */





