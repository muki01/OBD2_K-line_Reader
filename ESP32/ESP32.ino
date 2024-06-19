#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "PIDs.h"

JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#define K_Serial Serial
#define K_line_RX 20
#define K_line_TX 21
#define Led 8
#define Buzzer 1
#define voltagePin 0

#define READ_DELAY 5
int REQUEST_DELAY;

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol;
int page = -1;

int SPEED = 0, RPM = 0, THROTTLE = 0, COOLANT_TEMP = 0, INTAKE_TEMP = 0, TIMINGADVANCE = 0, ENGINELOAD = 0, MAF = 0;
double VOLTAGE = 0;
bool KLineStatus = false;

static unsigned long lastReqestTime = 5000, lastWsTime = 100, lastDTCTime = 1000;

void setup() {
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(voltagePin, INPUT);
  digitalWrite(Led, HIGH);

  initSpiffs();
  readSettings();

  initWiFi();
  initWebSocket();
  initWebServer();
  K_Serial.begin(10400, SERIAL_8N1);
  if (protocol == "ISO14230_Fast") {
    REQUEST_DELAY = 50;
  } else {
    REQUEST_DELAY = 500;
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
  for (int i = 1; i <= count; i++) {
    digitalWrite(Led, LOW);
    delay(time);
    digitalWrite(Led, HIGH);
    if (i != count) {
      delay(time);
    }
  }
}
