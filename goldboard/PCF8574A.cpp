

#include "PCF8574A.h"
#include "Wire.h"

// default constructor
PCF8574A::PCF8574A()
{
	_pcfdata = 0;
	_writeNeeded = true;
	_address = 0;
}

void PCF8574A::init(uint8_t address)
{
	_address = address;
}

void PCF8574A::setPin(uint8_t pin, bool val)
{
	uint8_t oldPCFData = _pcfdata;
	if (val)
		_pcfdata |= (1 << pin);
	else
		_pcfdata &= ~(1 << pin);

	if (oldPCFData != _pcfdata)
		_writeNeeded = true;
}

uint8_t PCF8574A::read()
{
	Wire.beginTransmission(_address);
	Wire.requestFrom(_address, (uint8_t) 1);
	_pcfdata = Wire.read();
	Wire.endTransmission();
	return _pcfdata;
}

void PCF8574A::write()
{
	// only write if it's really needed, i2c writing costs time...
	if (!_writeNeeded)
		return;
	if (Wire.isTransmitting())
		return;
	Wire.beginTransmission(_address);
	Wire.write(&_pcfdata, 1);
	Wire.endTransmission();
	//twi_writeTo(_address,&_pcfdata,1,1,1);
	_writeNeeded = false;
}
