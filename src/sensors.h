#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "defines.h"
#include "Goldboard4.h"

/***** Hardware Komponenten ***********/
extern Goldboard4 gb;
//SonarSRF08 US[4] = { SonarSRF08(112),SonarSRF08(114),SonarSRF08(122),SonarSRF08(126)};
extern SRF08 US[4];
extern CMPS03 kompass;
extern usring ring;
extern PixyI2C pixy;


/***** Sensor Werte ***********/
extern int UltraschallWert[4];
extern int IRWert[4];
extern uint16_t BodenWert;
extern int Pixy_BlockCount;
extern int Pixy_Sieht_Ball;
extern int Pixy_BallWert[2];
extern int Pixy_TorWert[2];
extern int KompassWert;

void updateSensorValue();

#endif
