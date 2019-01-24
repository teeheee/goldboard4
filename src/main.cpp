#include "Goldboard4.h"



Goldboard4 gb;
//SonarSRF08 US[4] = { SonarSRF08(112),SonarSRF08(114),SonarSRF08(122),SonarSRF08(126)};
SRF08 US[4];
CMPS03 kompass;
usring ring;
PixyI2C pixy;
LiquidCrystal_I2C lcd(63,16,4);

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

/**** PARAMETER ********/

#define SEARCH_SPEED 100
#define FOLLOW_SPEED 100

#define LINE_SPEED 100
#define LINE_SCHWELLE 240

#define FOLLOW_X_PFACTOR 1.0 // muss FOLLOW_KOMPASS_PFACTOR aufwiegen
#define FOLLOW_Y_PFACTOR 1.0 // darf nicht Schwingen
#define FOLLOW_KOMPASS_PFACTOR 1.0 // je gr??er desdo eher verliert er der Ball
#define FOLLOW_RADIUS 3.0 //je gr??er desdo kleiner der Radius


/***SENSOR WERTE****/
int UltraschallWert[4];
int IRWert[4];
uint16_t BodenWert;
int Pixy_BlockCount;
int Pixy_BallWert[2];
int Pixy_TorWert[2];
int KompassWert;

/* initialisiert alle Komponenten */
void init() {
	gb.scanI2C();
	lcd.begin(16, 4);
	lcd.blink_on();
	lcd.print("init Ultraschall");
	//for(int i = 0; i < 4; i++)
		//US[i].begin();
	US[0].init(0);
	US[1].init(2);
	US[2].init(10);
	US[3].init(14);
	lcd.clear();
	lcd.print("init kompass");
	kompass.init();
	kompass.setAs128Degree();
	lcd.clear();
	lcd.print("init boden");
	ring.init();
	ring.setThresholdValueGolbal(LINE_SCHWELLE);
	lcd.clear();
	lcd.print("init pixy");
	pixy.init();
	lcd.clear();
}

/* ueberprueft ob alle Kompontenten funktionieren */
void selfCheck(){
	gb.selftest();
}

/* Macht das Sensor-/Pinmapping*/
void updateSensorValue() {
#define US_DELAY 65
#define KOMPASS_DELAY 20
#define PIXY_DELAY 20
#define BODEN_DELAY 2
#define IRWert_DELAY 2
	static unsigned long US_time = 0;
	static int counter = 0;

	if (millis() - US_time > US_DELAY && US_ENABLE) {
		/*switch(counter)
		{
		case 0:
			UltraschallWert[0] = US[US_RECHTSVORNE].readRange();
			break;
		case 1:
			UltraschallWert[1] = US[US_RECHTSHINTEN].readRange();
			break;
		case 2:
			UltraschallWert[2] = US[US_LINKSHINTEN].readRange();
			break;
		case 3:
			UltraschallWert[3] = US[US_LINKSVORNE].readRange();
			break;
		}*/
		switch(counter)
		{
			case 0:
				UltraschallWert[0] = US[US_RECHTSVORNE].getValueCMBlocking();
				break;
			case 1:
				UltraschallWert[1] = US[US_RECHTSHINTEN].getValueCMBlocking();
				break;
			case 2:
				UltraschallWert[2] = US[US_LINKSHINTEN].getValueCMBlocking();
				break;
			case 3:
				UltraschallWert[3] = US[US_LINKSVORNE].getValueCMBlocking();
				break;
		}
		counter = (counter + 1) % 4;
		US_time = millis();
	}

	static unsigned long kompass_time = 0;
	if (millis() - kompass_time > KOMPASS_DELAY && KOMPASS_ENABLE) {
		KompassWert = kompass.getValue();
		kompass_time = millis();
	}

	static unsigned long pixy_time = 0;
	if (millis() - pixy_time > PIXY_DELAY && PIXY_ENABLE) {
		Pixy_BlockCount = pixy.getBlocks();
		if(Pixy_BlockCount > 0)
		{
			Pixy_BallWert[0] = pixy.blocks[0].x;
			Pixy_BallWert[1] = pixy.blocks[0].y;
		}
		pixy_time = millis();
	}

	static unsigned long boden_time = 0;
	if (millis() - boden_time > BODEN_DELAY && BODEN_ENABLE) {
		BodenWert = ring.getValue();
		boden_time = millis();
	}

	static unsigned long IRWert_time = 0;
	if (millis() - IRWert_time > IRWert_DELAY  && IR_ENABLE) {
		IRWert[0] = gb.getPWMPulsedLight(IRWert_VORNE);
		IRWert[1] = gb.getPWMPulsedLight(IRWert_RECHTS);
		IRWert[2] = gb.getPWMPulsedLight(IRWert_HINTEN);
		IRWert[3] = gb.getPWMPulsedLight(IRWert_LINKS);
		IRWert_time = millis();
	}
}

