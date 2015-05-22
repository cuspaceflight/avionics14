/*
 * L3G4200D Driver 
 * Cambridge University Spaceflight
 *
 * Assumption made that the address 
 * of the I2C driver is &I2CD1.
 *
 * In config, must call wakeup function
 * on DRDY/INT2 interrupt.
 */

#include <stdlib.h>
#include "l3g4200d.h"  


// Register Addresses
#define L3G4200D_RA_WHO_AM_I          0x0F
#define L3G4200D_RA_CTRL_REG1         0x20
#define L3G4200D_RA_CTRL_REG2         0x21
#define L3G4200D_RA_CTRL_REG3         0x22
#define L3G4200D_RA_CTRL_REG4         0x23
#define L3G4200D_RA_CTRL_REG5         0x24

#define L3G4200D_RA_FIFO_CTRL_REG     0x2E
#define L3G4200D_RA_FIFO_SRC_REG      0x2F

#define L3G4200D_RA_OUT_TEMP          0x26
#define L3G4200D_RA_OUT_BURST         0xA8


// Register description
#define L3G4200D_CR1_RATE_BIT         7
#define L3G4200D_CR1_RATE_LENGTH      2
#define L3G4200D_CR1_BW_BIT           5
#define L3G4200D_CR1_BW_LENGTH        2
#define L3G4200D_CR1_PD_ENABLE_BIT    3
#define L3G4200D_CR1_X_ENABLE_BIT     2
#define L3G4200D_CR1_Y_ENABLE_BIT     1
#define L3G4200D_CR1_Z_ENABLE_BIT     0

#define L3G4200D_CR3_DRDY_ENABLE_BIT  3
#define L3G4200D_CR3_WTM_ENABLE_BIT   2 // FIFO watermark interrupt
#define L3G4200D_CR3_OF_ENABLE_BIT    1 // FIFO overflow interrupt
#define L3G4200D_CR3_EMPTY_ENABLE_BIT 0 // FIFO empty interrupt

#define L3G4200D_CR4_DATA_MODE_BIT    7
#define L3G4200D_CR4_BLE_BIT          6 // 0: Data LSB @ lower address
#define L3G4200D_CR4_SCALE_BIT        5
#define L3G4200D_CR4_SCALE_LENGTH     2
#define L3G4200D_CR4_SELF_TEST_BIT    2
#define L3G4200D_CR4_SELF_TEST_LENGTH 2

#define L3G4200D_CR5_FIFO_ENABLE_BIT  6

#define L3G4200D_FIFO_CTRL_FM_BIT     7
#define L3G4200D_FIFO_CTRL_FM_LENGTH  3
#define L3G4200D_FIFO_CTRL_WTM_BIT    4
#define L3G4200D_FIFO_CTRL_WRM_LENGTH 5

#define L3G4200D_DATA_RATE_100HZ      0x01
#define L3G4200D_DATA_RATE_200HZ      0x02
#define L3G4200D_DATA_RATE_400HZ      0x03
#define L3G4200D_DATA_RATE_800HZ      0x04

#define L3G4200D_SCALE_250DPS         0x00
#define L3G4200D_SCALE_500DPS         0x01
#define L3G4200D_SCALE_2000DPS        0x02

#define L3G4200D_DATA_MODE_CONTINUOUS 0x00
#define L3G4200D_DATA_MODE_BLOCK      0x01

#define L3G4200D_FIFO_MODE_BYPASS     0x00
#define L3G4200D_FIFO_MODE_FIFO       0x01
#define L3G4200D_FIFO_MODE_STREAM     0x02
#define L3G4200D_FIFO_MODE_STF        0x03 // Stream To FIFO
#define L3G4200D_FIFO_MODE_BTS        0x04 // Bypass to Stream

#define L3G4200D_I2C_WRITE_ADDR    0x69 
#define L3G4200D_I2C_READ_ADDR     0x69

static Thread *tpL3G4200D = NULL;

