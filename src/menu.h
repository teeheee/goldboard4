#ifndef _MENU_H_
#define _MENU_H_

#include "LiquidCrystal_I2C.h"
#include "defines.h"


#define ENTPRELL_TIMER 100

#define TASK 1
#define SUBMENU 2

class Menu_Class;

typedef struct menu_item{
  const char* text;
  uint8_t type;
  Menu_Class* sub_menu;
  void (*task)();
}menu_item_t;

class Menu_Class{
  public:
    Menu_Class();
    void run();
    void add(const char* text, void (*task)());
    void add(const char* text, Menu_Class *sub_menu);
  private:
    int menu_index;
    menu_item_t sub_menues[4];
};

#endif
