#include <stdlib.h>
#include <stdbool.h>
#include "microsd.h"
#include "config.h"

/* Tests the basic succesful case.
 */
int main() {
    bool success = config_init("config_example.txt");

    if (!success) {
        printf("Config init failed!\n");
        return -1;
    }

    printf("Ignition velocity:    EXPECTED=3.4,    ACTUAL=%f\n",
        IGNITION_VELOCITY);
    printf("Burnout acceleration: EXPECTED=7.3453, ACTUAL=%f\n",
        BURNOUT_ACCELERATION);
    printf("Burnout timer:        EXPECTED=459,    ACTUAL=%d\n",
        BURNOUT_TIMER);
    printf("Pyro drogue body 1:   EXPECTED=true,   ACTUAL=%s\n",
        PYRO_DROGUE_BODY_1 ? "true" : "false");

    return 0;
}
