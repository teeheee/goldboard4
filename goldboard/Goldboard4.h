/**********************************************************************
@file       Goldboard4.h

@brief      Driver for Goldboard4 (w. Atmega32). Contains also access to the compass and sonar sensor.

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef __GOLDBOARD4_H__
#define __GOLDBOARD4_H__

#include "global.h"
#include "i2c.h"
#include "Motor.h"
#include "Serial.h"
#include "SRF08.h"
#include "CMPS03.h"

class Goldboard4
{
//variables
public:
Motor motor[4];

SoftwareSerial serial;
CMPS03 compass;
SRF08 sonar[4];

protected:
private:
	PCF8574A _pcf8574;

	bool _isLED[2];
	uint8_t _analogPulseInputConfiguration;
	

//functions
public:
	Goldboard4();
	
	/** Puts off all motors.
	*/
	void setMotorsOff();
	
	/** Sets a motor offset
	*/
	void setMotorsOffset(int16_t value);
	
	/** sets the given led id as led (NOTE: Then this pin cannot be used as button anymore)
	*/
	void initLED(uint8_t i);

	/** Puts LED i on if state is true, else off
	*/
	void setLed(uint8_t i, bool state);

	/** Puts the power output i on if state is true, else off
	*/
	void setPower(uint8_t i, bool state);

	/** Checks the state of button i. If it is pressed, true is returned,
	*  else false.
	*/
	bool getButton(uint8_t i);
	
	/** Waits until button i is pressed and released again.
	*/
	void waitForButton(uint8_t i);

	/** returns the value of the analog port i. 0 <= value <= 255
	*/
	uint8_t getAnalog(uint8_t i);

	/** returns true if the pwm port is logical high, else false.
	*/
	bool getPWM(uint8_t i);
	
	/** returns true if the digital port is logical high, else false.
	*/
	bool getDigital(uint8_t i);

	/** returns pwm value
	*/
	//uint8_t getPWM(uint8_t i);

	/** returns the registered pulse length of the digital port i. 0 <= value <= 255
	*/
	uint8_t getDigitalPulsedLight(uint8_t i);

	/** returns the registered pulse length of the pwm port i. 0 <= value <= 255
	*/
	uint8_t getPWMPulsedLight(uint8_t i);
protected:
private:
	Goldboard4( const Goldboard4 &c );
	Goldboard4& operator=( const Goldboard4 &c );
	
	uint8_t calcPulsedValue(unsigned long* val);

}; //Goldboard4

extern Goldboard4 gb;

#endif //__GOLDBOARD4_H__
