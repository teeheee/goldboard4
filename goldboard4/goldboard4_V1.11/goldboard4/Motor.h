/**********************************************************************
@file       Motor.h

@brief      Driver for motors on Goldboard4

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
         
@editor 2017 Alexander Ulbrich
         alexander.ulbrich@uni-ulm.de
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
		void init(uint8_t pwmPin, PCF8574A* directionPortexpander);
		void rotate(int16_t speed);
		void setAcceleration(uint8_t percentPerHundretMs);
		//void stop(bool bremsen);
		
	private:
		uint8_t _pwmPin;
		PCF8574A* _directionPortexpander;
		
};

void motor_isr();

#endif