void run();
void testSensors();
void testFunctions();
void config();

void Menu() {
	int auswahl = 0;

	lcd.clear();
	gb.setMotorsOff();
	lcd.print("Start");
	lcd.setCursor(0, 1);
	lcd.print("Test Sensors");
	lcd.setCursor(0, 2);
	lcd.print("test");
	lcd.setCursor(0, 3);
	lcd.print("Test Functions");
	while (1) {
		lcd.setCursor(15, auswahl);
		if (BUTTON0) {
			auswahl++;
		} else if (BUTTON2) {
			auswahl+=3;
		} else if (BUTTON1)
			break;
		else
			continue;
		auswahl = auswahl%4;
		delay(200);
		while(BUTTON0 || BUTTON1 || BUTTON2);
		delay(200);
	}

	lcd.clear();
	switch (auswahl) {
	case 0:
		run();
		break;
	case 1:
		testSensors();
		break;
	case 2:
		config();
		break;
	case 3:
		testFunctions();
		break;
	}
	delay(500);
}

void testSensors() {
	int mode = 0;
	unsigned long time = millis();
	while (!BUTTON0) {
		if (BUTTON2) {
			mode++;
			delay(500);
			lcd.clear();
			lcd.print("next Mode");
		}
		switch (mode) {
		case 0:
			updateSensorValue();
			if (millis() - time > 1000) {
				lcd.clear();

				lcd.setCursor(0, 0);
				lcd.print(UltraschallWert[0]);
				lcd.setCursor(4, 0);
				lcd.print(UltraschallWert[1]);
				lcd.setCursor(8, 0);
				lcd.print(UltraschallWert[2]);
				lcd.setCursor(12, 0);
				lcd.print(UltraschallWert[3]);

				lcd.setCursor(0, 1);
				lcd.print(KompassWert);
				lcd.setCursor(4, 1);
				lcd.print(Pixy_BlockCount);
				if(Pixy_BlockCount > 0){
					lcd.setCursor(8, 1);
					lcd.print(Pixy_BallWert[0]);
					lcd.setCursor(12, 1);
					lcd.print(Pixy_BallWert[1]);
				}

				lcd.setCursor(0, 2);
				lcd.print(BodenWert,BIN);

				lcd.setCursor(0, 3);
				lcd.print(IRWert[0]);
				lcd.setCursor(4, 3);
				lcd.print(IRWert[1]);
				lcd.setCursor(8, 3);
				lcd.print(IRWert[2]);
				lcd.setCursor(12, 3);
				lcd.print(IRWert[3]);

				time = millis();
			}
			break;
		case 1:
			lcd.clear();
			for (int i = 0; i < 16; i++) {
				lcd.setCursor((i * 4) % 16, i / 4);
				lcd.print(ring.getanalogValue(i));
			}
			delay(1000);
			break;
		case 2:
			lcd.clear();
			lcd.print("Motor Test");
			gb.motor[MOTOR0].rotate(MOTOR0_DIR * 100);
			delay(500);
			gb.motor[MOTOR0].rotate(MOTOR0_DIR * -100);
			delay(500);
			gb.motor[MOTOR0].rotate(0);
			delay(1000);
			gb.motor[MOTOR1].rotate(MOTOR1_DIR * 100);
			delay(500);
			gb.motor[MOTOR1].rotate(MOTOR1_DIR * -100);
			delay(500);
			gb.motor[MOTOR1].rotate(0);
			delay(1000);
			gb.motor[MOTOR2].rotate(MOTOR2_DIR * 100);
			delay(500);
			gb.motor[MOTOR2].rotate(MOTOR2_DIR * -100);
			delay(500);
			gb.motor[MOTOR2].rotate(0);
			delay(1000);
			gb.motor[MOTOR3].rotate(MOTOR3_DIR * 100);
			delay(500);
			gb.motor[MOTOR3].rotate(MOTOR3_DIR * -100);
			delay(500);
			gb.motor[MOTOR3].rotate(0);
			delay(1000);
			gb.motor[MOTOR0].rotate(MOTOR0_DIR * 50);
			gb.motor[MOTOR1].rotate(MOTOR1_DIR * 50);
			gb.motor[MOTOR2].rotate(MOTOR2_DIR * 50);
			gb.motor[MOTOR3].rotate(MOTOR3_DIR * 50);
			delay(1000);
			gb.setMotorsOff();
			mode = 0;
			break;
		default:
			mode = 0;
			break;
		}
	}
}

