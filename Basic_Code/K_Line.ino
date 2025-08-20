byte resultBuffer[64];
String storedDTCBuffer[32];
String pendingDTCBuffer[32];
byte supportedLiveData[32];
byte supportedFreezeFrame[32];
byte supportedVehicleInfo[32];
byte supportedComponentMonitoring[32];

void obdTask() {
  Serial.println();
  Serial.println("Live Data: ");
  vehicleSpeedValue = getPID(readLiveData, VEHICLE_SPEED);
  Serial.print("Speed: "), Serial.println(vehicleSpeedValue);
  engineRpmValue = getPID(readLiveData, ENGINE_RPM);
  Serial.print("Engine RPM: "), Serial.println(engineRpmValue);
  engineCoolantTemp = getPID(readLiveData, ENGINE_COOLANT_TEMP);
  Serial.print("Coolant Temp: "), Serial.println(engineCoolantTemp);
  intakeAirTempValue = getPID(readLiveData, INTAKE_AIR_TEMP);
  Serial.print("Intake Air Temp: "), Serial.println(intakeAirTempValue);
  throttlePositionValue = getPID(readLiveData, THROTTLE_POSITION);
  Serial.print("Throttle: "), Serial.println(throttlePositionValue);
  timingAdvanceValue = getPID(readLiveData, TIMING_ADVANCE);
  Serial.print("Timing Advance: "), Serial.println(timingAdvanceValue);
  engineLoadValue = getPID(readLiveData, ENGINE_LOAD);
  Serial.print("Engine Load: "), Serial.println(engineLoadValue);
  mafAirFlowRate = getPID(readLiveData, MAF_FLOW_RATE);
  Serial.print("MAF Flow Rate: "), Serial.println(mafAirFlowRate);
  Serial.println();

  Serial.println("Stored DTCs: ");
  int storedDTCsLength = get_DTCs(readStoredDTCs);
  if (storedDTCsLength) {
    for (int i = 0; i < storedDTCsLength; i++) {
      Serial.print(storedDTCBuffer[i]);
      Serial.print(", ");
    }
    Serial.println();
  } else {
    Serial.println("No Stored DTCs Found.");
  }
  Serial.println();

  Serial.println("Pending DTCs: ");
  int pendingDTCsLength = get_DTCs(readPendingDTCs);
  if (pendingDTCsLength) {
    for (int i = 0; i < pendingDTCsLength; i++) {
      Serial.print(pendingDTCBuffer[i]);
      Serial.print(", ");
    }
    Serial.println();
  } else {
    Serial.println("No Pending DTCs Found.");
  }
  Serial.println();

  Serial.println("Freeze Frame: ");
  if (get_DTCs(readStoredDTCs)) {
    freeze_SPEED = getPID(readFreezeFrameData, VEHICLE_SPEED);
    Serial.print("Speed: "), Serial.println(freeze_SPEED);
    freeze_RPM = getPID(readFreezeFrameData, ENGINE_RPM);
    Serial.print("Engine RPM: "), Serial.println(freeze_RPM);
    freeze_COOLANT_TEMP = getPID(readFreezeFrameData, ENGINE_COOLANT_TEMP);
    Serial.print("Coolant Temp: "), Serial.println(freeze_COOLANT_TEMP);
    freeze_ENGINELOAD = getPID(readFreezeFrameData, ENGINE_LOAD);
    Serial.print("Engine Load: "), Serial.println(freeze_ENGINELOAD);
  } else {
    Serial.println("No Stored DTCs Found for Freeze Frame.");
  }
  Serial.println();

  Serial.println("Supported LiveData: ");
  int supportedLiveDataLength = readSupportedData(0x01);
  if (supportedLiveDataLength) {
    for (int i = 0; i < supportedLiveDataLength; i++) {
      Serial.print(supportedLiveData[i], HEX);
      Serial.print(", ");
    }
    Serial.println();
  } else {
    Serial.println("No Supported LiveData Found.");
  }
  Serial.println();

  Serial.println("Supported FreezeFrame: ");
  int supportedFreezeFrameLength = readSupportedData(0x02);
  if (supportedFreezeFrameLength) {
    for (int i = 0; i < supportedFreezeFrameLength; i++) {
      Serial.print(supportedFreezeFrame[i], HEX);
      Serial.print(", ");
    }
    Serial.println();
  } else {
    Serial.println("No Supported FreezeFrame Found.");
  }
  Serial.println();

  Serial.println("Supported VehicleInfo: ");
  int supportedVehicleInfoLength = readSupportedData(0x09);
  if (supportedVehicleInfoLength) {
    for (int i = 0; i < supportedVehicleInfoLength; i++) {
      Serial.print(supportedVehicleInfo[i], HEX);
      Serial.print(", ");
    }
    Serial.println();
  } else {
    Serial.println("No Supported VehicleInfo Found.");
  }
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
  size_t length = (mode == readFreezeFrameData || mode == testOxygenSensor) ? 7 : (mode == init_OBD || mode == readStoredDTCs || mode == clearStoredDTCs || mode == readPendingDTCs) ? 5
                                                                                                                                                                                     : 6;

  if (protocol == "ISO9141") {
    message[0] = (mode == readFreezeFrameData || mode == testOxygenSensor) ? 0x69 : 0x68;
    message[1] = 0x6A;
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    message[0] = (mode == readFreezeFrameData || mode == testOxygenSensor) ? 0xC3 : (mode == init_OBD || mode == readStoredDTCs || mode == clearStoredDTCs || mode == readPendingDTCs) ? 0xC1
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

int readData() {
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
          if (bytesRead >= sizeof(resultBuffer)) {
            Serial.println("\nBuffer is full. Stopping data reception.");
            return bytesRead;
          }

          resultBuffer[bytesRead] = K_Serial.read();
          Serial.print(resultBuffer[bytesRead], HEX);
          Serial.print(" ");
          bytesRead++;
          lastByteTime = millis();  // Reset timer
        }
      }

      // If no new data is received within 60ms, exit the loop
      Serial.println("\nData reception completed.");
      return bytesRead;
    }
  }

  // If no data is received within 1 seconds
  Serial.println("Timeout: Not Received Data.");
  errors++;
  if (errors > 2) {
    errors = 0;
    if (conectionStatus) {
      conectionStatus = false;
    }
  }
  return 0;
}

