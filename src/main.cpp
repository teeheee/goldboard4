#include "Goldboard4.h"
#include "linie.h"
#include "ball.h"
#include "actoren.h"
#include "sensors.h"

/***** Hardware Komponenten ***********/
Goldboard4 gb;
//SonarSRF08 US[4] = { SonarSRF08(112),SonarSRF08(114),SonarSRF08(122),SonarSRF08(126)};
SRF08 US[4];
CMPS03 kompass;
usring ring;
PixyI2C pixy;
LiquidCrystal_I2C lcd(63,16,4);

/***** Sensor Werte ***********/
int UltraschallWert[4];
int IRWert[4];
uint16_t BodenWert;
int Pixy_BlockCount;
int Pixy_Sieht_Ball;
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

void run() {
	lcd.clear();
	lcd.print("running");
	while (!BUTTON0) {
		updateSensorValue();
		if(BodenWert > 0) //linie
			line_program();
		else
			ball_program();
	}
	gb.setMotorsOff();
}

void config() {
	boden_sensor_vektor_init();
	while (!BUTTON0) {
		updateSensorValue();
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("typ: ");
		lcd.setCursor(12, 0);
		lcd.print(get_line());
		lcd.setCursor(0, 1);
		lcd.print("richtung: ");
		lcd.setCursor(12, 0);
		lcd.print(linien_richtung());
		delay(500);
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
			if(Pixy_BlockCount == 0)
				searchBall();
			else
				gb.setMotorsOff();
			break;
		case 1: // ball verfolgen
			lcd.print("Follow");
			updateSensorValue();
			if(Pixy_BlockCount > 0)
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
