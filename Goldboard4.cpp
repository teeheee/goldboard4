//Hardware interfaces

#include "time.h"
#include "config.h"
#include "infrared_pulse.h"
#include "Wire.h"
#include "pin_configuration.h"
#include "pwm.h"
#include "uart.h"
#include "adc.h"
#include "servopwm.h"
#include "Motor.h"
#include "error.h"

#include "Goldboard4.h"


/******************* Goldboard Member Functions **********************/
// default constructor
Goldboard4::Goldboard4() {
	init();
} //Goldboard4

void Goldboard4::init(){
	sei();
	// uart
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));

	// Buttons (DEFAULT)
	BTLED_DDR |= (1 << BTLED_PIN0) | (1 << BTLED_PIN1);
	BTLED_PORT |= ((1 << BTLED_PIN0) | (1 << BTLED_PIN1));

	// Analog
	ADC_DDR &= ~((1 << ADC_PIN0) | (1 << ADC_PIN1) | (1 << ADC_PIN2)
			| (1 << ADC_PIN3));
	ADC_DDR &= ~((1 << ADC_PIN4) | (1 << ADC_PIN5) | (1 << ADC_PIN6)
			| (1 << ADC_PIN7));

	// Digital
	DIGITAL_DDR &= ~((1 << DIGITAL_PIN0) | (1 << DIGITAL_PIN1)
			| (1 << DIGITAL_PIN2) | (1 << DIGITAL_PIN3));


	uint8_t error = Wire.selfTest();
	if(error > 0)
	{
		Wire.resuscitateBus();
		error = Wire.selfTest();
		if(error > 1)
		{
			ERROR_MESSAGE("I2C cap");
		}
		else if(error == 1)
		{
			ERROR_MESSAGE("I2C short");
		}
	}

	// i2c
	Wire.begin();

	// pwm generation
	initPwm();

#ifdef PULSE_SENSOR_INPUT
	// pulsed light
	pulse_init();
#endif

	// Portexpanders
	digital.init(PCF8574A_DIGITAL_ADRESS);
	_pcf8574.init(PCF8574A_MOTOR_ADRESS);

	// Motors
	for (uint8_t count = 0; count < 4; count++)
		motor[count].init(count, &_pcf8574);

	init_timer();

	selftest();
}

/** Puts off all motors.
 */
void Goldboard4::setMotorsOff() {
	for (uint8_t i = 0; i < MOTORCOUNT; i++)
		motor[i].rotate(0);
}

/** Puts LED i on if state is true, else off
 */
void Goldboard4::setLed(uint8_t i, bool state) {
	if (i >= BTLEDCOUNT)
		return;
	if (!state)
		BTLED_PORT |= (1 << (BTLED_PIN0 + i));
	else
		BTLED_PORT &= ~(1 << (BTLED_PIN0 + i));

}

/** Puts the power output i on if state is true, else off
 */
void Goldboard4::setPower(uint8_t i, bool state) {
	if (i >= POWERCOUNT)
		return;
	if (state)
		setPwm(i + MOTORCOUNT, 0xff);
	else
		setPwm(i + MOTORCOUNT, 0);
	updatePwm();
}

void Goldboard4::setPowerPWM(uint8_t i, int state) {
	if (i >= POWERCOUNT)
		return;
	if(state > 0xff)
		state = 0xff;
	setPwm(i + MOTORCOUNT, state);
	updatePwm();
}

/** sets the Digital pin (0-4) as Servo Output and sends a certain value (0-255 ~ 1 - 2ms pulse) every 20 ms
 */
void Goldboard4::setPWMServo(uint8_t value, uint8_t pin)
{
	static uint8_t servo_handle = 0; //for better implementation of multiple servos
	if(pin >= 4)
		return;
	if(!servo_handle)
		servo_handle = init_servo(&DIGITAL_PORT,&DIGITAL_DDR,pin+DIGITAL_PIN0);
	set_servo(servo_handle,value);
}


/** Checks the state of button i. If it is pressed, true is returned,
 *  else false.
 */
bool Goldboard4::getButton(uint8_t i) {
	if (i >= BTLEDCOUNT)
		return false;
	uint8_t lastState = BTLED_PORT;
	BTLED_DDR &= ~(1 << (BTLED_PIN0 + i));
	BTLED_PORT |= (1 << (BTLED_PIN0 + i));
	bool ret = BTLED_PIN & (1 << (BTLED_PIN0 + i));
	BTLED_PORT = lastState;
	BTLED_DDR |= (1 << (BTLED_PIN0 + i));
	return ret?false:true;
}

/** Waits until button i is pressed and released again.
 */
void Goldboard4::waitForButton(uint8_t i) {
	if (i >= BTLEDCOUNT)
		return;

	while (!getButton(i))
		;
	while (getButton(i))
		;
}

