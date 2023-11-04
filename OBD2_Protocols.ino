#include <AltSoftSerial.h>
AltSoftSerial K_Serial;

#define K_line_RX 8
#define K_line_TX 9

//Speeds for KWP
#define READ_DELAY 5
#define WRITE_DELAY 5
#define REQUEST_DELAY 50

//Speeds for ISO9141
// #define READ_DELAY 5
// #define WRITE_DELAY 5
// #define REQUEST_DELAY 500

int SPEED, RPM, THROTTLE, COOLANT_TEMP, INTAKE_TEMP, VOLTAGE;

char buffer[20];
char initBuffer[20];
int result = 0;

const byte init_obd[4] = { 0xC1, 0x33, 0xF1, 0x81 };  // Init fast ISO14230

// Request data bytes for fast init (ISO14230)
const byte speed_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x0D };
const byte rpm_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x0C };
const byte throttle_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x11 };
const byte coolant_temp_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x05 };
const byte intake_temp_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x0F };

//byte gear_obd[6] = {0xC2, 0x33, 0xF1, 0x01, 0xA4, 0x8B};

// Request data bytes for slow init (ISO9141 and KWP slow)
// const byte speed_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x0D };
// const byte rpm_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x0C };
// const byte throttle_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x11 };
// const byte coolant_temp_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x05 };
// const byte intake_temp_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x0F };
// const byte voltage_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x42 };

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
}

void loop() {
  Serial.println("Initialising...");
  // bool init_success = init_ISO9141();
  bool init_success = init_KWP();
  if (init_success) {
    while (1) {
      read_K();
    }
  }

  delay(1000);
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

  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(25);
  digitalWrite(K_line_TX, HIGH), delay(25);

  K_Serial.begin(10400);
  Serial.println("SEND: C1 33 F1 81 66");
  writeData(init_obd, sizeof(init_obd));
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    for (int i = 0; i < result; i++) {
      initBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
      Serial.print(initBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    return true;
  }
  return false;
}

bool init_ISO9141() {
  K_Serial.end();

  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(200);
  digitalWrite(K_line_TX, HIGH), delay(400);
  digitalWrite(K_line_TX, LOW), delay(400);
  digitalWrite(K_line_TX, HIGH), delay(400);
  digitalWrite(K_line_TX, LOW), delay(400);
  digitalWrite(K_line_TX, HIGH), delay(200);

  K_Serial.begin(10400);
  delay(300);

  result = K_Serial.available();
  if (result > 0) {
    Serial.print("First 3 Bytes: ");
    for (int i = 0; i < result; i++) {
      initBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
      Serial.print(initBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(30);
    K_Serial.write(~initBuffer[2]);  //0xF7
    delay(50);

    result = K_Serial.available();
    if (result > 0) {
      Serial.print("Other Bytes: ");
      for (int i = 0; i < result; i++) {
        initBuffer[i] = K_Serial.read();
        delay(READ_DELAY);
        Serial.print(initBuffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      return true;
    }
  }
  return false;
}

void writeData(const byte data[], int length) {
  delay(WRITE_DELAY);
  byte checksum = calculateChecksum(data, length);
  for (int i = 0; i < length; i++) {
    K_Serial.write(data[i]);
    delay(WRITE_DELAY);
  }
  K_Serial.write(checksum);
}

void readData() {
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    for (int i = 0; i < result; i++) {
      buffer[i] = K_Serial.read();
      delay(READ_DELAY);
    }
  }
}

byte calculateChecksum(byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum % 256;
}