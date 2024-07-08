byte resultBuffer[70];
String dtcBuffer[50];
String supportedPIDs[32];

void read_K() {
  VOLTAGE = (double)analogRead(voltagePin) / 4096 * 17.4;
  VOLTAGE = round(VOLTAGE * 10) / 10;

  if (page == 1 || page == -1) {
    getPID(VEHICLE_SPEED);
    getPID(ENGINE_RPM);
    getPID(ENGINE_COOLANT_TEMP);
    getPID(INTAKE_AIR_TEMP);
    getPID(THROTTLE_POSITION);
    getPID(TIMING_ADVANCE);
    getPID(ENGINE_LOAD);
    getPID(MAF_FLOW_RATE);
  } else if (page == 0 || page == 2 || page == 5 || page == 6) {
    if (millis() - lastDTCTime >= 1000) {
      get_DTCs();
      lastDTCTime = millis();
    }
  } else if (page == 3) {
    get_DTCs();
    getFreezeFrame(VEHICLE_SPEED);
    getFreezeFrame(ENGINE_RPM);
    getFreezeFrame(ENGINE_COOLANT_TEMP);
    getFreezeFrame(INTAKE_AIR_TEMP);
    getFreezeFrame(THROTTLE_POSITION);
    getFreezeFrame(TIMING_ADVANCE);
    getFreezeFrame(ENGINE_LOAD);
    getFreezeFrame(MAF_FLOW_RATE);
  } else if (page == 4) {
    getPID(VEHICLE_SPEED);
  }

  K_Serial.flush();
}

bool init_OBD2() {
  // Request: C1 33 F1 81 66
  // Response: 83 F1 11 C1 8F EF C4

  if (protocol == "Automatic" || protocol == "ISO14230_Slow" || protocol == "ISO9141") {
    REQUEST_DELAY = 500;
    K_Serial.end();
    digitalWrite(K_line_TX, HIGH), delay(300);
    digitalWrite(K_line_TX, LOW), delay(200);
    digitalWrite(K_line_TX, HIGH), delay(400);
    digitalWrite(K_line_TX, LOW), delay(400);
    digitalWrite(K_line_TX, HIGH), delay(400);
    digitalWrite(K_line_TX, LOW), delay(400);
    digitalWrite(K_line_TX, HIGH), delay(200);

    K_Serial.begin(10400, SERIAL_8N1);
    readData();
    if (resultBuffer[0] == 0x55) {
      if (resultBuffer[1] == resultBuffer[2]) {
        protocol = "ISO9141";
      } else {
        protocol = "ISO14230_Slow";
      }
      delay(30);
      K_Serial.write(~resultBuffer[2]);  //0xF7
      REQUEST_DELAY = 50;
      readData();
      REQUEST_DELAY = 500;

      if (resultBuffer[1]) {
        return true;
      }
    }
  }

  if (protocol == "Automatic" || protocol == "ISO14230_Fast") {
    REQUEST_DELAY = 50;
    K_Serial.end();
    digitalWrite(K_line_TX, HIGH), delay(300);
    digitalWrite(K_line_TX, LOW), delay(25);
    digitalWrite(K_line_TX, HIGH), delay(25);

    K_Serial.begin(10400, SERIAL_8N1);
    writeData(start_Bytes, sizeof(start_Bytes), init_OBD);
    readData();
    if (resultBuffer[8] == 0xC1) {
      protocol = "ISO14230_Fast";
      return true;
    }
  }

  return false;
}

