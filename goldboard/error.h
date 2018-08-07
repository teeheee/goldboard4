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

#ifdef ERROR_MESSAGE_ACTIVE
	#define ERROR_MESSAGE(str) {uart_puts_P("ERROR: ");uart_puts_P(str);uart_puts_P("\r\n");}
#else
	#define ERROR_MESSAGE(str)
#endif

#endif /* GOLDBOARD_ERROR_H_ */