void clearEcho() {
  int result = K_Serial.available();
  if (result > 0) {
    Serial.print("Cleared Echo Data: ");
    for (int i = 0; i < result; i++) {
      byte receivedByte = K_Serial.read();
      Serial.print(receivedByte);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("Not Received Echo Data");
  }
}

float getPID(byte mode, byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  if (mode != readLiveData && mode != readFreezeFrameData) return;

  writeData(mode, pid);

  if (!readData()) return -1;             // Return if no response
  if (resultBuffer[4] != pid) return -2;  // Check if the received PID is correct

  byte A = resultBuffer[(mode == readLiveData) ? 5 : 6];
  byte B = resultBuffer[(mode == readLiveData) ? 6 : 7];


  switch (pid) {
    case 0x01:                                    // Monitor Status Since DTC Cleared (bit encoded)
    case 0x02:                                    // Monitor Status Since DTC Cleared (bit encoded)
    case 0x03:                                    // Fuel System Status (bit encoded)
      return A;                                   //
    case 0x04:                                    // Engine Load (%)
      return A * 100 / 255;                       //
    case 0x05:                                    // Coolant Temperature (°C)
      return A - 40;                              //
    case 0x06:                                    // Short Term Fuel Trim Bank 1 (%)
    case 0x07:                                    // Long Term Fuel Trim Bank 1 (%)
    case 0x08:                                    // Short Term Fuel Trim Bank 2 (%)
    case 0x09:                                    // Long Term Fuel Trim Bank 2 (%)
      return (int)((int8_t)A * 100 / 128);        //
    case 0x0A:                                    // Fuel Pressure (kPa)
      return A * 3;                               //
    case 0x0B:                                    // Intake Manifold Absolute Pressure (kPa)
      return A;                                   //
    case 0x0C:                                    // RPM
      return ((A * 256) + B) / 4;                 //
    case 0x0D:                                    // Speed (km/h)
      return A;                                   //
    case 0x0E:                                    // Timing Advance (°)
      return (int8_t)A / 2;                       //
    case 0x0F:                                    // Intake Air Temperature (°C)
      return A - 40;                              //
    case 0x10:                                    // MAF Flow Rate (grams/sec)
      return ((A * 256) + B) / 100;               //
    case 0x11:                                    // Throttle Position (%)
      return A * 100 / 255;                       //
    case 0x12:                                    // Commanded Secondary Air Status (bit encoded)
    case 0x13:                                    // Oxygen Sensors Present 2 Banks (bit encoded)
      return A;                                   //
    case 0x14:                                    // Oxygen Sensor 1A Voltage (V)
    case 0x15:                                    // Oxygen Sensor 2A Voltage (V)
    case 0x16:                                    // Oxygen Sensor 3A Voltage (V)
    case 0x17:                                    // Oxygen Sensor 4A Voltage (V)
    case 0x18:                                    // Oxygen Sensor 5A Voltage (V)
    case 0x19:                                    // Oxygen Sensor 6A Voltage (V)
    case 0x1A:                                    // Oxygen Sensor 7A Voltage (V)
    case 0x1B:                                    // Oxygen Sensor 8A Voltage (V)
      return A / 200;                             // Volt to mV (V*1000), float hesap gerekirse fonksiyon
    case 0x1C:                                    // OBD Standards This Vehicle Conforms To (bit encoded)
    case 0x1D:                                    // Oxygen Sensors Present 4 Banks (bit encoded)
    case 0x1E:                                    // Auxiliary Input Status (bit encoded)
      return A;                                   //
    case 0x1F:                                    // Run Time Since Engine Start (seconds)
    case 0x21:                                    // Distance Traveled With MIL On (km)
      return (A * 256) + B;                       //
    case 0x22:                                    // Fuel Rail Pressure (kPa)
    case 0x23:                                    // Fuel Rail Gauge Pressure (kPa)
      return ((A * 256) + B) / 10;                //
    case 0x24:                                    // Oxygen Sensor 1B (ratio voltage)
    case 0x25:                                    // Oxygen Sensor 2B
    case 0x26:                                    // Oxygen Sensor 3B
    case 0x27:                                    // Oxygen Sensor 4B
    case 0x28:                                    // Oxygen Sensor 5B
    case 0x29:                                    // Oxygen Sensor 6B
    case 0x2A:                                    // Oxygen Sensor 7B
    case 0x2B:                                    // Oxygen Sensor 8B
      return ((A * 256) + B) * 0.0000305 * 1000;  // ratio * 1000 (mV), float önerilir
    case 0x2C:                                    // Commanded EGR (%)
      return A * 100 / 255;                       //
    case 0x2D:                                    // EGR Error (%)
      return (int8_t)A * 100 / 128;               //
    case 0x2E:                                    // Commanded Evaporative Purge (%)
    case 0x2F:                                    // Fuel Tank Level Input (%)
      return A * 100 / 255;                       //
    case 0x30:                                    // Warm-ups Since Codes Cleared (count)
      return A;                                   //
    case 0x31:                                    // Distance Traveled Since Codes Cleared (km)
      return (A * 256) + B;                       //
    case 0x32:
      {                                                 // Evap System Vapor Pressure (Pa)
        int16_t signedValue = (int16_t)((A << 8) | B);  //
        return signedValue / 4;                         //
      }
    case 0x33:                            // Absolute Barometric Pressure (kPa)
      return A;                           //
    case 0x34:                            // Oxygen Sensor 1C (current)
    case 0x35:                            // Oxygen Sensor 2C
    case 0x36:                            // Oxygen Sensor 3C
    case 0x37:                            // Oxygen Sensor 4C
    case 0x38:                            // Oxygen Sensor 5C
    case 0x39:                            // Oxygen Sensor 6C
    case 0x3A:                            // Oxygen Sensor 7C
    case 0x3B:                            // Oxygen Sensor 8C
      return ((A * 256) + B) * 0.000488;  //
    case 0x3C:                            // Catalyst Temperature Bank 1 Sensor 1 (°C)
    case 0x3D:                            // Catalyst Temperature Bank 2 Sensor 1 (°C)
    case 0x3E:                            // Catalyst Temperature Bank 1 Sensor 2 (°C)
    case 0x3F:                            // Catalyst Temperature Bank 2 Sensor 2 (°C)
      return (A * 256) + B - 40;          //
    default:                              //
      return -4;                          // Unknown PID
  }
}

int get_DTCs(byte mode) {
  // Request: C2 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcCount = 0;
  String* targetArray = nullptr;

  if (mode == readStoredDTCs) {
    targetArray = storedDTCBuffer;
  } else if (mode == readPendingDTCs) {
    targetArray = pendingDTCBuffer;
  } else {
    return;  // Invalid mode
  }

  writeData(mode, 0x00);

  int len = readData();
  if (len >= 3) {
    for (int i = 0; i < len - 5; i += 2) {
      byte b1 = resultBuffer[4 + i];
      byte b2 = resultBuffer[4 + i + 1];

      if (b1 == 0 && b2 == 0) break;

      targetArray[dtcCount++] = decodeDTC(b1, b2);
    }
  }

  return dtcCount;
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

bool clear_DTC() {
  writeData(clearStoredDTCs, 0x00);
  int len = readData();
  if (len >= 3) {
    if (resultBuffer[3] == 0x44) {
      memset(storedDTCBuffer, 0, sizeof(storedDTCBuffer));
      memset(pendingDTCBuffer, 0, sizeof(pendingDTCBuffer));
      return true;
    }
  }
  return false;
}

String getVehicleInfo(byte pid) {
  // Request: C2 33 F1 09 02 F1
  // example Response: 87 F1 11 49 02 01 00 00 00 31 06
  //                   87 F1 11 49 02 02 41 31 4A 43 D5
  //                   87 F1 11 49 02 03 35 34 34 34 A8
  //                   87 F1 11 49 02 04 52 37 32 35 C8
  //                   87 F1 11 49 02 05 32 33 36 37 E6

  byte dataArray[64];
  int messageCount;
  int arrayNum = 0;

  if (pid == 0x02) {
    messageCount = 5;
  } else if (pid == 0x04 || pid == 0x06) {
    if (pid == 0x04) {
      writeData(readVehicleInfo, read_ID_Length);
    } else if (pid == 0x06) {
      writeData(readVehicleInfo, read_ID_Num_Length);
    } else {
      return "";
    }

    if (readData()) {
      messageCount = resultBuffer[5];
    } else {
      return "";
    }
  }

  writeData(readVehicleInfo, pid);

  if (readData()) {
    for (int j = 0; j < messageCount; j++) {
      if (pid == 0x02 && j == 0) {
        dataArray[arrayNum++] = resultBuffer[9];
        continue;
      }
      for (int i = 1; i <= 4; i++) {
        dataArray[arrayNum++] = resultBuffer[i + 5 + j * 11];
      }
    }
  }

  if (pid == 0x02 || pid == 0x04) {
    return convertHexToAscii(dataArray, arrayNum);
  } else if (pid == 0x06) {
    return convertBytesToHexString(dataArray, arrayNum);
  }
  return "";
}

int readSupportedData(byte mode) {
  int supportedCount = 0;
  int pidIndex = 0;
  int startByte = 0;
  int arraySize = 32;  // Size of supported data arrays
  byte* targetArray = nullptr;

  if (mode == readLiveData) {
    startByte = 5;
    targetArray = supportedLiveData;
  } else if (mode == readFreezeFrameData) {
    startByte = 6;
    targetArray = supportedFreezeFrame;
  } else if (mode == readVehicleInfo) {
    startByte = 6;
    targetArray = supportedVehicleInfo;
  } else if (mode == testOtherComponents) {
    startByte = 6;
    targetArray = supportedComponentMonitoring;
  } else {
    return -1;  // Invalid mode
  }

  writeData(mode, SUPPORTED_PIDS_1_20);
  if (readData()) {
    for (int i = 0; i < 4; i++) {
      byte value = resultBuffer[i + startByte];
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          targetArray[supportedCount++] = pidIndex + 1;
        }
        pidIndex++;
      }
    }
  }

  if (isInArray(targetArray, arraySize, 0x20)) {
    writeData(mode, SUPPORTED_PIDS_21_40);
    if (readData()) {
      for (int i = 0; i < 4; i++) {
        byte value = resultBuffer[i + startByte];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            targetArray[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }
  }

  if (isInArray(targetArray, arraySize, 0x40)) {
    writeData(mode, SUPPORTED_PIDS_41_60);
    if (readData()) {
      for (int i = 0; i < 4; i++) {
        byte value = resultBuffer[i + startByte];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            targetArray[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }
  }

  return supportedCount;
}