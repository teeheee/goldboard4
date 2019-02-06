#ifndef _LINIE_H_
#define _LINIE_H_

#include "defines.h"
#include "Goldboard4.h"
#include "actoren.h"

extern Goldboard4 gb;
extern LiquidCrystal_I2C lcd;

extern uint16_t BodenWert;
extern int KompassWert;

int get_line();
void boden_sensor_vektor_init();
int linien_richtung();

void line_program();

#endif
