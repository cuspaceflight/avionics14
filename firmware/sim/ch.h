#pragma once
#include <stdint.h>
#include <stdbool.h>
#define TRUE 1
#define FALSE !TRUE
typedef int msg_t;
typedef uint32_t systime_t;
void chSetTimeNow(int32_t t);
systime_t chTimeNow(void);
systime_t chTimeElapsedSince(systime_t t0);
void chThdSleepMilliseconds(int ms);
void chRegSetThreadName(const char* name);