void followBall(){
	float x = (Pixy_BallWert[0]-160) * FOLLOW_X_PFACTOR;
	float y = (Pixy_BallWert[1]-150) * FOLLOW_Y_PFACTOR;
	float e1 = (KompassWert-128) * FOLLOW_KOMPASS_PFACTOR;
	float e2 = e1*FOLLOW_RADIUS;

	float m[4];
	m[0] = -x-y-e1;
	m[1] = x-y+e2;
	m[2] = x+y+e2;
	m[3] = -x+y-e1;

	float max_speed = 0;
	for(int i = 0; i < 4; i++)
	{
		if(max_speed < m[i])
			max_speed = m[i];
	}

	float scaler = FOLLOW_SPEED/max_speed;
	for(int i = 0; i < 4; i++)
	{
		m[i] *= scaler;
	}

	gb.motor[MOTOR0].rotate(MOTOR0_DIR * m[0]);
	gb.motor[MOTOR1].rotate(MOTOR1_DIR * m[1]);
	gb.motor[MOTOR2].rotate(MOTOR2_DIR * m[2]);
	gb.motor[MOTOR3].rotate(MOTOR3_DIR * m[3]);
}

void searchBall(){
	if(Pixy_BallWert[0] < 160)
	{
		gb.motor[MOTOR0].rotate(MOTOR0_DIR * SEARCH_SPEED);
		gb.motor[MOTOR1].rotate(MOTOR1_DIR * SEARCH_SPEED);
		gb.motor[MOTOR2].rotate(MOTOR2_DIR * SEARCH_SPEED);
		gb.motor[MOTOR3].rotate(MOTOR3_DIR * SEARCH_SPEED);
	}
	else
	{
		gb.motor[MOTOR0].rotate(MOTOR0_DIR * -SEARCH_SPEED);
		gb.motor[MOTOR1].rotate(MOTOR1_DIR * -SEARCH_SPEED);
		gb.motor[MOTOR2].rotate(MOTOR2_DIR * -SEARCH_SPEED);
		gb.motor[MOTOR3].rotate(MOTOR3_DIR * -SEARCH_SPEED);
	}
}

int sensor_vektor_x[16];
int sensor_vektor_y[16];

/** sin und cos ist vertauscht da y vorne ist un 0 grad hat **/
void boden_sensor_vektor_init(){
	sensor_vektor_x[0] = sin(11.25*3.1415/180);
	sensor_vektor_y[0] = cos(11.25*3.1415/180);
	
	for(int sensor_index = 1; sensor_index < 16; sensor_index++)
	{
		sensor_vektor_x[sensor_index] = sin((11.25+22.5*sensor_index)*3.1415/180)*100;
		sensor_vektor_y[sensor_index] = cos((11.25+22.5*sensor_index)*3.1415/180)*100;
	}
}


/* TODO stimmt das wirklich??????*/
double arctan_ext(double x, double y){
	return atan2(y,x)*180/3.1415;
}

double fahr_richtung(){
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
	//angle = ((int)angle/90)*90;
	return (360+angle+90)%360;
}

