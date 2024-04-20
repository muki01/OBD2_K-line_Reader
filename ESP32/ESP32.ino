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

#define READ_DELAY 5
#define REQUEST_DELAY 50

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol;
int page = -1;

int SPEED = 1, RPM = 1, THROTTLE = 1, COOLANT_TEMP = 1, INTAKE_TEMP = 1, VOLTAGE = 1, TIMINGADVANCE = 1, ENGINELOAD = 1, MAF = 1;
bool KLineStatus = false;

static unsigned long lastReqestTime = 0, lastWsTime = 0;

void setup() {
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  digitalWrite(Led, HIGH);

  readSettings();

  initWiFi();
  initWebSocket();
  initWebServer();
  // if (protocol == "ISO14230_Fast"){
  //    digitalWrite(Led, LOW);
  // }
}

void loop() {
  if (KLineStatus == false) {
    if (millis() - lastReqestTime >= 5000) {
      bool init_success = init_KWP();
      if (init_success) {
        KLineStatus = true;
        digitalWrite(Led, LOW);
        read_DTC();
      }
      lastReqestTime = millis();
    }
  } else {
    read_K();
  }

  if (millis() - lastWsTime >= 100) {
    wsSend();
    lastWsTime = millis();
  }
}
