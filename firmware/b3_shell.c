#include "b3_shell.h"
#include <hal.h>
#include "chprintf.h"
#include "pyro.h"
#include "board.h"

static void cmd_beep(BaseSequentialStream *chp, int argc, char *argv[]) {
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

static void cmd_pyro(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void)argv;
    bool p1, p2, p3, p4 ;
    
    /* Continuity Check */
    p1 = pyro_continuity(GPIOE_PY1_CHK);
    p2 = pyro_continuity(GPIOE_PY2_CHK);
    p3 = pyro_continuity(GPIOE_PY3_CHK);
    p4 = pyro_continuity(GPIOE_PY4_CHK);
    
    chprintf(chp, "Results of Continuity Check: %u, %u, %u, %u\n", p1,
                                                    p2, p3, p4);
                                                    
    if (argc > 0) 
    {
        if (argv[1] == "1") 
        {
            pyro_fire_drogue();
        }
        
        else if (argv[1] == "2") 
        {
            pyro_fire_main();
        }
        
        if (argv[1] == "3") 
        {
            pyro_fire_separation();
        }  
      
        if (argv[1] == "4") 
        {
            pyro_fire_second_stage();
        }
    }
}

void b3_shell_run()
{
    static const ShellCommand commands[] = {
        {"mem", cmd_mem},
        {"threads", cmd_threads},
        {"rt", cmd_rt},
        {"beep", cmd_beep},
        {"pyro", cmd_pyro},
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
