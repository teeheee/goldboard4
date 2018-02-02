#include "CMPS03.h"


CMPS03::CMPS03()
{
	_initialized = false;
	init();
}

void CMPS03::init()
{
	_value = 0;
	_128DegreeValue = 0;
	_initialized = true;
	_128DegreeEnabled = false;
}

uint8_t CMPS03::getValue()
{
	if (!_initialized)
		return 0;

	Wire.beginTransmission(CMPS03_I2C_ADDR);
	Wire.write((uint8_t) CMPS03_LORES_REG);
	Wire.endTransmission();
	Wire.requestFrom(CMPS03_I2C_ADDR, 1);
	while(Wire.available() < 1);
	_value = Wire.read();
	
	uint8_t val = _value;

	if (_128DegreeEnabled)
	{
		val += _128DegreeValue;
		if (val >= 255)
			val = 0 + (val - 255);
		else if (val < 0)
			val = 255 + val;
	}

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
			return;
		}
	}
	
	_128DegreeValue = 128 - _128DegreeValue;
	_128DegreeEnabled = true;
}

bool CMPS03::isInitialized()
{
	return _initialized;
}
