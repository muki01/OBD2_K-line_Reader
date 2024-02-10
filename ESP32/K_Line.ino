uint8_t result = 0;
uint8_t resultBuffer[20];
uint8_t initBuffer[20];

int dtcs = 0;
char dtc_Byte_1, dtc_Byte_2;
String dtcBuffer[20];

const byte init_obd[4] = { 0xC1, 0x33, 0xF1, 0x81 };    // Init fast ISO14230
const byte read_DTCs[4] = { 0xC1, 0x33, 0xF1, 0x03 };   // Read Troubleshoot Codes
const byte clear_DTCs[4] = { 0xC1, 0x33, 0xF1, 0x04 };  // Clear Troubleshoot Codes

const byte read_PIDs_20[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x00 };  // Read suported PIDs 0-20
const byte read_PIDs_40[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x20 };  // Read suported PIDs 20-40
const byte read_PIDs_60[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x40 };  // Read suported PIDs 40-60
const byte read_PIDs_80[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x60 };  // Read suported PIDs 60-80
const byte read_PIDs_A0[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x80 };  // Read suported PIDs 80-A0
const byte read_PIDs_C0[5] = { 0xC2, 0x33, 0xF1, 0x01, 0xA0 };  // Read suported PIDs A0-C0
const byte read_PIDs_E0[5] = { 0xC2, 0x33, 0xF1, 0x01, 0xC0 };  // Read suported PIDs C0-E0

const byte speed_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x0D };
const byte rpm_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x0C };
const byte throttle_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x11 };
const byte coolant_temp_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x05 };
const byte intake_temp_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x0F };
const byte voltage_obd[5] = { 0xC2, 0x33, 0xF1, 0x01, 0x42 };

void read_K() {
  //------------------------------------------------------ get speed
  writeData(speed_obd, sizeof(speed_obd));
  readData();

  if (resultBuffer[10] == 0x0D) {
    SPEED = resultBuffer[11];
  }
  ws();

  //------------------------------------------------------ get rpm
  writeData(rpm_obd, sizeof(rpm_obd));
  readData();

  if (resultBuffer[10] == 0x0C) {
    RPM = (resultBuffer[11] * 256 + resultBuffer[12]) / 4;
  }
  ws();

  //------------------------------------------------------ get throttle
  writeData(throttle_obd, sizeof(throttle_obd));
  readData();

  if (resultBuffer[10] == 0X11) {
    THROTTLE = resultBuffer[11] * 100 / 255;
    // THROTTLE = resultBuffer[11] * 100 / 180 - 14;
  }
  ws();

  //------------------------------------------------------ get Coolant temp
  writeData(coolant_temp_obd, sizeof(coolant_temp_obd));
  readData();

  if (resultBuffer[10] == 0x05) {
    COOLANT_TEMP = resultBuffer[11] - 40;
  }
  ws();

  //------------------------------------------------------ get Intake temp
  writeData(intake_temp_obd, sizeof(intake_temp_obd));
  readData();

  if (resultBuffer[10] == 0x0F) {
    INTAKE_TEMP = resultBuffer[11] - 40;
  }
  ws();

  //------------------------------------------------------ get voltage
  // writeData(voltage_obd, sizeof(voltage_obd));
  // readData();

  // if (resultBuffer[10] == 0X42) {
  //   VOLTAGE = (resultBuffer[11] * 256 + resultBuffer[12]) / 1000;
  // }
  // ws();
}

void read_DTC() {
  writeData(read_DTCs, sizeof(read_DTCs));
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    for (int i = 0; i < result; i++) {
      resultBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
    }
    int length = result - 10;
    for (int i = 0; i < length; i++) {
      int index1 = 9 + i * 2;
      int index2 = 9 + i * 2 + 1;
      dtc_Byte_1 = resultBuffer[index1];
      dtc_Byte_2 = resultBuffer[index2];
      delay(READ_DELAY);

      if (dtc_Byte_1 == 0 && dtc_Byte_2 == 0) {
        return;
      } else {
        decodeDTC(dtc_Byte_1, dtc_Byte_2);
      }
    }
  }
}

void clear_DTC() {
  writeData(clear_DTCs, sizeof(clear_DTCs));
}

bool init_KWP() {
  K_Serial.end();

  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(25);
  digitalWrite(K_line_TX, HIGH), delay(25);

  K_Serial.begin(10400, SERIAL_8N1);
  writeData(init_obd, sizeof(init_obd));
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    for (int i = 0; i < result; i++) {
      initBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
    }
  }
  if (initBuffer[8] == 0xC1) {
    return true;
  } else {
    return false;
  }
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

byte calculateChecksum(const byte data[], int length) {
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
  dtcs++;
}