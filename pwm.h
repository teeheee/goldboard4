/*!
@file

 @brief      Intelligent PWM generation with adaptive interrupt timing.
 TODO still some minor glitches in the pulse generation

 -----------------------------------------------------------------------
 @author  2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.0  30.01.2018 First working version

@end
@ingroup hal
@addtogroup hal
*/

#ifndef __PWM_H__
#define __PWM_H__

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"
#include "pin_configuration.h"

#define PWM_PIN_COUNT 6

#define ALL_MOTOR_PINS ((1 << MOTOR_PIN0) | (1 << MOTOR_PIN1) | (1 << MOTOR_PIN2) | (1 << MOTOR_PIN3))
#define ALL_POWER_PINS ((1 << POWER_PIN0) | (1 << POWER_PIN1))

#define setBits(maske)	{MOTOR_PWM_PORT |= (maske & ALL_MOTOR_PINS); POWER_PWM_PORT |= ((maske >> 2) & ALL_POWER_PINS);}
#define clearBits(maske)	{MOTOR_PWM_PORT &= ~(maske & ALL_MOTOR_PINS); POWER_PWM_PORT &= ~((maske >> 2) & ALL_POWER_PINS);}

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
