#include "b3_shell.h"
#include <hal.h>
#include "chprintf.h"
#include "microsd.h"

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
}

static void cmd_beep(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)chp;
    (void)argv;
    (void)argc;

    palSetPad(GPIOA, GPIOA_BUZZER);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOA, GPIOA_BUZZER);
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

static void cmd_microsd(BaseSequentialStream* chp, int argc, char* argv[])
{
    (void)argv;
    (void)argc;
    SDFILE file;
    SDFS sd;
    SDRESULT res;

    int len = 110;
    char result[len];
    char* test_string = "Here is a string.\nIt tests the functionality\n\
of the micro sd card\nIf this comes out right, it was successful.\n";

    chprintf(chp, "Opening file\n");
    res = microsd_open_file(&file, "testfile", FA_CREATE_ALWAYS, &sd);
    if (res != FR_OK) {
        chprintf(chp, "returned error! %d\n", res);
        return;
    }

    chprintf(chp, "Writing test string to sd\n");
    res = microsd_write(&file, test_string, len);
    if (res != FR_OK) {
        chprintf(chp, "returned error! %d\n", res);
        return;
    }

    chprintf(chp, "Reading the following from sd:\n");
    res = microsd_read(&file, result, len);
    chprintf(chp, "%s\n", result);
    if (res != FR_OK) {
        chprintf(chp, "returned error! %d\n", res);
        return;
    }

    chprintf(chp, "Closing file\n");
    res = microsd_close_file(&file);
    if (res != FR_OK) {
        chprintf(chp, "returned error! %d\n", res);
        return;
    }
}

void b3_shell_run()
{
    static const ShellCommand commands[] = {
        {"mem", cmd_mem},
        {"threads", cmd_threads},
        {"rt", cmd_rt},
        {"beep", cmd_beep},
        {"gps_passthrough", cmd_gps_passthrough},
        {"microsd", cmd_microsd},
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
