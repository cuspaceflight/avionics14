#include "ch.h"
#include "hal.h"
#include "tweeter.h"

#define NO_BEEPING false

volatile bool error_states[ERROR_MAX];

static void beep(int n, int ontime, int offtime)
{
#if !NO_BEEPING
    int i;
    for(i=0; i<n; i++) {
        palSetPad(GPIOA, GPIOA_BUZZER);
        chThdSleepMilliseconds(ontime);
        palClearPad(GPIOA, GPIOA_BUZZER);
        chThdSleepMilliseconds(offtime);
    }
#else
    chThdSleepMilliseconds(n*(ontime + offtime));
#endif
}

msg_t tweeter_thread(void* arg)
{
    (void)arg;
    chRegSetThreadName("Tweeter");
    int i;
    for(i=0; i<ERROR_MAX; i++) {
        error_states[i] = false;
    }

    while(true) {
        bool all_ok = true;
        for(i=0; i<ERROR_MAX; i++) {
            if(error_states[i]) {
                all_ok = false;
                beep(i, 500, 200);
                chThdSleepMilliseconds(1000);
            }
        }
        if(all_ok) {
            beep(1, 10, 990);
        }
    }
}

void tweeter_set_error(tweeter_error_t err, bool set)
{
    error_states[err] = set;
}
