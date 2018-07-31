#include "Goldboard4.h"
#include "lcd.h"
#include "SonarSRF08.h"
#include "CMPS03.h"
#include "PixyI2C.h"
#include "usring.h"

Goldboard4 gb;
SonarSRF08 US[4] = { SonarSRF08(112),SonarSRF08(114),SonarSRF08(122),SonarSRF08(126)};
CMPS03 kompass;
usring ring;
PixyI2C pixy;

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

/**** PARAMETER ********/

#define SEARCH_SPEED 50

#define LINE_SPEED 100
#define LINE_SCHWELLE 100

#define FOLLOW_X_PFACTOR 1.0 // muss FOLLOW_KOMPASS_PFACTOR aufwiegen
#define FOLLOW_Y_PFACTOR 1.0 // darf nicht Schwingen
#define FOLLOW_KOMPASS_PFACTOR 1.0 // je größer desdo eher verliert er der Ball
#define FOLLOW_RADIUS 3.0 //je größer desdo kleiner der Radius


/***SENSOR WERTE****/
int UltraschallWert[4];
int IRWert[4];
uint16_t BodenWert;
int Pixy_BlockCount;
int Pixy_BallWert[2];
int Pixy_TorWert[2];
int KompassWert;

void init() {
	lcd_init(LCD_DISP_ON_CURSOR_BLINK, &gb.digital);
	for(int i = 0; i < 4; i++)
		US[i].begin();
	kompass.init();
	ring.init();
	pixy.init();
	kompass.setAs128Degree();
	ring.setThresholdValueGolbal(LINE_SCHWELLE);
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

	if (millis() - US_time > US_DELAY) {
		switch(counter)
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
		}
		counter = (counter + 1) % 4;
		US_time = millis();
	}

	static unsigned long kompass_time = 0;
	if (millis() - kompass_time > KOMPASS_DELAY) {
		KompassWert = kompass.getValue();
		kompass_time = millis();
	}

	static unsigned long pixy_time = 0;
	if (millis() - pixy_time > PIXY_DELAY) {
		Pixy_BlockCount = pixy.getBlocks();
		if(Pixy_BlockCount > 0)
		{
			Pixy_BallWert[0] = pixy.blocks[0].x;
			Pixy_BallWert[1] = pixy.blocks[0].y;
		}
		pixy_time = millis();
	}

	static unsigned long boden_time = 0;
	if (millis() - boden_time > BODEN_DELAY) {
		BodenWert = ring.getValue();
		boden_time = millis();
	}

	static unsigned long IRWert_time = 0;
	if (millis() - IRWert_time > IRWert_DELAY) {
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
	lcd_clrscr();
	gb.setMotorsOff();
	lcd_puts("Start");
	lcd_gotoxy(0, 1);
	lcd_puts("Test Sensors");
	lcd_gotoxy(0, 2);
	lcd_puts("Config");
	lcd_gotoxy(0, 3);
	lcd_puts("Test Functions");
	while (1) {
		lcd_gotoxy(15, auswahl);
		delay(10);
		if (BUTTON0) {
			auswahl++;
		} else if (BUTTON2) {
			auswahl--;
		} else if (BUTTON1)
			break;
		else
			continue;
		auswahl = constrain(auswahl, 0, 4);
		delay(500);
	}

	lcd_clrscr();
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
			lcd_clrscr();
			lcd_puts("next Mode");
		}
		switch (mode) {
		case 0:
			updateSensorValue();
			if (millis() - time > 1000) {
				lcd_clrscr();
				char buffer[16];

				lcd_gotoxy(0, 0);
				lcd_put_int(UltraschallWert[0]);
				lcd_gotoxy(4, 0);
				lcd_put_int(UltraschallWert[1]);
				lcd_gotoxy(8, 0);
				lcd_put_int(UltraschallWert[2]);
				lcd_gotoxy(12, 0);
				lcd_put_int(UltraschallWert[3]);

				lcd_gotoxy(0, 1);
				lcd_put_int(KompassWert);
				lcd_gotoxy(4, 1);
				lcd_put_int(Pixy_BlockCount);
				if(Pixy_BlockCount > 0){
					lcd_gotoxy(8, 1);
					lcd_put_int(Pixy_BallWert[0]);
					lcd_gotoxy(12, 1);
					lcd_put_int(Pixy_BallWert[1]);
				}

				lcd_gotoxy(0, 2);
				lcd_put_binary(BodenWert,16);

				lcd_gotoxy(0, 3);
				lcd_put_int(IRWert[0]);
				lcd_gotoxy(4, 3);
				lcd_put_int(IRWert[1]);
				lcd_gotoxy(8, 3);
				lcd_put_int(IRWert[2]);
				lcd_gotoxy(12, 3);
				lcd_put_int(IRWert[3]);

				time = millis();
			}
			break;
		case 1:
			lcd_clrscr();
			char buffer[10];
			for (int i = 0; i < 16; i++) {
				lcd_gotoxy((i * 4) % 16, i / 4);
				lcd_put_int(ring.getanalogValue(i));
			}
			delay(1000);
			break;
		case 2:
			lcd_clrscr();
			lcd_puts("Motor Test");
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

	int m0 = -x-y-e1;
	int m1 = x-y+e2;
	int m2 = x+y+e2;
	int m3 = -x+y-e1;

	gb.motor[MOTOR0].rotate(MOTOR0_DIR * m0);
	gb.motor[MOTOR1].rotate(MOTOR1_DIR * m1);
	gb.motor[MOTOR2].rotate(MOTOR2_DIR * m2);
	gb.motor[MOTOR3].rotate(MOTOR3_DIR * m3);
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

int Abprallrichtung(){
	int currentBlockSize = 0;
	int lastBlockSize = 0;
	int ret = -1;
	for(int i = 0; i < 32; i++){
		int index = i%16;
		if(BodenWert & (1<<index)){
			if(currentBlockSize > lastBlockSize){
				ret = (i - 1  - currentBlockSize/2)%16;
				lastBlockSize = currentBlockSize;
			}
			currentBlockSize = 0;
		}
		else{
			currentBlockSize++;
		}
	}
	return ret;
}

void bounceLine(){
	static long lastContact = 0;
	static int abprallrichtung = 0;
	if(millis()-lastContact > 200)
	{
		abprallrichtung = Abprallrichtung();
	}
	lastContact = millis();
	float abprallwinkel = (2.0*3.1415/16.0)*abprallrichtung;
	gb.motor[MOTOR0].rotate(MOTOR0_DIR * LINE_SPEED * cos(abprallwinkel));
	gb.motor[MOTOR1].rotate(MOTOR1_DIR * LINE_SPEED * -cos(abprallwinkel));
	gb.motor[MOTOR2].rotate(MOTOR2_DIR * LINE_SPEED * -cos(abprallwinkel));
	gb.motor[MOTOR3].rotate(MOTOR3_DIR * LINE_SPEED * cos(abprallwinkel));
}

void run() {
	lcd_clrscr();
	lcd_puts("running");
	long loop_time = millis();
	int printcounter = 0;
	char buffer[10];
	while (!BUTTON0) {
		if(printcounter==0)
		{
			lcd_clrscr();
			lcd_puts("LoopTime: ");
			lcd_put_int((int)(millis()-loop_time));
		}
		loop_time = millis();
		if(printcounter==0)
			lcd_gotoxy(0, 1);

		updateSensorValue();
		if(BodenWert > 0) //linie
		{
			if(printcounter==0)
				lcd_puts("Linie");
			bounceLine();
		}
		else if(Pixy_BlockCount>0)//sieht ball
		{
			if(printcounter==0)
				lcd_puts("Ball");
			followBall();
		}
		else //sieht ball nicht
		{
			if(printcounter==0)
				lcd_puts("Suchen");
			searchBall();
		}
		printcounter = (printcounter +1)%500;
	}
	gb.setMotorsOff();
}

void config() {
	lcd_puts("configing");
	while (!BUTTON0) {

	}
}


void testFunctions(){
	lcd_puts("testing Functions");
	int mode = 1;
	while (!BUTTON0) {
		lcd_clrscr();
		if (BUTTON2) {
			gb.setMotorsOff();
			mode++;
			lcd_puts("next Mode");
			delay(1000);
			lcd_clrscr();
			lcd_puts("starting in 2 sec");
			delay(2000);
		}
		switch (mode) {
		case 0: //ball search
			lcd_puts("Suchen");
			updateSensorValue();
			if(Pixy_BlockCount==0)
				searchBall();
			else
				gb.setMotorsOff();
			break;
		case 1: // ball verfolgen
			lcd_puts("Follow");
			updateSensorValue();
			if(Pixy_BlockCount>0)
				followBall();
			else
				gb.setMotorsOff();
			break;
		case 2:
			lcd_puts("Linie");
			updateSensorValue();
			if(BodenWert > 0)
				bounceLine();
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
	gb.scanI2C();
	init();
	while (1) {
		Menu();
	}
}

