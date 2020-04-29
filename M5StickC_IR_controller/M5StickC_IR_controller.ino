#include <M5StickC.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
 
//Port 32 for IR Remote Unit 
const uint16_t kIrLed = 32;
 
IRsend irsend(kIrLed);
 
struct REMOTE {
  char name[9];
  uint64_t command;
};
 
// リモコンコード一覧
REMOTE remote[] = {
  { "ON" , 0xE730E916UL},
  { "OFF", 0xE730D12EUL},
};
 
int cursor = 0; // カーソル位置
 
void setup() {
  M5.begin();     // M5StickC初期化
  irsend.begin(); // IR初期化
 
  // リモコン項目表示
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 8);
  for ( int i = 0 ; i < ( sizeof(remote) / sizeof(REMOTE) ) ; i++ ) {
    M5.Lcd.print((cursor == i) ? ">" : " ");
    M5.Lcd.println(remote[i].name);
  }
}
 
void loop() {
  M5.update();  // ボタン状態更新
 
  // M5ボタンで送信
  if ( M5.BtnA.wasPressed() ) {
    // 送信4Byte(カスタマーコード2Byte+リモコンコード+反転リモコンコード)
    irsend.sendNEC(remote[cursor].command);
 
    // デバッグ表示
    Serial.printf("Send IR : 0x%08LX", remote[cursor].command);
  }
 
  // 右ボタンでカーソル移動
  if ( M5.BtnB.wasPressed() ) {
    cursor++;
    cursor = cursor % ( sizeof(remote) / sizeof(REMOTE) );
 
    // カーソル描画
    M5.Lcd.setCursor(0, 8);
    for ( int i = 0 ; i < ( sizeof(remote) / sizeof(REMOTE) ) ; i++ ) {
      M5.Lcd.println((cursor == i) ? ">" : " ");
    }
  }
 
  delay(100);
}
