/*
 * HMC5883L Driver 
 * Cambridge University Spaceflight
 *
 * Based on magn_hmc5883l.h from https://github.com/DuinoPilot/trunetcopter/
 *
 */

#include <stdlib.h>

#include "hmc5883l.h"
#include "i2c_util.h"

#include <string.h>

/* Config Values */
#define HMC5883L_ADDRESS            0x1E // this device only has one address
#define HMC5883L_DEFAULT_ADDRESS    0x1E

#define HMC5883L_MEASUREMENT_PERIOD 6 // From receiving command to data ready (ms)

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

#define HMC5883L_I2CDRIVERPOINTER &I2CD1

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static const uint16_t HMC5883L_READY_FOR_I2C_COMMAND = 200; // Ready for I2C commands (µs)

static const float  HMC5883L_SELF_TEST_X_AXIS_ABSOLUTE_GAUSS = 1.16f;
static const float  HMC5883L_SELF_TEST_Y_AXIS_ABSOLUTE_GAUSS = 1.16f;
static const float  HMC5883L_SELF_TEST_Z_AXIS_ABSOLUTE_GAUSS = 1.08f;

static const I2CConfig i2cfg = {
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2,
};

uint8_t hmc5883l_buffer[6];
uint8_t hmc5883l_mode;
uint8_t hmc5883l_gain;
float hmc5883l_scaleFactors[8][3];

static Thread *tpHMC5883L = NULL;

bool_t hmc5883l_testConnection(void);

// CONFIG_A register
uint8_t hmc5883l_getSampleAveraging(void);
void hmc5883l_setSampleAveraging(uint8_t averaging);
uint8_t hmc5883l_getDataRate(void);
void hmc5883l_setDataRate(uint8_t rate);
uint8_t hmc5883l_getMeasurementBias(void);
void hmc5883l_setMeasurementBias(uint8_t bias);

// CONFIG_B register
uint8_t hmc5883l_getGain(void);
void hmc5883l_setGain(uint8_t newGain);

// MODE register
uint8_t hmc5883l_getMode(void);
void hmc5883l_setMode(uint8_t mode);

// DATA* registers
void hmc5883l_getHeading(int16_t *x, int16_t *y, int16_t *z);

void hmc5883l_getRawHeading(int16_t *x, int16_t *y, int16_t *z);

bool_t hmc5883l_calibrate(int8_t testGain);


static void hmc5883l_init(void)
{
	memset((void *)&hmc5883l_buffer, 0, sizeof(hmc5883l_buffer));

	// We need to wait a bit...
	chThdSleepMicroseconds(HMC5883L_READY_FOR_I2C_COMMAND);

	// write CONFIG_A register
	i2c_writeByte(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A,
		(HMC5883L_AVERAGING_8 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
		(HMC5883L_RATE_75 << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
		(HMC5883L_BIAS_NORMAL << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1)));

	// write CONFIG_B register
	hmc5883l_setGain(HMC5883L_GAIN_1090);

	// write MODE register
	hmc5883l_setMode(HMC5883L_MODE_CONTINUOUS);

	// TODO: Maybe it would be a good idea to use the EEPROM
	// to store the scale factors and recover the last valid
	// value in case of a calibration fail.
	uint8_t gain;
	for (gain = HMC5883L_GAIN_1370; gain <= HMC5883L_GAIN_220; gain++) {
		hmc5883l_scaleFactors[gain][0] = 1.0f;
		hmc5883l_scaleFactors[gain][1] = 1.0f;
		hmc5883l_scaleFactors[gain][2] = 1.0f;
	}
}

bool_t hmc5883l_testConnection(void) {
	if (i2c_readBytes(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_ID_A, 3, hmc5883l_buffer) == RDY_OK) {
		return (hmc5883l_buffer[0] == 'H' && hmc5883l_buffer[1] == '4' && hmc5883l_buffer[2] == '3');
	}
	return 0;
}

uint8_t hmc5883l_getSampleAveraging(void) {
	i2c_readBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_AVERAGE_BIT, HMC5883L_CRA_AVERAGE_LENGTH, hmc5883l_buffer);
	return hmc5883l_buffer[0];
}

