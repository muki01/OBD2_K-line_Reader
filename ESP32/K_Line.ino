int result = 0;
uint8_t resultBuffer[20];
uint8_t initBuffer[20];

int dtcs = 0;
char dtcBytes[2];
String dtcBuffer[20];

void read_K() {
  K_Serial.flush();
  if (page == 4) {
    writeData(start_Bytes2, sizeof(start_Bytes2), VEHICLE_SPEED);
    readData();

    if (resultBuffer[10] == VEHICLE_SPEED) {
      SPEED = resultBuffer[11];
    }
    wsSend();
  } else if (page == 1 || page == -1) {
    //------------------------------------------------------ get timingAdvance
    writeData(start_Bytes2, sizeof(start_Bytes2), TIMING_ADVANCE);
    readData();

    if (resultBuffer[10] == TIMING_ADVANCE) {
      TIMINGADVANCE = (resultBuffer[11] / 2) - 64;
    }
    wsSend();

    //------------------------------------------------------ get engineLoad
    writeData(start_Bytes2, sizeof(start_Bytes2), ENGINE_LOAD);
    readData();

    if (resultBuffer[10] == ENGINE_LOAD) {
      ENGINELOAD = resultBuffer[11] / 2.55;
    }
    wsSend();

    //------------------------------------------------------ get mafSensor
    writeData(start_Bytes2, sizeof(start_Bytes2), MAF_FLOW_RATE);
    readData();

    if (resultBuffer[10] == MAF_FLOW_RATE) {
      MAF = (256 * resultBuffer[11] + resultBuffer[12]) / 100;
    }
    wsSend();



    //------------------------------------------------------ get speed
    writeData(start_Bytes2, sizeof(start_Bytes2), VEHICLE_SPEED);
    readData();

    if (resultBuffer[10] == VEHICLE_SPEED) {
      SPEED = resultBuffer[11];
    }
    wsSend();

    //------------------------------------------------------ get rpm
    writeData(start_Bytes2, sizeof(start_Bytes2), ENGINE_RPM);
    readData();

    if (resultBuffer[10] == ENGINE_RPM) {
      RPM = (resultBuffer[11] * 256 + resultBuffer[12]) / 4;
    }
    wsSend();

    //------------------------------------------------------ get throttle
    writeData(start_Bytes2, sizeof(start_Bytes2), THROTTLE_POSITION);
    readData();

    if (resultBuffer[10] == THROTTLE_POSITION) {
      THROTTLE = resultBuffer[11] * 100 / 255;
      // THROTTLE = resultBuffer[11] * 100 / 180 - 14;
    }
    wsSend();

    //------------------------------------------------------ get Coolant temp
    writeData(start_Bytes2, sizeof(start_Bytes2), ENGINE_COOLANT_TEMP);
    readData();

    if (resultBuffer[10] == ENGINE_COOLANT_TEMP) {
      COOLANT_TEMP = resultBuffer[11] - 40;
    }
    wsSend();

    //------------------------------------------------------ get Intake temp
    writeData(start_Bytes2, sizeof(start_Bytes2), INTAKE_AIR_TEMP);
    readData();

    if (resultBuffer[10] == INTAKE_AIR_TEMP) {
      INTAKE_TEMP = resultBuffer[11] - 40;
    }
    wsSend();

    //------------------------------------------------------ get voltage
    // writeData(start_Bytes2, sizeof(start_Bytes2), CONTROL_MODULE_VOLTAGE);
    // readData();

    // if (resultBuffer[10] == CONTROL_MODULE_VOLTAGE) {
    //   VOLTAGE = (resultBuffer[11] * 256 + resultBuffer[12]) / 1000;
    // }
    // wsSend();
  }
}

void read_DTC() {
  writeData(start_Bytes1, sizeof(start_Bytes1), read_DTCs);
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    memset(resultBuffer, 0, sizeof(resultBuffer));
    for (int i = 0; i < result; i++) {
      resultBuffer[i] = K_Serial.read();
      delay(READ_DELAY);
    }
    int length = result - 10;
    for (int i = 0; i < length; i++) {
      int index1 = 9 + i * 2;
      int index2 = 9 + i * 2 + 1;
      dtcBytes[0] = resultBuffer[index1];
      dtcBytes[1] = resultBuffer[index2];
      delay(READ_DELAY);

      if (dtcBytes[0] == 0 && dtcBytes[1] == 0) {
        return;
      } else {
        decodeDTC(dtcBytes[0], dtcBytes[1]);
      }
    }
  }
}

void clear_DTC() {
  writeData(start_Bytes1, sizeof(start_Bytes1), clear_DTCs);
}

bool init_KWP() {
  K_Serial.end();

  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(25);
  digitalWrite(K_line_TX, HIGH), delay(25);

  K_Serial.begin(10400, SERIAL_8N1);
  writeData(start_Bytes1, sizeof(start_Bytes1), init_OBD);
  delay(REQUEST_DELAY);
  result = K_Serial.available();
  if (result > 0) {
    memset(initBuffer, 0, sizeof(initBuffer));
    int bytesRead = min(result, 20);
    for (int i = 0; i < bytesRead; i++) {
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

void writeData(const byte data[], int length, byte pid) {
  byte extendedData[length + 1];
  for (int i = 0; i < length; ++i) {
    extendedData[i] = data[i];
  }
  extendedData[length] = pid;

  byte checksum = calculateChecksum(extendedData, length + 1);
  K_Serial.write(extendedData, length + 1);
  K_Serial.write(checksum);
}

void readData() {
  delay(REQUEST_DELAY);
  memset(resultBuffer, 0, sizeof(resultBuffer));
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
