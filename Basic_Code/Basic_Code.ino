#include "PIDs.h"
#include <AltSoftSerial.h>
AltSoftSerial Alt_Serial;

#define K_Serial Alt_Serial
#define K_line_RX 8
#define K_line_TX 9
#define Led 13

#define READ_DELAY 5
int REQUEST_DELAY;

// Protocol 1 = ISO9141
// Protocol 2 = ISO14230 Slow
// Protocol 3 = ISO14230 Fast
int protocol = 3;

int SPEED = 0, RPM = 0, THROTTLE = 0, COOLANT_TEMP = 0, INTAKE_TEMP = 0, TIMINGADVANCE = 0, ENGINELOAD = 0, MAF = 0;
double VOLTAGE = 0;
bool KLineStatus = false;

static unsigned long lastReqestTime = 5000, lastDTCTime = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);

  if (protocol == 3) {
    REQUEST_DELAY = 50;
  } else {
    REQUEST_DELAY = 500;
  }
}

void loop() {
  if (KLineStatus == false) {
    if (millis() - lastReqestTime >= 5000) {
      Serial.println("Initialising KWP_Fast...");
      bool init_success;

      if (protocol == 3) {
        init_success = init_KWP();
      } else if (protocol == 1 || protocol == 2) {
        init_success = init_KWP_slow();
      }

      if (init_success) {
        KLineStatus = true;
        digitalWrite(Led, HIGH);
        Serial.println("Init Success !!");
      }
      lastReqestTime = millis();
    }
  } else {
    read_K();
  }
}
