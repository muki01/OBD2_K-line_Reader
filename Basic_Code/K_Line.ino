byte resultBuffer[64];
String dtcBuffer[32];
byte supportedLiveData[32];
byte supportedFreezeFrame[32];
byte supportedVehicleInfo[32];

void obdTask() {
  Serial.println();
  Serial.println("Live Data: ");
  getPID(VEHICLE_SPEED);
  Serial.print("Speed: "), Serial.println(vehicleSpeedValue);
  getPID(ENGINE_RPM);
  Serial.print("Engine RPM: "), Serial.println(engineRpmValue);
  getPID(ENGINE_COOLANT_TEMP);
  Serial.print("Coolant Temp: "), Serial.println(engineCoolantTemp);
  getPID(INTAKE_AIR_TEMP);
  Serial.print("Intake Air Temp: "), Serial.println(intakeAirTempValue);
  getPID(THROTTLE_POSITION);
  Serial.print("Throttle: "), Serial.println(throttlePositionValue);
  getPID(TIMING_ADVANCE);
  Serial.print("Timing Advance: "), Serial.println(timingAdvanceValue);
  getPID(ENGINE_LOAD);
  Serial.print("Engine Load: "), Serial.println(engineLoadValue);
  getPID(MAF_FLOW_RATE);
  Serial.print("MAF Flow Rate: "), Serial.println(mafAirFlowRate);
  Serial.println();

  Serial.println("DTCs: ");
  get_DTCs();
  for (int i = 0; i < 32; i++) {
    Serial.print(dtcBuffer[i]);
    Serial.print(", ");
  }
  Serial.println();
  Serial.println();

  Serial.println("Freeze Frame: ");
  getFreezeFrame(VEHICLE_SPEED);
  Serial.print("Speed: "), Serial.println(freeze_SPEED);
  getFreezeFrame(ENGINE_RPM);
  Serial.print("Engine RPM: "), Serial.println(freeze_RPM);
  getFreezeFrame(ENGINE_COOLANT_TEMP);
  Serial.print("Coolant Temp: "), Serial.println(freeze_COOLANT_TEMP);
  getFreezeFrame(ENGINE_LOAD);
  Serial.print("Engine Load: "), Serial.println(freeze_ENGINELOAD);
  Serial.println();

  Serial.println("Suported LiveData: ");
  getSupportedPIDs(0x01);
  for (int i = 0; i < 32; i++) {
    Serial.print(supportedLiveData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  Serial.println();

  Serial.println("Suported FreezeFrame: ");
  getSupportedPIDs(0x02);
  for (int i = 0; i < 32; i++) {
    Serial.print(supportedFreezeFrame[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  Serial.println();

  Serial.println("Suported VehicleInfo: ");
  getSupportedPIDs(0x09);
  for (int i = 0; i < 32; i++) {
    Serial.print(supportedVehicleInfo[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  Serial.println();
}


bool init_OBD2() {
  // Request: C1 33 F1 81 66
  // Response: 83 F1 11 C1 8F EF C4

  if (protocol == "Automatic" || protocol == "ISO14230_Slow" || protocol == "ISO9141") {
    Serial.println("Trying ISO9141 or ISO14230_Slow");
    K_Serial.end();
    pinMode(K_line_RX, INPUT_PULLUP);
    pinMode(K_line_TX, OUTPUT);
    digitalWrite(K_line_TX, HIGH), delay(3000);
    send5baud(0x33);

    begin_K_Serial();
    if (readData()) {
      if (resultBuffer[0] == 0x55) {
        if (resultBuffer[1] == resultBuffer[2]) {
          Serial.println("Your Protocol is ISO9141");
          protocol = "ISO9141";
        } else {
          Serial.println("Your Protocol is ISO14230_Slow");
          protocol = "ISO14230_Slow";
        }
        Serial.println("Writing KW2 Reversed");
        K_Serial.write(~resultBuffer[2]);  //0xF7

        if (readData()) {
          if (resultBuffer[0]) {
            return true;
          } else {
            Serial.println("No Data Retrieved from Car");
          }
        }
      }
    }
  }

  if (protocol == "Automatic" || protocol == "ISO14230_Fast") {
    Serial.println("Trying ISO14230_Fast");
    K_Serial.end();
    pinMode(K_line_RX, INPUT_PULLUP);
    pinMode(K_line_TX, OUTPUT);
    digitalWrite(K_line_TX, HIGH), delay(3000);
    digitalWrite(K_line_TX, LOW), delay(25);
    digitalWrite(K_line_TX, HIGH), delay(25);

    begin_K_Serial();
    writeData(init_OBD, 0x00);
    if (readData()) {
      if (resultBuffer[3] == 0xC1) {
        Serial.println("Your Protocol is ISO14230_Fast");
        protocol = "ISO14230_Fast";
        return true;
      }
    }
  }

  Serial.println("No Protocol Found");
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
  Serial.print("5 Baud Init for Module 0x");
  Serial.print(data, HEX);
  Serial.print(": ");
  for (int i = 0; i < 10; i++) {
    Serial.print(bits[i]);
    digitalWrite(K_line_TX, bits[i] ? HIGH : LOW);
    delay(200);
  }
  Serial.println();
}

void writeData(const byte mode, const byte pid) {
  Serial.println("Writing Data");
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
  Serial.println("Reading...");
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait up to 1 second for the first byte
  while (millis() - startMillis < 1000) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));
      errors = 0;

      // Inner loop: Read all data
      Serial.print("Received Data: ");
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          resultBuffer[bytesRead] = K_Serial.read();
          Serial.print(resultBuffer[bytesRead], HEX);
          Serial.print(" ");
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          // If buffer is full, stop reading and print message
          if (bytesRead >= sizeof(resultBuffer)) {
            Serial.println("\nBuffer is full. Stopping data reception.");
            return true;
          }
        }
      }

      // If no new data is received within 60ms, exit the loop
      Serial.println("\nData reception completed.");
      return true;
    }
  }

  // If no data is received within 1 seconds
  Serial.println("Timeout: Not Received Data.");
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
    Serial.print("Cleared Echo Data: ");
    for (int i = 0; i < result; i++) {
      Serial.print(K_Serial.read(), HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("Not Received Echo Data");
  }
}

void getPID(const byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  writeData(read_LiveData, pid);

  if (readData()) {
    if (resultBuffer[4] == pid) {
      if (pid == FUEL_SYSTEM_STATUS)
        fuelSystemStatus = resultBuffer[5];

      if (pid == ENGINE_LOAD)
        engineLoadValue = (100.0 / 255) * resultBuffer[5];

      if (pid == ENGINE_COOLANT_TEMP)
        engineCoolantTemp = resultBuffer[5] - 40;

      if (pid == SHORT_TERM_FUEL_TRIM_BANK_1)
        shortTermFuelTrimBank1 = (resultBuffer[5] / 1.28) - 100.0;

      if (pid == LONG_TERM_FUEL_TRIM_BANK_1)
        longTermFuelTrimBank1 = (resultBuffer[5] / 1.28) - 100.0;

      if (pid == SHORT_TERM_FUEL_TRIM_BANK_2)
        shortTermFuelTrimBank2 = (resultBuffer[5] / 1.28) - 100.0;

      if (pid == LONG_TERM_FUEL_TRIM_BANK_2)
        longTermFuelTrimBank2 = (resultBuffer[5] / 1.28) - 100.0;

      if (pid == FUEL_PRESSURE)
        fuelPressureValue = 3 * resultBuffer[5];

      if (pid == INTAKE_MANIFOLD_ABS_PRESSURE)
        intakeManifoldAbsPressure = resultBuffer[5];

      if (pid == ENGINE_RPM)
        engineRpmValue = (256 * resultBuffer[5] + resultBuffer[6]) / 4;

      if (pid == VEHICLE_SPEED)
        vehicleSpeedValue = resultBuffer[5];

      if (pid == TIMING_ADVANCE)
        timingAdvanceValue = (resultBuffer[5] / 2) - 64;

      if (pid == INTAKE_AIR_TEMP)
        intakeAirTempValue = resultBuffer[5] - 40;

      if (pid == MAF_FLOW_RATE)
        mafAirFlowRate = (256 * resultBuffer[5] + resultBuffer[6]) / 100.0;

      if (pid == THROTTLE_POSITION)
        throttlePositionValue = (100.0 / 255) * resultBuffer[5];

      if (pid == COMMANDED_SECONDARY_AIR_STATUS)
        secondaryAirStatus = resultBuffer[5];

      if (pid == OXYGEN_SENSORS_PRESENT_2_BANKS)
        oxygenSensorsPresent2Banks = resultBuffer[5];

      if (pid == OXYGEN_SENSOR_1_A) {
        oxygenSensor1Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim1 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_2_A) {
        oxygenSensor2Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim2 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_3_A) {
        oxygenSensor3Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim3 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_4_A) {
        oxygenSensor4Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim4 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_5_A) {
        oxygenSensor5Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim5 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_6_A) {
        oxygenSensor6Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim6 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_7_A) {
        oxygenSensor7Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim7 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OXYGEN_SENSOR_8_A) {
        oxygenSensor8Voltage = resultBuffer[5] / 200.0;
        shortTermFuelTrim8 = (100.0 / 128) * resultBuffer[6] - 100.0;
      }

      if (pid == OBD_STANDARDS)
        obdStandards = resultBuffer[5];

      if (pid == OXYGEN_SENSORS_PRESENT_4_BANKS)
        oxygenSensorsPresent4Banks = resultBuffer[5];

      if (pid == AUX_INPUT_STATUS)
        auxiliaryInputStatus = resultBuffer[5];

      if (pid == RUN_TIME_SINCE_ENGINE_START)
        runTimeSinceEngineStart = 256 * resultBuffer[5] + resultBuffer[6];

      if (pid == DISTANCE_TRAVELED_WITH_MIL_ON)
        distanceWithMilOn = 256 * resultBuffer[5] + resultBuffer[6];
    }
  }
}

void getFreezeFrame(const byte pid) {
  // example Request: C3 33 F1 02 05 00 EE
  // example Response: 84 F1 11 42 05 00 8A 57
  writeData(read_FreezeFrame, pid);

  if (readData()) {
    if (resultBuffer[4] == pid) {
      if (pid == VEHICLE_SPEED)
        freeze_SPEED = resultBuffer[6];
      if (pid == ENGINE_RPM)
        freeze_RPM = (resultBuffer[6] * 256 + resultBuffer[7]) / 4;
      if (pid == ENGINE_COOLANT_TEMP)
        freeze_COOLANT_TEMP = resultBuffer[6] - 40;
      if (pid == ENGINE_LOAD)
        freeze_ENGINELOAD = resultBuffer[6] / 2.55;
    }
  }
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
  writeData(clear_DTCs, 0x00);
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

  writeData(read_VehicleInfo, read_ID_Num_Length);

  if (readData()) {
    ID_messageCount = resultBuffer[5];

    writeData(read_VehicleInfo, read_ID_Num);

    if (readData()) {
      for (int j = 0; j < ID_messageCount; j++) {
        for (int i = 1; i <= 4; i++) {
          IDNum_Array[arrayNum++] = resultBuffer[i + 5 + j * 11];
        }
      }
    }
  }

  Vehicle_ID_Num = convertBytesToHexString(IDNum_Array, arrayNum);
}

void getSupportedPIDs(const byte option) {
  int pidIndex = 0;
  int supportedCount = 0;

  if (option == 0x01) {
    writeData(read_LiveData, SUPPORTED_PIDS_1_20);

    if (readData()) {

      for (int i = 5; i < 9; i++) {
        byte value = resultBuffer[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedLiveData[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }

    if (isInArray(supportedLiveData, sizeof(supportedLiveData), 0x20)) {
      writeData(read_LiveData, SUPPORTED_PIDS_21_40);

      if (readData()) {
        for (int i = 5; i < 9; i++) {
          byte value = resultBuffer[i];
          for (int bit = 7; bit >= 0; bit--) {
            if ((value >> bit) & 1) {
              supportedLiveData[supportedCount++] = pidIndex + 1;
            }
            pidIndex++;
          }
        }
      }
    }

    if (isInArray(supportedLiveData, sizeof(supportedLiveData), 0x40)) {
      writeData(read_LiveData, SUPPORTED_PIDS_41_60);

      if (readData()) {
        for (int i = 5; i < 9; i++) {
          byte value = resultBuffer[i];
          for (int bit = 7; bit >= 0; bit--) {
            if ((value >> bit) & 1) {
              supportedLiveData[supportedCount++] = pidIndex + 1;
            }
            pidIndex++;
          }
        }
      }
    }
  }
  if (option == 0x02) {
    writeData(read_FreezeFrame, SUPPORTED_PIDS_1_20);

    if (readData()) {
      for (int i = 6; i < 10; i++) {
        byte value = resultBuffer[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedFreezeFrame[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }
  }
  if (option == 0x09) {
    writeData(read_VehicleInfo, supported_VehicleInfo);

    if (readData()) {
      for (int i = 6; i < 10; i++) {
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
}