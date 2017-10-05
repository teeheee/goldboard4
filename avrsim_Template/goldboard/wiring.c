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
