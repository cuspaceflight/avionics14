#include <stdbool.h>
#include "microsd.h"
#include "config.h"

#define BUFFER_SIZE 128 // aka estimated max line length in config

//-----------------------------------------------------------------------------
// EXTERNAL CONFIG

config_t conf = {
    .location = TOP,
    .ignition_velocity = 0.0,
    .burnout_acceleration = 0.0,
    .burnout_timer = 0,
    .pyro_drogue_body_1 = false
};


//-----------------------------------------------------------------------------
// HELPER FUNCTIONS FOR READING CONFIG


/* reads exactly one integer from next line in file.
 * format string must contain one integer format such as %d.
 */

bool read_int(SDFILE* file, char buffer[], const char* format, int* attrib)
{
    SDRESULT res = microsd_fgets(file, buffer, BUFFER_SIZE);
    return res == SD_OK && sscanf(buffer, format, attrib) == 1;
}


/* reads exactly one float from next line in file.
 * format string much contain one float format such as %f.
 */

bool read_float(SDFILE* file, char buffer[], const char* format, float* attrib)
{
    SDRESULT res = microsd_fgets(file, buffer, BUFFER_SIZE);
    return res == SD_OK && sscanf(buffer, format, attrib) == 1;
}


/* reads exactly one bool from next line in file.
 * binary attributes are treated as integers in the config, ie. 0 is false,
 * any other number is true. format string must contain integer format.
 */

bool read_bool(SDFILE* file, char buffer[], const char* format, bool* attrib)
{
    SDRESULT res = microsd_fgets(file, buffer, BUFFER_SIZE);
    int int_result = 0;

    if (res == SD_OK && sscanf(buffer, format, &int_result) == 1)
    {
      *attrib = int_result != 0;
      return true;
    }
    return false;
}


/* returns true if succesfully read ALL the attributes.
 * strict in terms of ordering, no spaces, etc.
 */
bool read_config(SDFILE* file)
{
    char buffer[BUFFER_SIZE];

    return
        read_float(file, buffer, "ignition_velocity=%f\n",
            &conf.ignition_velocity) &&
        read_float(file, buffer, "burnout_acceleration=%f\n",
            &conf.burnout_acceleration) &&
        read_int(file, buffer, "burnout_timer=%d\n",
            &conf.burnout_timer) &&
        read_bool(file, buffer, "pyro_drogue_body_1=%d\n",
            &conf.pyro_drogue_body_1);
}


/* Initialises config. Returns false if some error occurred.
 */
bool config_init(void)
{
    // TODO: set location to whatever fits, how to determine this?

    SDFILE file;

    // initialise sd, mount file system
    microsd_card_try_init();

    // open config file with path <hardcoded>
    SDRESULT open = microsd_open_file(&file, "/path/to/config.txt", SD_READ);
    if (open != SD_OK) return false;

    // try to read the config
    return read_config(&file);

    // EXTRA: sanity check values?
    // maybe log/write the config data in use to the sd card, so that we can
    // later see exactly which configuration was used. probably unnecesary.
}
