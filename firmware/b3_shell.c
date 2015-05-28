#include "b3_shell.h"
#include <hal.h>
#include "chprintf.h"
#include "adxl3x5.h"
#include "l3g4200d.h" 
#include "rfm69.h"
#include "ms5611.h"


static void cmd_gps_passthrough(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    static const SerialConfig sc = {9600};
    sdStart(&SD1, &sc);
    EventListener elSerData;
    flagsmask_t flags;
    chEvtRegisterMask(chnGetEventSource(&SD1), &elSerData, EVENT_MASK(1));

    while (TRUE)
    {
       chEvtWaitOneTimeout(EVENT_MASK(1), MS2ST(10));
       flags = chEvtGetAndClearFlags(&elSerData);
       if (flags & CHN_INPUT_AVAILABLE)
       {
          msg_t charbuf;
          do
          {
             charbuf = chnGetTimeout(&SD1, TIME_IMMEDIATE);
             if ( charbuf != Q_TIMEOUT )
             {
               chSequentialStreamPut(chp, charbuf);
             }
          }
          while (charbuf != Q_TIMEOUT);
       }
    }


static void cmd_barotest(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    (void)argc;
    
    chprintf(chp, "Current Pressure: %d\r\n", global_pressure);
    chprintf(chp, "Current Temperature: %d\r\n", global_temperature);
}

static void cmd_beep(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)chp;
    (void)argv;
    (void)argc;

    palSetPad(GPIOA, GPIOA_BUZZER);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOA, GPIOA_BUZZER);
}


static WORKING_AREA(waRadio, 512);
	
static void cmd_radio_tx(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)chp;
	(void)argv;
	(void)argc;
	
	char* data = "Hello  ";
	chThdCreateStatic(waRadio, sizeof(waRadio), NORMALPRIO, rfm69_thread, NULL);
	
	int i;
	for (i = 0; i<10; i++) {
		chThdSleepMilliseconds(500);
        rfm69_log_c(8, data);
	}
}

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }

  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_led(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argc;	
    (void)argv;

    palSetPad(GPIOD, GPIOD_PYRO_GRN);
    chprintf(chp, "PYRO_LED_GRN is lit \n");
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, GPIOD_PYRO_GRN);


    palSetPad(GPIOD, GPIOD_PYRO_RED);
    chprintf(chp, "PYRO_LED_RED is lit \n");
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, GPIOD_PYRO_RED);

    palSetPad(GPIOD, GPIOD_RADIO_GRN);
    chprintf(chp, "RADIO_LED_GRN is lit \n");
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, GPIOD_RADIO_GRN);

    palSetPad(GPIOD, GPIOD_RADIO_RED);
    chprintf(chp, "RADIO_LED_RED is lit \n");
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, GPIOD_RADIO_RED);

    palSetPad(GPIOD, GPIOD_IMU_GRN);
    chprintf(chp, "IMU_LED_GRN is lit \n");
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, GPIOD_IMU_GRN);

    palSetPad(GPIOD, GPIOD_IMU_RED);
    chprintf(chp, "IMU_LED_GRN is lit \n");
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOD, GPIOD_IMU_RED);
    
    return;

}

static void cmd_gyro(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argc;	
    (void)argv;

    chprintf(chp," x: %d, y: %d, z: %d \n", global_gyro[0], global_gyro[1], global_gyro[2]);
    return;
}

static void cmd_accel(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argc;	
    (void)argv;

    chprintf(chp," x: %d, y: %d, z: %d \n", global_accel[0], global_accel[1], global_accel[2]);
    return;
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {THD_STATE_NAMES};
  uint64_t busy = 0, total = 0;
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }

  chprintf(chp, "name          addr    stack prio refs     state   time\r\n");
  chprintf(chp, "------------------------------------------------------\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%10s %.8lx %.8lx %4lu %4lu %9s %lu\r\n",
            chRegGetThreadName(tp),
            (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
            (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
            states[tp->p_state], (uint32_t)tp->p_time);
    if(tp->p_prio != 1) {
        busy += tp->p_time;
    }
    total += tp->p_time;
    tp = chRegNextThread(tp);
  } while (tp != NULL);
  chprintf(chp, "CPU Usage: %ld%%\r\n", busy*100/total);
}

static void cmd_rt(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    if(argc > 0) {
        chprintf(chp, "Usage: rt\r\n");
        chprintf(chp, "Prints current realtime clock ticks and frequency\r\n");
    }
    halrtcnt_t t = halGetCounterValue();
    halclock_t f = halGetCounterFrequency();
    chprintf(chp, "Current real time clock ticks: %u\r\n", t);
    chprintf(chp, "Real time clock frequency: %u\r\n", f);
}

void b3_shell_run()
{
    static const ShellCommand commands[] = {
        {"mem", cmd_mem},
        {"threads", cmd_threads},
        {"rt", cmd_rt},
        {"beep", cmd_beep},
        {"gps_passthrough", cmd_gps_passthrough},
        {"radio_tx", cmd_radio_tx},
        {"led", cmd_led},
        {"accel", cmd_accel},
        {"gyro", cmd_gyro},
        {"barotest", cmd_barotest},
        {NULL, NULL}
    };
    static const ShellConfig shell_cfg = {
      (BaseSequentialStream *)&SD2,
      commands
    };
    shellInit();
    sdStart(&SD2, NULL);
    shellCreate(&shell_cfg, 2048, NORMALPRIO);
}