void writeData(const byte data[], int length, const byte pid) {
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
    int bytesRead = min(result, int(sizeof(resultBuffer)));
    for (int i = 0; i < bytesRead; i++) {
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

void getPID(const byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  if (protocol == "ISO9141") {
    writeData(live_data_SLOW, sizeof(live_data_SLOW), pid);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(live_data, sizeof(live_data), pid);
  }
  readData();

  if (resultBuffer[10] == pid) {
    if (pid == VEHICLE_SPEED)
      SPEED = resultBuffer[11];
    if (pid == ENGINE_RPM)
      RPM = (resultBuffer[11] * 256 + resultBuffer[12]) / 4;
    if (pid == ENGINE_COOLANT_TEMP)
      COOLANT_TEMP = resultBuffer[11] - 40;
    if (pid == INTAKE_AIR_TEMP)
      INTAKE_TEMP = resultBuffer[11] - 40;
    if (pid == THROTTLE_POSITION)
      THROTTLE = resultBuffer[11] * 100 / 255;
    if (pid == TIMING_ADVANCE)
      TIMINGADVANCE = (resultBuffer[11] / 2) - 64;
    if (pid == ENGINE_LOAD)
      ENGINELOAD = resultBuffer[11] / 2.55;
    if (pid == MAF_FLOW_RATE)
      MAF = (256 * resultBuffer[11] + resultBuffer[12]) / 100;
  }
  sendDataToServer();
}

void getFreezeFrame(const byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  if (protocol == "ISO9141") {
    writeData(freeze_frame_SLOW, sizeof(freeze_frame_SLOW), pid);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(freeze_frame, sizeof(freeze_frame), pid);
  }
  readData();

  if (resultBuffer[10] == pid) {
    if (pid == VEHICLE_SPEED)
      freeze_SPEED = resultBuffer[11];
    if (pid == ENGINE_RPM)
      freeze_RPM = (resultBuffer[11] * 256 + resultBuffer[12]) / 4;
    if (pid == ENGINE_COOLANT_TEMP)
      freeze_COOLANT_TEMP = resultBuffer[11] - 40;
    if (pid == INTAKE_AIR_TEMP)
      freeze_INTAKE_TEMP = resultBuffer[11] - 40;
    if (pid == THROTTLE_POSITION)
      freeze_THROTTLE = resultBuffer[11] * 100 / 255;
    if (pid == TIMING_ADVANCE)
      freeze_TIMINGADVANCE = (resultBuffer[11] / 2) - 64;
    if (pid == ENGINE_LOAD)
      freeze_ENGINELOAD = resultBuffer[11] / 2.55;
    if (pid == MAF_FLOW_RATE)
      freeze_MAF = (256 * resultBuffer[11] + resultBuffer[12]) / 100;
  }
  sendDataToServer();
}

void get_DTCs() {
  // Request: C2 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcs = 0;
  char dtcBytes[2];

  if (protocol == "ISO9141") {
    writeData(start_Bytes_SLOW, sizeof(start_Bytes_SLOW), read_DTCs);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(start_Bytes, sizeof(start_Bytes), read_DTCs);
  }
  readData();

  int count = getArrayLength(resultBuffer);
  int length = count - 10;
  for (int i = 0; i < length; i++) {
    dtcBytes[0] = resultBuffer[9 + i * 2];
    dtcBytes[1] = resultBuffer[9 + i * 2 + 1];
    delay(READ_DELAY);

    if (dtcBytes[0] == 0 && dtcBytes[1] == 0) {
      break;
    } else {
      String ErrorCode = decodeDTC(dtcBytes[0], dtcBytes[1]);
      dtcBuffer[dtcs++] = ErrorCode;
    }
  }
  sendDataToServer();
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
  if (protocol == "ISO9141") {
    writeData(start_Bytes_SLOW, sizeof(start_Bytes_SLOW), clear_DTCs);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(start_Bytes, sizeof(start_Bytes), clear_DTCs);
  }
}

void getVIN() {
  // Request: C2 33 F1 09 02 F1
  // example Response: 87 F1 11 49 02 01 00 00 00 31 06
  //                   87 F1 11 49 02 02 41 31 4A 43 D5
  //                   87 F1 11 49 02 03 35 34 34 34 A8
  //                   87 F1 11 49 02 04 52 37 32 35 C8

  byte VIN_Array[17];
  int arrayNum = 0;

  if (protocol == "ISO9141") {
    writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_VIN);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(vehicle_info, sizeof(vehicle_info), read_VIN);
  }

  delay(200);
  readData();

  if (resultBuffer[11] == 0x01) {
    VIN_Array[arrayNum++] = resultBuffer[15];
    for (int j = 0; j < 4; j++) {
      for (int i = 1; i <= 4; i++) {
        VIN_Array[arrayNum++] = resultBuffer[i + 22 + j * 11];
      }
    }
  }

  Vehicle_VIN = convertHexToAscii(VIN_Array, sizeof(VIN_Array));
}

void getCalibrationID() {
  // Request: C2 33 F1 09 04 F3
  // example Response: 87 F1 11 49 04 01 4F 32 43 44 DF
  //                   87 F1 11 49 04 02 31 30 31 41 AB
  //                   87 F1 11 49 04 03 00 00 00 00 D9
  //                   87 F1 11 49 04 04 00 00 00 00 DA

  byte ID_Array[64];
  int ID_messageCount;
  int arrayNum = 0;

  if (protocol == "ISO9141") {
    writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID_Length);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(vehicle_info, sizeof(vehicle_info), read_ID_Length);
  }
  delay(200);
  readData();
  ID_messageCount = resultBuffer[11];

  if (protocol == "ISO9141") {
    writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(vehicle_info, sizeof(vehicle_info), read_ID);
  }
  delay(200);
  readData();

  if (resultBuffer[11] == 0x01) {
    for (int j = 0; j < ID_messageCount; j++) {
      for (int i = 1; i <= 4; i++) {
        ID_Array[arrayNum++] = resultBuffer[i + 11 + j * 11];
      }
    }
  }
  Vehicle_ID = convertHexToAscii(ID_Array, arrayNum);
}

