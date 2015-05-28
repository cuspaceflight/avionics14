#include "ch.h"
#include "hal.h"
#include "test.h"

/*
#include "ms5611.h"
#include "adxl3x5.h"
#include "hmc5883l.h"
#include "l3g4200d.h"
#include "b3_shell.h"

static WORKING_AREA(waIdleThread, 128);
static WORKING_AREA(waMS5611, 512);
static WORKING_AREA(waADXL345, 512);
static WORKING_AREA(waHMC5883L, 512);
static WORKING_AREA(waL3G4200D, 512);

*/

static WORKING_AREA(waI2CTest, 256);
msg_t i2c_test(void* args)
{
    (void)args;
    static const I2CConfig i2cconfig = {
        OPMODE_I2C, 100000, STD_DUTY_CYCLE
    };

    i2cStart(&I2CD2, &i2cconfig);

    uint8_t txbuf[32];
    uint8_t rxbuf[32];
    int16_t x, y, z;
    msg_t rv;

    txbuf[0] = 0x02;
    txbuf[1] = 0x01;
    rv = i2cMasterTransmit(&I2CD2, 0x1E, txbuf, 2, rxbuf, 0);

    txbuf[0] = 0x00;
    txbuf[1] = 0x11;
    rv = i2cMasterTransmit(&I2CD2, 0x1E, txbuf, 2, rxbuf, 0);

    /*txbuf[0] = 0xA0;*/
    /*rv = i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 1, rxbuf, 3, 500);*/

    while(1) {
        chThdSleepMilliseconds(200);

        txbuf[0] = 0x03;
        rv = i2cMasterTransmitTimeout(&I2CD2, 0x1E, txbuf, 1, rxbuf, 6, 500);

        x = rxbuf[0] << 8 | rxbuf[1];
        y = rxbuf[4] << 8 | rxbuf[5];
        z = rxbuf[2] << 8 | rxbuf[3];

        txbuf[0] = 0x02;
        txbuf[1] = 0x01;
        rv = i2cMasterTransmit(&I2CD2, 0x1E, txbuf, 2, rxbuf, 0);

    }

    return 0;
}

int main(void) {
    halInit();
    chSysInit();
    chRegSetThreadName("main");

    /*extStart(&EXTD1, &extcfg);*/

    chThdCreateStatic(waI2CTest, sizeof(waI2CTest), NORMALPRIO, i2c_test, NULL);

    /*chThdCreateStatic(waIdleThread, sizeof(waIdleThread), NORMALPRIO, IdleThread, NULL);*/

    /*chThdCreateStatic(waMS5611, sizeof(waMS5611), NORMALPRIO,*/
                      /*ms5611_thread, NULL);*/

    /*chThdCreateStatic(waADXL345, sizeof(waADXL345), NORMALPRIO,*/
                      /*adxl345_thread, NULL);*/

    /*chThdCreateStatic(waHMC5883L, sizeof(waHMC5883L), NORMALPRIO,*/
                      /*hmc5883l_thread, NULL);*/

    /*chThdCreateStatic(waL3G4200D, sizeof(waL3G4200D), NORMALPRIO,*/
                      /*l3g4200d_thread,NULL);*/

    /*b3_shell_run();*/


    while (TRUE) {
        palSetPad(GPIOD, GPIOD_PYRO_GRN);
        palClearPad(GPIOD, GPIOD_PYRO_RED);
        chThdSleepMilliseconds(500);
        
        palClearPad(GPIOD, GPIOD_PYRO_GRN);
        chThdSleepMilliseconds(500);
    }
}
