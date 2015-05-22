/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "ms5611.h"
#include "adxl3x5.h"
#include "hmc5883l.h"
#include "l3g4200d.h"

static WORKING_AREA(waMS5611, 512);
static WORKING_AREA(waADXL345, 512);
static WORKING_AREA(waHMC5883L, 512);
static WORKING_AREA(waL3G4200D, 512);

/*
 * Set up pin change interrupts for the various sensors that react to them.
 */
static const EXTConfig extcfg = {{
    {EXT_CH_MODE_AUTOSTART | EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOE,
        l3g4200d_wakeup}, /* Pin 0 - PE0 is Gyro DRDY */
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
        /*l3g4200d_wakeup}, [> Pin 14 - PE14 is the gyro DRDY <]*/
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

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_PYRO_GRN);
    palSetPad(GPIOD, GPIOD_RADIO_GRN);
    palSetPad(GPIOD, GPIOD_IMU_GRN);
    palSetPad(GPIOA, GPIOA_BUZZER);
    palClearPad(GPIOD, GPIOD_PYRO_RED);
    palClearPad(GPIOD, GPIOD_RADIO_RED);
    palClearPad(GPIOD, GPIOD_IMU_RED);
    chThdSleepMilliseconds(500);
    
    palClearPad(GPIOD, GPIOD_PYRO_GRN);
    palClearPad(GPIOD, GPIOD_RADIO_GRN);
    palClearPad(GPIOD, GPIOD_IMU_GRN);
    palClearPad(GPIOA, GPIOA_BUZZER);
    palSetPad(GPIOD, GPIOD_PYRO_RED);
    palSetPad(GPIOD, GPIOD_RADIO_RED);
    palSetPad(GPIOD, GPIOD_IMU_RED);
    chThdSleepMilliseconds(500);
  }
}

int main(void) {
  halInit();
  chSysInit();


  uint8_t cont_counter = 0;
  while(0) {
      if(palReadPad(GPIOE, GPIOE_PY1_CHK)) {
          /* No continuity */
          palClearPad(GPIOD, GPIOD_PYRO_GRN);
          palSetPad(GPIOD, GPIOD_PYRO_RED);
          cont_counter = 0;
      } else {
          /* Continuity */
          palSetPad(GPIOD, GPIOD_PYRO_GRN);
          palClearPad(GPIOD, GPIOD_PYRO_RED);
          cont_counter += 1;
          if(cont_counter == 50) {
              palSetPad(GPIOD, GPIOD_PYRO_RED);
              palSetPad(GPIOD, GPIOD_RADIO_RED);
              palSetPad(GPIOD, GPIOD_IMU_RED);
              palClearPad(GPIOD, GPIOD_PYRO_GRN);
              palClearPad(GPIOD, GPIOD_RADIO_GRN);
              palClearPad(GPIOD, GPIOD_IMU_GRN);

              palSetPad(GPIOE, GPIOE_PY1_TRG);
              chThdSleepMilliseconds(1000);
              palClearPad(GPIOE, GPIOE_PY1_TRG);
              cont_counter = 0;
              palClearPad(GPIOD, GPIOD_PYRO_RED);
              palClearPad(GPIOD, GPIOD_RADIO_RED);
              palClearPad(GPIOD, GPIOD_IMU_RED);
          }
      }
      chThdSleepMilliseconds(100);
  }

    /* Activate the EXTI pin change interrupts */
    /*extStart(&EXTD1, &extcfg);*/

  /*
   * If the user button is pressed after the reset then the test suite is
   * executed immediately before activating the various device drivers in
   * order to not alter the benchmark scores.
   */
    /*TestThread(&SD2);*/

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    /*chThdCreateStatic(waMS5611, sizeof(waMS5611), NORMALPRIO,*/
                      /*ms5611_thread, NULL);*/

    /*chThdCreateStatic(waADXL345, sizeof(waADXL345), NORMALPRIO,*/
                      /*adxl345_thread, NULL);*/

    /*chThdCreateStatic(waHMC5883L, sizeof(waHMC5883L), NORMALPRIO,*/
                      /*hmc5883l_thread, NULL);*/

    /*chThdCreateStatic(waL3G4200D, sizeof(waL3G4200D), NORMALPRIO,*/
                      /*l3g4200d_thread,NULL);*/

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
