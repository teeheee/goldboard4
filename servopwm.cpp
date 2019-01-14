

#include "servopwm.h"
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "error.h"

#ifdef SERVO_PWM
void init_servo_timer();

volatile uint8_t* servo_port_reg;
volatile uint8_t* servo_ddr_reg;
volatile uint8_t  servo_pin;
volatile uint8_t servo_value;

ISR(TIMER1_COMPA_vect) // ca 50 Takte
{
	static uint8_t flag = 0;
	 //255 + x on Time , 4845 - x off Time
	 if(flag) //off
	 {
		OCR1A = 255+servo_value;
		*servo_port_reg &= ~(1 << servo_pin);
	 }
	 else //on
	 {
		 OCR1A = 4845-servo_value;
		 *servo_port_reg |= (1 << servo_pin);
	 }
		
	flag ^= 1; //toggle flag
}

void init_servo_timer()
{
	static uint8_t is_initialized = 0;
	if(is_initialized)
		return;

	TCCR1B |= (1 << CS11) | (1 << CS10) | (1 << WGM11); //prescaler 64
	TIMSK |= (1 << OCIE1A);

	OCR1A = 100;

	is_initialized = 1;
}
#endif

uint8_t init_servo(volatile uint8_t* port_reg,volatile uint8_t* ddr_reg,uint8_t pin)
{
#ifdef SERVO_PWM
	servo_port_reg = port_reg;
	servo_ddr_reg = ddr_reg;
	servo_pin = pin;
	servo_value = 125;
	*servo_port_reg &= ~(1 << servo_pin);
	*servo_ddr_reg |= (1<<servo_pin);
	init_servo_timer();
	return 1;
#else
	ERROR_MESSAGE("SERVO_PWM ist nicht aktiv!");
	return 0;
#endif
}

void set_servo(uint8_t handle, uint8_t value)
{
#ifdef SERVO_PWM
	servo_value = value;
#else
	ERROR_MESSAGE("SERVO_PWM ist nicht aktiv!");
#endif
}

