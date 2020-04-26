#include <M5StickC.h>
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
//#define ESPALEXA_ASYNC
#include <Espalexa.h>
#include "config.h"

const char* device_name = "Printer";

boolean connectWifi();


//callback function prototype
void ThreeDPrinterSwitch(EspalexaDevice* dev);

boolean wifiConnected = false;

Espalexa espalexa;

void setupEspalexa()
{
  wifiConnected = connectWifi();

  if(wifiConnected){
    espalexa.addDevice(device_name, ThreeDPrinterSwitch, EspalexaDeviceType::onoff);//ここでデバイス追加を行っている。
    espalexa.begin();

  }else{
    while(1){
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}

void setup()
{
  M5.begin();     // M5StickC初期化
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 8);

  Serial.begin(115200);

  setupEspalexa();

}

void ThreeDPrinterSwitch(EspalexaDevice* d) {
  if(d == nullptr) return; //this is good practice, but not required

  //do what you need to do here
  Serial.print("A changed to ");
  if(d->getValue()){
    Serial.println("ON");
    M5.Lcd.println("ON");
  }else{
    Serial.println("OFF");
    M5.Lcd.println("OFF");
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
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    if (i > 40){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if(state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }else{
    Serial.println("Connection failed.");
  }
  return state;
}

void loop()
{
  M5.update();  // ボタン状態更新
  M5.Lcd.setCursor(0, 8);
  M5.Lcd.println("Alexa test");
  espalexa.loop();
  delay(1);
}

