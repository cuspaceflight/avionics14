/* Pyrotechnics Channel
 *
 * NOTE: Separation requires 2 channels 
 * (both input & output)
 *
 * Output Channels:
 * GPIOE_PYRO_MAIN_F : for parachute. 
 * GPIOE_PYRO_SEPARATION_1_F : for separation. 
 * GPIOE_PYRO_SEPARATION_2_F : for separation in bottom stage
 * GPIOE_PYRO_FIRE_SECOND_STAGE_F   : for launching second stage in top
 * GPIOE_PYRO_DROGUE_F : for drogue. 
 *
 * Input Channels:
 * GPIOE_PYRO_MAIN_C : for parachute. 
 * GPIOE_PYRO_SEPARATION_1_C : for separation.
 * GPIOE_PYRO_SEPARATION_2_F : for separation in bottom stage
 * GPIOE_PYRO_FIRE_SECOND_STAGE_F  : for launching second stage in top
 * GPIOE_PYRO_DROGUE_C : for drogue. 
 */

 
 
/* Header files need defining */ 
#include "pyro.h" 
/* #include "microsd.h" */
#include "config.h"
#include <hal.h>
#include "board.h"


#define GPIOE_PYRO_MAIN_C                 GPIOE_PY1_CHK              
#define GPIOE_PYRO_MAIN_F                 GPIOE_PY1_TRG       
#define GPIOE_PYRO_SEPARATION_1_F         GPIOE_PY2_TRG                
#define GPIOE_PYRO_SEPARATION_1_C         GPIOE_PY2_CHK    
#define GPIOE_PYRO_SEPARATION_2_C         GPIOE_PY3_CHK               
#define GPIOE_PYRO_SEPARATION_2_F         GPIOE_PY3_TRG
#define GPIOE_PYRO_FIRE_SECOND_STAGE_C    GPIOE_PY3_CHK               
#define GPIOE_PYRO_FIRE_SECOND_STAGE_F    GPIOE_PY3_TRG                 
#define GPIOE_PYRO_DROGUE_F               GPIOE_PY4_TRG               
#define GPIOE_PYRO_DROGUE_C               GPIOE_PY4_CHK  


/* TODO: Tidy this up and put into one function? */
void pyro_off_1(void* arg);
void pyro_off_2(void* arg);
void pyro_off_3(void* arg);
void pyro_off_4(void* arg);

int board_location = TOP_BOARD ; /* CHANGE THIS IF NECESSARY */

/* Checks the input channels for continuity.
 * The pad will be low if the e-match is intact
 * The pad will be high if the e-match is not present
 */

bool_t pyro_continuity(uint8_t pad)
{
    if((palReadPad(GPIOE, pad)) == PAL_LOW)
        return TRUE ;
    else
        return FALSE ;
}


/* Checks continuity of all pyro input channels
 * against the expected ones for the location of
 * the board
 */
 
bool_t pyro_continuity_check()
{
    int16_t p1, p2, p3 = 0;
    
    /* check continuity of expected connections */
    if(board_location == TOP_BOARD)
    {
         p1 = (int16_t)pyro_continuity(GPIOE_PYRO_MAIN_C);
         p2 = (int16_t)pyro_continuity(GPIOE_PYRO_DROGUE_C);
         p3 = (int16_t)pyro_continuity(GPIOE_PYRO_FIRE_SECOND_STAGE_C);
         /* log the results */
         /* microsd_log_s16(CHAN_PYRO_C, p1, p2, p3, 0); */
         /* return an indication of whether all of them are connected */
         return (p1 && p2 && p3) ;
    }
    else if (board_location == BOTTOM_BOARD)
    {
         p1 = (int16_t)pyro_continuity(GPIOE_PYRO_MAIN_C);
         p2 = (int16_t)pyro_continuity(GPIOE_PYRO_SEPARATION_1_C);
         p3 = (int16_t)pyro_continuity(GPIOE_PYRO_SEPARATION_2_C);
         /* log the results */
         /* microsd_log_s16(CHAN_PYRO_C, p1, p2, p3, 0); */
         /* return an indication of whether all of them are connected */
         return (p1 && p2 && p3) ;
    }
    return false;
}

