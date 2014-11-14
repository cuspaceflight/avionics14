/* Exposes functions for reading config file from sd card.
 */

typedef struct config_ {
    float ignition_velocity;
    float burnout_acceleration;
    // TODO: more fields
} config_t;

extern config_t conf; // global config, defined in config.c

// Something like this just might work ...
#define IGNITION_VELOCITY (conf.ignition_velocity)
#define BURNOUT_ACCELERATION (conf.burnout_acceleration)

/* Read configuration from the file on the sd card if any, else use default
 * values.
 */
void config_init(void);
