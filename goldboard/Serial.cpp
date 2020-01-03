/**********************************************************************
@file       Serial.cpp

@brief      Contains the functions for the Goldboard4 Serial/UART

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/


#include "Serial.h"

// default constructor
SoftwareSerial::SoftwareSerial()
{
} //Serial

void SoftwareSerial::begin(unsigned int baud)
{
	uart_init(UART_BAUD_SELECT(baud,F_CPU) );
}
size_t SoftwareSerial::write(uint8_t buffer)
{
	uart_putc((unsigned char) buffer);
	return sizeof(buffer);
}

// default destructor
SoftwareSerial::~SoftwareSerial()
{
} //~Serial
