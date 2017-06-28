/**********************************************************************
@file       Serial.h

@brief      Contains the functions for the Goldboard4 Serial/UART

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define DEBUG

#ifdef DEBUG
#define SERIAL_PRINTLN(t) gb.serial.println(t)
#define SERIAL_PRINT(t) gb.serial.print(t)
#else
#define SERIAL_PRINTLN(t)
#define SERIAL_PRINT(t)
#endif

#include "Print.h"
#include "uart.h"
#include "config.h"

class SoftwareSerial : public Print
{
//variables
public:
protected:
private:

//functions
public:
	SoftwareSerial();
	~SoftwareSerial();
	void begin(unsigned long);
	virtual size_t write(uint8_t);
protected:
private:
	//SoftwareSerial( const SoftwareSerial &c );
	//SoftwareSerial& operator=( const SoftwareSerial &c );

}; //Serial

#endif //__SERIAL_H__
