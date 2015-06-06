#include <stdbool.h>
#include <stdio.h>
#include "hal.h"
#include "microsd.h"
#include "config.h"
#include "tweeter.h"

/* ------------------------------------------------------------------------- */

#define BUFFER_SIZE 128 // aka estimated max line length in config

static bool read_int(SDFILE* file, const char* name, int* attribute);
static bool read_float(SDFILE* file, const char* name, float* attribute);
static bool read_bool(SDFILE* file, const char* name, bool* attribute);

/* ------------------------------------------------------------------------- */

/* External config */
config_t conf = {
    .config_loaded = false,
    .stage = 0,
    .got_ignition = false,
    .got_separation = false,
    .got_drogue = false,
    .got_main = false,
    .accel_axis = 0,
    .pyro_firetime = 0,
    .pyro_1 = 0,
    .pyro_2 = 0,
    .pyro_3 = 0,
    .pyro_4 = 0,
    .ignition_accel = 0.0f,
    .burnout_time = 0.0f,
    .ignite_altitude = 0.0f,
    .ignite_time = 0.0f,
    .ignite_timeout = 0.0f,
    .apogee_time = 0.0f,
    .main_altitude = 0.0f,
    .main_time = 0.0f,
    .landing_time = 0.0f,
    .use_radio = 0,
    .use_magno = 0,
    .use_gyro = 0,
    .use_gps = 0,
};

/* ------------------------------------------------------------------------- */

/* read an integer attribute with config name <name> into <attribute>.
 * sscanf version:
 * char format[BUFFER_SIZE];
 * sprintf(format, "%s=%%lf", name);
 * sscanf(buffer, format, attribute) which returns 1 if succesful
 */
static bool read_int(SDFILE* file, const char* name, int* attribute)
{
    char buffer[BUFFER_SIZE];
    char format[BUFFER_SIZE];
    SDRESULT status;

    status = microsd_gets(file, buffer, BUFFER_SIZE);
    if (status != FR_OK) return false;

    sprintf(format, "%s=%%d", name);
    return sscanf(buffer, format, attribute) == 1;
}

/* read a float attribute with config name <name> into <attribute>.
 */
static bool read_float(SDFILE* file, const char* name, float* attribute)
{
    char buffer[BUFFER_SIZE];
    char format[BUFFER_SIZE];
    double placeholder;
    int n_matched;
    SDRESULT status;

    status = microsd_gets(file, buffer, BUFFER_SIZE);
    if (status != FR_OK) return false;

    sprintf(format, "%s=%%lf", name);
    n_matched = sscanf(buffer, format, &placeholder);
    *attribute = (float)placeholder;
    return n_matched == 1;
}


/* read a bool attribute (aka int) with config name <name> into <attribute>.
 */
static bool read_bool(SDFILE* file, const char* name, bool* attribute)
{
    int res = 0;
    if (!read_int(file, name, &res)) return false;
    *attribute = res != 0;
    return true;
}

/* returns true if succesfully read ALL the attributes.
 * strict in terms of ordering, no spaces, etc.
 */
