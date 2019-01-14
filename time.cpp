

#include "time.h"
#include "Motor.h"
#include "infrared_pulse.h"
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t time_in_ms;


ISR(TIMER0_COMP_vect) //1khz
{

	time_in_ms++;
#ifdef MOTOR_ACCELERATION
	static uint8_t cc = 0;
	if (cc == 10)
	{
		static uint8_t is_running = 0;
		if (is_running == 0)
		{
			is_running = 1;
			sei();
			motor_isr();
			cli();
			is_running = 0;
		}
		cc=0;
	}
	cc++;
#endif
#ifdef PULSE_SENSOR_INPUT
	pulse_isr();
#endif
}

void init_timer()
{
	TCCR0 |= (1 << CS00) | (1 << CS01) | (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0 = 250;
	time_in_ms = 0;
	sei();
}

uint32_t millis()
{
	return time_in_ms;
}

void delay(uint32_t ms)
{
	uint32_t now = millis();
	while (millis() - now < ms)
		;
}

void delayMicroseconds(unsigned int us)
{
        // calling avrlib's delay_us() function with low values (e.g. 1 or
        // 2 microseconds) gives delays longer than desired.
        //delay_us(us);
        // for the 16 MHz clock on most Arduino boards

        // for a one-microsecond delay, simply return.  the overhead
        // of the function call yields a delay of approximately 1 1/8 us.
        if (--us == 0)
        return;

        // the following loop takes a quarter of a microsecond (4 cycles)
        // per iteration, so execute it four times for each microsecond of
        // delay requested.
        us <<= 2;

        // account for the time taken in the preceeding commands.
        us -= 2;

        // busy wait
        __asm__ __volatile__ (
        "1: sbiw %0,1" "\n\t" // 2 cycles
        "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
        );	
}

