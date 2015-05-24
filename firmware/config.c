#include <stdbool.h>
#include <stdio.h>
#include "microsd.h"
#include "config.h"

/* ------------------------------------------------------------------------- */

#define BUFFER_SIZE 128 // aka estimated max line length in config

static bool read_int(SDFILE* file, const char* name, int* attribute);
static bool read_double(SDFILE* file, const char* name, double* attribute);
static bool read_bool(SDFILE* file, const char* name, bool* attribute);

/* ------------------------------------------------------------------------- */

/* External config */
config_t conf = {
    .stage = 1,
    .ignition_velocity = 10.5,
    .burnout_acceleration = 2.0,
    .burnout_timer = 5200,
    .apogee_timer = 55000,
    .main_deploy_altitude = 1450.0,
    .main_deploy_timer = 30000,
    .landed_timer = 300000,
    .pyro_firetime = 5000,
    .pyro_drogue_body_1 = false,
    .pyro_drogue_body_2 = false,
    .pyro_drogue_body_3 = false,
    .pyro_drogue_nose_1 = true,
    .pyro_drogue_nose_2 = true,
    .pyro_drogue_nose_3 = true,
    .pyro_main_body_1 = false,
    .pyro_main_body_2 = false,
    .pyro_main_body_3 = true,
    .pyro_main_nose_1 = false,
    .pyro_main_nose_2 = false,
    .pyro_main_nose_3 = false,
    .accel_thrust_axis = 2
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

/* read a double attribute with config name <name> into <attribute>.
 */
static bool read_double(SDFILE* file, const char* name, double* attribute)
{
    char buffer[BUFFER_SIZE];
    char format[BUFFER_SIZE];
    SDRESULT status;

    status = microsd_gets(file, buffer, BUFFER_SIZE);
    if (status != FR_OK) return false;

    sprintf(format, "%s=%%lf", name);
    return sscanf(buffer, format, attribute) == 1;
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
    return
        read_int(file, "stage", &conf.stage) &&
        read_double(file, "ignition_velocity", &conf.ignition_velocity) &&
        read_double(file, "burnout_acceleration",&conf.burnout_acceleration) &&
        read_int(file, "burnout_timer", &conf.burnout_timer) &&
        read_int(file, "apogee_timer", &conf.apogee_timer) &&
        read_double(file, "main_deploy_altitude",&conf.main_deploy_altitude) &&
        read_int(file, "main_deploy_timer", &conf.main_deploy_timer) &&
        read_int(file, "landed_timer", &conf.landed_timer) &&
        read_int(file, "pyro_firetime", &conf.pyro_firetime) &&
        read_bool(file, "pyro_drogue_body_1", &conf.pyro_drogue_body_1) &&
        read_bool(file, "pyro_drogue_body_2", &conf.pyro_drogue_body_2) &&
        read_bool(file, "pyro_drogue_body_3", &conf.pyro_drogue_body_3) &&
        read_bool(file, "pyro_drogue_nose_1", &conf.pyro_drogue_nose_1) &&
        read_bool(file, "pyro_drogue_nose_2", &conf.pyro_drogue_nose_2) &&
        read_bool(file, "pyro_drogue_nose_3", &conf.pyro_drogue_nose_3) &&
        read_bool(file, "pyro_main_body_1", &conf.pyro_main_body_1) &&
        read_bool(file, "pyro_main_body_2", &conf.pyro_main_body_2) &&
        read_bool(file, "pyro_main_body_3", &conf.pyro_main_body_3) &&
        read_bool(file, "pyro_main_nose_1", &conf.pyro_main_nose_1) &&
        read_bool(file, "pyro_main_nose_2", &conf.pyro_main_nose_2) &&
        read_bool(file, "pyro_main_nose_3", &conf.pyro_main_nose_3) &&
        read_int(file, "accel_thrust_axis", &conf.accel_thrust_axis);
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
    }

    microsd_close_file(&file);
    return success;
}
