#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "Controlador.h"
#include "Vista.h"

LiquidCrystal_I2C lcd(0x3F,16,2);

unsigned long timestamp = 0;

Vista vista(&lcd);
Controlador controlador(NULL, &vista);


void setup() {
  Serial.begin(9600);
  lcd.init();
  delay(1000);
  controlador.changeState(new StateA);
}

void loop() {
  if (Serial.available()){
    timestamp = millis();
    char in = Serial.read();
    if (in != '\r' && in != '\n'){
      controlador.newInput(in);
    }
  }

}
