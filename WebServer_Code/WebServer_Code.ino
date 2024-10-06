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

int fuelSystemStatus = 0, engineLoadValue = 0, engineCoolantTemp = 0, shortTermFuelTrimBank1 = 0;
int longTermFuelTrimBank1 = 0, shortTermFuelTrimBank2 = 0, longTermFuelTrimBank2 = 0, fuelPressureValue = 0;
int intakeManifoldAbsPressure = 0, engineRpmValue = 0, vehicleSpeedValue = 0, timingAdvanceValue = 0;
int intakeAirTempValue = 0, mafAirFlowRate = 0, throttlePositionValue = 0, secondaryAirStatus = 0;
int oxygenSensorsPresent2Banks = 0, oxygenSensor1Voltage = 0, shortTermFuelTrim1 = 0, oxygenSensor2Voltage = 0;
int shortTermFuelTrim2 = 0, oxygenSensor3Voltage = 0, shortTermFuelTrim3 = 0, oxygenSensor4Voltage = 0;
int shortTermFuelTrim4 = 0, oxygenSensor5Voltage = 0, shortTermFuelTrim5 = 0, oxygenSensor6Voltage = 0;
int shortTermFuelTrim6 = 0, oxygenSensor7Voltage = 0, shortTermFuelTrim7 = 0, oxygenSensor8Voltage = 0;
int shortTermFuelTrim8 = 0, obdStandards = 0, oxygenSensorsPresent4Banks = 0, auxiliaryInputStatus = 0;
int runTimeSinceEngineStart = 0, distanceWithMilOn = 0;

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