/* 10000 Hz might need to be changed to 100000 Hz */
static const I2CConfig i2cconfig = {
	OPMODE_I2C, 10000, STD_DUTY_CYCLE
};

static bool_t l3g4200d_writeRegister(uint8_t address, uint8_t data) {
	uint8_t buffer[2];
	buffer[0] = address;
	buffer[1] = data;

	msg_t rv;

	/* Transmit message */
	rv = i2cMasterTransmit(&I2CD1, L3G4200D_I2C_WRITE_ADDR, buffer,
		2, NULL, 0);

	if (rv == RDY_OK) {
		return TRUE;
    } else {
        i2cflags_t errs = i2cGetErrors(&I2CD1);
		return FALSE;
    }
}

static bool_t l3g4200d_receive(uint8_t readAddress, uint8_t *buf, uint8_t size)
{
    msg_t rv;
	
    rv = i2cMasterTransmit(&I2CD1, L3G4200D_I2C_READ_ADDR, &readAddress, 
	                          1, buf, size);
    
    if  (rv == RDY_OK)
	      return TRUE;
  	else
	      return FALSE;
}

static bool_t l3g4200d_init(void)
{
    bool_t success;
  
    /* Configure CTRL_REG1: Highest Data Rate- 800 Hz, Bandwidth- 50 (UNSURE) */
	success = l3g4200d_writeRegister(L3G4200D_RA_CTRL_REG1, 0xFF);

    /* Configure CTRL_REG2: Normal mode for filter, 1 Hz for high pass filter */
	success &= l3g4200d_writeRegister(L3G4200D_RA_CTRL_REG2, 0x26);

    /* 
     * Configure CTRL_REG3:
     * This allows interrupts on DRDY/INT2
     * when the FIFO has filled to a watermark
     * level specified in the FIFO_CTRL_REG.
     */

	success &= l3g4200d_writeRegister(L3G4200D_RA_CTRL_REG3, 0x04);

	/* Configure CTRL_REG5: Enable FIFO operation */
	success &= l3g4200d_writeRegister(L3G4200D_RA_CTRL_REG5, 0x40);
	
    /* 
     * Configure FIFO_CTRL_REG: 
     * Set the FIFO to operate in Stream Mode.
     * Set the Watermark level to be 4.
     * Interrupts on DRDY/INT2 occur when
     * the FIFO has filled to this level.
     */
	success &= l3g4200d_writeRegister(L3G4200D_RA_FIFO_CTRL_REG, 0x44);

	// TODO check connection

    return success;
}

bool_t l3g4200d_testConnection(void) {
	uint8_t buffer;
	if (l3g4200d_receive(L3G4200D_RA_WHO_AM_I, &buffer, 1) == RDY_OK) {
		return buffer == 0xD3;
	}
	return 0;
}

/* 
 * Interrupt handler- wake up when DRDY deasserted 
 * Relevant pin needs defining in main/config
 */
void l3g4200d_wakeup(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;
    chSysLockFromIsr();
    if(tpL3G4200D != NULL) 
    {
        chSchReadyI(tpL3G4200D);
        tpL3G4200D = NULL;
    }
    chSysUnlockFromIsr();
}

msg_t l3g4200d_thread(void *arg)
{
    (void)arg;
    const int bufsize = 6;
    uint8_t buf_data[bufsize];
	
    chRegSetThreadName("L3G4200D");

    i2cStart(&I2CD1, &i2cconfig);

	while (!l3g4200d_init()) {
		chThdSleepMilliseconds(5);
	}
    
	while(TRUE) {   
		if (l3g4200d_receive(0xA8,buf_data, bufsize)) {
			//TODO do something with received data
		} else
		    chThdSleepMilliseconds(20);
		
		/* Sleep until DRDY */
		chSysLock();
		tpL3G4200D = chThdSelf();
		chSchGoSleepTimeoutS(THD_STATE_SUSPENDED, 100);
		chSysUnlock();
	}
	
    return (msg_t)NULL;
}
