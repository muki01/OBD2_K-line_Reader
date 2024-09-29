#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "PIDs.h"

JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#define K_Serial Serial
#define K_line_RX 3
#define K_line_TX 1
#define Led 2
#define Buzzer 4
#define voltagePin 5

#define READ_DELAY 5
int REQUEST_DELAY;

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol, selectedProtocol = "", connectedProtocol = "";
int page = -1;

int SPEED = 0, RPM = 0, THROTTLE = 0, COOLANT_TEMP = 0, INTAKE_TEMP = 0, TIMINGADVANCE = 0, ENGINELOAD = 0, MAF = 0, DISTANCE_TRAVELED_WITH_MIL = 0;
int freeze_SPEED = 0, freeze_RPM = 0, freeze_COOLANT_TEMP = 0, freeze_ENGINELOAD = 0;
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
    tone(Buzzer, 500, 40);
    tone(Buzzer, 600, 60);
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

  if (millis() - lastWsTime >= 100) {
    sendDataToServer();
    lastWsTime = millis();
  }
}