void hmc5883l_setSampleAveraging(uint8_t averaging) {
	i2c_writeBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_AVERAGE_BIT, HMC5883L_CRA_AVERAGE_LENGTH, averaging);
}

uint8_t hmc5883l_getDataRate(void) {
	i2c_readBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_RATE_BIT, HMC5883L_CRA_RATE_LENGTH, hmc5883l_buffer);
	return hmc5883l_buffer[0];
}

void hmc5883l_setDataRate(uint8_t rate) {
	i2c_writeBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_RATE_BIT, HMC5883L_CRA_RATE_LENGTH, rate);
}

uint8_t hmc5883l_getMeasurementBias(void) {
	i2c_readBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_BIAS_BIT, HMC5883L_CRA_BIAS_LENGTH, hmc5883l_buffer);
	return hmc5883l_buffer[0];
}

void hmc5883l_setMeasurementBias(uint8_t bias) {
	i2c_writeBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_BIAS_BIT, HMC5883L_CRA_BIAS_LENGTH, bias);
}

uint8_t hmc5883l_getGain(void) {
	i2c_readBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_B, HMC5883L_CRB_GAIN_BIT, HMC5883L_CRB_GAIN_LENGTH, hmc5883l_buffer);
	return hmc5883l_buffer[0];
}

void hmc5883l_setGain(uint8_t newGain) {
	if (i2c_writeByte(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_CONFIG_B, newGain << (HMC5883L_CRB_GAIN_BIT - HMC5883L_CRB_GAIN_LENGTH + 1))) {
		hmc5883l_gain = newGain;
	}
}

uint8_t hmc5883l_getMode(void) {
	i2c_readBits(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_MODE, HMC5883L_MODEREG_BIT, HMC5883L_MODEREG_LENGTH, hmc5883l_buffer);
	return hmc5883l_buffer[0];
}

