/*
 * Pyrotechnic channel driver
 */

#ifndef PYRO_H
#define PYRO_H

#include <ch.h>


/* Check the pyro channel pad for continuity, returns TRUE or FALSE. */
bool_t pyro_continuity(uint8_t pad);

/* Check that all pyros that should be installed are installed. */
bool_t pyro_continuity_check(void);


/* Fire the pyro channel `channel` for `duration_ms` milliseconds.
 * Non-blocking.
 */
void pyro_fire(uint8_t channel, uint16_t duration_ms);

/* Fire the correct output channel.
 * Fires for PYRO_FIRETIME ms.
 * Does not block.
 */
void pyro_fire_drogue(void);
void pyro_fire_main(void);
void pyro_fire_separation(void);
void pyro_fire_ignite(void);


/* Checks pyro continuities continuously */
msg_t pyro_thread(void *arg);

#endif /* PYRO_H */
