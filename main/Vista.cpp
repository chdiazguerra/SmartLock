#include "Vista.h"

Vista::Vista(LiquidCrystal_I2C *lcd_){
  lcd = lcd_;
}

void Vista::newInput(const char &in){
  lcd->setCursor(puntero, 1);
  lcd->print(in);
  puntero++;
}

void Vista::deleteChar(){
  if (puntero>0){
    puntero--;
    lcd->setCursor(puntero, 1);
    lcd->print(' ');
  } 
}

void Vista::changeHead(const String &in){
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(in);
  puntero = 0;
}

void Vista::off(){
  lcd->clear();
  lcd->noBacklight();
}

void Vista::on(){
  lcd->backlight();
}
