/*
 * time.cpp
 *
 * Created: 16.11.2017 14:44:41
 *  Author: alexander
 */ 

#include "time.h"
#include <avr/io.h>
#include "Motor.h"
#include "infrared_pulse.h"

volatile uint32_t time_in_ms;

ISR(TIMER0_COMP_vect) //1khz
{
	PORTC |= (1<<PC6);
	time_in_ms++;
	if(time_in_ms%10 == 0)
		motor_isr();
	//pulse_isr();
	PORTC &= ~(1<<PC6);
}

void init_timer()
{
	TCCR0 |= (1 << CS00) | (1 << CS01) | (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0 = 250;
	sei();
}

uint32_t _millis()
{
	return time_in_ms;
}
