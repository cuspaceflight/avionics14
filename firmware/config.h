#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

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
 */

/* ------------------------------------------------------------------------- */

/* See docs/config.md for descriptions */

#define STAGE                   (conf.stage)
#define GOT_IGNITION            (conf.got_ignition)
#define GOT_SEPARATION          (conf.got_separation)
#define GOT_DROGUE              (conf.got_drogue)
#define GOT_MAIN                (conf.got_main)
#define ACCEL_AXIS              (conf.accel_axis)
#define PYRO_FIRETIME           (conf.pyro_firetime)
#define PYRO_1                  (conf.pyro_1)
#define PYRO_2                  (conf.pyro_2)
#define PYRO_3                  (conf.pyro_3)
#define PYRO_4                  (conf.pyro_4)
#define IGNITION_ACCEL          (conf.ignition_accel)
#define BURNOUT_TIME            (conf.burnout_time)
#define IGNITE_ALTITUDE         (conf.ignite_altitude)
#define IGNITE_TIME             (conf.ignite_time)
#define IGNITE_TIMEOUT          (conf.ignite_timeout)
#define APOGEE_TIME             (conf.apogee_time)
#define MAIN_ALTITUDE           (conf.main_altitude)
#define MAIN_TIME               (conf.main_time)
#define LANDING_TIME            (conf.landing_time)
#define USE_RADIO               (conf.use_radio)
#define USE_MAGNO               (conf.use_magno)
#define USE_GYRO                (conf.use_gyro)
#define USE_GPS                 (conf.use_gps)
#define CONFIG_LOADED           (conf.config_loaded)


/* Read config from the file on the sd card specified by <path>.
 * If none/error occurred, return false, if succeeded return true.
 */
bool config_init(const char* path);

/* Sanity check the loaded config.
 */
bool check_config(void);

//-----------------------------------------------------------------------------

typedef struct config_ {
    bool config_loaded;
    int stage;
    bool got_ignition, got_separation, got_drogue, got_main;
    int accel_axis;
    int pyro_firetime, pyro_1, pyro_2, pyro_3, pyro_4;
    float ignition_accel;
    float burnout_time;
    float ignite_altitude, ignite_time, ignite_timeout;
    float apogee_time;
    float main_altitude, main_time;
    float landing_time;
    bool use_radio, use_magno, use_gyro, use_gps;
} config_t;

/* This is the global configuration that can be accessed from any file.
 * In practice, it should ONLY be written to from config.c.
 * Use the macros above to get the config values.
 */
extern config_t conf;

msg_t config_thread(void* arg);

#endif  /* CONFIG_H */
