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

#ifdef FAST_IRDETECTOR
	void pulseISR();
#endif

#ifdef ISR_TIMER
	static uint8_t isr_time = 0;

	int Goldboard4::getISRtime()
	{
		return (isr_time*100)/256;	
	}
#endif

// Global interrupt
ISR(TIMER0_OVF_vect)
{
	#ifdef FAST_IRDETECTOR
		static uint8_t toggle = 0;
		if(toggle)
		{
			motorISR();
			toggle=0;
		}
		else
		{
			pulseISR();
			toggle=1;
		}
	#else
		wiringISR();
		motorISR();

	#endif
	#ifdef ISR_TIMER
		isr_time=TCNT0;
	#endif
}



/*


// digital sensors
#define DCOUNT 4
#define D0 4 PB0
#define D1 5 PB1
#define D2 6 PB2
#define D3 7 PB3
static const uint8_t DARRAY[] = {D0, D1, D2, D3};

// pwm sensors
#define PWMCOUNT 8
#define PWM0 28 PA3
#define PWM1 27 PA4
#define PWM2 26 PA5
#define PWM3 25 PA6
#define PWM4 17 PC1
#define PWM5 18 PC2
#define PWM6 19 PC3
#define PWM7 20 PC4
static const uint8_t PWMARRAY[] = {PWM0, PWM1, PWM2, PWM3, PWM4, PWM5, PWM6, PWM7};


*/

#ifdef FAST_IRDETECTOR
	#define PULSE_PRESCALER 3
	#define PULSE_BUFFER_SIZE 10

	static uint8_t prescaler_counter = 0;
	static uint8_t pwmBuffer[PULSE_BUFFER_SIZE];
	static uint8_t pwmBufferIndex = 0;


	void pulseISR() 
	{
		if(prescaler_counter>PULSE_PRESCALER) //10,416 kHz
		{
			pwmBuffer[pwmBufferIndex] = (( PINA >> 3) & 0x0f) | ((PINC << 3) & 0xf0);
			pwmBufferIndex++;
			if(pwmBufferIndex>PULSE_BUFFER_SIZE)
				pwmBufferIndex=0;
			prescaler_counter=0;
		}	
		prescaler_counter++;
	}
#endif


static uint8_t motorOffset = 0;
static uint16_t motorCounter = 255 + motorOffset;
static uint8_t motorSpeeds[4];

#ifdef POWER_PWM
	static uint8_t powerPortPower[2];
#endif


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
	

#ifdef POWER_PWM
	if (powerPortPower[0] == 0)
		cbi(PORTC,PC0);					//digitalWrite(PARRAY[0], LOW); 
	else if (((uint16_t) powerPortPower[0] + motorOffset)  >= motorCounter)
		sbi(PORTC,PC0);					//digitalWrite(PARRAY[0], HIGH);
	else
		cbi(PORTC,PC0);					//digitalWrite(PARRAY[0], LOW);


	if (powerPortPower[1] == 0)
		cbi(PORTD,PD7);					//digitalWrite(PARRAY[1], LOW); PD7
	else if (((uint16_t) powerPortPower[1] + motorOffset)  >= motorCounter)
		sbi(PORTD,PD7);					//digitalWrite(PARRAY[1], HIGH);
	else
		cbi(PORTD,PD7);					//digitalWrite(PARRAY[1], LOW); PD7
#endif

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
	{
		#ifdef LED_BUTTON
			pinMode(BTLEDARRAY[i],OUTPUT);
			_isLED[i] = true;
		#else
			return;
		#endif
	}

		
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

#ifdef POWER_PWM
	void Goldboard4::setPowerPWM(uint8_t i, uint8_t state)
	{
		if (i >= PCOUNT)
			return;
		
		powerPortPower[i]=state;
	}
#endif
/** Checks the state of button i. If it is pressed, true is returned,
*  else false.
*/
bool Goldboard4::getButton(uint8_t i)
{
	if (i >= BTLEDCOUNT)
		return false;
	
	if (_isLED[i])
	{
		
		#ifdef LED_BUTTON
			digitalWrite(BTLEDARRAY[i], HIGH);
			pinMode(BTLEDARRAY[i],INPUT);
			_isLED[i] = false;
		#else
			return false;
		#endif
	}	
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
#ifdef FAST_IRDETECTOR
	uint8_t Goldboard4::getPWMPulsedLight(uint8_t i)
	{
		if (i >= DCOUNT)
			return 0;
		uint8_t val = 0;
		for(uint8_t c = 0;c<PULSE_BUFFER_SIZE;c++)
		{
			if(pwmBuffer[c] & (1 << i))
				val++;
		}	

		return val;
	}

#else

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

#endif




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