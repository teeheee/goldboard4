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

#define UART_BAUD_RATE     115200UL //baud

#define I2C_SPEED   100   // kHz

#define ACCELERATION 10

//uncomment this line if you dont use a pulsed ball sensors
//this will make your code a little bit faster and shorter
#define PULSE_SENSOR_INPUT
#define MOTOR_ACCELERATION
#define SERVO_PWM

//goldboard intern i2c Adressen

#define PCF8574A_MOTOR_ADRESS 56 // 0x38
#define PCF8574A_DIGITAL_ADRESS 63 // 0x3F


/* LCD Config */

#define LCD_LINES           4     /* Anzahl der Zeilen*/
#define LCD_DISP_LENGTH    16     /* Anzahl der Zeichen pro Zeile*/

/*
 * Die verwendeten Pins müssen von einem PCF8574 Portexpander kommen.
 * Es kann der auf dem Goldboard verbaute, als auch ein externer verwendet werden.
 * Die definierte Zahl ist die Pinnummer des verwendeten Portexpanders.
 * */

#define LCD_DATA0_PIN    7            /*LCD-Pin DATA4 */
#define LCD_DATA1_PIN    6            /*LCD-Pin DATA5  */
#define LCD_DATA2_PIN    5            /*LCD-Pin DATA6  */
#define LCD_DATA3_PIN    4            /*LCD-Pin DATA7 */
#define LCD_RS_PIN       2            /*LCD-Pin RS       */
#define LCD_RW_PIN       1            /*LCD-Pin RW       */
#define LCD_E_PIN        0            /*LCD-Pin E     */

// Auskommentieren um Goldboardtest Funktionen zu aktivieren
//#define TEST



#endif /* CONFIG_H_ */
