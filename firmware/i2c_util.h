/*
* I2C Util
* Cambridge University Spaceflight
*
* Based on i2c_local.h from https://github.com/DuinoPilot/trunetcopter/
*
*/

#ifndef I2C_UTIL_H
#define I2C_UTIL_H

#include "ch.h"
#include "hal.h"

msg_t i2c_transmit(I2CDriver *i2cp, i2caddr_t addr, const uint8_t *txbuf, size_t txbytes, uint8_t *rxbuf, size_t rxbytes);
msg_t i2c_receive(I2CDriver *i2cp, i2caddr_t addr, uint8_t *rxbuf, size_t rxbytes);

int8_t i2c_readBits(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
int8_t i2c_readByte(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t *data);
int8_t i2c_readBytes(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

bool_t i2c_writeBits(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
bool_t i2c_writeByte(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t data);
bool_t i2c_writeBytes(I2CDriver *i2cp, i2caddr_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);


#endif /* I2C_UTIL_H*/