/**********************************************************************
@file       wiring_analog.c

@brief      Contains functions to read the analog inputs

-----------------------------------------------------------------------
@author  2015 Christoph Rothermel
         Christoph.Rothermel@gmx.de
-----------------------------------------------------------------------
@History

1.00 28.01.2015 First working version

@end
***********************************************************************/

#include "wiring_private.h"
#include "pin_configuration.h"

void init_adc()
{
	//interne Referenzspannung
	ADMUX = 0xC0;

	//AVCC
	//ADMUX = 0x40;

	//AREF
	//ADMUX = 0x00;


	ADCSRA = 0x80; //ADC aktivieren, Rest 0 setzen

	//ADCSRA |= 0x01; //Vorteiler 2
	//ADCSRA |= 0x02; //Vorteiler 4
	//ADCSRA |= 0x03; //Vorteiler 8
	//ADCSRA |= 0x04; //Vorteiler 16
	//ADCSRA |= 0x05; //Vorteiler 32
	//ADCSRA |= 0x06; //Vorteiler 64
	ADCSRA |= 0x07; //Vorteiler 128; da 16 Mhz Betrieb

	ADCSRA |= (1<<6); //Konvertierung starten

	while(ADCSRA & (1<<6)){}; //Kovertierung beendet

	uint16_t ergebnis = 0;

	ergebnis = ADCL;
	ergebnis += (ADCH<<8);
}

int analogRead(uint8_t pin)
{
	pin %= 8; // nur Werte von 0-7 erlaubt (vgl. PORTA)
	pin = 7 - pin; 
	ADMUX &= (0xE0); //Vorhandenen Wert fuer den Kanal loeschen

	//den Wert fuer den Kanal schreiben, dabei die vorderen 3 Bits vor evtl. fehlern schuetzen

	ADMUX |= (pin&(0x1F));

	ADCSRA |= (1<<6); //Konvertierung starten



	while(ADCSRA & (1<<6)){}; //Kovertierung beendet

	uint16_t ergebnis = 0;

	ergebnis = ADCL;
	ergebnis += (ADCH<<8);

	return ergebnis; //Ergebnis zurueckgeben

}

