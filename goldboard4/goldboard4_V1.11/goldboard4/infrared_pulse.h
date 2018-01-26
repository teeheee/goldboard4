/*
 * infrared_pulse.h
 *
 * Created: 10.11.2017 23:49:43
 *  Author: alexander
 */ 


#include <avr/io.h>

#define PULSE_BUFFER_SIZE 20

void pulse_isr();

void pulse_init();

uint8_t get_pulse_width(uint8_t pinNumber);