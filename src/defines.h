#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "Goldboard4.h"

#define BUTTON0 !gb.getDigital(18)
#define BUTTON1 !gb.getDigital(19)
#define BUTTON2 !gb.getDigital(17)

#define MOTOR0 0
#define MOTOR0_DIR -1
#define MOTOR1 1
#define MOTOR1_DIR 1
#define MOTOR2 2
#define MOTOR2_DIR 1
#define MOTOR3 3
#define MOTOR3_DIR 1

#define US_RECHTSVORNE 3
#define US_RECHTSHINTEN 2
#define US_LINKSHINTEN 1
#define US_LINKSVORNE 0

#define IRWert_RECHTS 0
#define IRWert_HINTEN 3
#define IRWert_LINKS 2
#define IRWert_VORNE 1


#define US_ENABLE 0
#define PIXY_ENABLE 1
#define KOMPASS_ENABLE 1
#define BODEN_ENABLE 1
#define IR_ENABLE 0

#define PIXY_BALL_SIGNATUR 0

/**** PARAMETER ********/

#define SEARCH_SPEED 100
#define FOLLOW_SPEED 100

#define LINE_SPEED 100
#define LINE_SCHWELLE 90

#define FOLLOW_X_PFACTOR 1.0 // muss FOLLOW_KOMPASS_PFACTOR aufwiegen
#define FOLLOW_Y_PFACTOR 1.0 // darf nicht Schwingen
#define FOLLOW_KOMPASS_PFACTOR 1.0 // je gr??er desdo eher verliert er der Ball
#define FOLLOW_RADIUS 3.0 //je gr??er desdo kleiner der Radius

#endif
