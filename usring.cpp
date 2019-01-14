#include "usring.h"
#include "Arduino.h"
#include "Wire.h"
#include "error.h"

const char error_prefix[] PROGMEM = "USRING: ";

usring::usring()
{
	_initialized = false;
}

void usring::init()
{

	uint8_t data[2];
	data[0] = 0;
	data[1] = 0;
	Wire.beginTransmission(USRING_I2C_ADDR);
	Wire.write(data, 2);
	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return;
	}
	_initialized = true;
}

uint16_t usring::getValue()
{
	if (!_initialized)
		return 0;
	uint16_t value = 0;

	Wire.beginTransmission(USRING_I2C_ADDR);
	uint8_t data = USRING_REG;
	Wire.write(&data, 1);
  	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}
	Wire.requestFrom((uint8_t) USRING_I2C_ADDR, (uint8_t) 2);
	value = Wire.read();
	value += Wire.read() << 8;
	Wire.endTransmission();

	return value;
}

bool usring::isInitialized()
{
	return _initialized;
}

bool usring::checkAck()
{
	Wire.beginTransmission(USRING_I2C_ADDR);
	uint8_t error = Wire.endTransmission();
	if (error == 0)
		return true;
	return false;
}

uint8_t usring::getanalogValue(uint8_t id)
{
	if (!_initialized)
		return 0;
	uint8_t value = 0;

	Wire.beginTransmission(USRING_I2C_ADDR);
	uint8_t data = USRING_AN_REG + id;
	Wire.write(&data, 1);
  	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}

	Wire.requestFrom((uint8_t) USRING_I2C_ADDR, (uint8_t) 1);
	value = Wire.read();
	Wire.endTransmission();

	return value;
}


uint8_t usring::getThresholdValue(uint8_t id){
	if (!_initialized)
		return 0;
	uint8_t value = 0;

	Wire.beginTransmission(USRING_I2C_ADDR);
	uint8_t data = USRING_TH_REG + id;
	Wire.write(&data, 1);
  	if (Wire.endTransmission() != 0) {
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}

	Wire.requestFrom((uint8_t) USRING_I2C_ADDR, (uint8_t) 1);
	value = Wire.read();
	Wire.endTransmission();

	return value;
}


void usring::setThresholdValueGolbal(uint8_t th)
{
	if (!_initialized)
		return;

	uint8_t data[2];
	data[0] = 1;
	data[1] = th;
	Wire.beginTransmission(USRING_I2C_ADDR);
	Wire.write(data, 2);
	Wire.endTransmission();
}

void usring::setFilterValue(uint8_t value)
{
	if (!_initialized)
		return;

	uint8_t data[2];
	data[0] = 2;
	data[1] = value;
	Wire.beginTransmission(USRING_I2C_ADDR);
	Wire.write(data, 2);
	Wire.endTransmission();
}

void usring::setThresholdValue(uint8_t id, uint8_t th)
{
	if (!_initialized)
		return;

	uint8_t data[2];
	data[0] = 3+id;
	data[1] = th;
	Wire.beginTransmission(USRING_I2C_ADDR);
	Wire.write(data, 2);
	Wire.endTransmission();
}
