#ifndef CONFIG_H
#define CONFIG_H

/* Exposes functions for reading config file from sd card.
 * Usage: config MUST be initialised with config_init, then macros can be used
 * to access config.
 *
 * NOTE: to add a config option, you have to add it the following places:
 *   - in the config_t type defined in here
 *   - as a macro in here
 *   - in the initialisation of the global conf in config.c
 *   - in the read_config function of config.c
 *
 * TODO: perhaps some way of telling why it failed, if it fails?
 * TODO: perhaps some sanity check of the read values?
 * TODO: figure out the exact config fields
 */

/* ------------------------------------------------------------------------- */

#define STAGE                (conf.stage) // stage is 1 (bottom) or 2 (top)
#define IGNITION_VELOCITY    (conf.ignition_velocity)
#define BURNOUT_ACCELERATION (conf.burnout_acceleration)
#define BURNOUT_TIMER        (conf.burnout_timer)
#define APOGEE_TIMER         (conf.apogee_timer)
#define MAIN_DEPLOY_ALTITUDE (conf.main_deploy_altitude)
#define MAIN_DEPLOY_TIMER    (conf.main_deploy_timer)
#define LANDED_TIMER         (conf.landed_timer)
#define PYRO_FIRETIME        (conf.pyro_firetime)
#define PYRO_DROGUE_BODY_1   (conf.pyro_drogue_body_1)
#define PYRO_DROGUE_BODY_2   (conf.pyro_drogue_body_2)
#define PYRO_DROGUE_BODY_3   (conf.pyro_drogue_body_3)
#define PYRO_DROGUE_NOSE_1   (conf.pyro_drogue_nose_1)
#define PYRO_DROGUE_NOSE_2   (conf.pyro_drogue_nose_2)
#define PYRO_DROGUE_NOSE_3   (conf.pyro_drogue_nose_3)
#define PYRO_MAIN_BODY_1     (conf.pyro_main_body_1)
#define PYRO_MAIN_BODY_2     (conf.pyro_main_body_2)
#define PYRO_MAIN_BODY_3     (conf.pyro_main_body_3)
#define PYRO_MAIN_NOSE_1     (conf.pyro_main_nose_1)
#define PYRO_MAIN_NOSE_2     (conf.pyro_main_nose_2)
#define PYRO_MAIN_NOSE_3     (conf.pyro_main_nose_3)
#define ACCEL_THRUST_AXIS    (conf.accel_thrust_axis)

/* Read config from the file on the sd card specified by <path>.
 * If none/error occurred, return false, if succeeded return true.
 */
bool config_init(const char* path);

//-----------------------------------------------------------------------------

typedef struct config_ {
    int stage;
    double ignition_velocity;
    double burnout_acceleration;
    int burnout_timer;
    int apogee_timer;
    double main_deploy_altitude;
    int main_deploy_timer;
    int landed_timer;
    int pyro_firetime;
    bool pyro_drogue_body_1;
    bool pyro_drogue_body_2;
    bool pyro_drogue_body_3;
    bool pyro_drogue_nose_1;
    bool pyro_drogue_nose_2;
    bool pyro_drogue_nose_3;
    bool pyro_main_body_1;
    bool pyro_main_body_2;
    bool pyro_main_body_3;
    bool pyro_main_nose_1;
    bool pyro_main_nose_2;
    bool pyro_main_nose_3;
    int accel_thrust_axis;
} config_t;

/* This is the global configuration that can be accessed from any file.
 * In practice, it should ONLY be written to from config.c.
 * Use the macros above to get the config values.
 */
extern config_t conf;

#define TOP_BOARD  1 
#define BOTTOM_BOARD  0 

#endif /* CONFIG_H */
