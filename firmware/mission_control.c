/*
 * Combined Mission Control
 *
 * See docs/states.png for state transition diagram.
 */

#include <math.h>
#include "config.h"
#include "state_estimation.h"
#include "datalogging.h"
#include "pyro.h"

typedef enum {
    STATE_PAD = 0, STATE_IGNITION, STATE_POWERED_ASCENT, STATE_BURNOUT,
    STATE_FREE_ASCENT, STATE_IGNITE, STATE_WAIT_IGNITION, STATE_SEPARATE,
    STATE_SEPARATED_ASCENT, STATE_APOGEE, STATE_DROGUE_DEPLOY,
    STATE_DROGUE_DESCENT, STATE_MAIN_DEPLOY, STATE_MAIN_DESCENT,
    STATE_TOUCHDOWN, STATE_LANDED, NUM_STATES
} state_t;

typedef struct instance_data {
    systime_t t_launch, t_last_ignite_attempt, t_last_ignition, t_last_burnout,
              t_apogee;
    int current_stage_position;
    state_estimate_t state;
} instance_data_t;

typedef state_t state_func_t(instance_data_t *data);
state_t run_state(state_t cur_state, instance_data_t *data);

static state_t do_state_pad(instance_data_t *data);
static state_t do_state_ignition(instance_data_t *data);
static state_t do_state_powered_ascent(instance_data_t *data);
static state_t do_state_burnout(instance_data_t *data);
static state_t do_state_free_ascent(instance_data_t *data);
static state_t do_state_ignite(instance_data_t *data);
static state_t do_state_wait_ignition(instance_data_t *data);
static state_t do_state_separate(instance_data_t *data);
static state_t do_state_separated_ascent(instance_data_t *data);
static state_t do_state_apogee(instance_data_t *data);
static state_t do_state_drogue_deploy(instance_data_t *data);
static state_t do_state_drogue_descent(instance_data_t *data);
static state_t do_state_main_deploy(instance_data_t *data);
static state_t do_state_main_descent(instance_data_t *data);
static state_t do_state_touchdown(instance_data_t *data);
static state_t do_state_landed(instance_data_t *data);

state_func_t* const state_table[NUM_STATES] =
{
    do_state_pad, do_state_ignition, do_state_powered_ascent, do_state_burnout,
    do_state_free_ascent, do_state_ignite, do_state_wait_ignition,
    do_state_separate, do_state_separated_ascent, do_state_apogee,
    do_state_drogue_deploy, do_state_drogue_descent, do_state_main_deploy,
    do_state_main_descent, do_state_touchdown, do_state_landed
};

state_t run_state(state_t cur_state, instance_data_t *data)
{
    return state_table[cur_state](data);
}

static state_t do_state_pad(instance_data_t *data)
{
    if(chTimeNow() > 10000 && data->state.a > IGNITION_ACCEL) {
        data->t_launch = chTimeNow();
        return STATE_IGNITION;
    } else {
        return STATE_PAD;
    }
}

static state_t do_state_ignition(instance_data_t *data)
{
    data->t_last_ignition = chTimeNow();
    return STATE_POWERED_ASCENT;
}

static state_t do_state_powered_ascent(instance_data_t *data)
{
    systime_t time_since_ignition = chTimeElapsedSince(data->t_last_ignition);
    if(data->state.a < 0.0f || time_since_ignition > BURNOUT_TIME) {
        return STATE_BURNOUT;
    } else {
        return STATE_POWERED_ASCENT;
    }
}

static state_t do_state_burnout(instance_data_t *data)
{
    data->t_last_burnout = chTimeNow();
    if(data->current_stage_position == 1) {
        if(GOT_SEPARATION) {
            return STATE_SEPARATE;
        } else {
            return STATE_SEPARATED_ASCENT;
        }
    } else {
        if(GOT_IGNITION) {
            return STATE_FREE_ASCENT;
        } else {
            return STATE_WAIT_IGNITION;
        }
    }
}

static state_t do_state_free_ascent(instance_data_t *data)
{
    systime_t time_since_burnout = chTimeElapsedSince(data->t_last_burnout);
    if(data->state.h > IGNITE_ALTITUDE || time_since_burnout > IGNITE_TIME) {
        return STATE_IGNITE;
    } else {
        return STATE_FREE_ASCENT;
    }
}

static state_t do_state_ignite(instance_data_t *data)
{
    pyro_fire_ignite();
    return STATE_WAIT_IGNITION;
}

static state_t do_state_wait_ignition(instance_data_t *data)
{
    systime_t time_in_state = chTimeElapsedSince(data->t_last_ignite_attempt);
    if(data->state.a > IGNITION_ACCEL) {
        data->current_stage_position--;
        return STATE_IGNITION;
    } else if(time_in_state > IGNITE_TIMEOUT) {
        return STATE_SEPARATED_ASCENT;
    } else {
        return STATE_WAIT_IGNITION;
    }
}

static state_t do_state_separate(instance_data_t *data)
{
    pyro_fire_separation();
    return STATE_SEPARATED_ASCENT;
}

static state_t do_state_separated_ascent(instance_data_t *data)
{
    systime_t time_since_burnout = chTimeElapsedSince(data->t_last_burnout);
    if(data->state.v < 0 || time_since_burnout > APOGEE_TIME) {
        return STATE_APOGEE;
    } else {
        return STATE_SEPARATED_ASCENT;
    }
}

static state_t do_state_apogee(instance_data_t *data)
{
    data->t_apogee = chTimeNow();
    if(GOT_DROGUE) {
        return STATE_DROGUE_DEPLOY;
    } else {
        return STATE_DROGUE_DESCENT;
    }
}

static state_t do_state_drogue_deploy(instance_data_t *data)
{
    pyro_fire_drogue();
    return STATE_DROGUE_DESCENT;
}

static state_t do_state_drogue_descent(instance_data_t *data)
{
    systime_t time_since_apogee = chTimeElapsedSince(data->t_apogee);
    if(data->state.h < MAIN_ALTITUDE || time_since_apogee > MAIN_TIME) {
        if(GOT_MAIN) {
            return STATE_MAIN_DEPLOY;
        } else {
            return STATE_MAIN_DESCENT;
        }
    } else {
        return STATE_DROGUE_DESCENT;
    }
}

static state_t do_state_main_deploy(instance_data_t *data)
{
    pyro_fire_main();
    return STATE_MAIN_DESCENT;
}

static state_t do_state_main_descent(instance_data_t *data)
{
    systime_t time_since_apogee = chTimeElapsedSince(data->t_apogee);
    if(fabs(data->state.v) < 1.0f || time_since_apogee > LANDING_TIME) {
        return STATE_TOUCHDOWN;
    } else {
        return STATE_MAIN_DESCENT;
    }
}

static state_t do_state_touchdown(instance_data_t *data)
{
    return STATE_LANDED;
}

static state_t do_state_landed(instance_data_t *data)
{
    return STATE_LANDED;
}


msg_t mission_thread(void* arg)
{
    (void)arg;
    state_t cur_state = STATE_PAD;
    state_t new_state;
    instance_data_t data;
    data.t_launch = data.t_last_ignition = data.t_last_burnout = 0;
    data.current_stage_position = STAGE;

    chRegSetThreadName("Mission");

    while(1) {
        /* Run Kalman prediction step */
        data.state = state_estimation_get_state();

        /* Run state machine's current state function */
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
