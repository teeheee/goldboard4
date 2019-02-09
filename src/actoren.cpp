
#include "actoren.h"

float m[4];

void set_motors(int speed){
    float max_speed = 0;
    for(int i = 0; i < 4; i++)
    {
        if(max_speed < m[i])
            max_speed = m[i];
    }
    if(speed < max_speed){
      	float scaler = (float)speed/max_speed;
      	for(int i = 0; i < 4; i++)
      	{
      		  m[i] *= scaler;
      	}
    }
    if(gb.getButton(0))
    {
    	gb.motor[MOTOR0].rotate(MOTOR0_DIR * m[0]);
    	gb.motor[MOTOR1].rotate(MOTOR1_DIR * m[1]);
    	gb.motor[MOTOR2].rotate(MOTOR2_DIR * m[2]);
    	gb.motor[MOTOR3].rotate(MOTOR3_DIR * m[3]);
    }
}

void drehen(int speed){
    for(int i = 0; i < 4; i++)
        m[i]=speed;
    set_motors(speed);
}

void set_vector(int x, int y){
    m[0] = -x-y;
    m[1] = x-y;
    m[2] = x+y;
    m[3] = -x+y;
}

void fahren_vector(int x, int y, int speed){
    set_vector(x,y);
    set_motors(speed);
}


void fahren_vector_rotation(int x, int y, int e1, int e2, int speed){
    set_vector(x,y);
    m[0] -= e1;
    m[1] += e2;
    m[2] += e2;
    m[3] -= e1;
    set_motors(speed);
}

void fahren_winkel(int winkel, int speed){
    int x = cos(winkel*180.0/3.1415)*speed;
    int y = sin(winkel*180.0/3.1415)*speed;
    set_vector(x,y);
    set_motors(speed);
}

void fahren_vector_relativ(int x, int y, int speed, int typ){
    int winkel = atan2(x,y)*180.0/3.1415;
    if(typ == RELATIV_ZUM_FELD)
      winkel = (winkel + KompassWert)%360;
    fahren_winkel(winkel, speed);
}

void fahren_winkel_relativ(int winkel, int speed, int typ){
    if(typ == RELATIV_ZUM_FELD)
      winkel = (winkel + KompassWert)%360;
    fahren_winkel(winkel, speed);
}

void fahren_stop(){
  gb.setMotorsOff();
}
