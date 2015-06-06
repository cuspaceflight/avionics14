#include "ch.h"
#include "hal.h"
#include "test.h"

#include "rfm69.h"
#include "ms5611.h"
#include "adxl3x5.h"
#include "hmc5883l.h"
#include "l3g4200d.h"
#include "b3_shell.h"
#include "ublox.h"
#include "rfm69.h"
#include "state_estimation.h"
#include "mission_control.h"
#include "datalogging.h"
#include "config.h"
#include "pyro.h"
#include "tweeter.h"

static WORKING_AREA(waMS5611, 512);
static WORKING_AREA(waADXL345, 512);
/*static WORKING_AREA(waHMC5883L, 512);*/
static WORKING_AREA(waL3G4200D, 1024);
static WORKING_AREA(waGPS, 4096);
static WORKING_AREA(waRadio, 1024);
static WORKING_AREA(waDatalogging, 2048);
static WORKING_AREA(waPyro, 2048);
static WORKING_AREA(waConfig, 4096);
static WORKING_AREA(waMission, 1024);
static WORKING_AREA(waTweeter, 512);

/*
 * Set up pin change interrupts for the various sensors that react to them.
 */
static const EXTConfig extcfg = {{
    {EXT_CH_MODE_AUTOSTART | EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOE,
        l3g4200d_wakeup},         /* Pin 0 - PE0 is Gyro DRDY */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 1 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 2 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 3 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 4 */
    {EXT_CH_MODE_AUTOSTART | EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOC,
        adxl345_wakeup},          /* Pin 5 - PC5 is the accel INT1 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 6 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 7 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 8 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 9 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 10 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 11 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 12 */
    {EXT_CH_MODE_DISABLED, NULL}, /* Pin 13 */
    {EXT_CH_MODE_AUTOSTART | EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOE, NULL},
    {EXT_CH_MODE_AUTOSTART | EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOE,
        hmc5883l_wakeup}, /* Pin 0 - PE15 is the magnetometer DRDY */
    {EXT_CH_MODE_DISABLED, NULL}, /* 16 - PVD */
    {EXT_CH_MODE_DISABLED, NULL}, /* 17 - RTC Alarm */
    {EXT_CH_MODE_DISABLED, NULL}, /* 18 - USB OTG FS Wakeup */
    {EXT_CH_MODE_DISABLED, NULL}, /* 19 - Ethernet Wakeup */
    {EXT_CH_MODE_DISABLED, NULL}, /* 20 - USB OTG HS Wakeup */
    {EXT_CH_MODE_DISABLED, NULL}, /* 21 - RTC Tamper/Timestamp */
    {EXT_CH_MODE_DISABLED, NULL}  /* 22 - RTC Wakeup */
}};

int main(void) {
    halInit();
    chSysInit();
    chRegSetThreadName("Main");

    state_estimation_init();

    b3_shell_run();

    chThdCreateStatic(waTweeter, sizeof(waTweeter), NORMALPRIO, tweeter_thread, NULL);

    chThdCreateStatic(waConfig, sizeof(waConfig), NORMALPRIO, config_thread, NULL);

    chThdSleepMilliseconds(500);

    chThdCreateStatic(waDatalogging, sizeof(waDatalogging), NORMALPRIO,
                      datalogging_thread, NULL);

    chThdCreateStatic(waMS5611, sizeof(waMS5611), NORMALPRIO,
                      ms5611_thread, NULL);

    chThdCreateStatic(waADXL345, sizeof(waADXL345), NORMALPRIO,
                      adxl345_thread, NULL);

    /*chThdCreateStatic(waHMC5883L, sizeof(waHMC5883L), NORMALPRIO,*/
                      /*hmc5883l_thread, NULL);*/

    chThdCreateStatic(waRadio, sizeof(waRadio), NORMALPRIO, rfm69_thread, NULL);                  
    
    chThdCreateStatic(waL3G4200D, sizeof(waL3G4200D), NORMALPRIO,
                      l3g4200d_thread, NULL);

    chThdCreateStatic(waGPS, sizeof(waGPS), NORMALPRIO, ublox_thread, NULL);

    chThdCreateStatic(waPyro, sizeof(waPyro), NORMALPRIO, pyro_thread, NULL);

    chThdCreateStatic(waMission, sizeof(waMission), NORMALPRIO,
            mission_thread, NULL);


    extStart(&EXTD1, &extcfg);


    while (TRUE) {
        palSetPad(GPIOD, GPIOD_IMU_GRN);
        chThdSleepMilliseconds(500);
        palClearPad(GPIOD, GPIOD_IMU_GRN);
        chThdSleepMilliseconds(500);
    }
}
