/*!
@file


 @brief      This is a faster and better implementation of the pulsed ball sensor driver.

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

#include <avr/io.h>
#include "pin_configuration.h"

#define PULSE_BUFFER_SIZE 255

#define ALL_PULSE1_PINS ((1<<PULSE1_PIN3) | (1<<PULSE1_PIN2) | (1<<PULSE1_PIN1) | (1<<PULSE1_PIN0))
#define ALL_PULSE2_PINS ((1<<PULSE2_PIN3) | (1<<PULSE2_PIN2) | (1<<PULSE2_PIN1) | (1<<PULSE2_PIN0))

void pulse_isr();

void pulse_init();

uint8_t get_pulse_width(uint8_t pinNumber);
