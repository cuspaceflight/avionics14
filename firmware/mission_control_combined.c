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
    STATE_FREE_ASCENT, STATE_IGNITE, STATE_SEPARATE, STATE_SEPARATED_ASCENT,
    STATE_APOGEE, STATE_DROGUE_DEPLOY, STATE_DROGUE_DESCENT, STATE_MAIN_DEPLOY,
    STATE_MAIN_DESCENT, STATE_TOUCHDOWN, STATE_LANDED, NUM_STATES
} state_t;

typedef struct instance_data {
    uint32_t t_launch, t_last_ignition, t_last_burnout;
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
    do_state_free_ascent, do_state_ignite, do_state_separate,
    do_state_separated_ascent, do_state_apogee, do_state_drogue_deploy,
    do_state_drogue_descent, do_state_main_deploy, do_state_main_descent,
    do_state_touchdown, do_state_landed
};

state_t run_state(state_t cur_state, instance_data_t *data)
{
    return state_table[cur_state](data);
}

static state_t do_state_pad(instance_data_t *data)
{
    if(chTimeNow() > 10000 && data->state.v > IGNITION_VELOCITY) {
        data->t_launch = chTimeNow();
        return STATE_IGNITION;
    } else {
        return STATE_PAD;
    }
}

static state_t do_state_ignition(instance_data_t *data)
{
}

static state_t do_state_powered_ascent(instance_data_t *data)
{
}

static state_t do_state_burnout(instance_data_t *data)
{
}

static state_t do_state_free_ascent(instance_data_t *data)
{
}

static state_t do_state_ignite(instance_data_t *data)
{
}

static state_t do_state_separate(instance_data_t *data)
{
}

static state_t do_state_separated_ascent(instance_data_t *data)
{
}

static state_t do_state_apogee(instance_data_t *data)
{
}

static state_t do_state_drogue_deploy(instance_data_t *data)
{
}

static state_t do_state_drogue_descent(instance_data_t *data)
{
}

static state_t do_state_main_deploy(instance_data_t *data)
{
}

static state_t do_state_main_descent(instance_data_t *data)
{
}

static state_t do_state_touchdown(instance_data_t *data)
{
}

static state_t do_state_landed(instance_data_t *data)
{
}


msg_t mission_thread(void* arg)
{
    (void)arg;
    state_t cur_state = STATE_PAD;
    state_t new_state;
    instance_data_t data;
    data.t_launch = data.t_last_ignition = data.t_last_burnout = 0;

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
