/**********************************************************************
@file       CMPS03.cpp

@brief      Driver the CMPS03 compass module

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "BNO055.h"
#include "Goldboard4.h"


BNO055::BNO055()
{
	_initialized = false;
}

void BNO055::init()
{
	_value = 0;
	_lastValue = 0;
	_128DegreeValue = 0;
	_measurementStarted = false;
	_initialized = true;
	_128DegreeEnabled = false;
}

uint8_t BNO055::getValue()
{
	if (!_initialized)
		return 0;

	if (!_measurementStarted) // if measurement was not started, start it
		startMeasurement();

	//-- if measurement has already been started, simply grab the value
	// but first check if there was an error
	if (!i2cIsComplete())
		return _lastValue;
	if (i2cGetLastError () != TW_NO_INFO)
		return _lastValue;

	int16_t val = 0;
	if (!getMeasurementValue(&val))
		return _lastValue;


	if (_128DegreeEnabled)
	{
		val += _128DegreeValue;
		if (val >= 255)
			val = 0 + (val - 255);
		else if (val < 0)
			val = 255 + val;
	}

	// set last value
	_lastValue = (uint8_t) val;
	return (uint8_t) val;
}

void BNO055::setAs128Degree()
{
	if (!_initialized)
		return;

	uint8_t errorCounter = 0;

	_128DegreeValue = 0;
	_128DegreeEnabled = false;
	while (_128DegreeValue == 0)
	{
		_128DegreeValue = getValue();
		delay(10);

		errorCounter++;

		// error... infinity loop
		if (errorCounter > BNO055_ERROR_TIMEOUT)
		{
			return;
		}
	}

	_128DegreeValue = 128 - _128DegreeValue;
	_128DegreeEnabled = true;
}


void BNO055::startMeasurement()
{
	_measurementStarted = false;

	i2cRead (BNO055_I2C_ADDR, BNO055_LORES_REG, &_value, 1);

	_measurementStarted = true;
}

bool BNO055::getMeasurementValue(int16_t* val)
{
	//if (!i2cIsComplete())
	//{
	//    return false;
	//}

	//if (i2cGetLastError () != TW_NO_INFO)
	//{
	//    return false;
	//}

	*val = _value;

	_measurementStarted = false;
	return true;
}

bool BNO055::isInitialized()
{
	return _initialized;
}



void BNO055::calibrate(uint8_t type)
{

}

bool BNO055::isCalibrating()
{
	uint8_t calData = 0;
	i2cRead(BNO055_I2C_ADDR,BNO055_CALIB_STAT_ADDR,&calData,1);
	  if((calData & 0x03) == 3)
	  	return true;
	  else
	  	return false;
}
void BNO055::setmode(uint8_t mode)
{

  	i2cWriteRegister(BNO055_I2C_ADDR,BNO055_OPR_MODE_ADDR, mode);
	delay(30);
}