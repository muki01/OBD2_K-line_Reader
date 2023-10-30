#include <AltSoftSerial.h>
AltSoftSerial Serial1;

#define K_line_RX 8
#define K_line_TX 9
#define K_Serial Serial1

//Speeds for KWP
// #define READ_DELAY 5
// #define WRITE_DELAY 5
// #define REQUEST_DELAY 60

//Speeds for ISO9141
#define READ_DELAY 6
#define WRITE_DELAY 6
#define REQUEST_DELAY 500

int SPEED, RPM, THROTTLE, COOLANT_TEMP, INTAKE_TEMP, VOLTAGE;

char buffer[30];
int result = 0;

const byte init_obd[5] = { 0xC1, 0x33, 0xF1, 0x81, 0x66 };  // Init fast ISO14230

// byte speed_obd[6] = { 0xC2, 0x33, 0xF1, 0x01, 0x0D, 0xF4 };
// byte rpm_obd[6] = { 0xC2, 0x33, 0xF1, 0x01, 0x0C, 0xF3 };
// byte throttle_obd[6] = { 0xC2, 0x33, 0xF1, 0x01, 0x11, 0xF8 };
// byte coolant_temp_obd[6] = { 0xC2, 0x33, 0xF1, 0x01, 0x05, 0xEC };
// byte intake_temp_obd[6] = { 0xC2, 0x33, 0xF1, 0x01, 0x0F, 0xF6 };

//byte gear_obd[6] = {0xC2, 0x33, 0xF1, 0x01, 0xA4, 0x8B};


const byte speed_obd[6] = { 0x68, 0x6A, 0xF1, 0x01, 0x0D, 0xD1 };
const byte rpm_obd[6] = { 0x68, 0x6A, 0xF1, 0x01, 0x0C, 0xD0 };
const byte throttle_obd[6] = { 0x68, 0x6A, 0xF1, 0x01, 0x11, 0xD5 };
const byte coolant_temp_obd[6] = { 0x68, 0x6A, 0xF1, 0x01, 0x05, 0xC9 };
const byte intake_temp_obd[6] = { 0x68, 0x6A, 0xF1, 0x01, 0x0F, 0xD3 };
const byte voltage_obd[6] = { 0x68, 0x6A, 0xF1, 0x01, 0x42, 0x06 };

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
}

void loop() {
  bool init_success = init_ISO9141();
  // bool init_success = init_KWP();
  if (init_success) {
    while (1) {
      read_K();
    }
  }
}

void read_K() {
  Serial.flush();

  //------------------------------------------------------ get throttle

  writeData(throttle_obd, sizeof(throttle_obd));
  readData();

  if (buffer[10] == 0X11) {
    THROTTLE = buffer[11] * 100 / 255;
    // THROTTLE = buffer[11] * 100 / 180 - 14;
    Serial.print("Throttle: ");
    Serial.println(THROTTLE);
  }

  //------------------------------------------------------ get voltage

  // writeData(voltage_obd, sizeof(voltage_obd));
  // readData();

  // if (buffer[10] == 0X42) {
  //   VOLTAGE = (buffer[11] * 256 + buffer[12]) / 1000;
  //   Serial.print("Voltage: ");
  //   Serial.println(VOLTAGE);
  // }


  //------------------------------------------------------ get rpm
  writeData(rpm_obd, sizeof(rpm_obd));
  readData();

  if (buffer[10] == 0x0C) {
    RPM = (buffer[11] * 256 + buffer[12]) / 4;
    Serial.print("RPM: ");
    Serial.println(RPM);
  }


  //------------------------------------------------------ get Coolant temp
  writeData(coolant_temp_obd, sizeof(coolant_temp_obd));
  readData();

  if (buffer[10] == 0x05) {
    COOLANT_TEMP = buffer[11] - 40;
    Serial.print("Coolant Temp: ");
    Serial.println(COOLANT_TEMP);
  }

  //------------------------------------------------------ get Intake temp
  writeData(intake_temp_obd, sizeof(intake_temp_obd));
  readData();

  if (buffer[10] == 0x0F) {
    INTAKE_TEMP = buffer[11] - 40;
    Serial.print("Intake Temp: ");
    Serial.println(INTAKE_TEMP);
  }


  //------------------------------------------------------ get speed
  writeData(speed_obd, sizeof(speed_obd));
  readData();

  if (buffer[10] == 0x0D) {
    SPEED = buffer[11];
    Serial.print("Speed: ");
    Serial.println(SPEED);
  }

  Serial.println();
}

bool init_KWP() {
  K_Serial.end();
  char buffer1[30];
  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(25);
  digitalWrite(K_line_TX, HIGH), delay(25);
  K_Serial.begin(10400);  // -------------- ISO 14230-4 KWP 10.4 Kbaud
  for (int i = 0; i < 5; i++) {
    K_Serial.write(init_obd[i]);
    delay(WRITE_DELAY);
  }
  Serial.println("SEND: C1 33 F1 81 66");
  delay(REQUEST_DELAY);

  int n = K_Serial.available();
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      buffer1[i] = K_Serial.read();
      Serial.print(buffer1[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    return true;
  }
  delay(1000);
  return false;
}

bool init_ISO9141() {
  K_Serial.end();
  char buffer2[30];
  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(200);
  digitalWrite(K_line_TX, HIGH), delay(400);
  digitalWrite(K_line_TX, LOW), delay(400);
  digitalWrite(K_line_TX, HIGH), delay(400);
  digitalWrite(K_line_TX, LOW), delay(400);
  digitalWrite(K_line_TX, HIGH), delay(200);
  // K_Serial.begin(5);
  // K_Serial.write(0x33);
  // K_Serial.end();
  // digitalWrite(K_line_TX, HIGH);
  K_Serial.begin(10400);
  delay(300);  //103-105ms
  // while (!K_Serial.available())
  //   ;
  int n = K_Serial.available();
  Serial.print("size:");
  Serial.println(n);
  if (n > 0) {
    Serial.print("First 3 Bytes: ");
    for (int i = 0; i < n; i++) {
      buffer2[i] = K_Serial.read();
      Serial.print(buffer2[i], HEX);
      Serial.print(" ");
      delay(READ_DELAY);
    }
    Serial.println();
    delay(30);
    K_Serial.write(~buffer2[2]); //0xF7
    delay(50);
    n = K_Serial.available();
    if (n > 0) {
      Serial.print("Other Bytes: ");
      for (int i = 0; i < n; i++) {
        buffer2[i] = K_Serial.read();
        Serial.print(buffer2[i], HEX);
        Serial.print(" ");
        delay(READ_DELAY);
      }
      Serial.println();
      return true;
    }
  }
  delay(1000);
  return false;
}

void writeData(const byte data[], byte length) {
  for (int i = 0; i < length; i++) {
    K_Serial.write(data[i]);
    delay(WRITE_DELAY);
  }
}

void readData() {
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    for (int i = 0; i < result; i++) {
      buffer[i] = K_Serial.read();
      delay(READ_DELAY);
      //Serial.print(buffer[i],HEX);
      //Serial.print(" ");
    }
    //Serial.println("");
  }
}