#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "PIDs.h"
//DynamicJsonDocument jsonDoc(512);
JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#define K_Serial Serial
#define K_line_RX 20
#define K_line_TX 21
#define Led 8
#define Buzzer 0

#define READ_DELAY 5
int REQUEST_DELAY = 500;
//#define REQUEST_DELAY 50
//#define REQUEST_DELAY_Slow 500

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol;
int page = -1;

int SPEED = 1, RPM = 1, THROTTLE = 1, COOLANT_TEMP = 1, INTAKE_TEMP = 1, VOLTAGE = 1, TIMINGADVANCE = 1, ENGINELOAD = 1, MAF = 1;
bool KLineStatus = false;

static unsigned long lastReqestTime = 0, lastWsTime = 0, lastDTCTime = 0;

void setup() {
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Led, HIGH);

  readSettings();

  initWiFi();
  initWebSocket();
  initWebServer();
  if (protocol == "ISO14230_Fast") {
    REQUEST_DELAY = 50;
  }
}

void loop() {
  if (KLineStatus == false) {
    if (millis() - lastReqestTime >= 5000) {
      tone(Buzzer, 500, 40);
      tone(Buzzer, 600, 60);
      bool init_success;

      if (protocol == "ISO14230_Fast") {
        init_success = init_KWP();
      } else if (protocol == "ISO14230_Slow" || protocol == "ISO9141") {
        init_success = init_KWP_slow();
      }

      if (init_success) {
        connection();
        KLineStatus = true;
        digitalWrite(Led, LOW);
        get_DTCs();
      }
      lastReqestTime = millis();
    }
  } else {
    read_K();
  }

  if (millis() - lastWsTime >= 100) {
    sendDataToServer();
    lastWsTime = millis();
  }
}

void connection() {
  tone(Buzzer, 660, 50);
  delay(80);
  tone(Buzzer, 1320, 55);
  delay(70);
  tone(Buzzer, 1760, 100);
}

void disconnection() {
  tone(Buzzer, 660, 50);
  delay(80);
  tone(Buzzer, 1760, 55);
  delay(70);
  tone(Buzzer, 1320, 100);
}

void mode1() {
  tone(Buzzer, 1100, 50);
  delay(10);
  tone(Buzzer, 1300, 50);
}

void mode2() {
  tone(Buzzer, 1320, 50);
  delay(10);
  tone(Buzzer, 1570, 50);
}

void BlinkLed(int time, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(Led, LOW);
    delay(time);
    digitalWrite(Led, HIGH);
    delay(time);
  }
}
