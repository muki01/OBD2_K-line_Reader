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

int SPEED = 0, RPM = 0, THROTTLE = 0, COOLANT_TEMP = 0, INTAKE_TEMP = 0, TIMINGADVANCE = 0, ENGINELOAD = 0, MAF = 0, DISTANCE_TRAVELED_WITH_MIL = 0;
int freeze_SPEED = 0, freeze_RPM = 0, freeze_THROTTLE = 0, freeze_COOLANT_TEMP = 0, freeze_INTAKE_TEMP = 0, freeze_TIMINGADVANCE = 0, freeze_ENGINELOAD = 0, freeze_MAF = 0;
double VOLTAGE = 0;
String Vehicle_VIN = "", Vehicle_ID = "", Vehicle_ID_Num = "";

bool KLineStatus = false;

static unsigned long lastDTCTime = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
  K_Serial.begin(10400);
}

void loop() {
  if (KLineStatus == false) {
    Serial.println("Initialising KWP_Fast...");
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
