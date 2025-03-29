byte resultBuffer[70];
String dtcBuffer[32];
byte supportedLiveData[32];
byte desiredLiveData[32];
byte supportedFreezeFrame[32];
byte supportedVehicleInfo[32];

void read_K() {
  if (page == 1 || page == -1) {
    for (const auto& mapping : liveDataMappings) {
      if (isInArray(desiredLiveData, sizeof(desiredLiveData), mapping.pid)) {
        getPID(mapping.pid);
      }
    }
    if (page == -1) {
      get_DTCs();
    }
  } else if (page == 0 || page == 2 || page == 5 || page == 6) {
    if (millis() - lastDTCTime >= 1000) {
      get_DTCs();
      if (page == 2) {
        if (isInArray(supportedLiveData, sizeof(supportedLiveData), DISTANCE_TRAVELED_WITH_MIL_ON)) {
          getPID(DISTANCE_TRAVELED_WITH_MIL_ON);
        }
      }
      lastDTCTime = millis();
    }
  } else if (page == 3) {
    get_DTCs();
    for (const auto& mapping : freezeFrameMappings) {
      if (isInArray(supportedFreezeFrame, sizeof(supportedFreezeFrame), mapping.pid)) {
        getPID(mapping.pid);
      }
    }
  } else if (page == 4) {
    if (isInArray(supportedLiveData, sizeof(supportedLiveData), VEHICLE_SPEED)) {
      getPID(VEHICLE_SPEED);
    }
  }

  K_Serial.flush();
}

bool init_OBD2() {
  // Request: C1 33 F1 81 66
  // Response: 83 F1 11 C1 8F EF C4

  if (protocol == "Automatic" || protocol == "ISO14230_Slow" || protocol == "ISO9141") {
    debugPrintln("Trying ISO9141 or ISO14230_Slow");
    REQUEST_DELAY = 500;
    K_Serial.end();
    digitalWrite(K_line_TX, HIGH), delay(3000);
    send5baud(0x33);

#ifdef ESP32
    K_Serial.begin(10400, SERIAL_8N1, K_line_RX, K_line_TX);
#elif defined(ESP8266)
    K_Serial.begin(10400, SERIAL_8N1);
#endif
    readData();
    if (resultBuffer[0] == 0x55) {
      if (resultBuffer[1] == resultBuffer[2]) {
        debugPrintln("Your Protocol is ISO9141");
        protocol = "ISO9141";
      } else {
        debugPrintln("Your Protocol is ISO14230_Slow");
        protocol = "ISO14230_Slow";
      }
      delay(30);
      debugPrintln("Writing KW2 Reversed");
      K_Serial.write(~resultBuffer[2]);  //0xF7
      REQUEST_DELAY = 50;
      readData();
      REQUEST_DELAY = 500;

      if (resultBuffer[1]) {
        return true;
      }else{
        debugPrintln("No Data Retrieved from Car");
      }
    }
  }

  if (protocol == "Automatic" || protocol == "ISO14230_Fast") {
    debugPrintln("Trying ISO14230_Fast");
    REQUEST_DELAY = 50;
    K_Serial.end();
    digitalWrite(K_line_TX, HIGH), delay(3000);
    digitalWrite(K_line_TX, LOW), delay(25);
    digitalWrite(K_line_TX, HIGH), delay(25);

#ifdef ESP32
    K_Serial.begin(10400, SERIAL_8N1, K_line_RX, K_line_TX);
#elif defined(ESP8266)
    K_Serial.begin(10400, SERIAL_8N1);
#endif
    writeData(start_Bytes, sizeof(start_Bytes), init_OBD);
    readData();
    if (resultBuffer[8] == 0xC1) {
      debugPrintln("Your Protocol is ISO14230_Fast");
      protocol = "ISO14230_Fast";
      return true;
    }
  }

  debugPrintln("No Protocol Found");
  return false;
}

