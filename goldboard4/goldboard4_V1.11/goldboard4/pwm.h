/*
 * pwm.h
 *
 * Created: 10.11.2017 23:45:08
 *  Author: alexander
 */ 

#ifndef __PWM_H__
#define __PWM_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"


//#define TEST

#define MOTOR_PWM_PORT PORTB //Pin NUmmer 0, 1, 2, 3
#define MOTOR_PWM_DDR  DDRB

#define MOTOR_PIN0 0
#define MOTOR_PIN1 1
#define MOTOR_PIN2 2
#define MOTOR_PIN3 3

#define POWER_PWM_PORT PORTD //Pin Nummer 1, 2
#define POWER_PWM_DDR  DDRD

#define POWER_PIN0 1
#define POWER_PIN1 2

#define PWM_PIN_COUNT 6

/*
 This offset is for preventing close PWM edges to overlap
 it should be as low as possible.
 If its to low, the PWM_TEST 5 will Fail 
*/
#define MINIMAL_OCR_OFFSET 10


void initPwm();
void setPwm(uint8_t id, uint8_t duty);
void updatePwm();

#ifdef TEST
void testPwm();
#endif


#endif