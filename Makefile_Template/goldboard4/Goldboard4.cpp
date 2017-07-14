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

// function prototype for ISR
void motorISR();
void acceleration_control();
void pulseISR();
void servo_control(uint8_t zeit);

/* main interrupt routine */
ISR(TIMER0_OVF_vect) // 16Mhz/256 = 62,5 kHz
{
	static uint8_t toggle = 0;
	if(toggle == 0)
	{
		motorISR(); //20,833 kHZ (Hörschwelle = 20Khz um nerviges piepsen zu vermeiden) (ca 72 Takte)
		toggle++;
	}
	else if(toggle == 1)
	{
		#ifdef FAST_IRDETECTOR
		pulseISR(); //20,833 kHZ (ca 20 Takte)
		#endif
		toggle++;
	}
	else//20,833 kHZ
	{
		toggle=0;
		static uint16_t prescaler = 0;
		if(prescaler == 520) //50Hz ~ 20ms
		{
			#ifdef ACCELERATION
			acceleration_control(); // (ca 400 Takte ~ 1 Interrupt verpasst)
			#endif
			prescaler=0;
		}
		#ifdef SERVO_OUTPUT
		else if(prescaler == 1) //schalte alle Servopins 1 ms an
		servo_control(0);
		else if(prescaler >= 20 && prescaler <= 60) //die zweite ms wird geregelt
		servo_control(prescaler);
		#endif
		prescaler++;
	}
	wiringISR();  // 62,5 kHz (ca 16 Takte)
}


uint8_t servoValue[8];


/* generates simpel servo pwm with low resolution */
void servo_control(uint8_t zeit) //zeit ist zwischen 10 und 30
{
	if(zeit == servoValue[0]) //on
	cbi(PORTC,PC7);
	else if(zeit == 0 && servoValue[0])
	sbi(PORTC,PC7);
	
	if(zeit == servoValue[1]) //on
	cbi(PORTC,PC6);
	else if(zeit == 0 && servoValue[1])
	sbi(PORTC,PC6);
	
	if(zeit == servoValue[2]) //on
	cbi(PORTC,PC5);
	else if(zeit == 0 && servoValue[2])
	sbi(PORTC,PC5);
	
	if(zeit == servoValue[3]) //on
	cbi(PORTC,PC4);
	else if(zeit == 0 && servoValue[3])
	sbi(PORTC,PC4);
	
	if(zeit == servoValue[4]) //on
	cbi(PORTD,PD4);
	else if(zeit == 0 && servoValue[4])
	sbi(PORTD,PD4);
	
	if(zeit == servoValue[5]) //on
	cbi(PORTD,PD5);
	else if(zeit == 0 && servoValue[5])
	sbi(PORTD,PD5);
	
	if(zeit == servoValue[6]) //on
	cbi(PORTD,PD6);
	else if(zeit == 0 && servoValue[6])
	sbi(PORTD,PD6);
	
	if(zeit == servoValue[7]) //on
	cbi(PORTD,PD7);
	else if(zeit == 0 && servoValue[7])
	sbi(PORTD,PD7);
}




#define PULSE_PRESCALER 2
#define PULSE_BUFFER_SIZE 10

static uint8_t prescaler_counter = 0;
static uint8_t pwmBuffer[PULSE_BUFFER_SIZE];
static uint8_t pwmBufferIndex = 0;

/* sampels the ir detector data over 1 ms */
void pulseISR()
{
	if(prescaler_counter>PULSE_PRESCALER) //10 kHz
	{
		pwmBuffer[pwmBufferIndex] = (PIND & 0xf0) | (PINC >> 4); //PWM numbers 3,2,1,0,4,5,6,7
		pwmBufferIndex++;
		if(pwmBufferIndex>PULSE_BUFFER_SIZE)
		pwmBufferIndex=0;
		prescaler_counter=0;
	}
	prescaler_counter++;
}



static uint8_t motorOffset = 0;
static uint16_t motorCounter = 255 + motorOffset;
static uint8_t motorSpeeds[4];
static uint8_t powerPortPower[2];



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

	if (motorCounter == 0)
	motorCounter = 255 + motorOffset;
	else
	motorCounter--;
}


Motor* motors;


/* Beschleunigungs Interrupt. Ruft abwechselnd den accelerationHandler der einzelnen Motoren auf */
void acceleration_control()
{
	static volatile uint8_t toggle = 0;
	if(toggle == 0)
	{
		motors[0].accelerationHandler(); //alle 80ms, ca 400 Takte
		toggle++;
	}
	else if(toggle == 1)
	{
		motors[1].accelerationHandler(); //alle 80ms, ca 400 Takte
		toggle++;
	}
	else if(toggle == 2)
	{
		motors[2].accelerationHandler(); //alle 80ms, ca 400 Takte
		toggle++;
	}
	else
	{
		motors[3].accelerationHandler(); //alle 80ms, ca 400 Takte
		toggle = 0;
	}
}


/******************* Goldboard Member Functions **********************/



// default constructor
Goldboard4::Goldboard4()
{
	
	#ifdef DEBUG
	#warning Bluetooth version deaktiviert SERIAL_PRINTLN...
	//serial.begin(UART_BAUD_RATE); // initializes the uart interface
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
	
	// Portexpanders
	_pcf8574.init(PCF8574A_MOTOR_ADDRESS);
	digital.init(PCF8574A_DIGITAL_ADDRESS);
	
	// Motors
	count = 0;
	for (count=0; count<MCOUNT; count++)
	motor[count].init(MDARRAY[count][0], MDARRAY[count][1], MEARRAY[count], &_pcf8574, &motorSpeeds[count]);
	
	powerPortPower[0]=0;
	powerPortPower[1]=0;
	
	for(count=0; count<8; count++)
	servoValue[count]=0;
	
	motors = motor;
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
	
	powerPortPower[i]=state;
}

/** sets the PWM pin (0-7) as Servo Output and sends a certain value (0-255 ~ 0.8-2.8ms pulse) every 20 ms
*/
void Goldboard4::setPWMServo(uint8_t value, uint8_t pin)
{
	pinMode(PWMARRAY[pin],OUTPUT);
	value = value*(40.0d/255.0d); //Wertebereich anpassen ...
	if(value>40)
	value = 40;
	servoValue[pin]=value+20;
}

/**stops the PWM pulse. Dont use this, if you want to stop a brushless motor. Use setPWMServo(0,0) instead.
*/
void Goldboard4::resetPWMServo(uint8_t pin)
{
	pinMode(PWMARRAY[pin],INPUT_PULLUP);
	servoValue[pin]=0;
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
	pinMode(PWMARRAY[i],INPUT_PULLUP);
	if(i<4)		//PWM numbers 3,2,1,0,4,5,6,7 to PWM numbers 0,1,2,3,4,5,6,7
		i=3-i;  
	if (i >= DCOUNT)
	return 0;
	uint8_t val = 0;
	for(uint8_t c = 0; c < PULSE_BUFFER_SIZE; c++)
	{
		if(pwmBuffer[c] & (1 << i))
		val++;
	}
	return val;
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

void Goldboard4::scanI2C()
{
	serial.println("start scanning i2c:");
	for(uint8_t addr = 2; addr < 244; addr+=2)
	{
		 if(i2cCheckAck(addr))
		 {
			serial.print("Adresse 0x");
			serial.print(addr,HEX);
			serial.println(" aktiv");
		 }
	}
	serial.println("finished.");
}