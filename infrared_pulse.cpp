

#include "infrared_pulse.h"
#include "config.h"
#include <avr/interrupt.h>

#ifdef PULSE_SENSOR_INPUT

volatile uint8_t pulseBuffer[PULSE_BUFFER_SIZE];

void pulse_isr()
{
	sei();
	static uint8_t index = 0;
	uint8_t tmp = 0;
	tmp |= (PULSE1_PIN & ALL_PULSE1_PINS); //PD7 ... PD4
	tmp |= (PULSE2_PIN & ALL_PULSE2_PINS) >> 4; //PC7 ... PC4
	pulseBuffer[index] = tmp;
	index++;
	if (index > PULSE_BUFFER_SIZE)
		index = 0;
	cli();
}

void pulse_init()
{
	PULSE1_DDR &= ~(ALL_PULSE1_PINS); //as input
	PULSE2_DDR &= ~(ALL_PULSE2_PINS); //as input
	PULSE1_PORT |= (ALL_PULSE1_PINS); //pull up on
	PULSE2_PORT |= (ALL_PULSE2_PINS); //pull up on
	for (uint8_t i = 0; i < PULSE_BUFFER_SIZE; i++)
		pulseBuffer[i] = 0;
}


uint8_t get_pulse_width(uint8_t pinNumber)
{
	if(pinNumber < 4)
		pinNumber = 3-pinNumber;

	uint8_t retCounter = 0;
	for (uint8_t i = 0; i < PULSE_BUFFER_SIZE; i++)
		if (!(pulseBuffer[i] & (1 << pinNumber)))
			retCounter++;
	return retCounter;
}


#endif
