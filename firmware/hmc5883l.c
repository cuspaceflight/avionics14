/*
 * HMC5883L Driver 
 * Cambridge University Spaceflight
 */

#include <stdlib.h>
#include "microsd.h"
#include "config.h"
#include "state_estimation.h"

#include "hmc5883l.h"  

#define HMC5883L_I2C_WRITE_ADDR    0x3C 
#define HMC5883L_I2C_READ_ADDR     0x3D

static Thread *tpHMC5883L = NULL;
static float counts_to_Tesla = 0.000000092 ;


/* Transmit data to sensor */
static bool_t hmc5883l_transmit(uint8_t *buf)
{
    size_t n = 2;  /* Transmit 2 bytes */
    systime_t timeout;
    msg_t rv;

    /* Determine timeout in systicks (ms) - UNSURE ABOUT THIS */
    timeout = n / 100 + 10; 

    /* Transmit message */
    rv = i2cMasterTransmitTimeout(&I2CD1, HMC5883L_I2C_WRITE_ADDR, buf, 
	                              n, NULL, 0, timeout);
								  
	if (rv == RDY_OK)
	    return TRUE;
	else
	    return FALSE;

}

/* Read data about magnetic field from sensor into buffer.
 * Do so through a transmit operation.
 * Place output data(6 bytes) into a (different) buffer
 */

static bool_t hmc5883l_receive(uint8_t *buf, int8_t *buf_data)
{
    msg_t rv;
	buf[0] = 0x06;
	buf[1] = NULL;
	
	/* Timeout is somewhat arbitrary- CHECK THIS */
	
    rv = i2cMasterTransmitTimeout(&I2CD1, HMC5883L_I2C_READ_ADDR, buf, 
	                              1, buf_data, 6, 100);
    
	if  (rv == RDY_OK)
	    return TRUE;
	else
	    return FALSE;
}

static bool_t hmc5883l_init(uint8_t *buf, int8_t *buf_data)
{
 	bool_t success;
  
    /* Configure Reg A - Highest Data Rate- 75 Hz */
    buf[0] = 0x00;
    buf[1] = 0x18;
    success = hmc5883l_transmit(buf) ; 
								  
    /* Configure Mode Register: High Speed, Continuous Measurement */
    buf[0] = 0x02;
    buf[1] = 0x00;
    success &= hmc5883l_transmit(buf) ;
	
	/* Wait for 20 ms whilst measurements are taken, then read them */
	
	chThdSleepMilliseconds(20) ;
	
	success &= hmc5883l_receive(buf, buf_data) ;
	
	return success;
  
}

/* 
 * buf_data contains 3 pairs of 8 bit entries.
 * Each pair corresponds to field strength along X, Y, and Z axes.
 * Function generates three 16 bit readings and converts them into
 * floats corresponding to field strength along each axis.
 * Multiplication by constant necessary to account for the 
 * conversion factor from counts measured into Tesla.
 */
static void hmc5883l_field_convert(int8_t *buf_data, float *field)
{
    int16_t temp ;
	
	temp = (buf_data[0] << 8 | buf_data[1] );
	field[0] = (float) temp * counts_to_Tesla;
	
	temp = (buf_data[2] << 8 | buf_data[3] );
	field[1] = (float) temp * counts_to_Tesla;	
	
	temp = (buf_data[4] << 8 | buf_data[5] ) ;
	field[2] = (float) temp * counts_to_Tesla;
}

/* 
 * Interrupt handler- wake up when DRDY deasserted 
 * Relevant pin needs defining in main/config
 */
void hmc5883l_wakeup(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;
	chSysLockFromIsr();
    if(tpHMC5883L != NULL) 
	{
        chSchReadyI(tpHMC5883L);
        tpHMC5883L = NULL;
    }
    chSysUnlockFromIsr();
}

msg_t hmc5883l_thread(void *arg)
{
    (void)arg;
	const int bufsize_1 = 2;
	const int bufsize_2 = 6;

	uint8_t buf[bufsize_1];
	int8_t buf_data[bufsize_2];
	float field[3] ;
	
	chRegSetThreadName("HMC5883L");
	
/* 
 * Reset Magno so it's in a known state 
 * Uncomment afer pin allocation completed
 */

    /* 
	 * palClearPad(GPIOB, GPIOB_GPS_RESET); 
     * chThdSleepMilliseconds(100);
     * palSetPad(GPIOB, GPIOB_GPS_RESET); 
     * chThdSleepMilliseconds(500);
	 */

    i2cStart(&I2CD1, &i2cconfig);

    if(!hmc5883l_init(buf, buf_data) 
	{
        while(1) chThdSleepMilliseconds(5);
    }
    
	i2cStart(&I2CD1, &i2cconfig);
	
	while(TRUE)
	{   
		if (hmc5883l_receive(buf, buf_data))
		{
		    hmc5883l_field_convert(buf_data, field);
			microsd_log_s16(CHAN_IMU_MAGNO /*define*/, 
			                field[0], field[1], field[2], 0);
			/*define this state estimation function */
            state_estimation_new_magno(field[0], 
			                           field[1], field[2]);
		}
		
		else
		    chThdSleepMilliseconds(20);
		
		
		/* Sleep until DRDY */
		chSysLock();
		tpHMC5883L = chThdSelf();
		chSchGoSleepTimeoutS(THD_STATE_SUSPENDED, 100);
		chSysUnlock();
	}
	
    return (msg_t)NULL;
}
