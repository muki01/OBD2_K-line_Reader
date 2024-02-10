#include <AltSoftSerial.h>
AltSoftSerial K_Serial;

// Define Pins
#define K_line_RX 8
#define K_line_TX 9
#define Led 13

// Define Delays
#define READ_DELAY 5
#define REQUEST_DELAY 500

int SPEED, RPM, THROTTLE, COOLANT_TEMP, INTAKE_TEMP, VOLTAGE;
bool KLineStatus = false;
static unsigned long lastReqestTime = 0;

uint8_t result = 0;
uint8_t resultBuffer[20];
uint8_t initBuffer[20];

int dtcs = 0;
char dtc_Byte_1, dtc_Byte_2;
String dtcBuffer[20];

const byte read_DTCs[4] = { 0x68, 0x6A, 0xF1, 0x03 };   // Read Troubleshoot Codes
const byte clear_DTCs[4] = { 0x68, 0x6A, 0xF1, 0x04 };  // Clear Troubleshoot Codes

// Request data bytes for slow init (ISO9141 and KWP slow)
const byte read_PIDs_20[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x00 };  // Read suported PIDs 0-20
const byte read_PIDs_40[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x20 };  // Read suported PIDs 20-40
const byte read_PIDs_60[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x40 };  // Read suported PIDs 40-60
const byte read_PIDs_80[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x60 };  // Read suported PIDs 60-80
const byte read_PIDs_A0[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x80 };  // Read suported PIDs 80-A0
const byte read_PIDs_C0[5] = { 0x68, 0x6A, 0xF1, 0x01, 0xA0 };  // Read suported PIDs A0-C0
const byte read_PIDs_E0[5] = { 0x68, 0x6A, 0xF1, 0x01, 0xC0 };  // Read suported PIDs A0-E0

const byte speed_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x0D };
const byte rpm_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x0C };
const byte throttle_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x11 };
const byte coolant_temp_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x05 };
const byte intake_temp_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x0F };
const byte voltage_obd[5] = { 0x68, 0x6A, 0xF1, 0x01, 0x42 };

void setup() {
  Serial.begin(9600);
  pinMode(K_line_RX, INPUT_PULLUP);
  pinMode(K_line_TX, OUTPUT);
  pinMode(Led, OUTPUT);
}

void loop() {
  if (KLineStatus == false) {
    if (millis() - lastReqestTime >= 3000) {
      Serial.println("Initialising ISO9141...");
      bool init_success = init_ISO9141();
      if (init_success) {
        KLineStatus = true;
        digitalWrite(Led, HIGH);
        Serial.println("Init Success !!");
        read_DTC();
      }
      lastReqestTime = millis();
    }
  } else {
    read_K();
  }
}

void read_K() {
  //------------------------------------------------------ get speed
  writeData(speed_obd, sizeof(speed_obd));
  readData();

  if (resultBuffer[10] == 0x0D) {
    SPEED = resultBuffer[11];
    Serial.print("Speed: ");
    Serial.println(SPEED);
  }

  //------------------------------------------------------ get rpm
  writeData(rpm_obd, sizeof(rpm_obd));
  readData();

  if (resultBuffer[10] == 0x0C) {
    RPM = (resultBuffer[11] * 256 + resultBuffer[12]) / 4;
    Serial.print("RPM: ");
    Serial.println(RPM);
  }

  //------------------------------------------------------ get throttle
  writeData(throttle_obd, sizeof(throttle_obd));
  readData();

  if (resultBuffer[10] == 0X11) {
    THROTTLE = resultBuffer[11] * 100 / 255;
    Serial.print("Throttle: ");
    Serial.println(THROTTLE);
  }

  //------------------------------------------------------ get Coolant temp
  writeData(coolant_temp_obd, sizeof(coolant_temp_obd));
  readData();

  if (resultBuffer[10] == 0x05) {
    COOLANT_TEMP = resultBuffer[11] - 40;
    Serial.print("Coolant Temp: ");
    Serial.println(COOLANT_TEMP);
  }

  //------------------------------------------------------ get Intake temp
  writeData(intake_temp_obd, sizeof(intake_temp_obd));
  readData();

  if (resultBuffer[10] == 0x0F) {
    INTAKE_TEMP = resultBuffer[11] - 40;
    Serial.print("Intake Temp: ");
    Serial.println(INTAKE_TEMP);
  }

  //------------------------------------------------------ get voltage
  // writeData(voltage_obd, sizeof(voltage_obd));
  // readData();

  // if (resultBuffer[10] == 0X42) {
  //   VOLTAGE = (resultBuffer[11] * 256 + resultBuffer[12]) / 1000;
  //   Serial.print("Voltage: ");
  //   Serial.println(VOLTAGE);
  // }

  Serial.println();
}

