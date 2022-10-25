#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <vfs.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#include "Database.h" 
#include "Controlador.h"
#include "Vista.h"

#define RXSIM 0 //D3
#define TXSIM 16 //D0

SoftwareSerial sim(RXSIM, TXSIM);

LiquidCrystal_I2C lcd(0x3F,16,2);

Datausers dbUsers("/SD0/datausers.db");
Dataphone dbPhones("/SD0/phones.db");

Vista vista(&lcd);
Controlador controlador(NULL, &vista, &dbUsers, &dbPhones);

unsigned long timestamp = 0;
bool activo = false;
char phoneCall[20] = {'\0'};

void setup() {
  Serial.begin(9600);
  sim.begin(4800);
  SPI.begin();
  vfs_mount("/SD0", 15);
  sqlite3_initialize();
  lcd.init();
  delay(1000);
  controlador.changeState(new StateA);

  //Handshake SIM
  sim.println("AT");
  delay(500);
  sim.println("AT+CLIP=1");
  delay(500);
  while (sim.available()){
    sim.readString();
    delay(200);
  }
}

void loop() {
  //Manejo de llamadas
  if (sim.available()){
    String message = sim.readStringUntil('\r');
    message.trim();
    bool correctLine = message.startsWith("+CLIP");
    if (correctLine){
      sim.println("ATH");
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
