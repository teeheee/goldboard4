#include "menu.h"

extern LiquidCrystal_I2C lcd;
extern Goldboard4 gb;

Menu_Class::Menu_Class(){
  menu_index = 0;
}


void Menu_Class::run(){
  //zeige das Menü
  lcd.clear();
  for(int x = 0; x < menu_index; x++){
    lcd.setCursor(0, x);
    lcd.print(sub_menues[x].text);
  }
  //warte bis alle buttons losgelassen wurden
  while(BUTTON0 || BUTTON1 || BUTTON2);
  delay(ENTPRELL_TIMER);

  // prüfe buttons
  int auswahl = 0;
  while (1) {
    lcd.setCursor(15, auswahl);
    if (BUTTON0) {
      auswahl++;
    } else if (BUTTON2) {
      auswahl+=(menu_index-1);
    } else if (BUTTON1) {
      if(sub_menues[auswahl].type == TASK){
        	lcd.clear();
        	lcd.print(sub_menues[auswahl].text);
        	while (BUTTON1);
        	delay(500);
          sub_menues[auswahl].task();
      }else{
          sub_menues[auswahl].sub_menu->run();
      }
      break;
    }
    auswahl = auswahl%menu_index;
    //warte bis alle buttons losgelassen wurden
    while(BUTTON0 || BUTTON1 || BUTTON2);
    delay(ENTPRELL_TIMER);
    //warte bis ein button gedrückt wurden
    while(!BUTTON0 && !BUTTON1 && !BUTTON2);
    delay(ENTPRELL_TIMER);
  }
}

void Menu_Class::add(const char* text, void (*task)()){
  if(menu_index==4)
    return;
  sub_menues[menu_index].text = text;
  sub_menues[menu_index].task = task;
  sub_menues[menu_index].type = TASK;
  menu_index++;
}

void Menu_Class::add(const char* text, Menu_Class *sub_menu){
  if(menu_index==4)
    return;
  sub_menues[menu_index].text = text;
  sub_menues[menu_index].sub_menu = sub_menu;
  sub_menues[menu_index].type = SUBMENU;
  menu_index++;
}
