#include "b3_shell.h"
#include <hal.h>
#include "chprintf.h"
#include "hmc5883l.h"

static void cmd_beep(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    (void)argc;

    palSetPad(GPIOA, GPIOA_BUZZER);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOA, GPIOA_BUZZER);
}

static void cmd_magnotest(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    (void)argc;
    
    chprintf(chp, "Magno X: 0x%x %d\r\n", global_magnoxyz[0]);
    chprintf(chp, "Magno Y: 0x%x %d\r\n", global_magnoxyz[1]);
    chprintf(chp, "Magno Z: 0x%x %d\r\n", global_magnoxyz[2]);
    chprintf(chp, "Wait 2 seconds \n");
    chThdSleepMilliseconds(2000);
    chprintf(chp, "Run 2: \n");
    chprintf(chp, "Magno X: 0x%x %d\r\n", global_magnoxyz[0]);
    chprintf(chp, "Magno Y: 0x%x %d\r\n", global_magnoxyz[1]);
    chprintf(chp, "Magno Z: 0x%x %d\r\n", global_magnoxyz[2]);

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

void b3_shell_run()
{
    static const ShellCommand commands[] = {
        {"mem", cmd_mem},
        {"threads", cmd_threads},
        {"rt", cmd_rt},
        {"beep", cmd_beep},
        {"magnotest", cmd_magnotest},
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