/** returns the value of the analog port i. 0 <= value <= 1024
 */
uint8_t Goldboard4::getAnalog(uint8_t i) {
	if (i >= ADCCOUNT)
		return 0;

	//slow
	return adc_read(ADC_PRESCALER_4,ADC_VREF_AVCC,i)/4;


	//fast TODO
}

/** returns true if the digital port is logical high, else false.
 */
bool Goldboard4::getDigital(uint8_t i) {
	if (i < DIGITALCOUNT) {
		DIGITAL_DDR &= ~(1 << (DIGITAL_PIN0 + i));
		DIGITAL_PORT |= (1 << (DIGITAL_PIN0 + i));
		bool ret = DIGITAL_PIN & (1 << (DIGITAL_PIN0 + i));
		DIGITAL_PORT &= ~(1 << (DIGITAL_PIN0 + i));
		return ret;
	} else if (i < DIGITALCOUNT + 8) {
		i -= DIGITALCOUNT;
		return digital.read() & (1 << i);
	} else if (i < DIGITALCOUNT + 8 + PULSECOUNT) {
		i -= (DIGITALCOUNT + 8);
		if (i < 4) {
			i = 3 - i; // invert
			PULSE2_DDR &= ~(1 << (PULSE2_PIN0 + i));
			PULSE2_PORT |= (1 << (PULSE2_PIN0 + i));
			bool ret = PULSE2_PIN & (1 << (PULSE2_PIN0 + i));
			PULSE2_PORT &= ~(1 << (PULSE2_PIN0 + i));
			return ret;
		} else {
			i = i - 4;
			PULSE1_DDR &= ~(1 << (PULSE1_PIN0 + i));
			PULSE1_PORT |= (1 << (PULSE1_PIN0 + i));
			bool ret = PULSE1_PIN & (1 << (PULSE1_PIN0 + i));
			PULSE1_PORT &= ~(1 << (PULSE1_PIN0 + i));
			return ret;
		}
	} else if (i < DIGITALCOUNT + 8 + PULSECOUNT + ADCCOUNT) {
		i -= DIGITALCOUNT + 8 + PULSECOUNT;
		ADC_DDR &= ~(1 << (ADC_PIN0 + i));
		ADC_PORT |= (1 << (ADC_PIN0 + i));
		bool ret = ADC_PIN & (1 << (ADC_PIN0 + i));
		ADC_PORT &= ~(1 << (ADC_PIN0 + i));
		return ret;
	} else
		return false;
}

void Goldboard4::setDigital(uint8_t i, bool state) {
	if (i < DIGITALCOUNT) {
		bitWrite(DIGITAL_DDR, DIGITAL_PIN0+i, true);
		bitWrite(DIGITAL_PORT, DIGITAL_PIN0+i, state);
	} else if (i < DIGITALCOUNT + 8) {
		i -= DIGITALCOUNT;
		digital.setPin(i, state);
		digital.write();

	} else if (i < DIGITALCOUNT + 8 + PULSECOUNT) {
		i -= DIGITALCOUNT + 8;
		if (i < 4) {
			i = 3 - i; // invert
			bitWrite(PULSE2_DDR, PULSE2_PIN0+i, true);
			bitWrite(PULSE2_PORT, PULSE2_PIN0+i, state);
		} else {
			i = i - 4;
			bitWrite(PULSE1_DDR, PULSE1_PIN0+i, true);
			bitWrite(PULSE1_PORT, PULSE1_PIN0+i, state);
		}
	} else if (i < DIGITALCOUNT + 8 + PULSECOUNT + ADCCOUNT) {
		i -= DIGITALCOUNT + 8 + PULSECOUNT;
		bitWrite(ADC_DDR, ADC_PIN0+i, true);
		bitWrite(ADC_PORT, ADC_PIN0+i, state);
	}
}

/** returns the registered pulse length of the digital port i. 0 <= value <= 255
 */
uint8_t Goldboard4::getPWMPulsedLight(uint8_t i) {
#ifdef PULSE_SENSOR_INPUT
	if (i >= PULSECOUNT)
		return 0;
	return get_pulse_width(i);
#else
	ERROR_MESSAGE("PULSE_SENSOR_INPUT wurde nicht aktiviert!");
	return 0;
#endif
}


/** prints all i2c adresses on the Serial Interface
 */
