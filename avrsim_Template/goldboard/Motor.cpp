/**********************************************************************
@file       Motor.cpp

@brief      Driver for motors on Goldboard4

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "Motor.h"
#include "Goldboard4.h"

Motor::Motor()
{
	
}

/*Motor::Motor(uint8_t directionPinFirst, uint8_t directionPinSecond, uint8_t speedPin)
{
	init(directionPinFirst, directionPinSecond, speedPin);
}*/

void Motor::init(uint8_t directionPinFirst, uint8_t directionPinSecond, uint8_t spPin, PCF8574A* pcf8574, uint8_t* gbSpeed)
{
	// private
	_directionPinFirst = directionPinFirst;
	_directionPinSecond = directionPinSecond;
	_pcf8574 = pcf8574;
	
	// public
	speed = gbSpeed;

	// set pin modes
	// NOTE: DIRECTION PINS are on PCF5784
	pinMode(spPin, OUTPUT);
	
	// set as leerlauf
	stop(false);	
}

void Motor::rotate(int16_t sp)
{
	if (sp == 0)
	{
		stop(false);
		return;
	}
	
	if(sp > 0)
	{
		_pcf8574->setPin(_directionPinFirst, LOW);
		_pcf8574->setPin(_directionPinSecond, HIGH);
	}
	else
	{
		_pcf8574->setPin(_directionPinFirst, HIGH);
		_pcf8574->setPin(_directionPinSecond, LOW);
	}
	_pcf8574->write();
	
	// Offset beachten!
	sp = abs(sp);
	if (sp > 255)
		sp = 255;
		
	
	*speed = (uint8_t) sp;
}

void Motor::stop(bool bremsen)
{
	if(bremsen) // motor bremsen (sollte nur im notfall verwendet werden)
	{	
		*speed = 255;
		_pcf8574->setPin(_directionPinFirst, HIGH);
		_pcf8574->setPin(_directionPinSecond, HIGH);
	}
	else // motor auslaufen lassen
	{
		*speed = 0;
		_pcf8574->setPin(_directionPinFirst, LOW);
		_pcf8574->setPin(_directionPinSecond, LOW);
	}
	_pcf8574->write();
}
