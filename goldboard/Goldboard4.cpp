/**********************************************************************
@file       Goldboard4.cpp

@brief      Driver for Goldboard4 (w. Atmega32). Contains also access to the compass and sonar sensor.

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "Goldboard4.h"

// function prototype for motorISR
void motorISR();

// Global interrupt
ISR(TIMER0_OVF_vect)
{
	wiringISR();
	motorISR();
}

static uint8_t motorOffset = 0;
static uint16_t motorCounter = 255 + motorOffset;
static uint8_t motorSpeeds[4];

/** controls the motor speeds
*/
void motorISR()
{
	if (motorSpeeds[0] == 0)
		cbi(PORTB, 0);
	else if (((uint16_t) motorSpeeds[0] + motorOffset) >= motorCounter)
		sbi(PORTB, 0);
	else
		cbi(PORTB, 0);
	
	if (motorSpeeds[1] == 0)
		cbi(PORTB, 1);
	else if (((uint16_t) motorSpeeds[1] + motorOffset)  >= motorCounter)
		sbi(PORTB, 1);
	else
		cbi(PORTB, 1);
	
	if (motorSpeeds[2] == 0)
		cbi(PORTB, 2);
	else if (((uint16_t) motorSpeeds[2] + motorOffset)  >= motorCounter)
		sbi(PORTB, 2);
	else
		cbi(PORTB, 2);
	
	if (motorSpeeds[3] == 0)
		cbi(PORTB, 3);
	else if (((uint16_t) motorSpeeds[3] + motorOffset)  >= motorCounter)
		sbi(PORTB, 3);
	else
		cbi(PORTB, 3);
	
	if (motorCounter == 0)
		motorCounter = 255 + motorOffset;
	else
		motorCounter--;
}

// default constructor
Goldboard4::Goldboard4()
{
#ifdef DEBUG
	serial.begin(UART_BAUD_RATE); // initializes the uart interface
#warning Debug ist aktiviert. Bitte schalte Debug aus, um das Programm zu beschleunigen.
#endif

	init(); // initializes interrupts etc
	i2cInit(I2C_SPEED);  // initializes the i2c bus
	init_adc(); // Analogwandler initialisieren
	
	_analogPulseInputConfiguration = 0;
	
	uint8_t count;
	
	// Buttons (DEFAULT)
	count = 0;
	for (count=0; count<BTLEDCOUNT; count++)
	{
		pinMode(BTLEDARRAY[count], INPUT_PULLUP);
		digitalWrite(BTLEDARRAY[count], HIGH);
		_isLED[count] = false;
	}
	
	// PWM Input
	count = 0;
	for (count=0; count<PWMCOUNT; count++)
		pinMode(PWMARRAY[count], INPUT_PULLUP); //TODO: Change this for real pwm sensors
	
	// Analog
	count = 0;
	for (count=0; count<ACOUNT; count++)
		pinMode(AARRAY[count], INPUT);
	
	// Digital
	count = 0;
	for (count=0; count<DCOUNT; count++)
		pinMode(DARRAY[count], INPUT_PULLUP);
	
	// Power
	count = 0;
	for (count=0; count<PCOUNT; count++)
		pinMode(PARRAY[count], OUTPUT);
	
	
	// Motors
	count = 0;
	for (count=0; count<MCOUNT; count++)
		motor[count].init(MDARRAY[count][0], MDARRAY[count][1], MEARRAY[count], &_pcf8574, &motorSpeeds[count]); 
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
		
	motorOffset = val;
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
	
	if (!_isLED[i])
		return;
		
	digitalWrite(BTLEDARRAY[i], !state);
}

/** Puts the power output i on if state is true, else off
*/
void Goldboard4::setPower(uint8_t i, bool state)
{
	if (i >= PCOUNT)
		return;
	
	digitalWrite(PARRAY[i], state);
}

/** Checks the state of button i. If it is pressed, true is returned,
*  else false.
*/
bool Goldboard4::getButton(uint8_t i)
{
	if (i >= BTLEDCOUNT)
		return false;
	
	// low is true
	return !digitalRead(BTLEDARRAY[i]);
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
		return false;
	
	return !digitalRead(DARRAY[i]);
}

/** returns true if the pwm port is logical high, else false.
*/
bool Goldboard4::getPWM(uint8_t i)
{
	if (i >= PWMCOUNT)
	return false;
	
	return !digitalRead(PWMARRAY[i]);
}

/** PWM for Ball Sensor SFH 5110
*/
//uint8_t Goldboard4::getPWM(uint8_t i)
//{
	//if (i >= PWMCOUNT)
		//return 0;
	//
	//return (uint8_t) ((int) (analogRead(PWMARRAY[i]) / 4));
//}

/** returns the registered pulse length of the analog port i. 0 <= value <= 255
*/
//uint8_t Goldboard4::getAnalogPulsedLight(uint8_t i)
//{
	//if (i >= ACOUNT)
		//return 0;
		//
	//if (!(_analogPulseInputConfiguration & (1 << i)))
	//{
		//pinMode(AARRAY[i], INPUT_PULLUP);
		//SERIAL_PRINTLN("SET AS PULLUP");
		//_analogPulseInputConfiguration |= (1 << i);
	//}
	//
	//unsigned long duration = pulseIn(AARRAY[i],LOW,1500);
	//return calcPulsedValue(&duration);
//}

/** returns the registered pulse length of the digital port i. 0 <= value <= 255
*/
uint8_t Goldboard4::getDigitalPulsedLight(uint8_t i)
{
	if (i >= DCOUNT)
		return 0;
		
	unsigned long duration = pulseIn(DARRAY[i],LOW,1500);
	return calcPulsedValue(&duration);
}

/** returns the registered pulse length of the pwm port i. 0 <= value <= 255
*/
uint8_t Goldboard4::getPWMPulsedLight(uint8_t i)
{
	if (i >= PWMCOUNT)
		return 0;
		
	unsigned long duration = pulseIn(PWMARRAY[i],LOW,1500);
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