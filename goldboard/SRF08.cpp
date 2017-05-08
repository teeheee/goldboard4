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


/** For debugging */
//#define SRF08_SERIAL_PRINT(T)		SERIAL_PRINT(T)
//#define SRF08_SERIAL_PRINTLN(T)		SERIAL_PRINTLN(T)
#define SRF08_SERIAL_PRINT(T) 
#define SRF08_SERIAL_PRINTLN(T)


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
}

void SRF08::init(uint8_t address)
{
	_address = SRF08_I2C_START_ADDRESS + (address * 2);

	_lastValue = 0;
	_maxValue = 0;
	_state = STATE_START;
	
	// amplification register to center improvement
	setGain (0);
	//srf08SetGain (id, 2);
	_delay_ms (100);

	// 70: range register to 3053mm (formula: range=(register*43)+43 mm)
	// 35: range register to 1548mm (formula: range=(register*43)+43 mm)
	// 47: 2064mm
	// 60: 2623mm
	//srf08SetRange (id, 1548);
	setRange (2623);
	_delay_ms (100);
	
	
	uint8_t i2cError;
	uint8_t ver;
	i2cError = i2cWait ();
	i2cReadRegister (_address, SRF08_SW_VER, &ver);
	i2cError = i2cWait ();
	if (i2cError != TW_NO_INFO)
	{
		SRF08_SERIAL_PRINT("SRF08::init i2cError ");
		SRF08_SERIAL_PRINTLN(i2cError);
		return;
	}
	_delay_ms (50);
	_initialized = true;
}

// getValueCM
int SRF08::getValueCM()
{
	if (!_initialized)
		return 0;
	
	uint8_t buffer [2];
	uint8_t i2cError;
	
	// 1) request value as centimeters and return the last value
	if (_state == STATE_START || _state == STATE_RESULT_RECEIVED)
	{
		i2cError = i2cWait ();
		if (i2cError != TW_NO_INFO)
		{
			SRF08_SERIAL_PRINT("SRF08::getValueCM i2cError");
			SRF08_SERIAL_PRINTLN(i2cError);
			return _lastValue;
		}
		// request centimeters
		i2cWriteRegister (_address, SRF08_REG_CMD, SRF08_CENTIMETERS);
		
		_state = STATE_CM_SENT;
		
		return _lastValue;
	}

	// not finished?
	// 2) datasheet says, that this takes at least 65ms
	if (!i2cIsComplete())
		return _lastValue;
	
	i2cReadRegister (_address, SRF08_SW_VER, buffer);
	i2cError = i2cWait ();
	if (i2cError != TW_NO_INFO)
	{
		SRF08_SERIAL_PRINT("SRF08::getValueCM i2cError");
		SRF08_SERIAL_PRINTLN(i2cError);
		return _lastValue;
	}
	if (buffer [0] == SRF08_MEASUREMENT_IN_PROGRESS)
	{
		return _lastValue;
	}

	// 3) instruct sonar to return value
	if (_state == STATE_CM_SENT)
	{
		i2cRead (_address, SRF08_ECHO_1_HIGH, buffer, 2);
		i2cError = i2cWait ();
		if (i2cError != TW_NO_INFO)
		{
			SRF08_SERIAL_PRINT("SRF08::getValueCM i2cError");
			SRF08_SERIAL_PRINTLN(i2cError);
			return _lastValue;
		}
		
		_state = STATE_RESULT_REQUEST_SENT;
	}

	// 4) request new value
	if (_state == STATE_RESULT_REQUEST_SENT)
	{		
		// 5, 6) finally get value
		_lastValue  = buffer [0] << 8;
		_lastValue &= 0xFF00;
		_lastValue += buffer [1];
		
		if (_lastValue == 0)
			_lastValue = _maxValue;
		else if (_lastValue < 2)
			_lastValue = 0;
		_state = STATE_RESULT_RECEIVED;
	}

	// 7) start measurement for the next time (this also returns the next value)
	if (_state == STATE_RESULT_RECEIVED)
	{
		_state = STATE_START;
		return getValueCM();
	}

	return _lastValue;
}

// changes the address of the sonar
void SRF08::changeAddress(uint8_t newAddress)
{
	if (!_initialized)
		return;
	
	uint8_t data [] = {0xA0, 0xAA, 0xA5, (uint8_t) (SRF08_I2C_START_ADDRESS + (newAddress * 2))};
	for (int i = 0; i < 4; i++)
	{
		i2cWriteRegister (_address, SRF08_REG_CMD, data[i]);
		_delay_ms (50);
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
	
	i2cWriteRegister (_address, SRF08_REG_GAIN, gain);
} 


void SRF08::setRange (uint16_t millimeters)
{
	_maxValue = millimeters / 10;
	
	millimeters = (millimeters / 43) - 1;
	if (millimeters > 0xff)
		millimeters = 0xff;

	i2cWriteRegister (_address, SRF08_REG_RANGE, millimeters);
}