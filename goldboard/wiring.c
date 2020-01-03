/**********************************************************************
@file       wiring.c

@brief      The "Timer" of the Goldboard4

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "wiring_private.h"

volatile unsigned long timCnt = 0;

void wiringISR()
{
   static uint8_t timMs = 0;

   if (++timMs >= TICKS_PER_MSEC)
   {
	   timCnt++;
	   timMs = 0;
   }
}

unsigned long millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timCnt;
	SREG = oldSREG;

	return m;
}

void delay(unsigned long ms)
{
	unsigned long startTime = millis();
	while((millis() - startTime) < ms);
}

/* Delay for the given number of microseconds.  Assumes a 1, 8, 12, 16, 20 or 24 MHz clock. */
void delayMicroseconds(unsigned int us)
{

	// for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/4 of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2; // x4 us, = 4 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 19 (21) cycles above, remove 5, (5*4=20)
	// us is at least 8 so we can substract 5
	us -= 5; // = 2 cycles,


	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
	// return = 4 cycles
}

void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
	
	// enable timers
	TCCR0 = (1 << CS00);  // clkI/O/(No prescaling) - divide by 256
	// Timer/Counter Interrupt Mask Register
	TIMSK |= (1 << TOIE0);
}
