/**********************************************************************
@file       config.h

@brief      Contains all configuration data for the Goldboard 4.

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef F_CPU
#define F_CPU 16000000L
#endif

/* 9600 baud */
#define UART_BAUD_RATE      57600

#define I2C_SPEED   100   // kHz

#endif /* CONFIG_H_ */