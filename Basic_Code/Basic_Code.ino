#include "PIDs.h"

#ifdef ESP32
#define K_Serial Serial1
#define K_line_RX 25
#define K_line_TX 26
#define Led 2
#elif defined(ARDUINO)
#include <AltSoftSerial.h>
AltSoftSerial Alt_Serial;
#define K_Serial Alt_Serial
#define K_line_RX 8
#define K_line_TX 9
#define Led 13
#endif

#define WRITE_DELAY 5             // Delay between each byte of the transmitted data (5ms - 20ms)
#define DATA_REQUEST_INTERVAL 60  // Time to wait before sending a new request after receiving a response (55ms - 5000ms)

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
int errors = 0;

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);

  Serial.print("Selected Protocol: ");
  Serial.println(protocol);

  begin_K_Serial();
}

void loop() {
  if (KLineStatus == false) {
    Serial.println("Initialising...");
    bool init_success = init_OBD2();

    if (init_success) {
      Serial.println("Init Success !!");
      KLineStatus = true;
      digitalWrite(Led, HIGH);
    }
  } else {
    obdTask();
  }
}
