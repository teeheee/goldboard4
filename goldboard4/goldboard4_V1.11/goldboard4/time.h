/*
 * time.h
 *
 * Created: 16.11.2017 14:44:53
 *  Author: alexander
 */ 


#ifndef TIME_H_
#define TIME_H_

#ifndef F_CPU
#define F_CPU 16000000L
#endif


#include <util/delay.h>



#define delay(x) _delay_ms(x)

void init_timer();
uint32_t _millis();

#endif /* TIME_H_ */