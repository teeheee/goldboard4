/**********************************************************************
@file       Motor.h

@brief      Driver for motors on Goldboard4

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef _MOTOR_h
#define _MOTOR_h

#include "global.h"
#include "PCF8574A.h"

class Motor
{
	public:
		Motor();
		//Motor(uint8_t directionPinFirst, uint8_t directionPinSecond, uint8_t speedPin);
		void init(uint8_t directionPinFirst, uint8_t directionPinSecond, uint8_t speedPin, PCF8574A*, uint8_t*);
		void rotate(int16_t speed);
		void stop(bool bremsen);
		
		uint8_t* speed;
	private:
		PCF8574A* _pcf8574;
		uint8_t _directionPinFirst;
		uint8_t _directionPinSecond;
};


#endif

