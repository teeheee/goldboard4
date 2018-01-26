/*
 * infrared_pulse.c
 *
 * Created: 10.11.2017 23:49:34
 *  Author: alexander
 */ 

#include "infrared_pulse.h"

volatile uint8_t pulseBuffer[PULSE_BUFFER_SIZE];

void pulse_isr()
{
	static uint8_t index = 0;
	uint8_t tmp = 0;
	tmp |= (PIND & 0b1111000)<<1; //PD6 ... PD3
	tmp |= (PINC & 0b1111000)>>3; //PC6 ... PC3
	pulseBuffer[index] = tmp;
	index++;
	if(index > PULSE_BUFFER_SIZE)
		index = 0;
}

void pulse_init();

uint8_t get_pulse_width(uint8_t pinNumber)
{
	uint8_t retCounter = 0;
	for(uint8_t i = 0; i < PULSE_BUFFER_SIZE; i++)
		if(pulseBuffer[i] & (1<<pinNumber))
			retCounter++;
	return retCounter;
}