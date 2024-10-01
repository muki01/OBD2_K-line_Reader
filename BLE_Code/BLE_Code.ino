#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "PIDs.h"
JsonDocument jsonDoc;


#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define K_Serial Serial
#define K_line_RX 20
#define K_line_TX 21
#define Led 8
#define Buzzer 1
#define voltagePin 0

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

  init_BLE();
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
}
