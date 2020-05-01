#include <M5Atom.h>
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
//#define ESPALEXA_ASYNC
#include <Espalexa.h>
#include <ESP32Servo.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "config.h"

TaskHandle_t th[4];

const char* device_name = "Printer";

//Port 32 for IR Remote Unit 
const uint16_t kIrLed = 26;
IRsend irsend(kIrLed);

enum {
  REMOTE_OFF = 0,
  REMOTE_ON,
};

const uint64_t remote_cmd[] = {
  0xE730D12EUL, //OFF
  0xE730E916UL, //ON
};
 
boolean connectWifi();

enum {
  SW_NOT_SET = 0,
  SW_ON,
  SW_OFF,
};

int8_t sw_state = SW_NOT_SET;

//Servo
Servo servo1; // create four servo objects 
int32_t servo1_pin = 25;
const int32_t MIN_US = 500;
const int32_t MAX_US = 2400;

int16_t cur_servo_pos = 0;
int16_t goal_servo_pos = 0;
const int16_t PWR_ON_POS = 30;
const int16_t PWR_OFF_POS = 0;
const int16_t MOVE_AMOUNT = 5;

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

void setupServo(void)
{
  servo1.setPeriodHertz(50); // Standard 50hz servo
  servo1.attach(servo1_pin, MIN_US, MAX_US);

  cur_servo_pos = servo1.read();
  Serial.print("cur_servo_pos:");
  Serial.println(cur_servo_pos);
}

void setupIRRemote(void)
{
  irsend.begin();
}

void setup()
{
  M5.begin(true, false, true);

  Serial.begin(115200);

  setupEspalexa();
  setupServo();
  setupIRRemote();

  xTaskCreatePinnedToCore(servoControl, "servoControl", 4096, NULL, 1, &th[0], 0);
}

void switchON(void)
{
  Serial.println("ON");
  sw_state = SW_ON;
  goal_servo_pos = PWR_ON_POS;
  irsend.sendNEC(remote_cmd[REMOTE_ON]);
}

void switchOFF(void)
{
  sw_state = SW_OFF;
  Serial.println("OFF");
  goal_servo_pos = PWR_OFF_POS;
  irsend.sendNEC(remote_cmd[REMOTE_OFF]);
}

void ThreeDPrinterSwitch(EspalexaDevice* d) {
  if(d == nullptr) return; //this is good practice, but not required

  //do what you need to do here
  Serial.print("A changed to ");
  if(d->getValue()){
    switchON();
  }else{
    switchOFF();
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

void printStatus()
{
  switch(sw_state){
  case SW_ON:
    M5.dis.drawpix(0, 0xf00000);
    break;
  case SW_OFF:
    M5.dis.drawpix(0, 0x00f000);
    break;
  case SW_NOT_SET:
  default:
    M5.dis.drawpix(0, 0x0000f0);
    break;
  }
}

void checkButton()
{
  if(M5.Btn.wasPressed()){
    if(sw_state == SW_ON){
      switchOFF();
    }else{
      switchON();
    }
  }
}

void loop()
{
  M5.update();  // ボタン状態更新
  printStatus();
  checkButton();
  espalexa.loop();
  delay(1);
}

void moveServo()
{
  if(goal_servo_pos == cur_servo_pos) return;

  servo1.write(goal_servo_pos);
  cur_servo_pos = goal_servo_pos;
  delay(100);
}

void servoControl(void *pvParameters)
{
  while(1){
    moveServo();
    delay(1);
  }
}