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
#warning F_CPU not defined!
#define F_CPU 16000000UL
#endif

#include <avr/io.h>

/*!
@fn delay(uint32_t ms)
@brief Wartet um Millisekunden.
@ingroup zf
@addtogroup zf
*/
void delay(uint32_t ms);

/*!
@fn millis()
@brief Gibt die aktuelle Zeit in Millisekunden seit der Roboter Strom hat zurück.
@ingroup zf
@addtogroup zf
*/
uint32_t millis();

/*!
@fn delayMicroseconds(unsigned int)
@brief Wartet um Microsekunden. Achtung: maximal 65535 us ~ 65 ms. Sehr ungenau bei großen Zeiten.
@ingroup zf
@addtogroup zf
*/
void delayMicroseconds(unsigned int);


void init_timer();


#endif /* TIME_H_ */
