#ifndef _BALL_H_
#define _BALL_H_

#include "defines.h"
#include "Goldboard4.h"
#include "actoren.h"

extern Goldboard4 gb;

extern int Pixy_Sieht_Ball;
extern int Pixy_BallWert[2];
extern int KompassWert;

void followBall();
void searchBall();
void ball_program();

#endif
