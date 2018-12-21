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



#define UART_BAUD_RATE     115200UL //baud

#define I2C_SPEED   100   // kHz

#define ACCELERATION 10

#define BOOTLOADER

//uncomment this line if you dont use a pulsed ball sensors
//this will make your code a little bit faster and shorter
#define PULSE_SENSOR_INPUT
#define MOTOR_ACCELERATION
#define SERVO_PWM

//goldboard intern i2c Adressen

#define PCF8574A_MOTOR_ADRESS 56 // 0x38
#define PCF8574A_DIGITAL_ADRESS 63 // 0x3F


/* LCD Config */


/*
 * Die verwendeten Pins müssen von einem PCF8574 Portexpander kommen.
 * Es kann der auf dem Goldboard verbaute, als auch ein externer verwendet werden.
 * LCD_PIN_MAPPING definiert die Pinbelegung am Portexpander
 * 0 = RS
 * 1 = RW
 * 2 = EN
 * f = not connected
 * 4 = DATA4
 * 5 = DATA5
 * 6 = DATA6
 * 7 = DATA7
 * die Reihenfolge gibt an welcher Pin vom LCD am jeweiligen Pin des Portexpanders angeschlossen ist.
 * #define LCD_PIN_MAPPING 0x<D7><D6><D5><D4>f<EN><RW><RS>
 * D7 bekommt dann die pin nummer des Portexpanders
 * das f bedeutet, dass nichts angeschlossen ist
 * Bsp
 * EN -> PCF_0
 * RW -> PCF_1
 * RS -> PCF_2
 * DATA7 -> PCF_4
 * DATA6 -> PCF_5
 * DATA5 -> PCF_6
 * DATA4 -> PCF_7
 *
 * 			   
 * #define LCD_PIN_MAPPING 0x4567f012
 * */

#define LCD_PIN_MAPPING 0x4567f012

#define ERROR_MESSAGE_ACTIVE

// Auskommentieren um Goldboardtest Funktionen zu aktivieren
//#define TEST



#endif /* CONFIG_H_ */
