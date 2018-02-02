/*!
 @file       

 @brief      System clock generation with parallel process handling.
 Triggers motor_isr and pulse_isr.

 -----------------------------------------------------------------------
 @author  2018 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.0  30.01.2018 First working version

@end
@ingroup gb4
@addtogroup gb4
*/


#ifndef TIME_H_
#define TIME_H_

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#include <avr/io.h>

void delay(uint32_t ms);
void init_timer();
uint32_t millis();
void delayMicroseconds(unsigned int);

#endif /* TIME_H_ */
