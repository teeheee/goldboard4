/**********************************************************************
@file       config.h

@brief      Contains all configuration data for the Goldboard 4.

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
		 
@editor  2017 Alexander Ulbrich
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
#define UART_BAUD_RATE      57600UL  

#define I2C_SPEED   100   // kHz

#define FAST_IRDETECTOR
//#define ACCELERATION
#define SERVO_OUTPUT

#define PCF8574A_MOTOR_ADDRESS 0x70
#define PCF8574A_DIGITAL_ADDRESS 0x7E


#endif /* CONFIG_H_ */