#include "Goldboard4.h"
#include "linie.h"
#include "ball.h"
#include "actoren.h"
#include "sensors.h"
#include "menu.h"

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

void print_boden(){
	while (!BUTTON1) {
		updateSensorValue();
		lcd.clear();
		for (int i = 0; i < 16; i++) {
			lcd.setCursor((i * 4) % 16, i / 4);
			lcd.print(ring.getanalogValue(i));
		}
		delay(1000);
	}
}

void print_all(){
	while (!BUTTON1) {
		updateSensorValue();
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
	}
}

void test_ball_suchen(){
	while (!BUTTON1) {
		updateSensorValue();
		if(Pixy_Sieht_Ball)//sieht ball
			gb.setMotorsOff();
		else //sieht ball nicht
			searchBall();
	}
	gb.setMotorsOff();
}

void test_linie(){
	while (!BUTTON1) {
		updateSensorValue();
		if(line_program()) //linie
			gb.setMotorsOff();
	}
	gb.setMotorsOff();
}

void test_ball_annehmen(){
	while (!BUTTON1) {
		updateSensorValue();
		if(Pixy_Sieht_Ball)//sieht ball
			followBall();
		else //sieht ball nicht
			gb.setMotorsOff();
	}
	gb.setMotorsOff();
}

void run() {
	while (!BUTTON1) {
		updateSensorValue();
		if(line_program()) //linie
			ball_program();
	}
	gb.setMotorsOff();
}


void empty(){
		lcd.clear();
		lcd.print("Empty");
		delay(1000);
}

void start_menu() {
	Menu_Class sensoren_menu;
	sensoren_menu.add("Boden",print_boden);
	sensoren_menu.add("All",print_all);
	Menu_Class test_menu;
	test_menu.add("linie",test_linie);
	test_menu.add("ball suche",test_ball_suchen);
	test_menu.add("ball annahme",test_ball_annehmen);
	test_menu.add("tor finden",empty);
	Menu_Class main_menu;
	main_menu.add("Sensoren",&sensoren_menu);
	main_menu.add("Test",&test_menu);
	main_menu.add("Start",run);
	while(1)
		main_menu.run();
}

int main(void) {
	init();
	while (1) {
		start_menu();
	}
}
