byte resultBuffer[30];
String dtcBuffer[20];

void read_K() {
  getPID(VEHICLE_SPEED);
  getPID(ENGINE_RPM);
  getPID(ENGINE_COOLANT_TEMP);
  getPID(INTAKE_AIR_TEMP);
  getPID(THROTTLE_POSITION);
  getPID(TIMING_ADVANCE);
  getPID(ENGINE_LOAD);
  getPID(MAF_FLOW_RATE);

  writeData(start_Bytes2, sizeof(start_Bytes2), SUPPORTED_PIDS_1_20);
  readData();
  writeData(start_Bytes2, sizeof(start_Bytes2), SUPPORTED_PIDS_21_40);
  readData();
  writeData(start_Bytes2, sizeof(start_Bytes2), SUPPORTED_PIDS_41_60);
  readData();

  if (millis() - lastDTCTime >= 1000) {
    get_DTCs();
    lastDTCTime = millis();
  }
}

bool init_KWP() {
  // Request: C1 33 F1 81 66
  // Response: 83 F1 11 C1 8F EF C4
  K_Serial.end();

  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(25);
  digitalWrite(K_line_TX, HIGH), delay(25);

  K_Serial.begin(10400);
  writeData(start_Bytes1, sizeof(start_Bytes1), init_OBD);
  readData();
  if (resultBuffer[8] == 0xC1) {
    return true;
  } else {
    return false;
  }
}

bool init_KWP_slow() {
  K_Serial.end();

  digitalWrite(K_line_TX, HIGH), delay(300);
  digitalWrite(K_line_TX, LOW), delay(200);
  digitalWrite(K_line_TX, HIGH), delay(400);
  digitalWrite(K_line_TX, LOW), delay(400);
  digitalWrite(K_line_TX, HIGH), delay(400);
  digitalWrite(K_line_TX, LOW), delay(400);
  digitalWrite(K_line_TX, HIGH), delay(227);

  K_Serial.begin(10400);
  readData();
  if (resultBuffer[0] == 0x55) {
    delay(30);
    K_Serial.write(~resultBuffer[2]);  //0xF7
    readData();

    if (resultBuffer[1]) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void writeData(const byte data[], int length, const byte pid) {
  Serial.println("Writing Data");
  byte extendedData[length + 2];
  memcpy(extendedData, data, length);
  extendedData[length] = pid;
  byte checksum = calculateChecksum(extendedData, length + 1);
  extendedData[length + 1] = checksum;

  K_Serial.write(extendedData, length + 2);
}

void readData() {
  delay(REQUEST_DELAY);
  int result = K_Serial.available();
  if (result > 0) {
    memset(resultBuffer, 0, sizeof(resultBuffer));
    int bytesRead = min(result, 30);
    Serial.print("Received Data: ");
    for (int i = 0; i < bytesRead; i++) {
      resultBuffer[i] = K_Serial.read();
      Serial.print(resultBuffer[i], HEX);
      Serial.print(" ");
      delay(READ_DELAY);
    }
    Serial.println();
  }
}

byte calculateChecksum(const byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum % 256;
}

void getPID(const byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  if (protocol == 1 || protocol == 2) {
    writeData(start_Bytes4, sizeof(start_Bytes4), pid);
  } else if (protocol == 3) {
    writeData(start_Bytes2, sizeof(start_Bytes2), pid);
  }
  readData();

  if (resultBuffer[10] == pid) {
    if (pid == VEHICLE_SPEED) {
      SPEED = resultBuffer[11];
      Serial.print("Speed: ");
      Serial.println(SPEED);
    }
    if (pid == ENGINE_RPM) {
      RPM = (resultBuffer[11] * 256 + resultBuffer[12]) / 4;
      Serial.print("RPM: ");
      Serial.println(RPM);
    }
    if (pid == ENGINE_COOLANT_TEMP) {
      COOLANT_TEMP = resultBuffer[11] - 40;
      Serial.print("Coolant Temp: ");
      Serial.println(COOLANT_TEMP);
    }
    if (pid == INTAKE_AIR_TEMP) {
      INTAKE_TEMP = resultBuffer[11] - 40;
      Serial.print("Intake Temp: ");
      Serial.println(INTAKE_TEMP);
    }
    if (pid == THROTTLE_POSITION) {
      THROTTLE = resultBuffer[11] * 100 / 255;
      Serial.print("Throttle: ");
      Serial.println(THROTTLE);
    }
    if (pid == TIMING_ADVANCE) {
      TIMINGADVANCE = (resultBuffer[11] / 2) - 64;
      Serial.print("Timing Advance: ");
      Serial.println(TIMINGADVANCE);
    }
    if (pid == ENGINE_LOAD) {
      ENGINELOAD = resultBuffer[11] / 2.55;
      Serial.print("Engine Load: ");
      Serial.println(ENGINELOAD);
    }
    if (pid == MAF_FLOW_RATE) {
      MAF = (256 * resultBuffer[11] + resultBuffer[12]) / 100;
      Serial.print("MAF Flow Rate: ");
      Serial.println(MAF);
    }
  }
}

void get_DTCs() {
  // Request: C2 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcs = 0;
  char dtcBytes[2];

  if (protocol == 1 || protocol == 2) {
    writeData(start_Bytes3, sizeof(start_Bytes3), read_DTCs);
  } else if (protocol == 3) {
    writeData(start_Bytes1, sizeof(start_Bytes1), read_DTCs);
  }
  readData();

  int count = getArrayLength(resultBuffer);
  int length = count - 10;
  Serial.print("Errors: ");
  for (int i = 0; i < length; i++) {
    dtcBytes[0] = resultBuffer[9 + i * 2];
    dtcBytes[1] = resultBuffer[9 + i * 2 + 1];
    delay(READ_DELAY);

    if (dtcBytes[0] == 0 && dtcBytes[1] == 0) {
      break;
    } else {
      String ErrorCode = decodeDTC(dtcBytes[0], dtcBytes[1]);
      Serial.print(ErrorCode);
      Serial.print(" ");
      dtcBuffer[dtcs] = ErrorCode;
      dtcs++;
    }
  }
  Serial.println();
}

String decodeDTC(char input_byte1, char input_byte2) {
  String ErrorCode = "";
  const static char type_lookup[4] = { 'P', 'C', 'B', 'U' };
  const static char digit_lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  ErrorCode += type_lookup[(input_byte1 >> 6) & 0b11];
  ErrorCode += digit_lookup[(input_byte1 >> 4) & 0b11];
  ErrorCode += digit_lookup[input_byte1 & 0b1111];
  ErrorCode += digit_lookup[input_byte2 >> 4];
  ErrorCode += digit_lookup[input_byte2 & 0b1111];

  return ErrorCode;
}

void clear_DTC() {
  if (protocol == 1 || protocol == 2) {
    writeData(start_Bytes3, sizeof(start_Bytes3), clear_DTCs);
  } else if (protocol == 3) {
    writeData(start_Bytes1, sizeof(start_Bytes1), clear_DTCs);
  }
}

int getArrayLength(byte arr[]) {
  int count = 0;
  while (arr[count] != '\0') {
    count++;
  }
  return count;
}
