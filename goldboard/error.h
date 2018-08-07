/*
 * error.h
 *
 *  Created on: 01.03.2018
 *      Author: Alexander Ulbrich
 */

#ifndef GOLDBOARD_ERROR_H_
#define GOLDBOARD_ERROR_H_


#include "uart.h"
#include "config.h"
#include <avr/pgmspace.h>



#ifdef ERROR_MESSAGE_ACTIVE
	const char progmem_string_error[] PROGMEM = "ERROR: ";
	const char progmem_string_lineend[] PROGMEM = "\r\n";

	const char progmem_no_init[] PROGMEM = "no init";
	const char progmem_no_ack[] PROGMEM = "no ack";
	const char progmem_timeout[] PROGMEM = "timeout";

	#define ERROR_MESSAGE(str) {uart_puts_p(progmem_string_error);uart_puts_P(str);uart_puts_p(progmem_string_lineend);}
	#define ERROR_MESSAGE_PREFIX(pref,str) {uart_puts_p(progmem_string_error);uart_puts_p(pref);uart_puts_p(str);uart_puts_p(progmem_string_lineend);}
	#define ERROR_MESSAGE_PREFIX_(pref,str) {uart_puts_p(progmem_string_error);uart_puts_p(pref);uart_puts_P(str);uart_puts_p(progmem_string_lineend);}

#else
	#define ERROR_MESSAGE(str)
#endif

#endif /* GOLDBOARD_ERROR_H_ */
