/**********************************************************************
@file       PCF8574A.h

@brief      Driver for the PCF8574A Remote 8-Bit I/O expander

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef PCF8574A_H_
#define PCF8574A_H_

#include "i2c.h"


class PCF8574A
{
	//variables
	public:
	protected:
	private:
	uint8_t _pcfdata;
	uint8_t _writeNeeded;

	//functions
	public:
	PCF8574A();
	
	void setPin(uint8_t, bool);
	
	uint8_t read();
	void write();
	
	protected:
	private:

}; //PCF8574Pin



#endif /* PCF8574A_H_ */