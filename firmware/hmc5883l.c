/*
 * HMC5883L Driver 
 * Cambridge University Spaceflight
 */

#include <stdlib.h>


#include "hal.h"

#include "hmc5883l.h"  

#define HMC5883L_I2C_ADDR    0x1E

static Thread *tpHMC5883L = NULL;
static float counts_to_Tesla = 9.2E-8;

uint16_t global_magnoxyz[3];

/* TODO: Validate timings against AN4235 */
/* Magic I2C timing numbers. Computed via reference manual. */
static const I2CConfig i2cconfig = {
	OPMODE_I2C, 10000, STD_DUTY_CYCLE
};


/* Transmit data to sensor (used in init function only) */
static bool_t hmc5883l_transmit(uint8_t *buf)
{
    size_t n = 2;  /* Transmit 2 bytes */
    msg_t rv;

    /* Transmit message */
    rv = i2cMasterTransmitTimeout(&I2CD2, HMC5883L_I2C_ADDR, buf, n, NULL, 0, 1000);
    
	if (rv == RDY_OK)
	    return TRUE;
	else
	    return FALSE;

}

/* Read data about magnetic field from sensor into buffer.
 * Do so through a transmit operation.
 * Place output data(6 bytes) into a (different) buffer
 */

 /* Draw data from the sensor in 6 consecutive bytes. Make sure that the registry is pointing to 0x03 first! */
static bool_t hmc5883l_receive(uint8_t *buf, uint8_t *buf_data)
{
    msg_t rv;
    uint8_t txbuf = 0x03;

    /* Sends 0x03 first, so that registry is pointing in correct location, then reads 6 consecutive bytes.
    Location of measurements are X,Z,Y in 0x03,0x04,0x05 respectively. (note the order!) */
    rv = i2cMasterTransmitTimeout(&I2CD2, HMC5883L_I2C_ADDR, txbuf, 1, buf_data, 6, 1000);
    
	if  (rv == RDY_OK)
	    return TRUE;
	else
	    return FALSE;
}

static bool_t hmc5883l_init(uint8_t *buf, uint8_t *buf_data)
{
    bool_t success = TRUE;

    i2cMasterTransmitTimeout(&I2CD2, HMC5883L_I2C_ADDR, buf, n, NULL, 0, 1000)
    
    buf[0] = 0x00; /*Register 00 (1Byte): [0][00=avg ovr 1 sample][100=15Hz][00=normal operation]*/
    buf[1] = 0x10; /*Send 00010000*/
    success &= hmc5883l_transmit(buf);

    buf[0] = 0x01; /*Register 01 (1Byte): [001 = default gain][00000]*/
    buf[1] = 0x20; /*Send 00100000*/
    success &= hmc5883l_transmit(buf);

    buf[0] = 0x02; /*Register 02 (1Byte): [000000][00 = continuous measurement]*/
    buf[1] = 0x00; /*Send 00000000*/
    success &= hmc5883l_transmit(buf);

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
static void hmc5883l_field_convert(uint8_t *buf_data, float *field)
{
    int16_t temp ;
    int i;
    
    for (i =0; i<3; i++)
    {
    	temp = ((int16_t) (buf_data[i*2]) << 8) | ((uint16_t) 
                                                  (buf_data[i*2+1])) ;
    	field [i] = ((float) (temp));// * counts_to_Tesla;
    }
    
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
    uint8_t buf_data[bufsize_2];
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

    i2cStart(&I2CD2, &i2cconfig);
    

    if(!hmc5883l_init(buf, buf_data))
    {
        while(1) chThdSleepMilliseconds(5);
    }
    
    
    while(TRUE)
    {   
        if (hmc5883l_receive(buf, buf_data))
        {
            /*hmc5883l_field_convert(buf_data, field);*/
            /* Note the order of received is X,Z,Y, so re-arrangement is done here. */
            global_magnoxyz[0] = ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
            global_magnoxyz[1] = ((uint16_t)buf[4] << 8) | (uint16_t)buf[5];
            global_magnoxyz[2] = ((uint16_t)buf[2] << 8) | (uint16_t)buf[3];
            /* microsd_log_s16(CHAN_IMU_MAGNO , 
                field[0], field[1], field[2], 0); */
            /*define this state estimation function 
            state_estimation_new_magno(field[0], 
			       field[1], field[2]); */
        }

        else
        {   
            chThdSleepMilliseconds(20);
        }

        /* Sleep until DRDY */
        chSysLock();
        tpHMC5883L = chThdSelf();
        chSchGoSleepTimeoutS(THD_STATE_SUSPENDED, 100);
        chSysUnlock();
    }

    return (msg_t)NULL;
}
