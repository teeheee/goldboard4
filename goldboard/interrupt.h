/*!
@file


 @brief      This is an implementation of a software interrupt

 -----------------------------------------------------------------------
 @author  2019 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.0  6.05.2019 First test

@end
@ingroup hal
@addtogroup hal
*/

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <avr/io.h>
#include "pin_configuration.h"


#define ALL_PULSE1_PINS ((1<<PULSE1_PIN3) | (1<<PULSE1_PIN2) | (1<<PULSE1_PIN1) | (1<<PULSE1_PIN0))
#define ALL_PULSE2_PINS ((1<<PULSE2_PIN3) | (1<<PULSE2_PIN2) | (1<<PULSE2_PIN1) | (1<<PULSE2_PIN0))

void interrupt_isr();
void interrupt_init();
void interrupt_attach(uint8_t pin, void (*callback)(), uint8_t mode);
void interrupt_detach(uint8_t pin);

#endif
