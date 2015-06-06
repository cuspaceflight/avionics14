#include "ch.h"

static systime_t ch_time_now = 0;

void chSetTimeNow(int32_t t)
{
    ch_time_now = t;
}

systime_t chTimeNow()
{
    return ch_time_now;
}

void chThdSleepMilliseconds(int ms)
{
    (void)ms;
}

void chRegSetThreadName(const char* name)
{
    (void)name;
}

systime_t chTimeElapsedSince(systime_t t0)
{
    return chTimeNow() - t0;
}
