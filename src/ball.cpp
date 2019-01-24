#include "ball.h"


void ball_program(){
  if(Pixy_Sieht_Ball)//sieht ball
  {
    followBall();
  }
  else //sieht ball nicht
  {
    searchBall();
  }
}


void followBall(){
	int x = (Pixy_BallWert[0]-160) * FOLLOW_X_PFACTOR;
	int y = (Pixy_BallWert[1]-150) * FOLLOW_Y_PFACTOR;
	int e1 = (KompassWert-128) * FOLLOW_KOMPASS_PFACTOR;
	int e2 = e1*FOLLOW_RADIUS;
  fahren_vector_rotation(x, y, e1, e2, FOLLOW_SPEED);
}

void searchBall(){
	if(Pixy_BallWert[0] < 160){
    drehen(SEARCH_SPEED);
	}
	else{
    drehen(-SEARCH_SPEED);
	}
}
