/*!
 * \mainpage Goldboard4
 *
 * \author Christoph Rothermel und Alexander Ulbrich
 *
 * \section install_sec Installation in AS7
 *
 * \subsection step1 Schritt 1:
 *	Importiere das Projekt Template. File->Import->Project-Template
 * \subsection step2 Schritt 2:
 *	wähle die goldboard4-AS7-template.zip Datei aus und drücke ok.
 *
 * \section neues Projekt erstellen
 * \subsection step1 Schritt 1:
 * 	File->New->Project
 * \subsection step2 Schritt :
 * 	goldboard4 auswählen, Projektname und Speicherort festlegen. Mit OK bestätigen.
 *
 * @defgroup zf  Zusammenfassung
 * @defgroup external Externe Komponenten
 * @defgroup hal 	  Hardwareabstraktionsschicht
 * @defgroup gb4 	  Interne Komponenten
 * @defgroup sensors  Sensoren
 */

/*!
@file

@brief      Driver for Goldboard4 (w. Atmega32).

 -----------------------------------------------------------------------
 @author  2015 Christoph Rothermel
 Christoph.Rothermel@gmx.de

 @editor 2017 Alexander Ulbrich
 alexander.ulbrich@uni-ulm.de
 -----------------------------------------------------------------------
 @History

 1.00 28.01.2015 First working version
 1.1  6.7.2017   New features added
 2.0  30.01.2018 new back-end added


@end
@ingroup gb4
@addtogroup gb4
*/

#ifndef __GOLDBOARD4_H__
#define __GOLDBOARD4_H__

#include "config.h"
#include "time.h"
#include "Motor.h"
#include "PCF8574A.h"
#include "Arduino.h"
#include "SerialPrint.h"
#include "LiquidCrystal_I2C.h"
#include "SonarSRF08.h"
#include "CMPS03.h"
#include "CMPS11.h"
#include "PixyI2C.h"
#include "usring.h"
#include "SRF08.h"

/*!
@def SERIAL_PRINTLN(x)
@brief gibt die Daten x (int, unsigned int, float, "text") über usb aus und hängt einen Zeilenumbruch daran.
@ingroup zf
@addtogroup zf
*/
#define SERIAL_PRINTLN(x) serial.println(x)

/*!
@def SERIAL_PRINT(x)
@brief gibt die Daten x (int, unsigned int, float, "text") über usb aus.
@ingroup zf
@addtogroup zf
*/
#define SERIAL_PRINT(x) serial.print(x)



/*!
@class Goldboard4
@brief Von dieser Klasse darf nur ein Objekt (gb) erstellt werden. Es bietet sich an dies global zu definieren.
@ingroup zf
@addtogroup zf
*/
class Goldboard4
{
//variables
public:

/*! Motor attribut Array. siehe Motor für die Benutzung
*/
	Motor motor[4];

/*! PCF8574A attribute für Digital Pin 4 bis 11. Siehe PCF8574A für die Benutzung
*	Digital Pin 4 bis 11 können aber auch über getDigital und setDigital abgefragt werden.
*/
	PCF8574A digital;

//functions
public:
	Goldboard4();


	void init();
	/** schaltet alle Motoren auf aus.
	 */
	void setMotorsOff();

	/** Schaltet die Led mit der nummer i (0 bis 1) an wenn state = true und aus wenn state = false.
	 */
	void setLed(uint8_t i, bool state);

	/** Schaltet den Powerausgang i an wenn state = true und aus wenn state = false
	 */
	void setPower(uint8_t i, bool state);

	/** Schaltet den Powerausgang i (0 oder 1) in PWM modus. state bestimmt hier die "Anschaltzeit"
	 *  mit 255 durchgehend an, 125 halb an halb aus und 0 komplett aus. Die Frequenz ist 2kHz.
	 */
	void setPowerPWM(uint8_t i, int state);

	/** Gibt den Zustand des Buttons mit der nummer i (0 oder 1) zurück. true ist gedrückt und false ist nicht gedrückt.
	 * Achtung getButton()==1 ist nicht das selbe wie getButton()==true. Letzteres ist richtig.
	 */
	bool getButton(uint8_t i);

	/** Wartet auf das Drücken und loslassen des Buttons mit der Nummer i (0 oder 1)
	 */
	void waitForButton(uint8_t i);

	/** Gibt den Wert des Analog Pins i (0 bis 7) zurück. 0 <= Rückgabewert <= 255. 0  entspricht 0V und 255 entspricht VCC=5V
	 */
	uint8_t getAnalog(uint8_t i);

	/** Gibt true zurück falls der Digital Pin mit der nummer i (0 bis 21) 5V hat und False falls er 0V hat.
	 * 	Achtung getDigital()==1 ist nicht das selbe wie getDigital()==true. Letzteres ist richtig.
	 * 	Achtung Digital Pin 4 bis 11 sind langsamer da sie über i2c laufen.
	 */
	bool getDigital(uint8_t i);

	/** Setzt denn Digital Pin mit der nummer i (0 bis 21) auf 5V wenn state=true und 0V wenn state=false.
	 * 	Achtung Digital Pin 4 bis 11 sind langsamer da sie über i2c laufen.
	 */
	void setDigital(uint8_t i, bool state);

	/** Setzt den Digital pin (0 - 3) als Servo PWM modus (1-2ms pulse alle 20ms).
	* value (0-255) dreht den Servo auf einen bestimmten Winkel.
	* Pulselaenge kann in servopwm.h angepasst werden, falls der Servo sich nicht bis zum Anschlag bewegt
	 */
	void setPWMServo(uint8_t value,uint8_t pin);

	/* Gibt einen intensitäts wert für den an PWM pin i angeschlossenen Ball-Sensor zurueck. Return-wert ist im bereich 0-255.
	*/
	uint8_t getPWMPulsedLight(uint8_t i);

	/** Schreibt alle angeschlossenen I2C Sensoradressen auf den Serialport.
	 *  Adresse 56 und 63 sind schon für Portexpander belegt.
	 */
	void scanI2C();

	/** Test all functions of the Goldboard when nothing is connected. Connected Sensors can cause error reports.
	 */
	bool selftest();

	/** Wenn der i2c bus sich aufhängt kann man versuchen mit dieser Funktion ihn wieder zu reseten
	 *
	 */
	void i2cReset();

#ifdef TEST
	/**
	#@code
	 */
	void testPWM();
	void testTime();
	void testPowerpins();
	void testMotors();
	void testServos();
	void testDigital();
	void testAnalog();
	void testPulse();
	void speedTest();
	/**
	#@endcode
	*/
#endif

protected:
private:

	PCF8574A _pcf8574; // motor Portexpander

protected:
private:
	Goldboard4(const Goldboard4 &c);
	Goldboard4& operator=(const Goldboard4 &c);

};
//Goldboard4



#endif //__GOLDBOARD4_H__
