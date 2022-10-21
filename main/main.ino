#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <vfs.h>
#include <SPI.h>

#include "Database.h" 
#include "Controlador.h"
#include "Vista.h"

LiquidCrystal_I2C lcd(0x3F,16,2);
Datausers dbUsers("/SD0/datausers.db");
Dataphone dbPhones("/SD0/phones.db");

unsigned long timestamp = 0;
bool activo = false;

Vista vista(&lcd);
Controlador controlador(NULL, &vista, &dbUsers, &dbPhones);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  vfs_mount("/SD0", 15);
  sqlite3_initialize();
  lcd.init();
  delay(1000);
  controlador.changeState(new StateA);
}

void loop() {
  if (activo && (millis()-timestamp > 15000)){
    activo = false;
    controlador.changeState(new StateA);
  }
  if (Serial.available()){
    timestamp = millis();
    activo = true;
    char in = Serial.read();
    if (in != '\r' && in != '\n'){
      controlador.newInput(in);
    }
  }

}
