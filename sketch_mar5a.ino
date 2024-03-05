
#include"blescan.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LEDS_COUNT  8
#define LEDS_PIN    48 //48号脚默认为板载RGB
#define CHANNEL        0
#include <USB.h>
#include <USBHIDMouse.h>
#include <USBHIDKeyboard.h>
USBHIDMouse esp32_Mouse;
USBHIDKeyboard esp32_Keyboard;
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB); //初始化LED灯

void force_off(){
  strip.setLedColor(0, 0, 0, 255);
  digitalWrite(38,LOW);
  esp32_Keyboard.press(KEY_LEFT_GUI);
  delay(500);
  esp32_Keyboard.press('l');
  delay(500);
  esp32_Keyboard.release(KEY_LEFT_GUI);
  esp32_Keyboard.release('l');
  while(1){
    while(digitalRead(0)== LOW){
      while(digitalRead(0)== LOW){}
      strip.setLedColor(0, 0,255, 0);
      turn_on_by_ble = true;
      digitalWrite(38,HIGH);
      lastspot = millis();
      return;
    }
  }
}

void setup() {
  strip.begin();//启用
  strip.setBrightness(10);//设置亮度/强度 0 - 100
  pinMode(0, INPUT_PULLUP); //button pin
  Serial.begin(115200);
  pinMode(38, OUTPUT);
  digitalWrite(38,LOW);  //输出高电平，LOW就是低电平
  esp32_Keyboard.begin();
  esp32_Mouse.begin();
  USB.begin();

  Serial.println("Starting NimBLE Client");
    /** Initialize NimBLE, no device name spcified as we are not advertising */
    NimBLEDevice::init("");
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);
#ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
    NimBLEDevice::setPower(9); /** +9db */
#endif
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pScan->setInterval(45);
    pScan->setWindow(15);
    pScan->setActiveScan(true);
    pScan->start(scanTime, scanEndedCB);
}

void loop() {
  while(!applewatchhere){
        delay(1);
        if(NimBLEDevice::getScan()->isScanning() == false) {
          NimBLEDevice::getScan()->start(scanTime,scanEndedCB);
          Serial.println("Restart Scan");
        }
    if (millis() - lastspot > 120000 && turn_on_by_ble){
      Serial.println("Time out reached lets turn off");
      esp32_Keyboard.press(KEY_LEFT_GUI);
      delay(500);
      esp32_Keyboard.press('l');
      delay(500);
      esp32_Keyboard.release(KEY_LEFT_GUI);
      esp32_Keyboard.release('l');
      strip.setLedColor(0, 255, 0, 0);
      digitalWrite(38,LOW);
      turn_on_by_ble = false;
    }
    while(digitalRead(0)== LOW){
      while(digitalRead(0)== LOW){}
      force_off();
    }
  }
  if(!turn_on_by_ble){
    Serial.println("turned on by ble");
    strip.setLedColor(0, 0, 255, 0);
    digitalWrite(38,HIGH);
    turn_on_by_ble=true;
  }
  unsigned int wait_time = millis();
  while(millis()-wait_time < 30000){
    while(digitalRead(0)== LOW){
      while(digitalRead(0)== LOW){}
      force_off();
      break;
    }
  }
  applewatchhere = false;
  NimBLEDevice::getScan()->start(scanTime,scanEndedCB);
  //esp32_Mouse.move(10, 10);
}
