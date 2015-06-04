/*
 * Combined Mission Control
 *
 * See docs/states.png for state transition diagram.
 */

#ifndef MISSION_CONTROL_COMBINED_H
#define MISSION_CONTROL_COMBINED_H

#include "ch.h"
#include "state_estimation.h"

msg_t mission_thread(void *arg);

#endif /* MISSION_CONTROL_COMBINED_H */