void send5baud(uint8_t data) {
  byte even = 1;  // Variable to calculate the parity bit
  byte bits[10];

  bits[0] = 0;  // Start bit
  bits[9] = 1;  // Stop bit

  // Calculate 7-bit data and parity
  for (int i = 1; i <= 7; i++) {
    bits[i] = (data >> (i - 1)) & 1;
    even ^= bits[i];  // Update parity
  }

  bits[8] = (even == 0) ? 1 : 0;  // Set parity bit

  // Transmit bits sequentially (5 baud = 1 bit per 200 ms)
  debugPrint("5 Baud Init for Module 0x");
  debugPrintHex(data);
  debugPrint(": ");
  for (int i = 0; i < 10; i++) {
    debugPrint(bits[i]);
    digitalWrite(K_line_TX, bits[i] ? HIGH : LOW);
    delay(200);
  }
  debugPrintln();
}

void writeData(const byte mode, const byte pid) {
  debugPrintln("Writing Data");
  byte message[7] = { 0 };
  size_t length = (mode == read_FreezeFrame) ? 7 : (mode == init_OBD || mode == read_DTCs || mode == clear_DTCs) ? 5
                                                                                                                 : 6;

  if (protocol == "ISO9141") {
    message[0] = (mode == read_FreezeFrame) ? 0x69 : 0x68;
    message[1] = 0x6A;
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    message[0] = (mode == read_FreezeFrame) ? 0xC3 : (mode == init_OBD || mode == read_DTCs || mode == clear_DTCs) ? 0xC1
                                                                                                                   : 0xC2;
    message[1] = 0x33;
  }

  message[2] = 0xF1;
  message[3] = mode;
  if (length > 5) message[4] = pid;
  if (length == 7) message[5] = 0x00;

  message[length - 1] = calculateChecksum(message, length - 1);

  for (size_t i = 0; i < length; i++) {
    K_Serial.write(message[i]);
    delay(WRITE_DELAY);
  }

  clearEcho();
}

bool readData() {
  debugPrintln("Reading...");
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait up to 1 second for the first byte
  while (millis() - startMillis < 1000) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));
      errors = 0;

      // Inner loop: Read all data
      debugPrint("Received Data: ");
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          resultBuffer[bytesRead] = K_Serial.read();
          debugPrintHex(resultBuffer[bytesRead]);
          debugPrint(" ");
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          // If buffer is full, stop reading and print message
          if (bytesRead >= sizeof(resultBuffer)) {
            debugPrintln("\nBuffer is full. Stopping data reception.");
            return true;
          }
        }
      }

      // If no new data is received within 60ms, exit the loop
      debugPrintln("\nData reception completed.");
      return true;
    }
  }

  // If no data is received within 1 seconds
  debugPrintln("Timeout: Not Received Data.");
  errors++;
  if (errors > 2) {
    errors = 0;
    if (KLineStatus) {
      KLineStatus = false;
    }
  }
  return false;
}

void clearEcho() {
  int result = K_Serial.available();
  if (result > 0) {
    debugPrint("Cleared Echo Data: ");
    for (int i = 0; i < result; i++) {
      debugPrintHex(K_Serial.read());
      debugPrint(" ");
    }
    debugPrintln();
  } else {
    debugPrintln("Not Received Echo Data");
  }
}

