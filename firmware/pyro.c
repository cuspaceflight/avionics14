/* Pyrotechnics Channel
 * Need to confirm pin/pad allocation
 * I've just defined a few different output channels
 * and have assumed we're using GPIO E (as Adam did)
 *
 * Output Channels:
 * GPIOE_PYRO_MAIN_F : for parachute. Pin 9
 * GPIOE_PYRO_SEPARATION_F : for separation. Pin 10
 * GPIOE_PYRO_DROGUE_F : for drogue. Pin 11
 * GPIOE_PYRO_SECOND_STAGE_F : for second stage. Pin 12
 *
 * Input Channels:
 * GPIOE_PYRO_MAIN_C : for parachute. Pin 5
 * GPIOE_PYRO_SEPARATION_C : for separation. Pin 6
 * GPIOE_PYRO_DROGUE_C : for drogue. Pin 7
 * GPIOE_PYRO_SECOND_STAGE_C : for second stage. Pin 8
 *
 *
 * In config file, we'll need to define:
 *
 * extern int board_location
 *
 * and:
 * 
 * #define TOP_BOARD = 1
 * #define BOTTOM_BOARD = 0
 * or something to that effect
 */
 
 
 
/* Header files need defining */ 
#include "pyro.h" 
#include "config.h"
#include "microsd.h"
#include <hal.h>

/* Not defined, so I've commented them out
*void pyro_off_1(void* arg);
*void pyro_off_2(void* arg);
*void pyro_off_3(void* arg);
*/

/* Checks the input channels for continuity.
 * The pad will be low if the e-match is intact
 * The pad will be high if the e-match is not present
 */

bool_t pyro_continuity(pyro_channel channel)
{
    if((palReadPad(pyro_channel channel) == PAL_LOW)
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
         p1 = int16_t(pyro_continuity(GPIOE_PYRO_MAIN_C));
         p2 = int16_t(pyro_continuity(GPIOE_PYRO_DROGUE_C));
         p3 = int16_t(pyro_continuity(GPIOE_PYRO_FIRE_SECOND_STAGE_C));
         /* log the results */
         /* microsd_log_s16(CHAN_PYRO_C, p1, p2, p3, 0); */
         /* return an indication of whether all of them are connected */
         return (p1 & p2 & p3) ;
    }
    else if (board_location == BOTTOM_BOARD)
    {
         p1 = int16_t(pyro_continuity(GPIOE_PYRO_MAIN_C));
         p2 = int16_t(pyro_continuity(GPIOE_PYRO_SEPARATION_C));
         /* log the results */
         /* microsd_log_s16(CHAN_PYRO_C, p1, p2, p3, 0); */
         /* return an indication of whether all of them are connected */
         return (p1 & p2) ;
    }


/*
 * Fire the pyro output channels for a specified time
 */
void pyro_fire(uint8_t channel, uint16_t duration_ms)
{
    palSetPad(GPIOE, channel);

    /* Log this event */
    /* microsd_log_s16(CHAN_PYRO_F, channel, 0); */
    chThdSleepMilliseconds(duration_ms);

    palClearPad(GPIOE, channel);

    chThdSleepMilliseconds(duration_ms);
}

/* SPECIFIC FIRING FUNCTIONS
 * Keep firing the circuit until continuity of that
 * particular firing channel breaks.
 * This indicates a successful fire.
 */

/* Fire the drogue deployment pyros */
void pyro_fire_drogue()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_DROGUE, 9) ;
        if(!pyro_continuity(GPIOE_PYRO_DROGUE))
            break;
    }
}

/* Fire  main chute deployment pyro */
void pyro_fire_main()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_MAIN, 9) ;
        if(!pyro_continuity(GPIOE_PYRO_MAIN))
            break;
    }
}

/* Fire  separation pyro */
void pyro_fire_separation()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_SEPARATION, 9) ;
        if(!pyro_continuity(GPIOE_PYRO_SEPARATION))
            break;
    }
}

/* Fire  second stage pyro */
void pyro_fire_second_stage()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_SECOND_STAGE, 9) ;
        if(!pyro_continuity(GPIOE_PYRO_SECOND_STAGE))
            break;
    }
}

/* Have left in the original implementation
 * Continuously checks continuity
 * If at launch there's a problem, we should therefore know
 */

 msg_t pyro_continuity_thread(void *arg)
 {
    (void)arg;
    chRegSetThreadName("Pyros");
 
    while(TRUE) 
    {
        if(pyro_continuities()) 
        {
            palSetPad(GPIOA, GPIOA_LED_PYROS);
            chThdSleepMilliseconds(10);
            palClearPad(GPIOA, GPIOA_LED_PYROS);
            chThdSleepMilliseconds(990);
        } 
        else 
        {
            /* TODO: report sadness up the chain */
            palSetPad(GPIOA, GPIOA_LED_PYROS);
            chThdSleepMilliseconds(100);
            palClearPad(GPIOA, GPIOA_LED_PYROS);
            chThdSleepMilliseconds(100);
        }
    }
 }
  
