#include "PIDs.h"
#include <AltSoftSerial.h>
AltSoftSerial Alt_Serial;

#define K_Serial Alt_Serial
#define K_line_RX 8
#define K_line_TX 9
#define Led 13

#define READ_DELAY 5
int REQUEST_DELAY;

String protocol = "Automatic";
// String protocol = "ISO9141";
// String protocol = "ISO14230_Slow";
// String protocol = "ISO14230_Fast";

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
String Vehicle_VIN = "", Vehicle_ID = "", Vehicle_ID_Num = "";

bool KLineStatus = false;

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  K_Serial.begin(10400);
}

void loop() {
  if (KLineStatus == false) {
    Serial.println("Initialising...");
    bool init_success = init_OBD2();

    if (init_success) {
      KLineStatus = true;
      digitalWrite(Led, HIGH);
      Serial.println("Init Success !!");
    }
  } else {
    read_K();
  }
}