void Goldboard4::scanI2C()
{
	uart_puts_P("start i2c scan\r\n");
	for(uint8_t address = 1; address < 127; address++ )
	{
		Wire.beginTransmission(address);
		uint8_t error = Wire.endTransmission();

		if (error == 0)
		 {
			uart_puts_P("   ");
			 SERIAL_PRINT((int)address);
			 if(address == 0x68>>1)
				 uart_puts_P(" Usring\r\n");
			 else if(address == 0xC0>>1)
				 uart_puts_P(" Kompass\r\n");
			 else if(address == 0x54)
				 uart_puts_P(" Pixy\r\n");
			 else if(address >= 0xE0>>1 && address <= 0xFE>>1)
				 uart_puts_P(" Ultraschall\r\n");
			 else if(address == 0x29)
				 uart_puts_P(" Laser\r\n");
			 else if(address >= 0x38 && address <= 0x63)
				 uart_puts_P(" Portexpander\r\n");
			 else
				 uart_puts_P(" unknown\r\n");

		 }
	}
	uart_puts_P("end i2c scan\r\n");
}

/* performs self test */
bool Goldboard4::selftest(){
	Wire.beginTransmission(PCF8574A_MOTOR_ADRESS);
	uint8_t error = Wire.endTransmission();
	if(error > 0)
	{
		ERROR_MESSAGE("Goldboard: i2c portexpander error");
		return false;
	}

	Wire.beginTransmission(PCF8574A_DIGITAL_ADRESS);
	error = Wire.endTransmission();
	if(error > 0)
	{
		ERROR_MESSAGE("Goldboard: i2c portexpander error");
		return false;
	}
	return true;
}

void Goldboard4::i2cReset(){
	uint8_t error = Wire.selfTest();
	if(error > 0)
	{
		Wire.resuscitateBus();
		error = Wire.selfTest();
		if(error > 1)
		{
			ERROR_MESSAGE("I2C Cap");
		}
		else if(error == 1)
		{
			ERROR_MESSAGE("I2C short");
		}
	}
}

#ifdef TEST

void Goldboard4::testPWM()
{
	SERIAL_PRINTLN("start PWM test");
	testPwm();
	SERIAL_PRINTLN("stop PWM test");
	delay(1000);
}

void Goldboard4::testTime()
{
	SERIAL_PRINTLN("start Time test");
	delay(1000);
	SERIAL_PRINTLN("1s");
	delay(1000);
	SERIAL_PRINTLN("2s");
	delay(1000);
	SERIAL_PRINTLN("3s");
	SERIAL_PRINTLN("stop Time test");
}

void Goldboard4::testPowerpins()
{
	SERIAL_PRINTLN("start Power pin test");
	SERIAL_PRINTLN("Fading Pin 0");
	for(int i = 0; i < 1000; i+=10)
	{
		setPowerPWM(0,i);
		delay(3);
	}
	setPowerPWM(0,0);
	SERIAL_PRINTLN(("Fading Pin 1"));
	for(int i = 0; i < 1000; i+=10)
	{
		setPowerPWM(1,i);
		delay(3);
	}
	setPowerPWM(1,0);
	SERIAL_PRINTLN(("PIN0 toggle"));
	for(int i = 0; i < 5; i++)
	{
		setPower(0,true);
		delay(20);
		setPower(0,false);
		delay(1000);
	}
	SERIAL_PRINTLN(("PIN1 toggle"));
	for(int i = 0; i < 5; i++)
	{
		setPower(1,true);
		delay(20);
		setPower(1,false);
		delay(1000);
	}
	SERIAL_PRINTLN(("Wrong PIN test"));
	for(int i = 2; i < 100; i+=10)
	{
		setPower(i,true);
		delay(100);
	}
	SERIAL_PRINTLN(("finished Power pin test"));
}


void Goldboard4::testMotors()
{
	SERIAL_PRINTLN("start Motor test");
	for(int i = 0; i < 4; i++)
	{
		SERIAL_PRINT("Rotate Motor");
		SERIAL_PRINT(i);
		SERIAL_PRINTLN("");
		for(int speed = 0; speed < 200; speed++)
		{
			motor[i].rotate(speed-100);
			delay(20);
		}
		motor[i].rotate(0);
	}
	setMotorsOff();
	delay(10);
	SERIAL_PRINTLN("Rotate all Motors");
	for(int i = 0; i < 10; i++)
	{
		for(int speed = 0; speed < 200; speed++)
		{
			motor[0].rotate(speed-100);
			motor[1].rotate((speed+30)%200-100);
			motor[2].rotate((speed+60)%200-100);
			motor[3].rotate((speed+90)%200-100);
			delay(5);
		}
	}
	setMotorsOff();
	delay(10);
	SERIAL_PRINTLN("ende Motor test");

}

void Goldboard4::testDigital()
{
	SERIAL_PRINTLN("start Digital Test");
	for(int i = 0; i < DIGITALCOUNT + 8 + PULSECOUNT + ADCCOUNT; i++)
	{
		while(getDigital(i))
			setDigital(i,false);
		SERIAL_PRINT("D");
		while(!getDigital(i))
			setDigital(i,true);
		SERIAL_PRINT("I");
		while(getDigital(i))
			setDigital(i,false);
		SERIAL_PRINT("G");
		while(!getDigital(i))
			setDigital(i,true);
		SERIAL_PRINT("ITAL");
		SERIAL_PRINTLN(i);
	}
	SERIAL_PRINTLN("ende Digital test");
}

