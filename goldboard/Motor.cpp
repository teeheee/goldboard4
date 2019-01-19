

#include "Motor.h"
#include "pwm.h"
#include "Goldboard4.h"
#include "PCF8574A.h"

uint8_t acceleration = ACCELERATION;

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

void setAcceleration(uint8_t percent_per_ms)
{
	acceleration = percent_per_ms;
}

#ifdef MOTOR_ACCELERATION

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
			directionPortexpander->setPin(id*2, true);
			directionPortexpander->setPin(id*2 + 1, false);
			setPwm(id, uint8_t(-istNeu));
		}
		else if(istNeu > 0)
		{
			directionPortexpander->setPin(id*2, false);
			directionPortexpander->setPin(id*2 + 1, true);
			setPwm(id, uint8_t(istNeu));
		}
		else
		{
			directionPortexpander->setPin(id*2, false);
			directionPortexpander->setPin(id*2 + 1, false);
			setPwm(id, uint8_t(istNeu));
		}
		ist_motor_speed[id] = istNeu;
		pwm_change_flag = 1;
	}
}

void motor_isr() //100hz
{
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
}

#else

void setMotorSpeed(uint8_t id, int speed)
{
	if (speed < 0)
	{
		directionPortexpander->setPin(id*2, true);
		directionPortexpander->setPin(id*2 + 1, false);
		directionPortexpander->write();
		setPwm(id, uint8_t(-speed));
	}
	else if(speed > 0)
	{
		directionPortexpander->setPin(id*2, false);
		directionPortexpander->setPin(id*2 + 1, true);
		directionPortexpander->write();
		setPwm(id, uint8_t(speed));
	}
	else
	{

		directionPortexpander->setPin(id*2, false);
		directionPortexpander->setPin(id*2 + 1, false);
		directionPortexpander->write();
		setPwm(id, 0);
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
	_minspeed = MIN_SPEED;
	_maxspeed = MAX_SPEED;
	_speed_scale = (255.0-_minspeed)/_maxspeed;
}

void Motor::setMinSpeed(uint8_t minspeed)
{
	_minspeed = minspeed;
	_speed_scale = (255.0-_minspeed)/_maxspeed;
}

void Motor::setMaxSpeed(uint8_t maxspeed)
{
	_maxspeed = maxspeed;
	_speed_scale = (255.0-_minspeed)/_maxspeed;
}

void Motor::rotate(int16_t sp)
{
	if(sp > 0){
		sp = _minspeed + _speed_scale*sp;
		sp = constrain(sp,_minspeed,_maxspeed);
	}
	if(sp < 0){
		sp = -_minspeed + _speed_scale*sp;
		sp = constrain(sp,-_maxspeed,-_minspeed);
	}
	SERIAL_PRINT("sp: ");
	SERIAL_PRINTLN(sp);
	setMotorSpeed(_pwmPin, sp);
}

void Motor::setAcceleration(uint8_t percentPerHundretMs)
{
	setAcceleration(percentPerHundretMs);
}
