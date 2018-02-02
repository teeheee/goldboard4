

#include "Motor.h"
#include "pwm.h"
#include "Goldboard4.h"
#include "PCF8574A.h"

uint8_t acceleration = 50;

PCF8574A* directionPortexpander;

int ist_motor_speed[4] =
{ 0 };
volatile int soll_motor_speed[4] =
{ 0 };

uint8_t pwm_change_flag = 0;

void initMotors(PCF8574A* portexpander);
void setMotorSpeed(uint8_t id, int speed);
void setAcceleration(uint8_t percent_per_ms);


void initMotors(PCF8574A* portexpander)
{
	static uint8_t initOnlyOnce = 1;
	if (initOnlyOnce)
	{
		directionPortexpander = portexpander;
		initPwm();
		initOnlyOnce = 0;
	}
}

#ifdef MOTOR_ACCELERATION

void setAcceleration(uint8_t percent_per_ms)
{
	acceleration = percent_per_ms;
}

void setMotorSpeed(uint8_t id, int speed)
{
	soll_motor_speed[id] = speed;
}

inline void accelerationHandle(uint8_t id)
{
	int ist = ist_motor_speed[id];
	int soll = soll_motor_speed[id];
	if (ist != soll)
	{
		int diff = soll - ist;
		int istNeu = ist;
		if (diff > acceleration)
			istNeu += acceleration;
		else if (diff < -acceleration)
			istNeu -= acceleration;
		else
			istNeu = soll;
		if (istNeu < 0)
		{
			directionPortexpander->setPin(id, true);
			directionPortexpander->setPin(id + 1, false);
			setPwm(id, uint8_t(-istNeu));
		}
		else
		{
			directionPortexpander->setPin(id, false);
			directionPortexpander->setPin(id + 1, true);
			setPwm(id, uint8_t(istNeu));
		}
		ist_motor_speed[id] = istNeu;
		pwm_change_flag = 1;
	}
}

void motor_isr() //100hz
{
	sei();
	accelerationHandle(0);
	accelerationHandle(1);
	accelerationHandle(2);
	accelerationHandle(3);
	if (pwm_change_flag)
	{
		updatePwm();
		directionPortexpander->write();
		pwm_change_flag = 0;
	}
	cli();
}

#else

void setMotorSpeed(uint8_t id, int speed)
{
	if (speed < 0)
	{
		directionPortexpander->setPin(id, true);
		directionPortexpander->setPin(id + 1, false);
		setPwm(id, uint8_t(-speed));
	}
	else
	{
		directionPortexpander->setPin(id, false);
		directionPortexpander->setPin(id + 1, true);
		setPwm(id, uint8_t(speed));
	}
	updatePwm();
}

#endif

Motor::Motor()
{
	_pwmPin = 0;
	_directionPortexpander = 0;
}

void Motor::init(uint8_t pwmPin, PCF8574A* pcf8574)
{
	initMotors(pcf8574);
	_directionPortexpander = pcf8574;
	_pwmPin = pwmPin;
}

void Motor::rotate(int16_t sp)
{
	sp = constrain(sp,-100,100);
	setMotorSpeed(_pwmPin, (sp * 255) / 100);
}

void Motor::setAcceleration(uint8_t percentPerHundretMs)
{
	setAcceleration(percentPerHundretMs);
}

