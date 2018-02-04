/**********************************************************************
 @file       config.h

 @brief      Contains all configuration data for the Goldboard 4 library.
 This file needs to be changed for better performance.

 -----------------------------------------------------------------------
 @author  2015 Christoph Rothermel
 Christoph.Rothermel@gmx.de

 @editor  2018 Alexander Ulbrich
 Christoph.Rothermel@gmx.de
 -----------------------------------------------------------------------
 @History

 1.00 28.01.2015 First working version
 1.1  30.01.2018 New Stuff

 @end
 ***********************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#define UART_BAUD_RATE     9600UL //baud

#define I2C_SPEED   100   // kHz

#define ACCELERATION 10

//uncomment this line if you dont use a pulsed ball sensors
//this will make your code a little bit faster and shorter
#define PULSE_SENSOR_INPUT
#define MOTOR_ACCELERATION

//goldboard intern i2c Adressen

#define PCF8574A_MOTOR_ADRESS 56 // 0x38
#define PCF8574A_DIGITAL_ADRESS 63 // 0x3F

#define TEST

#endif /* CONFIG_H_ */
