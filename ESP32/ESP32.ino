#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "webpage.h"
DynamicJsonDocument jsonDoc(1024);

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
String JSONtxt;

#define K_Serial Serial
#define K_line_RX 20
#define K_line_TX 21
#define Led 8

#define READ_DELAY 5
#define WRITE_DELAY 5
#define REQUEST_DELAY 50

int SPEED = 10, RPM = 920, THROTTLE = 0, COOLANT_TEMP = 0, INTAKE_TEMP = 0, VOLTAGE = 0;
static unsigned long lastReqestTime = 0;
bool KLineStatus = false;

void setup() {
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  digitalWrite(Led, HIGH);

  initWiFi();
  initWebServer();
  webSocket.begin();
}

void loop() {
  ws();

  if (KLineStatus == false) {
    unsigned long reqestTime = millis() - lastReqestTime;
    if (reqestTime > 5000) {
      bool init_success = init_KWP();
      if (init_success) {
        KLineStatus = true;
        digitalWrite(Led, LOW);
        // read_DTC();
      }
      lastReqestTime = millis();
    }
  } else {
    read_K();
  }
}