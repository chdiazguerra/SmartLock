#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "SD.h"
#include <FS.h>

#include "Database.h" 
#include "Controlador.h"
#include "Vista.h"

LiquidCrystal_I2C lcd(0x3F,16,2);

Datausers dbUsers("/sd/datausers.db");
Dataphone dbPhones("/sd/phones.db");

Vista vista(&lcd);
Controlador controlador(NULL, &vista, &dbUsers, &dbPhones);

unsigned long timestamp = 0;
bool activo = false;
char phoneCall[20] = {'\0'};

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);  //SIM
  SPI.begin();
  SD.begin();
  sqlite3_initialize();
  lcd.init();
  delay(1000);
  controlador.changeState(new StateA);

  //Handshake SIM
  Serial2.println("AT");
  delay(500);
  Serial2.println("AT+CLIP=1");
  delay(500);
  while (Serial2.available()){
    Serial2.readString();
    delay(200);
  }
}

void loop() {
  //Manejo de llamadas
  if (Serial2.available()){
    String message = Serial2.readStringUntil('\r');
    message.trim();
    bool correctLine = message.startsWith("+CLIP");
    if (correctLine){
      Serial2.println("ATH");
      delay(200);
      Serial.println("CORRECTA");
      Serial.println(message);
      extractPhone(message);
      Serial.println(phoneCall);
      int res = dbPhones.verify(phoneCall);
      if (res == ALLOWED){
        controlador.relayOn();
      } else if (res == DENIED){
        Serial.println("NUMERO DENEGADO");
      } else {
        Serial.println("PROBLEMAS DATABASE");
      }
    } else {
      Serial.println(message);
    }
  }

  
  //Manejo de LCD
  if (activo && (millis()-timestamp > 15000)){
    activo = false;
    controlador.off();
  }
  if (Serial.available()){
    timestamp = millis();
    activo = true;
    char in = Serial.read();
    if (in != '\r' && in != '\n'){
      controlador.newInput(in);
    }
  }

  delay(200);

}

void extractPhone(const String &line){
  char puntero = 0;
  char letra;
  bool inside = false;
  
  for(int i=0; ;i++){
    letra = line.charAt(i);
    if(!inside && isdigit(letra)){
      phoneCall[puntero] = letra;
      puntero++;
      inside = true;
    } else if (inside && isdigit(letra)){
      phoneCall[puntero] = letra;
      puntero++;
    } else if (inside && !isdigit(letra)){
      phoneCall[puntero] = '\0';
      return;
    }
  }
}
