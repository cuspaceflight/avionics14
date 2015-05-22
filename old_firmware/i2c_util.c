/*
* I2C Util
* Cambridge University Spaceflight
*
* Based on i2c_local.h from https://github.com/DuinoPilot/trunetcopter/
*
*/

#include "i2c_util.h"
#include <string.h>

static systime_t calc_timeout(I2CDriver *i2cp, size_t txbytes, size_t rxbytes){
	const uint32_t bitsinbyte = 10;
	uint32_t tmo;
	tmo = ((txbytes + rxbytes + 1) * bitsinbyte * 1000);
	tmo /= i2cp->config->clock_speed;
	tmo += 5; /* some additional time to be safer */
	return MS2ST(tmo);
}

msg_t i2c_transmit(I2CDriver* i2cp, i2caddr_t addr, const uint8_t *txbuf, size_t txbytes, uint8_t *rxbuf, size_t rxbytes){
	msg_t status = RDY_OK;
	systime_t tmo = calc_timeout(i2cp, txbytes, rxbytes);
	status = i2cMasterTransmitTimeout(i2cp, addr, txbuf, txbytes, rxbuf, rxbytes, tmo);
	return status;
}

msg_t i2c_receive(I2CDriver* i2cp, i2caddr_t addr, uint8_t *rxbuf, size_t rxbytes){
	msg_t status = RDY_OK;
	systime_t tmo = calc_timeout(i2cp, 0, rxbytes);

	status = i2cMasterReceiveTimeout(i2cp, addr, rxbuf, rxbytes, tmo);
	return status;
}

int8_t i2c_readBytes(I2CDriver* i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
	return i2c_transmit(i2cp, devAddr, &regAddr, 1, data, length);
}

int8_t i2c_readByte(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t *data) {
	return i2c_readBytes(i2cp, devAddr, regAddr, 1, data);
}

int8_t i2c_readBits(I2CDriver* i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
	// 01101001 read byte
	// 76543210 bit numbers
	//    xxx   args: bitStart=4, length=3
	//    010   masked
	//   -> 010 shifted
	uint8_t count, b;
	//if ((count = i2c_readByte(devAddr, regAddr, &b)) != 0) {
	if ((count = i2c_readByte(i2cp, devAddr, regAddr, &b)) == RDY_OK) {
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
		b &= mask;
		b >>= (bitStart - length + 1);
		*data = b;
	}
	return count;
}

bool_t i2c_writeBytes(I2CDriver* i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
	uint8_t i;
	uint8_t txbuf[20], rxbuf[1];

	memset((void *)&txbuf, 0, sizeof(txbuf));
	memset((void *)&rxbuf, 0, sizeof(rxbuf));

	txbuf[0] = regAddr;
	for (i = 0; i<14; i++)
		txbuf[i + 1] = data[i];

	return i2c_transmit(i2cp, devAddr, txbuf, length + 1, rxbuf, 0);
}

bool_t i2c_writeByte(I2CDriver* i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t data) {
	return i2c_writeBytes(i2cp, devAddr, regAddr, 1, &data);
}

bool_t i2c_writeBits(I2CDriver* i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
	//      010 value to write
	// 76543210 bit numbers
	//    xxx   args: bitStart=4, length=3
	// 00011100 mask byte
	// 10101111 original value (sample)
	// 10100011 original & ~mask
	// 10101011 masked | value
	uint8_t b;
	if (i2c_readByte(i2cp, devAddr, regAddr, &b) == RDY_OK) {
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
		data <<= (bitStart - length + 1); // shift data into correct position
		data &= mask; // zero all non-important bits in data
		b &= ~(mask); // zero all important bits in existing byte
		b |= data; // combine data with existing byte
		return i2c_writeByte(i2cp, devAddr, regAddr, b);
	}
	else {
		return 0;
	}
}