void getPID(const byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  writeData(read_LiveData, pid);

  if (readData()) {
    if (resultBuffer[4] == pid) {
      for (int i = 0; i < 64; i++) {
        if (liveDataMappings[i].pid == pid) {

          if (pid == FUEL_SYSTEM_STATUS) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == ENGINE_LOAD) {
            liveDataMappings[i].value = (100.0 / 255) * resultBuffer[5];
          } else if (pid == ENGINE_COOLANT_TEMP) {
            liveDataMappings[i].value = resultBuffer[5] - 40;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_1) {
            liveDataMappings[i].value = (resultBuffer[5] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_1) {
            liveDataMappings[i].value = (resultBuffer[5] / 1.28) - 100.0;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_2) {
            liveDataMappings[i].value = (resultBuffer[5] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_2) {
            liveDataMappings[i].value = (resultBuffer[5] / 1.28) - 100.0;
          } else if (pid == FUEL_PRESSURE) {
            liveDataMappings[i].value = 3 * resultBuffer[5];
          } else if (pid == INTAKE_MANIFOLD_ABS_PRESSURE) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == ENGINE_RPM) {
            liveDataMappings[i].value = (256 * resultBuffer[5] + resultBuffer[6]) / 4;
          } else if (pid == VEHICLE_SPEED) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == TIMING_ADVANCE) {
            liveDataMappings[i].value = (resultBuffer[5] / 2) - 64;
          } else if (pid == INTAKE_AIR_TEMP) {
            liveDataMappings[i].value = resultBuffer[5] - 40;
          } else if (pid == MAF_FLOW_RATE) {
            liveDataMappings[i].value = (256 * resultBuffer[5] + resultBuffer[6]) / 100.0;
          } else if (pid == THROTTLE_POSITION) {
            liveDataMappings[i].value = (100.0 / 255) * resultBuffer[5];
          } else if (pid == COMMANDED_SECONDARY_AIR_STATUS) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == OXYGEN_SENSORS_PRESENT_2_BANKS) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == OXYGEN_SENSOR_1_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim1 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_2_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim2 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_3_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim3 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_4_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim4 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_5_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim5 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_6_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim6 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_7_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim7 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OXYGEN_SENSOR_8_A) {
            liveDataMappings[i].value = resultBuffer[5] / 200.0;
            shortTermFuelTrim8 = (100.0 / 128) * resultBuffer[6] - 100.0;
          } else if (pid == OBD_STANDARDS) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == OXYGEN_SENSORS_PRESENT_4_BANKS) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == AUX_INPUT_STATUS) {
            liveDataMappings[i].value = resultBuffer[5];
          } else if (pid == RUN_TIME_SINCE_ENGINE_START) {
            liveDataMappings[i].value = 256 * resultBuffer[5] + resultBuffer[6];
          } else if (pid == DISTANCE_TRAVELED_WITH_MIL_ON) {
            liveDataMappings[i].value = 256 * resultBuffer[5] + resultBuffer[6];
          }
        }
      }
    }
  }

  sendDataToServer();
}

void getFreezeFrame(const byte pid) {
  // example Request: C3 33 F1 02 05 00 EE
  // example Response: 84 F1 11 42 05 00 8A 57

  writeData(read_FreezeFrame, pid);

  if (readData()) {
    if (resultBuffer[4] == pid) {
      for (int i = 0; i < 64; i++) {
        if (freezeFrameMappings[i].pid == pid) {
          if (pid == FUEL_SYSTEM_STATUS) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == ENGINE_LOAD) {
            freezeFrameMappings[i].value = (100.0 / 255) * resultBuffer[6];
          } else if (pid == ENGINE_COOLANT_TEMP) {
            freezeFrameMappings[i].value = resultBuffer[6] - 40;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_1) {
            freezeFrameMappings[i].value = (resultBuffer[6] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_1) {
            freezeFrameMappings[i].value = (resultBuffer[6] / 1.28) - 100.0;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_2) {
            freezeFrameMappings[i].value = (resultBuffer[6] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_2) {
            freezeFrameMappings[i].value = (resultBuffer[6] / 1.28) - 100.0;
          } else if (pid == FUEL_PRESSURE) {
            freezeFrameMappings[i].value = 3 * resultBuffer[6];
          } else if (pid == INTAKE_MANIFOLD_ABS_PRESSURE) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == ENGINE_RPM) {
            freezeFrameMappings[i].value = (256 * resultBuffer[6] + resultBuffer[7]) / 4;
          } else if (pid == VEHICLE_SPEED) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == TIMING_ADVANCE) {
            freezeFrameMappings[i].value = (resultBuffer[6] / 2) - 64;
          } else if (pid == INTAKE_AIR_TEMP) {
            freezeFrameMappings[i].value = resultBuffer[6] - 40;
          } else if (pid == MAF_FLOW_RATE) {
            freezeFrameMappings[i].value = (256 * resultBuffer[6] + resultBuffer[7]) / 100.0;
          } else if (pid == THROTTLE_POSITION) {
            freezeFrameMappings[i].value = (100.0 / 255) * resultBuffer[6];
          } else if (pid == COMMANDED_SECONDARY_AIR_STATUS) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == OXYGEN_SENSORS_PRESENT_2_BANKS) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == OXYGEN_SENSOR_1_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim1 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_2_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim2 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_3_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim3 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_4_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim4 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_5_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim5 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_6_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim6 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_7_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim7 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OXYGEN_SENSOR_8_A) {
            freezeFrameMappings[i].value = resultBuffer[6] / 200.0;
            shortTermFuelTrim8 = (100.0 / 128) * resultBuffer[7] - 100.0;
          } else if (pid == OBD_STANDARDS) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == OXYGEN_SENSORS_PRESENT_4_BANKS) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == AUX_INPUT_STATUS) {
            freezeFrameMappings[i].value = resultBuffer[6];
          } else if (pid == RUN_TIME_SINCE_ENGINE_START) {
            freezeFrameMappings[i].value = 256 * resultBuffer[6] + resultBuffer[7];
          } else if (pid == DISTANCE_TRAVELED_WITH_MIL_ON) {
            freezeFrameMappings[i].value = 256 * resultBuffer[6] + resultBuffer[7];
          }
        }
      }
    }
  }

  sendDataToServer();
}

