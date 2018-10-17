/*
 * SRF08.cpp
 *
 *  Created on: 06.08.2018
 *      Author: alex
 */


/**********************************************************************
@file       SRF08.cpp

@brief      Driver for the SRF08 sonar sensor

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "SRF08.h"
#include "Goldboard4.h"
#include "error.h"
#include "time.h"




/** SRF08 defines */
#define SRF08_I2C_START_ADDRESS	0xE0
#define SRF08_I2C_BROADCAST_ADDRESS   0

#define SRF08_MIN_GAIN        0      /* sets gain to 94   */
#define SRF08_MAX_GAIN        31     /* sets gain to 1025 */
#define SRF08_MIN_RANGE       0      /* in millimeters    */
#define SRF08_MAX_RANGE       11008  /* in millimeters    */

#define SRF08_MEASUREMENT_IN_PROGRESS   0xFF   /* measurement is in progress */

#define SRF08_INCHES             0x50
#define SRF08_CENTIMETERS        0x51
#define SRF08_MICROSECONDS       0x52
#define SRF08_ANN_INCHES         0x53
#define SRF08_ANN_CENTIMETERS    0x54
#define SRF08_ANN_MICROSECONDS   0x55

/* Write register */
#define SRF08_REG_CMD         0
#define SRF08_REG_GAIN        1
#define SRF08_REG_RANGE       2

/* Read register */
#define SRF08_SW_VER          0
#define SRF08_LIGHT           1
#define SRF08_ECHO_1_HIGH     2
#define SRF08_ECHO_1_LOW      3
#define SRF08_ECHO_MULTI_1    4

SRF08::SRF08()
{
	_initialized = false;
	_lastValue = 0;
	_maxValue = 0;
	_state = STATE_START;
	_address = 0;
}

void SRF08::init(uint8_t address)
{
	_address = SRF08_I2C_START_ADDRESS/2 + (address);
	_lastValue = 0;
	_maxValue = 0;
	_state = STATE_START;

	// amplification register to center improvement
	setGain (0);
	//srf08SetGain (id, 2);
	delay (100);

	// 70: range register to 3053mm (formula: range=(register*43)+43 mm)
	// 35: range register to 1548mm (formula: range=(register*43)+43 mm)
	// 47: 2064mm
	// 60: 2623mm
	//srf08SetRange (id, 1548);
	setRange (2623);
	delay (100);

	if( ! checkACK() )
	{
		ERROR_MESSAGE("SRF08 init");
		return;
	}
	delay (50);
	_initialized = true;
}

bool SRF08::measurementStart()
{
	if (!_initialized)
		return false;
	if (_state == STATE_START || _state == STATE_RESULT_RECEIVED)
	{
		// request centimeters
		_i2cWriteRegister (SRF08_REG_CMD, SRF08_CENTIMETERS);
		_state = STATE_CM_SENT;
		return true;
	}
	return false;
}

bool SRF08::checkACK()
{
	Wire.beginTransmission(PCF8574A_MOTOR_ADRESS);
	uint8_t error = Wire.endTransmission();
	if(error > 0)
		return false;
	else
		return true;
}

bool SRF08::measurementComplete()
{
	if (!_initialized)
		return false;
	Wire.beginTransmission(_address);  //starts communication with CMPS11
	Wire.write(0);           //Sends the register we wish to start reading from
	if(Wire.endTransmission()!=0)
	{
		return false;
	}
	Wire.requestFrom(_address, 1);
	unsigned long time = millis();
	while(Wire.available() < 1){
		if(millis()-time > 1000)
		{
			ERROR_MESSAGE("SRF timeout");
			return false;
		}
	}
	if(Wire.read()<0xff)
		return true;
	return false;
}

int SRF08::measurementGet()
{
	if (!_initialized)
		return 0;
	if (_state == STATE_CM_SENT)
	{
		Wire.beginTransmission(_address);  //starts communication with CMPS11
		Wire.write(SRF08_ECHO_1_HIGH);           //Sends the register we wish to start reading from
		if(Wire.endTransmission()!=0)
		{
			ERROR_MESSAGE("SRF error");
		  	return 0;
		}
		Wire.requestFrom(_address, 2);
		unsigned long time = millis();
		while(Wire.available() < 2){
			if(millis()-time > 1000)
			{
				ERROR_MESSAGE("SRF timeout");
				return 0;
			}
		}
		_lastValue = Wire.read();               // Read back the 5 bytes
		_lastValue = Wire.read() + (_lastValue << 8);


		if (_lastValue == 0)
			_lastValue = _maxValue;
		else if (_lastValue < 2)
			_lastValue = 0;
		_state = STATE_RESULT_RECEIVED;
		return _lastValue;
	}
	return 0;
}

// getValueCM
int SRF08::getValueCM()
{
	if(measurementStart())
		return _lastValue;
	if(!measurementComplete())
		return _lastValue;
	return measurementGet();
}

// getValueCM
int SRF08::getValueCMBlocking()
{
	measurementStart();
	unsigned long time = millis();
	do{
		if(millis()-time > 1000)
		{
			ERROR_MESSAGE("SRF timeout");
			return false;
		}
	}while(!measurementComplete());
	return measurementGet();
}

// changes the address of the sonar
void SRF08::changeAddress(uint8_t newAddress)
{
	if (!_initialized)
		return;

	uint8_t data [] = {0xA0, 0xAA, 0xA5, (uint8_t) (SRF08_I2C_START_ADDRESS + (newAddress * 2))};
	for (int i = 0; i < 4; i++)
	{
		_i2cWriteRegister (SRF08_REG_CMD, data[i]);
		delay(50);
	}

	// finally set address of this sonar
	_address = newAddress;
}

bool SRF08::isInitialized()
{
	return _initialized;
}

void SRF08::setGain (uint8_t gain)
{
	if (gain > SRF08_MAX_GAIN)
		gain = SRF08_MAX_GAIN;

	_i2cWriteRegister (SRF08_REG_GAIN, gain);
}


void SRF08::setRange (uint16_t millimeters)
{
	_maxValue = millimeters / 10;

	millimeters = (millimeters / 43) - 1;
	if (millimeters > 0xff)
		millimeters = 0xff;

	_i2cWriteRegister (SRF08_REG_RANGE, millimeters);
}

void SRF08::_i2cWriteRegister(uint8_t reg,uint8_t data)
{
	Wire.beginTransmission(_address);  //starts communication with CMPS11
	Wire.write(reg);           //Sends the register we wish to start reading from
	Wire.write(data);
	if(Wire.endTransmission()!=0)
	{
		  ERROR_MESSAGE("SRF write");
	}
}
