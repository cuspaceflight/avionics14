/*
 * HMC5883L Driver 
 * Cambridge University Spaceflight
 */

#include <stdlib.h>
#include "hal.h"
#include "hmc5883l.h"

/* Config Values */
#define HMC5883L_ADDRESS            0x1E /* this device only has one address */
#define HMC5883L_DEFAULT_ADDRESS    0x1E

#define HMC5883L_MEASUREMENT_PERIOD 6 /* From receiving command to data ready (ms) */

#define HMC5883L_RA_CONFIG_A        0x00
#define HMC5883L_RA_CONFIG_B        0x01
#define HMC5883L_RA_MODE            0x02
#define HMC5883L_RA_DATAX_H         0x03
#define HMC5883L_RA_DATAX_L         0x04
#define HMC5883L_RA_DATAZ_H         0x05
#define HMC5883L_RA_DATAZ_L         0x06
#define HMC5883L_RA_DATAY_H         0x07
#define HMC5883L_RA_DATAY_L         0x08
#define HMC5883L_RA_STATUS          0x09
#define HMC5883L_RA_ID_A            0x0A
#define HMC5883L_RA_ID_B            0x0B
#define HMC5883L_RA_ID_C            0x0C

#define HMC5883L_CRA_AVERAGE_BIT    6
#define HMC5883L_CRA_AVERAGE_LENGTH 2
#define HMC5883L_CRA_RATE_BIT       4
#define HMC5883L_CRA_RATE_LENGTH    3
#define HMC5883L_CRA_BIAS_BIT       1
#define HMC5883L_CRA_BIAS_LENGTH    2

#define HMC5883L_AVERAGING_1        0x00
#define HMC5883L_AVERAGING_2        0x01
#define HMC5883L_AVERAGING_4        0x02
#define HMC5883L_AVERAGING_8        0x03

#define HMC5883L_RATE_0P75          0x00
#define HMC5883L_RATE_1P5           0x01
#define HMC5883L_RATE_3             0x02
#define HMC5883L_RATE_7P5           0x03
#define HMC5883L_RATE_15            0x04
#define HMC5883L_RATE_30            0x05
#define HMC5883L_RATE_75            0x06

#define HMC5883L_BIAS_NORMAL        0x00
#define HMC5883L_BIAS_POSITIVE      0x01
#define HMC5883L_BIAS_NEGATIVE      0x02

#define HMC5883L_CRB_GAIN_BIT       7
#define HMC5883L_CRB_GAIN_LENGTH    3

#define HMC5883L_GAIN_1370          0x00
#define HMC5883L_GAIN_1090          0x01
#define HMC5883L_GAIN_820           0x02
#define HMC5883L_GAIN_660           0x03
#define HMC5883L_GAIN_440           0x04
#define HMC5883L_GAIN_390           0x05
#define HMC5883L_GAIN_330           0x06
#define HMC5883L_GAIN_220           0x07

static const uint16_t HMC5883L_LSB_PER_GAUS[] = {
	1370, 1090, 820, 660, 440, 390, 330, 230
};

#define HMC5883L_MODEREG_BIT        1
#define HMC5883L_MODEREG_LENGTH     2

#define HMC5883L_MODE_CONTINUOUS    0x00
#define HMC5883L_MODE_SINGLE        0x01
#define HMC5883L_MODE_IDLE          0x02

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static const float  HMC5883L_SELF_TEST_X_AXIS_ABSOLUTE_GAUSS = 1.16f;
static const float  HMC5883L_SELF_TEST_Y_AXIS_ABSOLUTE_GAUSS = 1.16f;
static const float  HMC5883L_SELF_TEST_Z_AXIS_ABSOLUTE_GAUSS = 1.08f;

#define HMC5883L_I2C_WRITE_ADDR    0x1E 
#define HMC5883L_I2C_READ_ADDR     0x1E

static Thread *tpHMC5883L = NULL;
static float counts_to_Tesla = 9.2E-8;

/* TODO: Validate timings against AN4235 */
/* Magic I2C timing numbers. Computed via reference manual. */
static const I2CConfig i2cconfig = {
	OPMODE_I2C, 10000, STD_DUTY_CYCLE
};


static bool_t hmc5883l_writeRegister(uint8_t address, uint8_t data) {
	uint8_t buffer[2];
	buffer[0] = address;
	buffer[1] = data;

	msg_t rv;

	/* Transmit message */
	rv = i2cMasterTransmit(&I2CD2, HMC5883L_I2C_WRITE_ADDR, buffer,
		2, NULL, 0);

	if (rv == RDY_OK)
		return TRUE;
	else
		return FALSE;

}

static bool_t hmc5883l_receive(uint8_t address, uint8_t *data, uint8_t size)
{
    msg_t rv;
	
    rv = i2cMasterTransmit(&I2CD2, HMC5883L_I2C_READ_ADDR, &address, 1, data, size);
    
	if  (rv == RDY_OK)
	    return TRUE;
	else
	    return FALSE;
}

static bool_t hmc5883l_init(void)
{
    bool_t success;
  
      /* Configure Mode Register: High Speed, Continuous Measurement */
	success = hmc5883l_writeRegister(HMC5883L_RA_MODE, 0x00);


    /* Configure Reg A - Highest Data Rate- 75 Hz */
	success &= hmc5883l_writeRegister(HMC5883L_RA_CONFIG_A, 0x18);

	/* TODO check connection */


    return success;
}

bool_t hmc5883l_testConnection(void) {
	uint8_t buffer[3];
	if (hmc5883l_receive(HMC5883L_RA_ID_A, buffer, 3)) {
		return (buffer[0] == 'H' && buffer[1] == '4' && buffer[2] == '3');
	}
	return 0;
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
    if(tpHMC5883L != NULL) {
        chSchReadyI(tpHMC5883L);
        tpHMC5883L = NULL;
    }
    chSysUnlockFromIsr();
}

msg_t hmc5883l_thread(void *arg) {
    (void)arg;
    const int bufsize = 6;
    
    uint8_t buf[bufsize];
	
    chRegSetThreadName("HMC5883L");

    i2cStart(&I2CD2, &i2cconfig);

	while (!hmc5883l_init()) {
		chThdSleepMilliseconds(5);
	}
    
    if(!hmc5883l_testConnection()) {
        chSysHalt();
    }
    
    while(TRUE) {   
		if (hmc5883l_receive(0x06, buf, bufsize)) {
			/* TODO do something with the data */
            /* send it to the global variable by concatenating */
            global_magnoxyz[0] = (buf[0] << 8) | buf[1];
            global_magnoxyz[1] = (buf[2] << 8) | buf[3];
            global_magnoxyz[2] = (buf[4] << 8) | buf[5];
		} else {   
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
