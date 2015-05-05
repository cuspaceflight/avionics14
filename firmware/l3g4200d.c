/*
 * L3G4200D Driver 
 * Cambridge University Spaceflight
 *
 */

#include <stdlib.h>
#include "l3g4200d.h"  
#include "i2c_util.h"

#include <string.h>

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

#define L3G4200D_DEFAULT_ADDRESS      0x69
#define L3G4200D_I2CDRIVERPOINTER &I2CD1

void l3g4200d_enableDataReadyInterrupt(uint8_t enabled);

void l3g4200d_setFifoEnabled(uint8_t enabled);

void l3g4200D_setFifoMode(uint8_t mode);

static const uint16_t L3G4200D_READY_FOR_I2C_COMMAND = 200;

static Thread *tpL3G4200D = NULL;

uint8_t l3g4200d_buffer[6];

static const I2CConfig i2cfg = {
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2,
};

static void l3g4200d_init(void) {
	memset((void *)&l3g4200d_buffer, 0, sizeof(l3g4200d_buffer));
	chThdSleepMicroseconds(L3G4200D_READY_FOR_I2C_COMMAND);

	//CTRL REG 1
	i2c_writeByte(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG1,
		((uint8_t)(L3G4200D_DATA_RATE_800HZ << (L3G4200D_CR1_RATE_BIT - L3G4200D_CR1_RATE_LENGTH + 1))) | 0x0F); // 0x0F -> device active and all axes enabled

	//CTRL REG 2 -> Defaults fine

	//CTRL REG 3
	l3g4200d_enableDataReadyInterrupt(1);

	//CTRL REG 4 -> Defaults fine

	//CTRL REG 5
	l3g4200d_setFifoEnabled(1);

	//FIFO CTRL REG
	l3g4200D_setFifoMode(L3G4200D_FIFO_MODE_STREAM);
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

bool_t l3g4200d_testConnection(void) {
	if (i2c_readBytes(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_WHO_AM_I, 1, l3g4200d_buffer) == RDY_OK) {
		return l3g4200d_buffer[0] == 0xD3;
	}
	return 0;
}

void l3g4200d_setDataRate(uint8_t rate) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG1, L3G4200D_CR1_RATE_BIT, L3G4200D_CR1_RATE_LENGTH, rate);
}

uint8_t l3g4200d_getDataRate(void) {
	i2c_readBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG1, L3G4200D_CR1_RATE_BIT, L3G4200D_CR1_RATE_LENGTH, l3g4200d_buffer);
	return l3g4200d_buffer[0];
}

void l3g4200d_setBandwidth(uint8_t bw) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG1, L3G4200D_CR1_BW_BIT, L3G4200D_CR1_BW_LENGTH, bw);
}

uint8_t l3g4200d_getBandwidth(void) {
	i2c_readBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG1, L3G4200D_CR1_BW_BIT, L3G4200D_CR1_BW_LENGTH, l3g4200d_buffer);
	return l3g4200d_buffer[0];
}

void l3g4200d_setActive(uint8_t poweredUp) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG1, L3G4200D_CR1_PD_ENABLE_BIT, 1, poweredUp);
}

void l3g4200d_enableDataReadyInterrupt(uint8_t enabled) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG3, L3G4200D_CR3_DRDY_ENABLE_BIT, 1, enabled);
}

void l3g4200d_enableWatermarkInterrupt(uint8_t enabled) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG3, L3G4200D_CR3_WTM_ENABLE_BIT, 1, enabled);
}

void l3g4200d_enableFifoOverflowInterrupt(uint8_t enabled) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG3, L3G4200D_CR3_OF_ENABLE_BIT, 1, enabled);
}

void l3g4200d_enableFifoEmptyInterrupt(uint8_t enabled) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG3, L3G4200D_CR3_EMPTY_ENABLE_BIT, 1, enabled);
}

void l3g4200d_setMode(uint8_t mode) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG4, L3G4200D_CR4_DATA_MODE_BIT, 1, mode);
}

void l3g4200d_setScale(uint8_t scale) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG4, L3G4200D_CR4_SCALE_BIT, L3G4200D_CR4_SCALE_LENGTH, scale);
}

uint8_t l3g4200d_getScale(void) {
	i2c_readBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG4, L3G4200D_CR4_SCALE_BIT, L3G4200D_CR4_SCALE_LENGTH, l3g4200d_buffer);
	return l3g4200d_buffer[0];
}

void l3g4200d_setFifoEnabled(uint8_t enabled) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_CTRL_REG5, L3G4200D_CR5_FIFO_ENABLE_BIT, 1, enabled);
}

void l3g4200D_setFifoMode(uint8_t mode) {
	i2c_writeBits(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_FIFO_CTRL_REG, L3G4200D_FIFO_CTRL_FM_BIT, L3G4200D_FIFO_CTRL_FM_LENGTH, mode);
}

void l3g4200d_getRawAngularRate(int16_t* x, int16_t* y, int16_t* z) {
	i2c_readBytes(L3G4200D_I2CDRIVERPOINTER, L3G4200D_DEFAULT_ADDRESS, L3G4200D_RA_OUT_BURST, 6, l3g4200d_buffer);

	*x = (((int16_t)l3g4200d_buffer[1]) << 8) | l3g4200d_buffer[0];
	*y = (((int16_t)l3g4200d_buffer[3]) << 8) | l3g4200d_buffer[2];
	*z = (((int16_t)l3g4200d_buffer[5]) << 8) | l3g4200d_buffer[4];
}

msg_t l3g4200d_thread(void *arg)
{
    chRegSetThreadName("L3G4200D");
    
	i2cStart(L3G4200D_I2CDRIVERPOINTER, &i2cfg);
	
	l3g4200d_init();

	if (l3g4200d_testConnection() != 1)
		chDbgPanic("L3G4200D not responding!");

	while(true)
	{   
		//TODO Read data and do stuff with it
		
		/* Sleep until DRDY */
		chSysLock();
		tpL3G4200D = chThdSelf();
		chSchGoSleepS(THD_STATE_SUSPENDED);
		chSysUnlock();
	}
	
    return (msg_t)NULL;
}
