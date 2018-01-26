/**********************************************************************
@file       PCF8574A.cpp

@brief      Driver for the PCF8574A Remote 8-Bit I/O expander

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "PCF8574A.h"
#include "twi.h"

// default constructor
PCF8574A::PCF8574A()
{
	_pcfdata = 0;
	_writeNeeded = true;
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
	//i2cRead (_address + 1, 0, &_pcfdata, 1);
	twi_read(_address,1,0);
	_pcfdata = *twi_wait();
	return _pcfdata;
}

void PCF8574A::write()
{
	// only write if it's really needed, i2c writing costs time...
	if (!_writeNeeded)
		return;
		
	//i2cWriteToSlave (_address, &_pcfdata, 1);
	twi_write(_address,&_pcfdata,1,0);
	_writeNeeded = false;
}