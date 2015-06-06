/*
 * Tweeter functionality. Reports state over the buzzer.
 */
#ifndef TWEETER_H
#define TWEETER_H

#include <stdbool.h>

typedef enum {
    ERROR_PYRO_1 = 1,
    ERROR_PYRO_2,
    ERROR_PYRO_3,
    ERROR_PYRO_4,
    ERROR_SD_CARD,
    ERROR_CONFIG,
    ERROR_RADIO,
    ERROR_GPS,
    ERROR_MAGNO,
    ERROR_GYRO,
    ERROR_ACCEL,
    ERROR_BARO,
    ERROR_MAX
} tweeter_error_t;

extern volatile bool error_states[ERROR_MAX];

/* Set a specific error.
 * If `set` is true then the error is occurring.
 */
void tweeter_set_error(tweeter_error_t err, bool set);
msg_t tweeter_thread(void* arg);

#endif /* TWEETER_H */