void hmc5883l_setMode(uint8_t newMode) {
	i2c_writeByte(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_MODE, hmc5883l_mode << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
	hmc5883l_mode = newMode;
}

void hmc5883l_getHeading(int16_t *x, int16_t *y, int16_t *z) {
	int16_t rawx, rawy, rawz;
	hmc5883l_getRawHeading(&rawx, &rawy, &rawz);
	
	*x = (int16_t)(hmc5883l_scaleFactors[hmc5883l_gain][0] * rawx);
	*y = (int16_t)(hmc5883l_scaleFactors[hmc5883l_gain][1] * rawy);
	*z = (int16_t)(hmc5883l_scaleFactors[hmc5883l_gain][2] * rawz);
}

void hmc5883l_getRawHeading(int16_t *x, int16_t *y, int16_t *z) {
	if (hmc5883l_mode == HMC5883L_MODE_SINGLE) {
		i2c_writeByte(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
		chThdSleepMilliseconds(HMC5883L_MEASUREMENT_PERIOD);
	}
	
	i2c_readBytes(HMC5883L_I2CDRIVERPOINTER, HMC5883L_DEFAULT_ADDRESS, HMC5883L_RA_DATAX_H, 6, hmc5883l_buffer);
	*x = (((int16_t)hmc5883l_buffer[0]) << 8) | hmc5883l_buffer[1];
	*y = (((int16_t)hmc5883l_buffer[4]) << 8) | hmc5883l_buffer[5];
	*z = (((int16_t)hmc5883l_buffer[2]) << 8) | hmc5883l_buffer[3];
}

bool_t hmc5883l_calibrate(int8_t testGain) {

	// Keep the current status ...
	uint8_t previousGain = hmc5883l_getGain();

	// Set the gain
	if (testGain < 0) {
		testGain = hmc5883l_gain;
	}
	hmc5883l_setGain(testGain);

	// To check the HMC5883L for proper operation, a self test
	// feature in incorporated in which the sensor offset straps
	// are excited to create a nominal field strength (bias field)
	// to be measured. To implement self test, the least significant
	// bits (MS1 and MS0) of configuration register A are changed
	// from 00 to 01 (positive bias) or 10 (negetive bias)
	hmc5883l_setMeasurementBias(HMC5883L_BIAS_POSITIVE);

	// Then, by placing the mode register into single-measurement mode ...
	hmc5883l_setMode(HMC5883L_MODE_SINGLE);

	// Two data acquisition cycles will be made on each magnetic vector.
	// The first acquisition will be a set pulse followed shortly by
	// measurement data of the external field. The second acquisition
	// will have the offset strap excited (about 10 mA) in the positive
	// bias mode for X, Y, and Z axes to create about a ±1.1 gauss self
	// test field plus the external field.
	// The first acquisition values will be subtracted from the
	// second acquisition, and the net measurement will be placed into
	// the data output registers.
	int16_t x, y, z;
	hmc5883l_getRawHeading(&x, &y, &z);

	// In the event the ADC reading overflows or underflows for the
	// given channel, or if there is a math overflow during the bias
	// measurement, this data register will contain the value -4096.
	// This register value will clear when after the next valid
	// measurement is made.
	if (min(x, min(y, z)) == -4096) {
		hmc5883l_scaleFactors[testGain][0] = 1.0f;
		hmc5883l_scaleFactors[testGain][1] = 1.0f;
		hmc5883l_scaleFactors[testGain][2] = 1.0f;
		return 0;
	}
	hmc5883l_getRawHeading(&x, &y, &z);

	if (min(x, min(y, z)) == -4096) {
		hmc5883l_scaleFactors[testGain][0] = 1.0f;
		hmc5883l_scaleFactors[testGain][1] = 1.0f;
		hmc5883l_scaleFactors[testGain][2] = 1.0f;
		return 0;
	}

	// Since placing device in positive bias mode
	// (or alternatively negative bias mode) applies
	// a known artificial field on all three axes,
	// the resulting ADC measurements in data output
	// registers can be used to scale the sensors.
	float xExpectedSelfTestValue =
		HMC5883L_SELF_TEST_X_AXIS_ABSOLUTE_GAUSS *
		HMC5883L_LSB_PER_GAUS[testGain];
	float yExpectedSelfTestValue =
		HMC5883L_SELF_TEST_Y_AXIS_ABSOLUTE_GAUSS *
		HMC5883L_LSB_PER_GAUS[testGain];
	float zExpectedSelfTestValue =
		HMC5883L_SELF_TEST_Z_AXIS_ABSOLUTE_GAUSS *
		HMC5883L_LSB_PER_GAUS[testGain];

	hmc5883l_scaleFactors[testGain][0] = xExpectedSelfTestValue / x;
	hmc5883l_scaleFactors[testGain][1] = yExpectedSelfTestValue / y;
	hmc5883l_scaleFactors[testGain][2] = zExpectedSelfTestValue / z;

	hmc5883l_setGain(previousGain);
	hmc5883l_setMeasurementBias(HMC5883L_BIAS_NORMAL);

	return 1;
}



/* 
 * Interrupt handler- wake up when DRDY deasserted 
 * Relevant pin needs defining in main/config
 */
void hmc5883l_wakeup(EXTDriver *extp, expchannel_t channel) {
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

msg_t hmc5883l_thread(void *arg) {
    chRegSetThreadName("HMC5883L");

	i2cStart(HMC5883L_I2CDRIVERPOINTER, &i2cfg);

	hmc5883l_init();

	if (hmc5883l_testConnection() != 1)
		chDbgPanic("HMC5883L not responding!");


	hmc5883l_calibrate(HMC5883L_GAIN_1090);


	while(true)
	{   
		//TODO Read Data and do stuff with it
		
		/* Sleep until DRDY */
		chSysLock();
		tpHMC5883L = chThdSelf();
		chSchGoSleepS(THD_STATE_SUSPENDED);
		chSysUnlock();
	}
	
    return 0;
}