void read_DTC() {
  writeData(read_DTCs, sizeof(read_DTCs));
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    // Serial.print("Buffer: ");
    for (int i = 0; i < result; i++) {
      resultBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
      // Serial.print(resultBuffer[i], HEX);
      // Serial.print(" ");
    }
    // Serial.println();
    int length = result - 10;
    for (int i = 0; i < length; i++) {
      int index1 = 9 + i * 2;
      int index2 = 9 + i * 2 + 1;
      dtc_Byte_1 = resultBuffer[index1];
      dtc_Byte_2 = resultBuffer[index2];
      delay(READ_DELAY);

      if (dtc_Byte_1 == 0 && dtc_Byte_2 == 0) {
        if (i == 0) {
          Serial.println("Not Found Errors !");
        } else {
          Serial.println("Errors ended !");
        }
        Serial.println();
        return;
      } else {
        // Serial.print("Errors: "), Serial.print(dtc_Byte_1, HEX), Serial.print(" "), Serial.println(dtc_Byte_2, HEX);
        decodeDTC(dtc_Byte_1, dtc_Byte_2);
      }
    }
  }
}

void clear_DTC() {
  writeData(clear_DTCs, sizeof(clear_DTCs));
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
  delay(REQUEST_DELAY);

  result = K_Serial.available();
  if (result > 0) {
    // Serial.print("First 3 Bytes: ");
    for (int i = 0; i < result; i++) {
      initBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
      // Serial.print(initBuffer[i], HEX);
      // Serial.print(" ");
    }
    // Serial.println();
    if (initBuffer[0] == 0x55) {
      delay(30);
      K_Serial.write(~initBuffer[2]);  //0xF7
      delay(50);

      result = K_Serial.available();
      if (result > 0) {
        // Serial.print("Other Bytes: ");
        for (int i = 0; i < result; i++) {
          initBuffer[i] = K_Serial.read();
          delay(READ_DELAY);
          // Serial.print(initBuffer[i], HEX);
          // Serial.print(" ");
        }
        // Serial.println();
        return true;
      }
    }
  }
  return false;
}

void writeData(const byte data[], int length) {
  byte checksum = calculateChecksum(data, length);
  K_Serial.write(data, length);
  K_Serial.write(checksum);
}

void readData() {
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    for (int i = 0; i < result; i++) {
      resultBuffer[i] = K_Serial.read();
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

void decodeDTC(char input_byte1, char input_byte2) {
  String ErrorCode = "";
  const static char type_lookup[4] = { 'P', 'C', 'B', 'U' };
  const static char digit_lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  ErrorCode += type_lookup[(input_byte1 >> 6) & 0b11];
  ErrorCode += digit_lookup[(input_byte1 >> 4) & 0b11];
  ErrorCode += digit_lookup[input_byte1 & 0b1111];
  ErrorCode += digit_lookup[input_byte2 >> 4];
  ErrorCode += digit_lookup[input_byte2 & 0b1111];

  dtcBuffer[dtcs] = ErrorCode;
  Serial.print("Error Code: ");
  Serial.println(dtcBuffer[dtcs]);
  dtcs++;
}