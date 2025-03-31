#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <Update.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "PIDs.h"

JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#ifdef ESP32
#define K_Serial Serial1
#define K_line_RX 10
#define K_line_TX 11
#define Led 7
#define Buzzer 9
#define voltagePin 8
#define DEBUG_Serial
#elif defined(ESP8266)
#define K_Serial Serial
#define K_line_RX 3
#define K_line_TX 1
#define Led 2
#define Buzzer 4
#define voltagePin 5
#endif

#ifdef DEBUG_Serial
#define debugPrint(x) Serial.print(x)
#define debugPrintln(x) Serial.println(x)
#define debugPrintHex(x) Serial.print(x, HEX)
#else
#define debugPrint(x) ((void)0)
#define debugPrintln(x) ((void)0)
#define debugPrintHex(x) ((void)0)
#endif

#define WRITE_DELAY 5             // Delay between each byte of the transmitted data (5ms - 20ms)
#define DATA_REQUEST_INTERVAL 60  // Time to wait before sending a new request after receiving a response (55ms - 5000ms)

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol, connectedProtocol = "";
int page = -1, errors = 0;

int oxygenSensor1Voltage = 0, shortTermFuelTrim1 = 0, oxygenSensor2Voltage = 0, shortTermFuelTrim2 = 0;
int oxygenSensor3Voltage = 0, shortTermFuelTrim3 = 0, oxygenSensor4Voltage = 0, shortTermFuelTrim4 = 0;
int oxygenSensor5Voltage = 0, shortTermFuelTrim5 = 0, oxygenSensor6Voltage = 0, shortTermFuelTrim6 = 0;
int oxygenSensor7Voltage = 0, shortTermFuelTrim7 = 0, oxygenSensor8Voltage = 0, shortTermFuelTrim8 = 0;

double VOLTAGE = 0;
String Vehicle_VIN = "", Vehicle_ID = "", Vehicle_ID_Num = "";

bool KLineStatus = false;

static unsigned long lastWsTime = 100, lastDTCTime = 1000;

void setup() {
#ifdef DEBUG_Serial
  Serial.begin(115200);
#endif
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(voltagePin, INPUT);
  digitalWrite(Led, HIGH);

  initSpiffs();
  readSettings();
  debugPrint("Selected Protocol: ");
  debugPrintln(protocol);

  initWiFi();
  initWebSocket();
  initWebServer();

#ifdef ESP32
  K_Serial.begin(10400, SERIAL_8N1, K_line_RX, K_line_TX);
#elif defined(ESP8266)
  K_Serial.begin(10400, SERIAL_8N1);
#endif
}

void loop() {
  if (KLineStatus == false) {
    debugPrintln("Initialising...");
    Melody3();
    bool init_success = init_OBD2();

    if (init_success) {
      debugPrintln("Init Success !!");
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

  VOLTAGE = (double)analogRead(voltagePin) / 4096.0 * 20.4;

  if (millis() - lastWsTime >= 100) {
    sendDataToServer();
    lastWsTime = millis();
  }
}
