

#include "CMPS11.h"
#include "Goldboard4.h"
#include "Wire.h"




CMPS11::CMPS11()
{
	_initialized = false;
	init();
}

void CMPS11::init()
{
	_value = 0;
	_128DegreeValue = 0;
	_initialized = true;
	_128DegreeEnabled = false;
}

uint8_t CMPS11::getValue()
{
	if (!_initialized)
		return 0;
		
	int16_t val = 0;

	  Wire.beginTransmission(CMPS11_I2C_ADDR);  //starts communication with CMPS11
	  Wire.write(CMPS11_ANGLE_8_REG);           //Sends the register we wish to start reading from
	  Wire.endTransmission();

	  // Request 5 bytes from the CMPS11
	  // this will give us the 8 bit bearing,
	  // both bytes of the 16 bit bearing, pitch and roll
	  Wire.requestFrom(CMPS11_I2C_ADDR, 1);
	  while(Wire.available() < 1);        // Wait for all bytes to come back
	  val = Wire.read();               // Read back the 5 bytes

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

void CMPS11::setAs128Degree()
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

		if (errorCounter > CMPS11_ERROR_TIMEOUT)
		{
			return;
		}
	}
	
	_128DegreeValue = 128 - _128DegreeValue;
	_128DegreeEnabled = true;
}

bool CMPS11::isInitialized()
{
	return _initialized;
}