void getCalibrationIDNum() {
  // Request: C2 33 F1 09 06 F5
  // example Response: 87 F1 11 49 06 01 00 00 67 0C 4C

  byte IDNum_Array[16];
  int ID_messageCount;
  int arrayNum = 0;

  if (protocol == "ISO9141") {
    writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID_Num_Length);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(vehicle_info, sizeof(vehicle_info), read_ID_Num_Length);
  }
  delay(200);
  readData();
  ID_messageCount = resultBuffer[11];

  if (protocol == "ISO9141") {
    writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID_Num);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(vehicle_info, sizeof(vehicle_info), read_ID_Num);
  }
  delay(200);
  readData();

  if (resultBuffer[11] == 0x01) {
    for (int j = 0; j < ID_messageCount; j++) {
      for (int i = 1; i <= 4; i++) {
        IDNum_Array[arrayNum++] = resultBuffer[i + 11 + j * 11];
      }
    }
  }
  Vehicle_ID_Num = convertBytesToHexString(IDNum_Array, arrayNum);
}

void getSupportedPIDs() {
  int pidIndex = 0;
  int supportedCount = 0;

  if (protocol == "ISO9141") {
    writeData(live_data_SLOW, sizeof(live_data_SLOW), SUPPORTED_PIDS_1_20);
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    writeData(live_data, sizeof(live_data), SUPPORTED_PIDS_1_20);
  }
  readData();

  for (int i = 11; i < 15; i++) {
    byte value = resultBuffer[i];
    for (int bit = 7; bit >= 0; bit--) {
      if ((value >> bit) & 1) {
        String pidString = String(pidIndex + 1, HEX);
        pidString.toUpperCase();

        if (pidString.length() == 1) {
          pidString = "0" + pidString;
        }
        supportedPIDs[supportedCount++] = pidString;
      }
      pidIndex++;
    }
  }

  if (isInArray(supportedPIDs, sizeof(supportedPIDs), "20")) {
    if (protocol == "ISO9141") {
      writeData(live_data_SLOW, sizeof(live_data_SLOW), SUPPORTED_PIDS_21_40);
    } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
      writeData(live_data, sizeof(live_data), SUPPORTED_PIDS_21_40);
    }
    readData();

    for (int i = 11; i < 15; i++) {
      byte value = resultBuffer[i];
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          String pidString = String(pidIndex + 1, HEX);
          pidString.toUpperCase();

          if (pidString.length() == 1) {
            pidString = "0" + pidString;
          }
          supportedPIDs[supportedCount++] = pidString;
        }
        pidIndex++;
      }
    }
  }
}
