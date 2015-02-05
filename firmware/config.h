/* Exposes functions for reading config file from sd card.
 * NOTE: to add a config option, you have to add it the following places:
 *   - in the config_t type defined in here
 *   - as a macro in here
 *   - in the initialisation of the global conf in config.c
 *   - in the read_config function of config.c
 *
 * TODO: perhaps some way of telling why it failed, if it fails?
 * TODO: figure out the exact config fields
 */

typedef enum location_ { TOP, BOTTOM } location_t;

/* actual configuration */

typedef struct config_ {
    location_t location;
    float ignition_velocity;
    float burnout_acceleration;
    int burnout_timer;
    bool pyro_drogue_body_1;
} config_t;

/* This is the global configuration that can be accessed from any file.
 * In practice, it should ONLY be written to from config.c.
 * Use the macros below to get the config values.
 */
extern config_t conf;

// ----------------------------------------------------------------------------

#define IGNITION_VELOCITY    (conf.ignition_velocity)
#define BURNOUT_ACCELERATION (conf.burnout_acceleration)
#define BURNOUT_TIMER        (conf.burnout_timer)
#define PYRO_DROGUE_BODY_1   (conf.pyro_drogue_body_1)

/* Read config from the file on the sd card specified by <path>.
 * If none/error occurred, return false, if succeeded return true. */
bool config_init(const char* path);
