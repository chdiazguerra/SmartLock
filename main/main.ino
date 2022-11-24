#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "SD.h"
#include <FS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <queue>

//Propios
#include "Database.h" 
#include "Controlador.h"
#include "Vista.h"


// Wifi network station credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "TOKEN"

TaskHandle_t Task1;
TaskHandle_t Task2;

std::queue<String> admin_messages;

LiquidCrystal_I2C lcd(0x3F,16,2);

Datausers dbUsers("/sd/datausers.db", &admin_messages);
Dataphone dbPhones("/sd/phones.db", &admin_messages);

Vista vista(&lcd);
Controlador controlador(NULL, &vista, &dbUsers, &dbPhones);

//LCD automatic off
unsigned long timestamp = 0;
bool activo = false;

//Phone number calling
char phoneCall[20] = {'\0'};

//Telegram bot variables
const unsigned long BOT_MTBS = 1000; // mean time between scan messages
const String id_admin = "ID";

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

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

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  xTaskCreatePinnedToCore(Task1Code, "Task1", 10000, NULL, 1, &Task1, 0);
  xTaskCreatePinnedToCore(Task2Code, "Task2", 10000, NULL, 1, &Task2, 1);
}

//Teclado y llamada
void Task1Code(void *pvParameters){
  for(;;){
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
          admin_messages.push("Telefono " + String(phoneCall) + " abrio la cerradura");
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
}

//Manejar mensajes telegram
void Task2Code(void *pvParameters){
  for(;;){
    if (millis() - bot_lasttime > BOT_MTBS)
    {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  
      while (numNewMessages)
      {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }
  
      bot_lasttime = millis();
    }
    while(!admin_messages.empty()){
      bot.sendMessage(id_admin, admin_messages.front(), "");
      admin_messages.pop();
    }
  }
}

void loop() {

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

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    String& id = bot.messages[i].chat_id;
    String& message = bot.messages[i].text;
    
    char params[5][20] = {0};
    char n_param = 0;
    char j = 0;
    for(char i=0; i<message.length(); i++){
      char c = message[i];
      if (c == ','){
        j = 0;
        n_param++;
        continue;
      }
      if(j>=20){
        continue;
      }
      params[n_param][j] = c;
      j++;
    }
    if(!isCorrect(params)){
      bot.sendMessage(id, "DATOS ERRONEOS", "");
      Serial.println("DATOS ERRONEOS");
      continue;
    }

    transaction(params[0], params[1], params[2][0], params[3], params[4], id);    
  }
}

bool isCorrect(char params[][20]){
  //Verificar usuario
  if(strlen(params[0])!=4){
    return false;
  }
  //Verificar clave
  if(strlen(params[1])!=4){
    return false;
  }
  //Verificar operacion (entre 0 y 8)
  if(strlen(params[2])!=1 || params[2][0]<48 || params[2][0]>56){
    return false;
  }
  char op = params[2][0];
  char *p1 = params[3];
  char *p2 = params[4];
  //Verificar largo primer parametro es 4, operaciones 1 a 6
  if(op>=49 && op<=54 && (strlen(p1)!=4 || !isNumber(p1))){
    return false;
  }
  //Verificar largo segundo parametro es 4, operaciones 1 y 3
  if((op=='1' || op=='3') && (strlen(p2)!=4 || !isNumber(p2))){
    return false;
  }
  //Verificar largo segundo parametro es 1, operaciones 5 y 6
  if((op=='5' || op=='6') && (strlen(p2)!=1 || p2[0]<48 || p2[0]>56)){
    return false;
  }
  return true;
}

bool isNumber(char *s)
{
  while(*s){
    if(!isdigit(*s)){
      return false;
    }
      s++;
    }
  return true;
}

void transaction(const char *user, const char *pass, const char &op, const char *p1, const char *p2, const String& id){
  dbUsers.setIdTransaction(String(user));
  dbPhones.setIdTransaction(String(user));
  int resp = dbUsers.verifyUser(user, pass);
  if (resp != CORRECT_DATA){
    Serial.println("Datos usuario incorrectos");
    bot.sendMessage(id, "Datos usuario incorrectos", "");
    return;
  }
  resp = dbUsers.verifyPermission(user, op);
  if (resp == DENIED){
    Serial.println("Denegado");
    bot.sendMessage(id, "Denegado", "");
    return;
  }
  switch(op){
    case '0':
      controlador.relayOn();
      admin_messages.push(String(user) + " abrio la cerradura");
      resp = SQLITE_OK;
      break;
    case '1':
      resp = dbUsers.newUser(p1, p2);
      break;
    case '2':
      resp = dbUsers.deleteUser(p1);
      break;
    case '3':
      if (strcmp(p1, "0000")==0){
        resp = NO_ADMIN;
      } else{
        resp = dbUsers.changePass(p1, p2);
      }
      break;
    case '4':
      resp = dbUsers.changePass(user, p1);
      break;
    case '5':
      resp = dbUsers.addPermission(p1, p2[0]);
      break;
    case '6':
      resp = dbUsers.revokePermission(p1, p2[0]);
      break;
    case '7':
       resp = dbPhones.addPhone(p1);
       break;
    case '8':
      resp = dbPhones.deletePhone(p1);
      break;
    default:
      ;
  }
  String head;
  if (resp==INCORRECT_DATA){
    head = "Datos erroneos";
  } else if (resp==DENIED){
    head = "Denegado";
  } else if (resp==SQLITE_CONSTRAINT){
    head = "Ya existe";
  } else if (resp==NO_ADMIN){
    head = "No puede hacerse";
  } else if (resp != SQLITE_OK) {
    head = "Ocurrio un error";
  } else {
    head = "Hecho";
  }

  Serial.println(head);
  bot.sendMessage(id, head, "");
  dbUsers.setIdTransaction("");
  dbPhones.setIdTransaction("");
}
