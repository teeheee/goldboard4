#include "CMPS11.h"
#include "Goldboard4.h"
#include "Wire.h"
#include "time.h"

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
	  if(Wire.endTransmission()!=0)
	  {
	  		  ERROR_MESSAGE("CMPS11 error");
	  		  return 0;
	  }

	  Wire.requestFrom(CMPS11_I2C_ADDR, 1);
	  while(Wire.available() < 1);
	  val = Wire.read();

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


int CMPS11::getAccelerometerX()
{
	if (!_initialized)
		return 0;
	int val = 0;
	Wire.beginTransmission(CMPS11_I2C_ADDR);  //starts communication with CMPS11
	Wire.write(CMPS11_ACCELERATION_X_REG);           //Sends the register we wish to start reading from
	  if(Wire.endTransmission()!=0)
	  {
	  		  ERROR_MESSAGE("CMPS11 error");
	  		  return 0;
	  }

	  Wire.requestFrom(CMPS11_I2C_ADDR, 2);
	  while(Wire.available() < 2);        // Wait for all bytes to come back
	  val = Wire.read();               // Read back the 5 bytes
	  val = Wire.read() + (val << 8);
	  return val;
}

int CMPS11::getAccelerometerY()
{
	if (!_initialized)
		return 0;
	int val = 0;
	Wire.beginTransmission(CMPS11_I2C_ADDR);  //starts communication with CMPS11
	Wire.write(CMPS11_ACCELERATION_Y_REG);           //Sends the register we wish to start reading from
	  if(Wire.endTransmission()!=0)
	  {
	  		  ERROR_MESSAGE("CMPS11 error");
	  		  return 0;
	  }

	  Wire.requestFrom(CMPS11_I2C_ADDR, 2);
	  while(Wire.available() < 2);        // Wait for all bytes to come back
	  val = Wire.read();               // Read back the 5 bytes
	  val = Wire.read() + (val << 8);
	  return val;
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

void CMPS11::cmd(uint8_t byte)
{
	  Wire.beginTransmission(CMPS11_I2C_ADDR);
	  Wire.write(0);
	  Wire.write(byte);
	  if(Wire.endTransmission()!=0)
	  {
	  		  ERROR_MESSAGE("CMPS11 error");
	  }
	  delay(50);
}

void CMPS11::startCalibration()
{
	  uint8_t buffer[3] = CMPS11_CALIBRATION_SEQUENCE;
	  cmd(buffer[0]);
	  cmd(buffer[1]);
	  cmd(buffer[2]);
}

void CMPS11::exitCalibration()
{
	  cmd(0xF8);
	  delay(50);
}
