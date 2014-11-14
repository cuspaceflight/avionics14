#include "microsd.h"
#include "config.h"

/* initialise config to default values */
config_t conf = {
    .ignition_velocity = 10.0,
    .burnout_acceleration = 2.0
};

/* TODO: read config from sd
 */
void config_init(void) {

    /* initialise sd, mount file system, open config file with path <hardcoded>
     * if there isn't any, return
     * if there IS one, then read it into a buffer
     * use sscanf to parse it directly into the config
     * (possibly do some check on the input data so it doesn't give completely
     * absurd values, default to default values)
     *
     * maybe log/write the config data in use to the sd card, so that we can
     * later see exactly which configuration was used.
     */

}

