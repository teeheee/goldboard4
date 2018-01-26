/**********************************************************************
@file       CMPS03.cpp

@brief      Driver the CMPS03 compass module

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
		 
@editor  2017 Alexander Ulbrich
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version
1.01 3.07.2017	Added checkAck

@end
***********************************************************************/

#include "CMPS03.h"
#include "Goldboard4.h"

/** For debugging */
//#define CMPS03_SERIAL_PRINT(T)		SERIAL_PRINT(T)
//#define CMPS03_SERIAL_PRINTLN(T)		SERIAL_PRINTLN(T)
#define CMPS03_SERIAL_PRINT(T)
#define CMPS03_SERIAL_PRINTLN(T)

CMPS03::CMPS03()
{
	_initialized = false;
}

void CMPS03::init()
{
	_value = 0;
	_lastValue = 0;
	_128DegreeValue = 0;
	_measurementStarted = false;
	_initialized = true;
	_128DegreeEnabled = false;
}

bool CMPS03::checkAck()
{
	return i2cCheckAck(CMPS03_I2C_ADDR);
}

uint8_t CMPS03::getValue()
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

	// if there is a 128 degree value, change value that it matches the 180degrees
	CMPS03_SERIAL_PRINT("_128DegreeCorrectionAFTER: ");
	CMPS03_SERIAL_PRINTLN(_128DegreeValue);
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

void CMPS03::setAs128Degree()
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
		if (errorCounter > CMPS03_ERROR_TIMEOUT)
		{
			CMPS03_SERIAL_PRINTLN("ERROR TIMEOUT");
			return;
		}
	}
	
	_128DegreeValue = 128 - _128DegreeValue;
	_128DegreeEnabled = true;
}


void CMPS03::startMeasurement()
{
	_measurementStarted = false;
	
	i2cRead (CMPS03_I2C_ADDR, CMPS03_LORES_REG, &_value, 1);
	
	_measurementStarted = true;
}

bool CMPS03::getMeasurementValue(int16_t* val)
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

bool CMPS03::isInitialized()
{
	return _initialized;
}
