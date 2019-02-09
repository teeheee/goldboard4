#include "linie.h"


int sensor_vektor_x[16];
int sensor_vektor_y[16];

bool init_run = false;


void boden_sensor_vektor_init(){
  init_run = true;
	sensor_vektor_x[0] = sin(11.25*3.1415/180);
	sensor_vektor_y[0] = cos(11.25*3.1415/180);

	for(int sensor_index = 1; sensor_index < 16; sensor_index++)
	{
		sensor_vektor_x[sensor_index] = sin((11.25+22.5*sensor_index)*3.1415/180)*100;
		sensor_vektor_y[sensor_index] = cos((11.25+22.5*sensor_index)*3.1415/180)*100;
	}
}

double arctan_ext(double x, double y){
	return atan2(y, x) * 180.0 / 3.1415;
}

int linien_richtung(){
	double x = 0;
	double y = 0;
	for(int sensor_index = 0; sensor_index < 16; sensor_index++)
	{
		if(BodenWert & (1 << sensor_index))
		{
			x += sensor_vektor_x[sensor_index];
			y += sensor_vektor_y[sensor_index];
		}
	}
	// normieren und spiegeln
	double vektor_norm = sqrt(x*x + y*y);
	x = -x/vektor_norm;
	y = -y/vektor_norm;
	int angle = (int)arctan_ext(x,y);

	return (360 + 90 + angle + KompassWert - 180) % 360;
}

int cut_faktor(){
  int max_offset = 0;
  for(int sensor_index = 0; sensor_index < 16; sensor_index++)
	{
    for(int sensor_offset = 0; sensor_offset < 16; sensor_offset++)
    {
      int mit_offset = (sensor_index + sensor_offset) % 16;
  		if(   BodenWert & (1 << sensor_index)
         && BodenWert & (1 << mit_offset)
         && max_offset < sensor_offset)
      {
        max_offset = sensor_offset;
  		}
    }
	}
  return max_offset;
}

int count_sensors(){
  int count = 0;
  for(int sensor_index = 0; sensor_index < 16; sensor_index++)
	{
		if(BodenWert & (1 << sensor_index))
		{
			count++;
		}
	}
  return count;
}

int is_linie(){
  return BodenWert;
}

#define KEINE_LINIE -1
#define LINIE_V 0
#define ECKE_VR 1
#define LINIE_R 2
#define ECKE_HR 3
#define LINIE_H 4
#define ECKE_HL 5
#define LINIE_L 6
#define ECKE_VL 7

int get_line(){
  if(!is_linie()){
    return KEINE_LINIE;
  }
  else if(count_sensors() > 6){
    int richtung = linien_richtung();
    int ecke = floor((richtung + 45)/90.0)*2 + ECKE_VR;
    return ecke;
  }
  else {
    int richtung = linien_richtung();
    int linie = floor(richtung/90.0)*2 + LINIE_V;
    return linie;
  }
}

int is_ecke(int linie){
  if(linie == ECKE_VR || linie == ECKE_HR || linie == ECKE_HL || linie == ECKE_VL)
    return 1;
  else
    return 0;
}

void fahre_linie(int linie){
  fahren_winkel_relativ(45*linie, LINE_SPEED, RELATIV_ZUM_FELD);
}

int linien_aenderung(int linie1, int linie2){
  if(linie1 == KEINE_LINIE || linie2 == KEINE_LINIE)
    return 0;
  int a = abs(linie1-linie2);
  int b = abs(8-(linie1-linie2));
  return a > b ? b : a;
}

int line_program()
{
  #define LINE_STATE_INSIDE 1
  #define LINE_STATE_OUTSIDE 2
  #define LINE_STATE_ONLINE_INSIDE 3
  #define LINE_STATE_ONLINE_OUTSIDE 4
  #define LINE_STATE_ONECKE_INSIDE  5
  #define LINE_STATE_ONECKE_OUTSIDE 6

  static int state = LINE_STATE_INSIDE;
  static int line_last = KEINE_LINIE;

  if(!init_run)
    boden_sensor_vektor_init();

	int line_now = get_line();

  static long display_time = 0;
  if(millis()-display_time > 500)
  {
    display_time = millis();
    lcd.clear();
    lcd.print("state:");
    lcd.setCursor(0, 1);
    lcd.print("now:");
    lcd.setCursor(0, 2);
    lcd.print("last:");
    lcd.setCursor(0, 3);
    lcd.print("diff:");

    int offset = 7;
    lcd.setCursor(0+offset, 0);
    switch(state){
        case LINE_STATE_INSIDE: lcd.print("inside"); break;
        case LINE_STATE_OUTSIDE: lcd.print("outside"); break;
        case LINE_STATE_ONLINE_INSIDE: lcd.print("line_in"); break;
        case LINE_STATE_ONLINE_OUTSIDE: lcd.print("line_out"); break;
        case LINE_STATE_ONECKE_INSIDE: lcd.print("edge_in"); break;
        case LINE_STATE_ONECKE_OUTSIDE: lcd.print("edge_out"); break;
    }
    lcd.setCursor(0+offset, 1);
    lcd.print(line_now);
    lcd.setCursor(0+offset, 2);
    lcd.print(line_last);
    lcd.setCursor(0+offset, 3);
    lcd.print(linien_aenderung(line_last, line_now));
  }


  if( is_ecke(line_now) ) {                                                     // ecke
      line_last = line_now;
      state = LINE_STATE_ONECKE_INSIDE;
  }

	switch(state)
	{

		case LINE_STATE_INSIDE:
  			if( line_now == KEINE_LINIE ) {                                         // sieht die linie nicht
  				   return 1;
        }else {                                                                 // auf der linie
            line_last = line_now;
            state = LINE_STATE_ONLINE_INSIDE;
        }
  		  break;

    case LINE_STATE_OUTSIDE:
  			if( line_now == KEINE_LINIE ) {                                         // sieht die linie nicht
  				   break;
        }else {                                                                 // auf der linie
            state = LINE_STATE_ONLINE_INSIDE;
        }
  			break;

		case LINE_STATE_ONLINE_INSIDE:
  			if( line_now == KEINE_LINIE ) {                                         // sieht die linie nicht
            state = LINE_STATE_INSIDE;
        }else if( linien_aenderung(line_last, line_now) == 4) {                 // drueber
            state = LINE_STATE_ONLINE_OUTSIDE;
      	}
        else if( linien_aenderung(line_last, line_now) == 2) {                  // um die ecke
            line_last = line_now;
  			}
  			break;

    case LINE_STATE_ONLINE_OUTSIDE:
        if( line_now == KEINE_LINIE ){                                          // sieht die linie nicht
            state = LINE_STATE_OUTSIDE;
        }
        else if( linien_aenderung(line_last, line_now) == 0) {                  // drueber
            state = LINE_STATE_ONLINE_INSIDE;
      	}
        else if( linien_aenderung(line_last, line_now) == 2){                   // um die ecke
            line_last = (line_now + 4) % 8;
        }
        break;

		case LINE_STATE_ONECKE_INSIDE:
      if( line_now == KEINE_LINIE ) {                                           // sieht die linie nicht
          state = LINE_STATE_INSIDE;
      }
      else if( linien_aenderung(line_last, line_now) > 2 ){                     // drueber
          state = LINE_STATE_ONECKE_OUTSIDE;
			}
			break;

    case LINE_STATE_ONECKE_OUTSIDE:
      if( linien_aenderung(line_last, line_now) <= 2 ){                         // drueber zurueck
          state = LINE_STATE_ONECKE_INSIDE;
      }
  		break;
	}
  fahre_linie(line_last);
  return 0;
}
