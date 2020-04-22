/*
 * This is a basic example on how to use Espalexa with RGB color devices.
 */ 
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
//#define ESPALEXA_ASYNC
#include <Espalexa.h>
#include "config.h"

// prototypes
boolean connectWifi();

//callback function prototype
void printerSwitch(EspalexaDevice* dev);

boolean wifiConnected = false;

Espalexa espalexa;

void setup()
{
  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if(wifiConnected){
    espalexa.addDevice("3D Printer", printerSwitch, EspalexaDeviceType::onoff);//ここでデバイス追加を行っている。
    //"Color Light"はAlexaアプリで表示される名前。
    //後でAlexaアプリから変更可能です。日本語も可能。
    //最大10台まで追加可能。

    espalexa.begin();//全てのデバイスを追加完了後(設定完了後)にbeginを実行する

  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}

void loop()
{
   espalexa.loop();
   delay(1);
}

//the color device callback function has two parameters
//ここで各デバイスのコールバックのコードを記述する。
void printerSwitch(EspalexaDevice* d) {
  if (d == nullptr) return; //this is good practice, but not required

  //do what you need to do here
  //EXAMPLE
  Serial.print("A changed to ");
  if (d->getValue()){
    Serial.println("ON");
  }
  else {
    Serial.println("OFF");
  }
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 40){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
