#include "CMPS03.h"

const char error_prefix[] PROGMEM = "CMPS03: ";

CMPS03::CMPS03()
{
	init();
}

void CMPS03::init(){
	_128DegreeValue = 0;
	_128DegreeEnabled = false;
}

uint8_t CMPS03::getValue()
{
	Wire.beginTransmission(CMPS03_I2C_ADDR);
	Wire.write((uint8_t) CMPS03_LORES_REG);
	if( Wire.endTransmission() > 0)
	{
		ERROR_MESSAGE_PREFIX(error_prefix, progmem_no_ack);
		return 0;
	}
	Wire.requestFrom(CMPS03_I2C_ADDR, 1);

	unsigned long time = millis();
	while(Wire.available() < 1)
		if(millis()-time > CMPS03_ERROR_TIMEOUT)
		{
			ERROR_MESSAGE_PREFIX(error_prefix, progmem_timeout);
			return false;
		}
	
	uint8_t val = Wire.read();

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
	uint8_t errorCounter = 0;

	_128DegreeValue = 0;
	_128DegreeEnabled = false;
	while (_128DegreeValue == 0)
	{
		_128DegreeValue = getValue();
		delay(10);

		errorCounter++;

		if (errorCounter > 10)
		{
			ERROR_MESSAGE_PREFIX_(error_prefix, "set 128 error");
			return;
		}
	}
	
	_128DegreeValue = 128 - _128DegreeValue;
	_128DegreeEnabled = true;
}

bool CMPS03::checkACK(){
	Wire.beginTransmission(CMPS03_I2C_ADDR);
	uint8_t error = Wire.endTransmission();
	if (error == 0)
		return true;
	return false;
}