void get_DTCs() {
  // Request: C2 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcs = 0;
  char dtcBytes[2];

  writeData(read_DTCs, 0x00);

  if (readData()) {
    int length = sizeof(resultBuffer);
    for (int i = 0; i < length; i++) {
      dtcBytes[0] = resultBuffer[4 + i * 2];
      dtcBytes[1] = resultBuffer[4 + i * 2 + 1];

      if (dtcBytes[0] == 0 && dtcBytes[1] == 0) {
        break;
      } else {
        String ErrorCode = decodeDTC(dtcBytes[0], dtcBytes[1]);
        dtcBuffer[dtcs++] = ErrorCode;
      }
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
  //                   87 F1 11 49 02 05 32 33 36 37 E6

  byte VIN_Array[17];
  int arrayNum = 0;

  writeData(read_VehicleInfo, read_VIN);

  if (readData()) {
    VIN_Array[arrayNum++] = resultBuffer[9];
    for (int j = 0; j < 4; j++) {
      for (int i = 1; i <= 4; i++) {
        VIN_Array[arrayNum++] = resultBuffer[i + 16 + j * 11];
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

  writeData(read_VehicleInfo, read_ID_Length);

  if (readData()) {
    ID_messageCount = resultBuffer[5];

    writeData(read_VehicleInfo, read_ID);

    if (readData()) {
      for (int j = 0; j < ID_messageCount; j++) {
        for (int i = 1; i <= 4; i++) {
          ID_Array[arrayNum++] = resultBuffer[i + 5 + j * 11];
        }
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

void getSupportedPIDs(const byte option) {
  int pidIndex = 0;
  int supportedCount = 0;

  if (option == 0x01) {
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
          supportedLiveData[supportedCount++] = pidIndex + 1;
        }
        pidIndex++;
      }
    }

    if (isInArray(supportedLiveData, sizeof(supportedLiveData), 0x20)) {
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
            supportedLiveData[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }
    memcpy(desiredLiveData, supportedLiveData, sizeof(supportedLiveData));
  }
  if (option == 0x02) {
    if (protocol == "ISO9141") {
      writeDataFreezeFrame(freeze_frame_SLOW, sizeof(freeze_frame_SLOW), 0x00);
    } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
      writeDataFreezeFrame(freeze_frame, sizeof(freeze_frame), 0x00);
    }
    readData();

    for (int i = 13; i < 17; i++) {
      byte value = resultBuffer[i];
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          supportedFreezeFrame[supportedCount++] = pidIndex + 1;
        }
        pidIndex++;
      }
    }
  }
  if (option == 0x09) {
    if (protocol == "ISO9141") {
      writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), 0x00);
    } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
      writeData(vehicle_info, sizeof(vehicle_info), 0x00);
    }
    readData();

    for (int i = 12; i < 16; i++) {
      byte value = resultBuffer[i];
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          supportedVehicleInfo[supportedCount++] = pidIndex + 1;
        }
        pidIndex++;
      }
    }
  }
}
