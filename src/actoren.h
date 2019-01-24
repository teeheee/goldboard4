#ifndef _AKTOREN_H_
#define _AKTOREN_H_

#include "defines.h"
#include "Goldboard4.h"

#define RELATIV_ZUM_FELD 1
#define RELATIV_ZUM_ROBOTER 2

extern Goldboard4 gb;
extern int KompassWert;

void drehen(int speed);
void fahren_vector(int x, int y, int speed);
void fahren_winkel(int winkel, int speed);
void fahren_vector_relativ(int x, int y, int speed, int typ);
void fahren_winkel_relativ(int winkel, int speed, int typ);
void fahren_vector_rotation(int x, int y, int e1, int e2, int speed);
void fahren_stop();


#endif