bool read_config(SDFILE* file)
{
    conf.config_loaded =
        read_int(file, "stage", &conf.stage) &&
        read_bool(file, "got_ignition", &conf.got_ignition) &&
        read_bool(file, "got_separation", &conf.got_separation) &&
        read_bool(file, "got_drogue", &conf.got_drogue) &&
        read_bool(file, "got_main", &conf.got_main) &&
        read_int(file, "accel_axis", &conf.accel_axis) &&
        read_int(file, "pyro_firetime", &conf.pyro_firetime) &&
        read_int(file, "pyro_1", &conf.pyro_1) &&
        read_int(file, "pyro_2", &conf.pyro_2) &&
        read_int(file, "pyro_3", &conf.pyro_3) &&
        read_int(file, "pyro_4", &conf.pyro_4) &&
        read_float(file, "ignition_accel", &conf.ignition_accel) &&
        read_float(file, "burnout_time", &conf.burnout_time) &&
        read_float(file, "ignite_altitude", &conf.ignite_altitude) &&
        read_float(file, "ignite_time", &conf.ignite_time) &&
        read_float(file, "ignite_timeout", &conf.ignite_timeout) &&
        read_float(file, "apogee_time", &conf.apogee_time) &&
        read_float(file, "main_altitude", &conf.main_altitude) &&
        read_float(file, "main_time", &conf.main_time) &&
        read_float(file, "landing_time", &conf.landing_time) &&
        read_bool(file, "use_radio", &conf.use_radio) &&
        read_bool(file, "use_magno", &conf.use_magno) &&
        read_bool(file, "use_gyro", &conf.use_gyro) &&
        read_bool(file, "use_gps", &conf.use_gps);

    tweeter_set_error(ERROR_CONFIG, !conf.config_loaded);
    return conf.config_loaded;
}

/* Sanity check the config we read to ensure it's at least reasonable and
 * consistent.
 */
bool check_config()
{
    bool ok = true;

    /* Basic sanity checks */
    ok &= conf.stage > 0 && conf.stage < 10;
    ok &= conf.accel_axis > 0 && conf.accel_axis < 7;
    ok &= conf.pyro_firetime > 0;
    ok &= conf.pyro_1 >= 0 && conf.pyro_1 < 5;
    ok &= conf.pyro_2 >= 0 && conf.pyro_2 < 5;
    ok &= conf.pyro_3 >= 0 && conf.pyro_3 < 5;
    ok &= conf.pyro_4 >= 0 && conf.pyro_4 < 5;
    ok &= conf.ignition_accel >= 0.0f && conf.ignition_accel < 1000.0f;
    ok &= conf.burnout_time >= 0.0f && conf.burnout_time < 1000.0f;
    ok &= conf.ignite_altitude >= 0.0f && conf.ignite_altitude < 100000.0f;
    ok &= conf.ignite_time >= 0.0f && conf.ignite_time < 10000.0f;
    ok &= conf.ignite_timeout >= 0.0f && conf.ignite_timeout < 10000.0f;
    ok &= conf.apogee_time >= 0.0f && conf.apogee_time < 10000.0f;
    ok &= conf.main_altitude >= 0.0f && conf.main_altitude < 100000.0f;
    ok &= conf.main_time >= 0.0f && conf.main_time < 10000.0f;
    ok &= conf.landing_time >= 0.0f && conf.landing_time < 10000.0f;

    /* Check pyro consistency */
    if(conf.got_ignition)
        ok &= conf.pyro_1 == 1 || conf.pyro_2 == 1 ||
              conf.pyro_3 == 1 || conf.pyro_4 == 1;
    if(conf.got_separation)
        ok &= conf.pyro_1 == 2 || conf.pyro_2 == 2 ||
              conf.pyro_3 == 2 || conf.pyro_4 == 2;
    if(conf.got_drogue)
        ok &= conf.pyro_1 == 3 || conf.pyro_2 == 3 ||
              conf.pyro_3 == 3 || conf.pyro_4 == 3;
    if(conf.got_main)
        ok &= conf.pyro_1 == 4 || conf.pyro_2 == 4 ||
              conf.pyro_3 == 4 || conf.pyro_4 == 4;

    tweeter_set_error(ERROR_CONFIG, !ok);

    return ok;
}


/* Initialises config. Returns false if some error occurred.
 */
bool config_init(const char* path)
{
    SDFILE file;
    SDFS sd;
    bool success = false;

    SDRESULT open = microsd_open_file(&file, path, FA_READ, &sd);

    if (open == FR_OK) {
        success = read_config(&file);
        success &= check_config();
    }

    microsd_close_file(&file);
    return success;
}

msg_t config_thread(void* arg)
{
    (void)arg;
    chRegSetThreadName("Config");
    config_init("config.txt");
    chThdSleep(TIME_INFINITE);
    return RDY_OK;
}
