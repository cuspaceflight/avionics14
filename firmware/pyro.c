/* Pyrotechnics Channel
 * Need to do pin/pad allocation
 * I've just defined a few different output channels
 * and have assumed we're using GPIO E (as Adam did)
 *
 * Output Channels:
 * GPIOE_PYRO_MAIN : for parachute. 
 * GPIOE_PYRO_SEPARATION : for separation
 * GPIOE_PYRO_DROGUE : for drogue
 * GPIOE_PYRO_SECOND_STAGE : for second stage
 *
 * Input Channels:
 * Need proper defining- don't quite understand how they work.
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
 * I've put this in pseudocode (ish) for now 
 *
 *
 * bool_t pyro_continuity(pyro_channel channel)
 * Check channel for continuity; ie, if they're connected.
 * if good return TRUE
 * else return FALSE
 */
 

/* Checks continuity of all pyro input channels
 * Again, in pseudocode (ish)
 *
 *
 * bool_t pyro_continuities()
 * pyro_continuity (channel 1)
 * pyro_continutiy (channel 2)
 * continue checking...
 * if continuity is correct according to config
 *     return TRUE
 * else
 *     return FALSE
 */

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


/* Fire the drogue deployment pyros */
void pyro_fire_drogue()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_DROGUE, 9)
        if(!pyro_continuities())
            break;
    }
}

/* Fire  main chute deployment pyro */
void pyro_fire_main()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_MAIN, 9)
        if(!pyro_continuities())
            break;
    }
}

/* Fire  separation pyro */
void pyro_fire_separation()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_SEPARATION, 9)
        if(!pyro_continuities())
            break;
    }
}

/* Fire  second stage pyro */
void pyro_fire_second_stage()
{
    int i;
    for(i=0; i<50; i++) 
    {  
        pyro_fire(GPIOE_PYRO_SECOND_STAGE, 9)
        if(!pyro_continuities())
            break;
    }
}

/* Do we need this? Not quite sure
 * msg_t pyro_continuity_thread(void *arg) {
 *   (void)arg;
 *  chRegSetThreadName("Pyros");
 *
 *   while(TRUE) {
 *       if(pyro_continuities()) {
 *           palSetPad(GPIOA, GPIOA_LED_PYROS);
 *           chThdSleepMilliseconds(10);
 *           palClearPad(GPIOA, GPIOA_LED_PYROS);
 *           chThdSleepMilliseconds(990);
 *       } else {
 *           /* TODO: report sadness up the chain */
 *           palSetPad(GPIOA, GPIOA_LED_PYROS);
 *           chThdSleepMilliseconds(100);
 *           palClearPad(GPIOA, GPIOA_LED_PYROS);
 *           chThdSleepMilliseconds(100);
 *       }
 *   }
 *}
 */
