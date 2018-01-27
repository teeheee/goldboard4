/**********************************************************************
@file       Goldboard4.cpp

@brief      Driver for Goldboard4 (w. Atmega32). Contains also access to the compass and sonar sensor.

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
Christoph.Rothermel@gmx.de

@editor 2017 Alexander Ulbrich
alexander.ulbrich@uni-ulm.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version
1.1  6.7.2017   New Features added

@end
***********************************************************************/

#include "Goldboard4.h"
#include "i2c.h"
#include "time.h"
#include "infrared_pulse.h"


/******************* Goldboard Member Functions **********************/



// default constructor
Goldboard4::Goldboard4()
{
	uint8_t count;
	init_timer();
	
	twi_init();
	
	// Portexpanders
	digital.init(PCF8574A_DIGITAL_ADDRESS);
	_pcf8574.init(PCF8574A_MOTOR_ADDRESS);
	
	// Motors
	for (count=0; count<4; count++)
		motor[count].init(count,&_pcf8574);
	
	
	_analogPulseInputConfiguration = 0;
	
	
	// Buttons (DEFAULT)
	count = 0;
	for (count=0; count<BTLEDCOUNT; count++)
	{
		//pinMode(BTLEDARRAY[count], INPUT_PULLUP);
		//digitalWrite(BTLEDARRAY[count], HIGH);
		_isLED[count] = false;
	}
	
	// PWM Input
	count = 0;
	for (count=0; count<PWMCOUNT; count++);
	//pinMode(PWMARRAY[count], INPUT_PULLUP); //TODO: Change this for real pwm sensors
	
	// Analog
	count = 0;
	for (count=0; count<ACOUNT; count++);
	//pinMode(AARRAY[count], INPUT);
	
	// Digital
	count = 0;
	for (count=0; count<DCOUNT; count++);
	//pinMode(DARRAY[count], INPUT_PULLUP);
	
	// Power
	count = 0;
	for (count=0; count<PCOUNT; count++);
	//pinMode(PARRAY[count], OUTPUT);
	
} //Goldboard4

/** Puts off all motors.
*/
void Goldboard4::setMotorsOff()
{
	uint8_t i;
	for (i=0; i<MCOUNT; i++)
	motor[i].rotate(0);
}

/** Sets a motor offset
*/
void Goldboard4::setMotorsOffset(int16_t val)
{
	val = abs(val);
	if (val > 255)
	val = 255;
}

/** sets the given led id as led (NOTE: Then this pin cannot be used as button anymore)
*/
void Goldboard4::initLED(uint8_t i)
{
	if (i >= BTLEDCOUNT)
	return;
	
	pinMode(BTLEDARRAY[i], OUTPUT);
	digitalWrite(BTLEDARRAY[i], HIGH);
	_isLED[i] = true;
}

/** Puts LED i on if state is true, else off
*/
void Goldboard4::setLed(uint8_t i, bool state)
{
	if (i >= BTLEDCOUNT)
	return;
	pinMode(BTLEDARRAY[i],OUTPUT);
	_isLED[i] = true;
	digitalWrite(BTLEDARRAY[i], !state);
}

/** Puts the power output i on if state is true, else off
*/
void Goldboard4::setPower(uint8_t i, bool state)
{
	if(state)
	setPowerPWM(i, 0xff);
	else
	setPowerPWM(i, 0);
}

void Goldboard4::setPowerPWM(uint8_t i, uint8_t state)
{
	if (i >= PCOUNT)
	return;
}

/** sets the PWM pin (0-7) as Servo Output and sends a certain value (0-255 ~ 0.8-2.8ms pulse) every 20 ms
*/
void Goldboard4::setPWMServo(uint8_t value, uint8_t pin)
{
	pinMode(PWMARRAY[pin],OUTPUT);
	value = value*(40.0f/255.0f); //Wertebereich anpassen ...
	if(value>40)
	value = 40;
}

/**stops the PWM pulse. Dont use this, if you want to stop a brushless motor. Use setPWMServo(0,0) instead.
*/
void Goldboard4::resetPWMServo(uint8_t pin)
{
	pinMode(PWMARRAY[pin],INPUT_PULLUP);
}

/** Checks the state of button i. If it is pressed, true is returned,
*  else false.
*/
bool Goldboard4::getButton(uint8_t i)
{
	if (i >= BTLEDCOUNT)
	return false;
	
	uint8_t ret;
	
	if (_isLED[i])
	{
		uint8_t ledstate = digitalRead(BTLEDARRAY[i]);
		pinMode(BTLEDARRAY[i],INPUT);
		_isLED[i] = false;
		ret = !digitalRead(BTLEDARRAY[i]);
		digitalWrite(BTLEDARRAY[i],ledstate);
	}
	else
	ret = !digitalRead(BTLEDARRAY[i]);
	// low is true
	return ret;
}

/** Waits until button i is pressed and released again.
*/
void Goldboard4::waitForButton(uint8_t i)
{
	if (i >= BTLEDCOUNT)
	return;
	
	while(!getButton(i));
	while(getButton(i));
}

/** returns the value of the analog port i. 0 <= value <= 255
*/
uint8_t Goldboard4::getAnalog(uint8_t i)
{
	if (i >= ACOUNT)
	return 0;
	
	return (uint8_t) ((int) (analogRead(AARRAY[i]) / 4));
}

/** returns true if the digital port is logical high, else false.
*/
bool Goldboard4::getDigital(uint8_t i)
{
	if (i >= DCOUNT)
	{
		if(i >= DCOUNT+8)
			return false;
		return (digital.read() & (1 << (i-DCOUNT)));
	}
	pinMode(DARRAY[i],INPUT_PULLUP);
	return !digitalRead(DARRAY[i]);
}

void Goldboard4::setDigital(uint8_t i,bool state)
{
	if (i < DCOUNT)
	{
		pinMode(DARRAY[i],OUTPUT);
		digitalWrite(DARRAY[i],state);
	}
	else if(i < DCOUNT+8)
	{
		digital.setPin(i-DCOUNT,state);
		digital.write();
	}
}

/** returns true if the pwm port is logical high, else false.
*/
bool Goldboard4::getPWM(uint8_t i)
{
	pinMode(PWMARRAY[i],INPUT_PULLUP);
	if (i >= PWMCOUNT)
	return false;
	
	return !digitalRead(PWMARRAY[i]);
}


#ifdef FAST_IRDETECTOR

/** returns the registered pulse length of the digital port i. 0 <= value <= 255
*/
uint8_t Goldboard4::getPWMPulsedLight(uint8_t i)
{
	return 0;
}

#else

/** returns the registered pulse length of the pwm port i. 0 <= value <= 255
*/
uint8_t Goldboard4::getPWMPulsedLight(uint8_t i)
{
	pinMode(PWMARRAY[i],INPUT_PULLUP);
	if (i >= PWMCOUNT)
	return 0;
	
	unsigned long duration = pulseIn(PWMARRAY[i],LOW,1500);
	return calcPulsedValue(&duration);
}

#endif

uint8_t Goldboard4::getDigitalPulsedLight(uint8_t i)
{
	if (i >= DCOUNT)
	return 0;
	
	unsigned long duration = pulseIn(DARRAY[i],LOW,1500);
	return calcPulsedValue(&duration);
}


uint8_t Goldboard4::calcPulsedValue(unsigned long* val)
{
	if (*val == 0)
	return 0;
	else
	{
		// val is the raw pulse value
		// test have shown that val is not higher than 100 which is in the 8bit sector
		uint8_t value = 0;
		if (*val > 255)
		value = 255;
		else
		value = (uint8_t) *val;
		
		return value;
	}
}