/*
 * Fire the e-match pyro output channels for a specified time
 * NOTE: This is specifically for e-match channels, not metrons
 */


static VirtualTimer vt1, vt2, vt3, vt4;
void pyro_fire(uint8_t channel, uint16_t duration_ms)
{
    uint8_t pad, pad_2 = 0;
    bool fire_separation = FALSE;

    if(channel == GPIOE_PYRO_DROGUE_F) {
        pad = GPIOE_PYRO_DROGUE_F;
        chVTReset(&vt1);
        chVTSet(&vt1, MS2ST(duration_ms), pyro_off_1, NULL);
        /* microsd_log_s16(GPIOE_PYRO_DROGUE_F, 1, 0, 0, 0); */
    } else if(channel == GPIOE_PYRO_MAIN_F) {
        pad = GPIOE_PYRO_MAIN_F;
        chVTReset(&vt2);
        chVTSet(&vt2, MS2ST(duration_ms), pyro_off_2, NULL);
        /* microsd_log_s16(CHAN_PYRO_F, 0, 1, 0, 0);*/
    } else if(channel == GPIOE_PYRO_SEPARATION_1_F) {
        pad = GPIOE_PYRO_SEPARATION_1_F;
        pad_2 = GPIOE_PYRO_SEPARATION_2_F;
        fire_separation = TRUE;
        chVTReset(&vt3);
        chVTSet(&vt3, MS2ST(duration_ms), pyro_off_3, NULL);
        /*microsd_log_s16(CHAN_PYRO_F, 0, 0, 1, 0);*/
    } else if(channel == GPIOE_PYRO_FIRE_SECOND_STAGE_F) {
        pad = GPIOE_PYRO_FIRE_SECOND_STAGE_F;
        chVTReset(&vt4);
        chVTSet(&vt4, MS2ST(duration_ms), pyro_off_4, NULL);
        /*microsd_log_s16(CHAN_PYRO_F, 0, 0, 1, 0);*/    
    } else {
        return;
    }

    palSetPad(GPIOE, pad);
    if (fire_separation) 
    {
        palSetPad(GPIOE, pad_2);
    }
         
}


/*
 * Turn off a pyro channel. Called by the virtual timer.
 */
void pyro_off_1(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PYRO_DROGUE_F);
}

void pyro_off_2(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PYRO_MAIN_F);
}

void pyro_off_3(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PYRO_SEPARATION_1_F);
    palClearPad(GPIOE, GPIOE_PYRO_SEPARATION_2_F);
}


void pyro_off_4(void* arg)
{
    (void)arg;
    palClearPad(GPIOE, GPIOE_PYRO_FIRE_SECOND_STAGE_F);
}



/* SPECIFIC FIRING FUNCTIONS */

/* Fire the drogue deployment pyros */

void pyro_fire_drogue()
{
    pyro_fire(GPIOE_PYRO_DROGUE_F, 1000) ;

}

/* Fire  main chute deployment pyro */
void pyro_fire_main()
{
    pyro_fire(GPIOE_PYRO_MAIN_F, 1000) ;

}

/* Fire  separation pyro */
void pyro_fire_separation()
{
    pyro_fire(GPIOE_PYRO_SEPARATION_1_F, 1000);
   
}


/* Fire  second stage pyro */
void pyro_fire_second_stage()
{
    pyro_fire(GPIOE_PYRO_FIRE_SECOND_STAGE_F, 1000) ;

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
            chThdSleepMilliseconds(1000);
            palClearPad(GPIOD, GPIOD_PYRO_GRN);
            chThdSleepMilliseconds(1000);
        } 
        else 
        {
            /* TODO: report sadness up the chain */
            palSetPad(GPIOD, GPIOD_PYRO_RED);
            chThdSleepMilliseconds(1000);
            palClearPad(GPIOD, GPIOD_PYRO_RED);
            chThdSleepMilliseconds(1000);
        }
        
    }
    return (msg_t)NULL;
 }
  