void fahren_winkel(int angle)
{
	gb.motor[MOTOR0].rotate(MOTOR0_DIR * LINE_SPEED * cos((-angle + 315)*3.1415/180.0));
	gb.motor[MOTOR1].rotate(MOTOR1_DIR * LINE_SPEED * cos((-angle + 45)*3.1415/180.0));
	gb.motor[MOTOR2].rotate(MOTOR2_DIR * LINE_SPEED * cos((-angle + 135)*3.1415/180.0));
	gb.motor[MOTOR3].rotate(MOTOR3_DIR * LINE_SPEED * cos((-angle + 225)*3.1415/180.0));	
}

void fahren_vector(int x, int y)
{
	fahren_winkel(arctan_ext(x,y));
}

void line_program()
{
	#define LINE_STATE_INSIDE 1
	#define LINE_STATE_OUTSIDE 2
	#define LINE_STATE_ONLINE 3
	static int state = LINE_STATE_INSIDE;
	static int first_contact_angle = 0;
	static int richtungs_differenz = 0;
	int contact_angle = fahr_richtung();
	if( BodenWert != 0 )
		richtungs_differenz = abs(first_contact_angle-contact_angle);
	switch(state)
	{
		case LINE_STATE_INSIDE:
			if( BodenWert != 0 )
			{
				state = LINE_STATE_ONLINE;
				first_contact_angle = contact_angle;
				fahren_winkel(first_contact_angle+180);
			}
			else
			{
				gb.setMotorsOff();
			}
			break;
		case LINE_STATE_ONLINE:	
			if( BodenWert == 0 ) // linie = false
			{
				if( richtungs_differenz > 90 && richtungs_differenz < 270 ) // drueber = true
				{
					state = LINE_STATE_OUTSIDE;
				}
				else // drueber = false
				{
					state = LINE_STATE_INSIDE;
				}
			}
			fahren_winkel(first_contact_angle+180);
			break;
		case LINE_STATE_OUTSIDE:
			if( BodenWert != 0 ) // linie = true
			{
				state = LINE_STATE_ONLINE;
			}
			fahren_winkel(first_contact_angle+180);
			break;
	}
}

void run() {
	lcd.clear();
	lcd.print("running");
	while (!BUTTON0) {
		updateSensorValue();
		if(BodenWert > 0) //linie
		{
			line_program();
		}
		else if(Pixy_BlockCount>0)//sieht ball
		{
			followBall();
		}
		else //sieht ball nicht
		{
			searchBall();
		}
	}
	gb.setMotorsOff();
}

void config() {
	boden_sensor_vektor_init();
	unsigned long print_time = 0;
	unsigned long looptime = millis();
	unsigned int looptime_min = 10000;
	unsigned int looptime_max = 0;
	while (!BUTTON0) {
		updateSensorValue();
		line_program();
		unsigned int time = millis()-looptime;
		looptime = millis();
		if(looptime_min > time)
			looptime_min = time;
		if(looptime_max < time)
			looptime_max = time;
		if(millis()-print_time > 1000)
		{
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("test");
			lcd.setCursor(0,1);
			lcd.print(looptime_min);
			lcd.setCursor(8,1);
			lcd.print(looptime_max);
		}
	}
	gb.setMotorsOff();
}


void testFunctions(){
	lcd.print("testing Functions");
	int mode = 1;
	while (!BUTTON0) {
		lcd.clear();
		if (BUTTON2) {
			gb.setMotorsOff();
			mode++;
			lcd.print("next Mode");
			delay(1000);
			lcd.clear();
			lcd.print("starting in 2 sec");
			delay(2000);
		}
		switch (mode) {
		case 0: //ball search
			lcd.print("Suchen");
			updateSensorValue();
			if(Pixy_BlockCount==0)
				searchBall();
			else
				gb.setMotorsOff();
			break;
		case 1: // ball verfolgen
			lcd.print("Follow");
			updateSensorValue();
			if(Pixy_BlockCount>0)
				followBall();
			else
				gb.setMotorsOff();
			break;
		case 2:
			lcd.print("Linie");
			updateSensorValue();
			if(BodenWert > 0)
				line_program();
			else
				gb.setMotorsOff();
			break;
		default:
			mode = 0;
			break;
		}
	}
	gb.setMotorsOff();
}


int main(void) {
	init();
	while (1) {
		Menu();
	}
}

