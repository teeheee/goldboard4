/**********************************************************************
@file       usring.cpp

@brief      Driver the usring Sensor module

-----------------------------------------------------------------------
@author  2017 Alexander Ulbrich
			alexander.ulbrich@uni-ulm.de
-----------------------------------------------------------------------
@History

1.00 29.6.2017 First working version

@end
***********************************************************************/

#include "usring.h"
#include "Goldboard4.h"


usring::usring()
{
	_initialized = false;
}

void usring::init()
{
	i2cWriteRegister(USRING_I2C_ADDR, 0, 1);
	delay(2000); // let the Sensor Calibrate
	_initialized = true;
}

uint16_t usring::getValue()
{
	if (!_initialized)
		return 0;
	uint16_t value = 0;

	i2cRead(USRING_I2C_ADDR, USRING_REG, (uint8_t*)&value, 2);

	return value;
}

bool usring::isInitialized()
{
	return _initialized;
}

bool usring::checkAck()
{
	return i2cCheckAck(USRING_I2C_ADDR);
}

uint8_t usring::getanalogValue(uint8_t id)
{
	if (!_initialized)
		return 0;
	uint8_t value = 0;

	i2cRead(USRING_I2C_ADDR, USRING_AN_REG + id, (uint8_t*)&value, 1);

	return value;
}


/*
uint8_t usring::getThresholdValue(uint8_t id)
{
	if (!_initialized)
		return 0;
	uint8_t value = 0;

	i2cRead(USRING_I2C_ADDR,USRING_TH_REG + id, (uint8_t*)&value, 1);

	return value;
}

void usring::setThresholdValue(uint8_t id, uint8_t th)
{
	if (!_initialized)
		return;

	i2cWriteRegister(USRING_I2C_ADDR, 2+id, th);

}*/

void usring::setThresholdValueGolbal(uint8_t th)
{
	if (!_initialized)
		return;

	i2cWriteRegister(USRING_I2C_ADDR, 1, th);

}