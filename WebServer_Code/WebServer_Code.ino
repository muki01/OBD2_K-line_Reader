#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <ArduinoJson.h>
#include "PIDs.h"

JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#ifdef ESP32
#define K_line_RX 20
#define K_line_TX 21
#define Led 8
#define Buzzer 1
#define voltagePin 0
#elif defined(ESP8266)
#define K_line_RX 3
#define K_line_TX 1
#define Led 2
#define Buzzer 4
#define voltagePin 5
#endif
#define K_Serial Serial

#define READ_DELAY 5
int REQUEST_DELAY;

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol, selectedProtocol = "", connectedProtocol = "";
int page = -1;

int oxygenSensor1Voltage = 0, shortTermFuelTrim1 = 0, oxygenSensor2Voltage = 0, shortTermFuelTrim2 = 0;
int oxygenSensor3Voltage = 0, shortTermFuelTrim3 = 0, oxygenSensor4Voltage = 0, shortTermFuelTrim4 = 0;
int oxygenSensor5Voltage = 0, shortTermFuelTrim5 = 0, oxygenSensor6Voltage = 0, shortTermFuelTrim6 = 0;
int oxygenSensor7Voltage = 0, shortTermFuelTrim7 = 0, oxygenSensor8Voltage = 0, shortTermFuelTrim8 = 0;

double VOLTAGE = 0;
String Vehicle_VIN = "", Vehicle_ID = "", Vehicle_ID_Num = "";

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
  selectedProtocol = protocol;

  initWiFi();
  initWebSocket();
  initWebServer();
  K_Serial.begin(10400, SERIAL_8N1);
}

void loop() {
  if (KLineStatus == false) {
    Melody3();
    bool init_success = init_OBD2();

    if (init_success) {
      KLineStatus = true;
      connectedProtocol = protocol;
      digitalWrite(Led, LOW);
      connectMelody();
      getVIN();
      getCalibrationID();
      getCalibrationIDNum();
      getSupportedPIDs(0x01);
      getSupportedPIDs(0x02);
      getSupportedPIDs(0x09);
    }
  } else {
    read_K();
  }

  VOLTAGE = (double)analogRead(voltagePin) / 4096 * 17.4;
  VOLTAGE = round(VOLTAGE * 10) / 10;

  if (millis() - lastWsTime >= 100) {
    sendDataToServer();
    lastWsTime = millis();
  }
}
