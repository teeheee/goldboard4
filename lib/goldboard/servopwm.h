/*!
@file

@brief      intelligent Servo PWM generation with adaptive interrupt timing
 	 	 	 Uses Timer 1 as interrupt. Just for 1 servo

-----------------------------------------------------------------------
@author  2018 Alexander Ulbrich
alexander.ulbrich@uni-ulm.de
-----------------------------------------------------------------------
@History

@end
@ingroup hal
@addtogroup hal
*/


#define SERVO_PERIOD_TIME_US 20000
#define SERVO_PULSE_MIN_TIME_US 950
#define SERVO_PULSE_MAX_TIME_US 2050

#define MAX_SERVO_COUNT 1 //if you need more ask U

#include <avr/io.h>

uint8_t init_servo(volatile uint8_t* port_reg,volatile uint8_t* ddr_reg,uint8_t pin);
void set_servo(uint8_t handle, uint8_t value);
