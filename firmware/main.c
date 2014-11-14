/*
 * M2FC
 * 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
 */

#include "ch.h"
#include "hal.h"

/*
 * Main Thread.
 * Starts all the other threads then puts itself to sleep.
 */
int main(void) {
    halInit();
    chSysInit();
    chRegSetThreadName("Main");

    

    /* Let the main thread idle now. */
    chThdSetPriority(LOWPRIO);
    chThdSleep(TIME_INFINITE);

    return 0;
}
