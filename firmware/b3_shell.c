#include "b3_shell.h"
#include <hal.h>
#include "chprintf.h"
#include "adxl3x5.h"
#include "l3g4200d.h" 
#include "rfm69.h"
#include "ms5611.h"
#include "pyro.h"
#include "board.h"
#include "microsd.h"
#include "hmc5883l.h"
#include "config.h"

static void cmd_gps_passthrough(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    static const SerialConfig sc = {
        9600, 0, USART_CR2_STOP1_BITS | USART_CR2_LINEN, 0};
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

static void cmd_radio_tx(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)chp;
	(void)argv;
	(void)argc;
	
    char* msg1 = "a234567a";
    char* msg2 = "b765432b";
	
    while(1) {
        rfm69_send_test_packet(msg1);
        chThdSleepMilliseconds(50);
        rfm69_send_test_packet(msg2);
        chThdSleepMilliseconds(50);
	}
}

static void cmd_magnotest(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    (void)argc;
    for(;;) {
        /* chprintf(chp, "Magno X: 0x%x %d\r\n", global_magno[0]);
        chprintf(chp, "Magno Y: 0x%x %d\r\n", global_magno[1]);
        chprintf(chp, "Magno Z: 0x%x %d\r\n", global_magno[2]); */
        chprintf(chp,"%09d %09d %09d \n", global_magno[0], global_magno[1], global_magno[2]);
        chThdSleepMilliseconds(100);
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

    for(;;) {
        /*chprintf(chp," x: %09d, y: %09d, z: %09d \n", global_gyro[0], global_gyro[1], global_gyro[2]);*/
        chprintf(chp,"%09d %09d %09d \n", global_gyro[0], global_gyro[1], global_gyro[2]);
        chThdSleepMilliseconds(1);
    }
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

  chprintf(chp, "name            addr    stack prio refs     state   time\r\n");
  chprintf(chp, "--------------------------------------------------------\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%12s %.8lx %.8lx %4lu %4lu %9s %lu\r\n",
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

static void cmd_pyro(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    bool p1, p2, p3, p4;
    
    /* Continuity Check */

    p1 = pyro_continuity(GPIOE_PY1_CHK);
    p2 = pyro_continuity(GPIOE_PY2_CHK);
    p3 = pyro_continuity(GPIOE_PY3_CHK);
    p4 = pyro_continuity(GPIOE_PY4_CHK);
    
    chprintf(chp, "Results of Continuity Check: 1:%u, 2:%u, 3:%u, 4:%u\n",
             p1, p2, p3, p4);
                                                    
    if (argc > 0) 
    {
        if (argv[0][0] == '1') 
        {
            pyro_fire(1, 1000);
        }
        
        else if (argv[0][0] == '2') 
        {
            pyro_fire(2, 1000);
        }
        
        if (argv[0][0] == '3') 
        {
            pyro_fire(3, 1000);
        }  
      
        if (argv[0][0] == '4') 
        {
            pyro_fire(4, 1000);
        }
    }
}


static void cmd_test_all(BaseSequentialStream *chp, int argc, char *argv[]){
	
	chprintf(chp, " \n\nRunning thread function \n \n");
    chThdSleepMilliseconds(1000);
	cmd_threads(chp, argc, argv);

	chprintf(chp, "\n\nRunning runtime test \n \n");
    chThdSleepMilliseconds(1000);
	cmd_rt(chp, argc, argv);

	chprintf(chp, "\n\nRunning beep test");
    chThdSleepMilliseconds(1000);
	cmd_beep(chp, argc, argv);

	chprintf(chp, "\n\nRunning memory test \n \n");
    chThdSleepMilliseconds(1000);
	cmd_mem(chp, argc, argv);

	chprintf(chp, "\n\nRunning LED test \n \n");
    chThdSleepMilliseconds(1000);
	cmd_led(chp, argc, argv);

	chprintf(chp, "\n\nRunning accelerometer test \n \n");
    chThdSleepMilliseconds(1000);
	cmd_accel(chp, argc, argv);

	chprintf(chp, "\n\nRunning barometer test \n \n");
    chThdSleepMilliseconds(1000);
	cmd_barotest(chp, argc, argv);

	chprintf(chp, "\n\nRunning pyro channel test \n \n");
    chThdSleepMilliseconds(1000);
    cmd_pyro(chp, argc, argv);

	return;
}



static void cmd_microsd(BaseSequentialStream* chp, int argc, char* argv[])
{
    (void)argv;
    (void)argc;
    SDFILE file;
    SDFS sd;
    SDRESULT res;

    static const SDMODE mode = FA_CREATE_ALWAYS | FA_READ | FA_WRITE;
    static const int len = 90;
    static const char* test_string = "\n\tHere is a string.\n\tIt tests the \
functionality\n\tof the micro sd card\n\tOne two three test\n";
    char result[len];

    chprintf(chp, "Opening file ...\n");
    res = microsd_open_file(&file, "testfile", mode, &sd);
    if (res != FR_OK) goto error;

    chprintf(chp, "Writing test string to SD card ...\n");
    res = microsd_write(&file, test_string, len);
    if (res != FR_OK) goto error;

    chprintf(chp, "Reading the following from SD card ...\n");
    res = microsd_read(&file, result, len);
    if (res != FR_OK) goto error;
    chprintf(chp, "%s\n", result);

    chprintf(chp, "Closing file ...\n");
    res = microsd_close_file(&file);
    if (res != FR_OK) goto error;

    return;

    error:
        chprintf(chp, "SD result error!!! SDRESULT: %d\n", res);
}

static void cmd_config(BaseSequentialStream* chp, int argc, char* argv[])
{
    (void)argv;
    (void)argc;


    chprintf(chp, "stage           = %d\n", conf.stage          );
    chprintf(chp, "got_ignition    = %d\n", conf.got_ignition   );
    chprintf(chp, "got_separation  = %d\n", conf.got_separation );
    chprintf(chp, "got_drogue      = %d\n", conf.got_drogue     );
    chprintf(chp, "got_main        = %d\n", conf.got_main       );
    chprintf(chp, "accel_axis      = %d\n", conf.accel_axis     );
    chprintf(chp, "pyro_firetime   = %d ms\n", conf.pyro_firetime  );
    chprintf(chp, "pyro_1          = %d\n", conf.pyro_1         );
    chprintf(chp, "pyro_2          = %d\n", conf.pyro_2         );
    chprintf(chp, "pyro_3          = %d\n", conf.pyro_3         );
    chprintf(chp, "pyro_4          = %d\n", conf.pyro_4         );
    chprintf(chp, "ignition_accel  = %d m/s/s\n", conf.ignition_accel );
    chprintf(chp, "burnout_time    = %d ms\n", conf.burnout_time   );
    chprintf(chp, "ignite_altitude = %d m ASL\n", conf.ignite_altitude);
    chprintf(chp, "ignite_time     = %d ms\n", conf.ignite_time    );
    chprintf(chp, "ignite_timeout  = %d ms\n", conf.ignite_timeout );
    chprintf(chp, "apogee_time     = %d ms\n", conf.apogee_time    );
    chprintf(chp, "main_altitude   = %d m ASL\n", conf.main_altitude  );
    chprintf(chp, "main_time       = %d ms\n", conf.main_time      );
    chprintf(chp, "landing_time    = %d ms\n", conf.landing_time   );
    chprintf(chp, "use_radio       = %d\n", conf.use_radio      );
    chprintf(chp, "use_magno       = %d\n", conf.use_magno      );
    chprintf(chp, "use_gyro        = %d\n", conf.use_gyro       );
    chprintf(chp, "use_gps         = %d\n", conf.use_gps        );

}

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
    {"baro", cmd_barotest},
    {"test_all", cmd_test_all},
    {"pyro", cmd_pyro},
    {"microsd", cmd_microsd},
    {"magno", cmd_magnotest},
    {"config", cmd_config},
    {NULL, NULL}
};

void b3_shell_run()
{
    static const ShellConfig shell_cfg = {
      (BaseSequentialStream *)&SD2,
      commands
    };
    shellInit();
    sdStart(&SD2, NULL);
    shellCreate(&shell_cfg, 4096, NORMALPRIO);
}
