/* Pyrotechnics Channel
 */
 
 
#include <hal.h>
#include "pyro.h" 
#include "config.h"
#include "datalogging.h"


void pyro_off_1(void* arg);
void pyro_off_2(void* arg);
void pyro_off_3(void* arg);
void pyro_off_4(void* arg);

/* Checks the input channels for continuity.
 * The pad will be low if the e-match is intact
 * The pad will be high if the e-match is not present
 */

bool_t pyro_continuity(uint8_t pad)
{
    if((palReadPad(GPIOE, pad)) == PAL_LOW)
        return TRUE;
    else
        return FALSE;
}


/* Checks continuity of all pyro input channels
 * against the expected ones for the location of
 * the board
 */
bool_t pyro_continuity_check()
{
    bool ok = true;
    uint16_t ch1, ch2, ch3, ch4;

    ch1 = pyro_continuity(GPIOE_PY1_CHK);
    ch2 = pyro_continuity(GPIOE_PY2_CHK);
    ch3 = pyro_continuity(GPIOE_PY3_CHK);
    ch4 = pyro_continuity(GPIOE_PY4_CHK);

    ok &= (PYRO_1 && ch1) || (!PYRO_1 && !ch1);
    ok &= (PYRO_2 && ch2) || (!PYRO_2 && !ch2);
    ok &= (PYRO_3 && ch3) || (!PYRO_3 && !ch3);
    ok &= (PYRO_4 && ch4) || (!PYRO_4 && !ch4);

    log_s16(CHAN_PYRO_C, ch1, ch2, ch3, ch4);

    return ok;
}

/*
 * Fire the e-match pyro output channels for a specified time
 * NOTE: This is specifically for e-match channels, not metrons
 */

static VirtualTimer vt1, vt2, vt3, vt4;
void pyro_fire(uint8_t channel, uint16_t duration_ms)
{
    uint8_t pad;
    
    if(channel == 1) {
        pad = GPIOE_PY1_TRG;
        chVTReset(&vt1);
        chVTSet(&vt1, MS2ST(duration_ms), pyro_off_1, NULL);
        log_s16(CHAN_PYRO_F, 1, 0, 0, 0);
    } else if(channel == 2) {
        pad = GPIOE_PY2_TRG;
        chVTReset(&vt2);
        chVTSet(&vt2, MS2ST(duration_ms), pyro_off_2, NULL);
        log_s16(CHAN_PYRO_F, 0, 1, 0, 0);
        
    } else if(channel == 3) {
        pad = GPIOE_PY3_TRG;
        chVTReset(&vt3);
        chVTSet(&vt3, MS2ST(duration_ms), pyro_off_3, NULL);
        log_s16(CHAN_PYRO_F, 0, 0, 1, 0);
        
    } else if(channel == 4) {
        pad = GPIOE_PY4_TRG;
        chVTReset(&vt4);
        chVTSet(&vt4, MS2ST(duration_ms), pyro_off_4, NULL);
        log_s16(CHAN_PYRO_F, 0, 0, 0, 1);
        
    } else {
        return;
    }

    palSetPad(GPIOE, pad);
         
}


/*
 * Turn off a pyro channel. Called by the virtual timer.
 */
void pyro_off_1(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PY1_TRG);
}

void pyro_off_2(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PY2_TRG);
}

void pyro_off_3(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PY3_TRG);
}

void pyro_off_4(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PY4_TRG);
}



/* SPECIFIC FIRING FUNCTIONS */

/* Fire all `usage` pyros, 1=ignition 2=separation 3=drogue 4=main */
static void pyro_fire_specific(uint8_t usage)
{
    if(PYRO_1 == usage) {
        pyro_fire(1, PYRO_FIRETIME);
    }
    
    if(PYRO_2 == usage) {
        pyro_fire(2, PYRO_FIRETIME);
    }
    
    if(PYRO_3 == 3) {
        pyro_fire(3, PYRO_FIRETIME);
    }

    if(PYRO_4 == usage) {
        pyro_fire(4, PYRO_FIRETIME);
    }
}

/* Fire the drogue deployment pyros */
void pyro_fire_drogue()
{
    pyro_fire_specific(3);
}

/* Fire main chute deployment pyro */
void pyro_fire_main()
{
    pyro_fire_specific(4);
}

/* Fire separation pyro */
void pyro_fire_separation()
{
    pyro_fire_specific(2);
}


/* Fire motor ignition pyro */
void pyro_fire_ignite()
{
    pyro_fire_specific(1);
}

/* Continuously checks continuity
 * If at launch there's a problem, we should therefore know
 */
msg_t pyro_continuity_thread(void *arg)
{
    (void)arg;
    chRegSetThreadName("Pyros");
 
    while(TRUE) 
    {
        if(pyro_continuity_check()) 
        {
            palSetPad(GPIOD, GPIOD_PYRO_GRN);
            chThdSleepMilliseconds(100);
            palClearPad(GPIOD, GPIOD_PYRO_GRN);
            chThdSleepMilliseconds(400);
        } 
        else 
        {
            /* TODO: report sadness up the chain */
            palSetPad(GPIOD, GPIOD_PYRO_RED);
            chThdSleepMilliseconds(400);
            palClearPad(GPIOD, GPIOD_PYRO_RED);
            chThdSleepMilliseconds(100);
        }
        
    }
}