void Goldboard4::testAnalog()
{
#define STEPS 4
#define RANGE 255

	SERIAL_PRINTLN("start Analog Test");
	for(int i = 0; i < ADCCOUNT; i++)
	{
		uint8_t buffer[STEPS] = {0};
		while(1)
		{
			uint8_t ret = 1;
			for(int k = 0; k < STEPS; k++)
				if(buffer[k]==0)
					ret = 0;
			if(ret)
				break;
			int val = getAnalog(i);
			for(int k = 0; k < STEPS; k++)
				if(val > RANGE*k/STEPS && val < RANGE*(k+1)/STEPS)
				{
					if(buffer[k]==0)
						SERIAL_PRINT(".");
					buffer[k]=1;
				}
		}
		SERIAL_PRINT("ANALOG");
		SERIAL_PRINTLN(i);
	}
	SERIAL_PRINTLN("ende Analog test");
}

void Goldboard4::testPulse()
{
	SERIAL_PRINTLN("start Pulse Test");
	for(int i = 0; i < PULSECOUNT; i++)
	{
		while(!getPWMPulsedLight(i));
		SERIAL_PRINT("PULSE");
		SERIAL_PRINTLN(i);
	}
	SERIAL_PRINTLN("ende Pulse test");
}


void Goldboard4::speedTest()
{
	SERIAL_PRINTLN("start Speed Test");
	//stress the cpu...
	motor[0].rotate(10);
	motor[1].rotate(20);
	motor[2].rotate(30);
	motor[3].rotate(40);
	setPowerPWM(0,50);
	setPowerPWM(1,60);

	SERIAL_PRINT("FLOPS: ");
	volatile float x = 2.32567;
	volatile float y = 2.12342;
	uint32_t time = millis();
	for(int i = 0; i < 5000; i++)
	{
		x = x*y;
		y = x/y;
	}
	SERIAL_PRINT((float)(millis()-time)/10000);
	SERIAL_PRINTLN(" ms");

	SERIAL_PRINT("motor[x].rotate: ");
	time = millis();
	for(int i = 0; i < 10000; i++)
	{
		motor[0].rotate(((i%100)*22)%100);
	}
	SERIAL_PRINT((float)(millis()-time)/2200);
	SERIAL_PRINTLN(" ms");

	SERIAL_PRINT("digitalWrite: ");
	time = millis();
	uint8_t state = 0;
	for(int i = 0; i < 100; i++)
	{
		for(int i = 0; i < DIGITALCOUNT + 8 + PULSECOUNT + ADCCOUNT; i++)
		{
			setDigital(i,state);
		}
		state ^= 1;
	}
	SERIAL_PRINT((float)(millis()-time)/2200);
	SERIAL_PRINTLN(" ms");

	SERIAL_PRINT("digitalRead: ");
	time = millis();
	for(int i = 0; i < 100; i++)
	{
		for(int i = 0; i < DIGITALCOUNT + 8 + PULSECOUNT + ADCCOUNT; i++)
		{
			getDigital(i);
		}
	}
	SERIAL_PRINT((float)(millis()-time)/2200);
	SERIAL_PRINTLN(" ms");

	SERIAL_PRINT("millis: ");
	time = millis();
	for(int i = 0; i < 10000; i++)
	{
		millis();
	}
	SERIAL_PRINT((float)(millis()-time)/10000);
	SERIAL_PRINTLN(" ms");


	SERIAL_PRINT("getAnalog: ");
	time = millis();
	for(int i = 0; i < 10000; i++)
	{
		getAnalog(i%8);
	}
	SERIAL_PRINT((float)(millis()-time)/10000);
	SERIAL_PRINTLN(" ms");

	SERIAL_PRINT("getDigitalPulsedLight: ");
	time = millis();
	for(int i = 0; i < 10000; i++)
	{
		getPWMPulsedLight(i%8);
	}
	SERIAL_PRINT((float)(millis()-time)/10000);
	SERIAL_PRINTLN(" ms");
	motor[0].rotate(0);
	motor[1].rotate(0);
	motor[2].rotate(0);
	motor[3].rotate(0);
	setPowerPWM(0,0);
	setPowerPWM(1,0);

	SERIAL_PRINTLN("ende Speed test");
}

void Goldboard4::testServos()
{
	SERIAL_PRINTLN("start test Servo on Digital pin 0");
	for(int i = 0; i < 255; i+=25)
	{
		setPWMServo(i,0);
		delay(500);
	}
	SERIAL_PRINTLN("ende test Servo");
}


#endif
