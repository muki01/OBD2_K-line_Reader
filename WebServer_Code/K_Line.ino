byte resultBuffer[64];
String storedDTCBuffer[32];
String pendingDTCBuffer[32];
byte supportedLiveData[32];
byte desiredLiveData[32];
byte supportedFreezeFrame[32];
byte supportedOxygenSensor[32];
byte supportedOtherComponents[32];
byte supportedControlComponents[32];
byte supportedVehicleInfo[32];

void obdTask() {
  if (clearDTC_Flag == true) {
    debugPrintln("Clearing DTCs");
    clearDTCs();
    clearDTC_Flag = false;
  }
  //Check DTCs in page -1, 0, 2, 3, 5, 6
  if (page == -1 || page == 0 || page == 2 || page == 3 || page == 5 || page == 6) {
    if (page != -1) {
      if (millis() - lastDTCTime >= 1000) {
        get_DTCs(read_storedDTCs);
        lastDTCTime = millis();
      }
    } else {
      get_DTCs(read_storedDTCs);
    }
  }

  //Get Desired LiveData in page -1 and 1
  if (page == -1 || page == 1) {
    for (const auto& mapping : liveDataMappings) {
      if (conectionStatus == false) {
        return;
      }
      if (isInArray(desiredLiveData, sizeof(desiredLiveData), mapping.pid)) {
        getPID(read_LiveData, mapping.pid);
      }
    }
  }
  //Get DISTANCE_TRAVELED_WITH_MIL_ON in page 2 if is in supportedLiveData and DTCs detected
  else if (page == 2) {
    if (storedDTCBuffer[0] != "") {
      if (isInArray(supportedLiveData, sizeof(supportedLiveData), DISTANCE_TRAVELED_WITH_MIL_ON)) {
        getPID(read_LiveData, DISTANCE_TRAVELED_WITH_MIL_ON);
      }
    }
  }
  //Get All FreezeFrame values in page 3 if detected DTCs
  else if (page == 3) {
    if (storedDTCBuffer[0] != "") {
      for (const auto& mapping : freezeFrameMappings) {
        if (conectionStatus == false) {
          return;
        }
        if (isInArray(supportedFreezeFrame, sizeof(supportedFreezeFrame), mapping.pid)) {
          getPID(read_FreezeFrame, mapping.pid);
        }
      }
    }
  }
  //Get Speed data in page 4
  else if (page == 4) {
    if (isInArray(supportedLiveData, sizeof(supportedLiveData), VEHICLE_SPEED)) {
      getPID(read_LiveData, VEHICLE_SPEED);
    }
  }
  //Get VIN, ID, ID_Num data in page 5
  else if (page == 5) {
    if (isInArray(supportedVehicleInfo, sizeof(supportedVehicleInfo), read_VIN)) {
      Vehicle_VIN = getVehicleInfo(read_VIN);
    }
    if (isInArray(supportedVehicleInfo, sizeof(supportedVehicleInfo), read_ID)) {
      Vehicle_ID = getVehicleInfo(read_ID);
    }
    if (isInArray(supportedVehicleInfo, sizeof(supportedVehicleInfo), read_ID_Num)) {
      Vehicle_ID_Num = getVehicleInfo(read_ID_Num);
    }
  }
}

bool initOBD2() {
  // Request: C1 33 F1 81 66
  // Response: 83 F1 11 C1 8F EF C4

  if (protocol == "Automatic" || protocol == "ISO14230_Slow" || protocol == "ISO9141") {
    debugPrintln("Trying ISO9141 or ISO14230_Slow");
    setSerial(false);
    send5baud(0x33);

    setSerial(true);
    if (readData()) {
      if (resultBuffer[0] == 0x55) {
        if (resultBuffer[1] == resultBuffer[2]) {
          debugPrintln("Your Protocol is ISO9141");
          protocol = "ISO9141";
        } else {
          debugPrintln("Your Protocol is ISO14230_Slow");
          protocol = "ISO14230_Slow";
        }
        debugPrintln("Writing KW2 Reversed");
        K_Serial.write(~resultBuffer[2]);  //0xF7

        if (readData()) {
          if (resultBuffer[0]) {
            return true;
          } else {
            debugPrintln("No Data Retrieved from Car");
          }
        }
      }
    }
  }

  if (protocol == "Automatic" || protocol == "ISO14230_Fast") {
    debugPrintln("Trying ISO14230_Fast");
    setSerial(false);
    digitalWrite(K_line_TX, LOW), delay(25);
    digitalWrite(K_line_TX, HIGH), delay(25);

    setSerial(true);
    writeData(init_OBD, 0x00);
    if (readData()) {
      if (resultBuffer[3] == 0xC1) {
        debugPrintln("Your Protocol is ISO14230_Fast");
        protocol = "ISO14230_Fast";
        return true;
      }
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
  size_t length = (mode == read_FreezeFrame || mode == test_OxygenSensors) ? 7 : (mode == init_OBD || mode == read_storedDTCs || mode == clear_DTCs || mode == read_pendingDTCs) ? 5
                                                                                                                                                                                     : 6;

  if (protocol == "ISO9141") {
    message[0] = (mode == read_FreezeFrame || mode == test_OxygenSensors) ? 0x69 : 0x68;
    message[1] = 0x6A;
  } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
    message[0] = (mode == read_FreezeFrame || mode == test_OxygenSensors) ? 0xC3 : (mode == init_OBD || mode == read_storedDTCs || mode == clear_DTCs || mode == read_pendingDTCs) ? 0xC1
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
  debugPrintln("Reading...");
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait up to 1 second for the first byte
  while (millis() - startMillis < 1000) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));
      unreceivedDataCount = 0;

      // Inner loop: Read all data
      debugPrint("Received Data: ");
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          if (bytesRead >= sizeof(resultBuffer)) {
            debugPrintln("\nBuffer is full. Stopping data reception.");
            return bytesRead;
          }

          resultBuffer[bytesRead] = K_Serial.read();
          debugPrintHex(resultBuffer[bytesRead]);
          debugPrint(" ");
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          // If buffer is full, stop reading and print message
        }
      }

      // If no new data is received within 60ms, exit the loop
      debugPrintln("\nData reception completed.");
      return bytesRead;
    }
  }

  // If no data is received within 1 seconds
  debugPrintln("Timeout: Not Received Data.");
  unreceivedDataCount++;
  if (unreceivedDataCount > 2) {
    unreceivedDataCount = 0;
    if (conectionStatus) {
      conectionStatus = false;
    }
  }
  return 0;
}

void clearEcho() {
  int result = K_Serial.available();
  if (result > 0) {
    debugPrint("Cleared Echo Data: ");
    for (int i = 0; i < result; i++) {
      byte receivedByte = K_Serial.read();
      debugPrintHex(receivedByte);
      debugPrint(" ");
    }
    debugPrintln();
  } else {
    debugPrintln("Not Received Echo Data");
  }
}

void getPID(uint8_t mode, uint8_t pid) {
  float value = getPIDValue(mode, pid);
  updatePIDMapping(mode, pid, value);

  sendDataToServer();
}

float getPIDValue(uint8_t mode, uint8_t pid) {
  writeData(mode, pid);

  int len = readData();
  if (len <= 0) {
    return -1;  // Data not received
  }

  if (resultBuffer[4] != pid) {
    return -2;  // Unexpected PID
  }

  uint8_t A = 0, B = 0, C = 0, D = 0;

  if (mode == read_LiveData) {
    int dataBytesLen = len - 6;
    A = (dataBytesLen >= 1) ? resultBuffer[5] : 0;
    B = (dataBytesLen >= 2) ? resultBuffer[6] : 0;
    C = (dataBytesLen >= 3) ? resultBuffer[7] : 0;
    D = (dataBytesLen >= 4) ? resultBuffer[8] : 0;
  } else if (mode == read_FreezeFrame) {
    int dataBytesLen = len - 7;
    A = (dataBytesLen >= 1) ? resultBuffer[6] : 0;
    B = (dataBytesLen >= 2) ? resultBuffer[7] : 0;
    C = (dataBytesLen >= 3) ? resultBuffer[8] : 0;
    D = (dataBytesLen >= 4) ? resultBuffer[9] : 0;
  }

  switch (pid) {
    case 0x01:                                      // Monitor Status Since DTC Cleared (bit encoded)
    case 0x02:                                      // Monitor Status Since DTC Cleared (bit encoded)
    case 0x03:                                      // Fuel System Status (bit encoded)
      return A;                                     //
    case 0x04:                                      // Engine Load (%)
      return A * 100.0f / 255.0f;                   //
    case 0x05:                                      // Coolant Temperature (°C)
      return A - 40.0f;                             //
    case 0x06:                                      // Short Term Fuel Trim Bank 1 (%)
    case 0x07:                                      // Long Term Fuel Trim Bank 1 (%)
    case 0x08:                                      // Short Term Fuel Trim Bank 2 (%)
    case 0x09:                                      // Long Term Fuel Trim Bank 2 (%)
      return A * 100.0f / 128.0f - 100.0f;          //
    case 0x0A:                                      // Fuel Pressure (kPa)
      return A * 3.0f;                              //
    case 0x0B:                                      // Intake Manifold Absolute Pressure (kPa)
      return A;                                     //
    case 0x0C:                                      // RPM
      return ((A * 256.0f) + B) / 4.0f;             //
    case 0x0D:                                      // Speed (km/h)
      return A;                                     //
    case 0x0E:                                      // Timing Advance (°)
      return A / 2.0f - 64.0f;                      //
    case 0x0F:                                      // Intake Air Temperature (°C)
      return A - 40.0f;                             //
    case 0x10:                                      // MAF Flow Rate (grams/sec)
      return ((A * 256.0f) + B) / 100.0f;           //
    case 0x11:                                      // Throttle Position (%)
      return A * 100.0f / 255.0f;                   //
    case 0x12:                                      // Commanded Secondary Air Status (bit encoded)
    case 0x13:                                      // Oxygen Sensors Present 2 Banks (bit encoded)
      return A;                                     //
    case 0x14:                                      // Oxygen Sensor 1A Voltage (V, %)
    case 0x15:                                      // Oxygen Sensor 2A Voltage (V, %)
    case 0x16:                                      // Oxygen Sensor 3A Voltage (V, %)
    case 0x17:                                      // Oxygen Sensor 4A Voltage (V, %)
    case 0x18:                                      // Oxygen Sensor 5A Voltage (V, %)
    case 0x19:                                      // Oxygen Sensor 6A Voltage (V, %)
    case 0x1A:                                      // Oxygen Sensor 7A Voltage (V, %)
    case 0x1B:                                      // Oxygen Sensor 8A Voltage (V, %)
      return A / 200.0f;                            // Voltage
    case 0x1C:                                      // OBD Standards This Vehicle Conforms To (bit encoded)
    case 0x1D:                                      // Oxygen Sensors Present 4 Banks (bit encoded)
    case 0x1E:                                      // Auxiliary Input Status (bit encoded)
      return A;                                     //
    case 0x1F:                                      // Run Time Since Engine Start (seconds)
    case 0x21:                                      // Distance Traveled With MIL On (km)
      return (A * 256.0f) + B;                      //
    case 0x22:                                      // Fuel Rail Pressure (kPa)
      return ((A * 256.0f) + B) * 0.079f;           //
    case 0x23:                                      // Fuel Rail Gauge Pressure (kPa)
      return ((A * 256.0f) + B) / 10.0f;            //
    case 0x24:                                      // Oxygen Sensor 1B (ratio, voltage)
    case 0x25:                                      // Oxygen Sensor 2B (ratio, voltage)
    case 0x26:                                      // Oxygen Sensor 3B (ratio, voltage)
    case 0x27:                                      // Oxygen Sensor 4B (ratio, voltage)
    case 0x28:                                      // Oxygen Sensor 5B (ratio, voltage)
    case 0x29:                                      // Oxygen Sensor 6B (ratio, voltage)
    case 0x2A:                                      // Oxygen Sensor 7B (ratio, voltage)
    case 0x2B:                                      // Oxygen Sensor 8B (ratio, voltage)
      return ((A * 256.0f) + B) / 32768.0f;         // ratio
    case 0x2C:                                      // Commanded EGR (%)
      return A * 100.0f / 255.0f;                   //
    case 0x2D:                                      // EGR Error (%)
      return A * 100.0f / 128.0f - 100.0f;          //
    case 0x2E:                                      // Commanded Evaporative Purge (%)
    case 0x2F:                                      // Fuel Tank Level Input (%)
      return A * 100.0f / 255.0f;                   //
    case 0x30:                                      // Warm-ups Since Codes Cleared (count)
      return A;                                     //
    case 0x31:                                      // Distance Traveled Since Codes Cleared (km)
      return (A * 256.0f) + B;                      //
    case 0x32:                                      // Evap System Vapor Pressure (Pa)
      return ((A * 256.0f) + B) / 4.0f;             //
    case 0x33:                                      // Absolute Barometric Pressure (kPa)
      return A;                                     //
    case 0x34:                                      // Oxygen Sensor 1C (current)
    case 0x35:                                      // Oxygen Sensor 2C
    case 0x36:                                      // Oxygen Sensor 3C
    case 0x37:                                      // Oxygen Sensor 4C
    case 0x38:                                      // Oxygen Sensor 5C
    case 0x39:                                      // Oxygen Sensor 6C
    case 0x3A:                                      // Oxygen Sensor 7C
    case 0x3B:                                      // Oxygen Sensor 8C
      return ((A * 256.0f) + B) / 32768.0f;         // ratio
    case 0x3C:                                      // Catalyst Temperature Bank 1 Sensor 1 (°C)
    case 0x3D:                                      // Catalyst Temperature Bank 2 Sensor 1 (°C)
    case 0x3E:                                      // Catalyst Temperature Bank 1 Sensor 2 (°C)
    case 0x3F:                                      // Catalyst Temperature Bank 2 Sensor 2 (°C)
      return ((A * 256.0f) + B) / 10.0f - 40.0f;    //
    case 0x41:                                      // Monitor status this drive cycle (bit encoded)
      return A;                                     //
    case 0x42:                                      // Control module voltage (V)
      return ((A * 256.0f) + B) / 1000.0f;          //
    case 0x43:                                      // Absolute load value (%)
      return ((A * 256.0f) + B) * 100.0f / 255.0f;  //
    case 0x44:                                      // Fuel/Air commanded equivalence ratio (lambda)
      return ((A * 256.0f) + B) / 32768.0f;         // ratio
    case 0x45:                                      // Relative throttle position (%)
      return A * 100.0f / 255.0f;                   //
    case 0x46:                                      // Ambient air temp (°C)
      return A - 40.0f;                             //
    case 0x47:                                      // Absolute throttle position B (%)
    case 0x48:                                      // Absolute throttle position C (%)
    case 0x49:                                      // Accelerator pedal position D (%)
    case 0x4A:                                      // Accelerator pedal position E (%)
    case 0x4B:                                      // Accelerator pedal position F (%)
    case 0x4C:                                      // Commanded throttle actuator (%)
      return A * 100.0f / 255.0f;                   //
    case 0x4D:                                      // Time run with MIL on (min)
    case 0x4E:                                      // Time since trouble codes cleared (min)
      return (A * 256.0f) + B;                      //
    case 0x4F:                                      // Max values for sensors (ratio, V, mA, kPa)
    case 0x50:                                      // Maximum value for air flow rate from mass air flow sensor (g/s)
    case 0x51:                                      // Fuel Type (bit encoded)
      return A;                                     //
    case 0x52:                                      // Ethanol fuel (%)
      return A * 100.0f / 255.0f;                   //
    case 0x53:                                      // Absolute evap system pressure (kPa)
      return ((A * 256.0f) + B) / 200.0f;           //
    case 0x54:                                      // Evap system vapor pressure (Pa)
      return (A * 256.0f) + B;                      //
    case 0x55:                                      // Short term secondary oxygen sensor trim, A: bank 1, B: bank 3 (%)
    case 0x56:                                      // Long term primary oxygen sensor trim, A: bank 1, B: bank 3 (%)
    case 0x57:                                      // Short term secondary oxygen sensor trim, A: bank 2, B: bank 4 (%)
    case 0x58:                                      // Long term secondary oxygen sensor trim, A: bank 2, B: bank 4 (%)
      return A * 100.0f / 128.0f - 100.0f;          //
    case 0x59:                                      // Fuel rail absolute pressure (kPa)
      return ((A * 256.0f) + B) * 10.0f;            //
    case 0x5A:                                      // Relative accelerator pedal position (%)
    case 0x5B:                                      // Hybrid battery pack remaining life (%)
      return A * 100.0f / 255.0f;                   //
    case 0x5C:                                      // Engine oil temperature (°C)
      return A - 40.0f;                             //
    case 0x5D:                                      // Fuel injection timing (°)
      return ((A * 256.0f) + B) / 128.0f - 210.0f;  //
    case 0x5E:                                      // Engine fuel rate (L/h)
      return ((A * 256.0f) + B) / 20.0f;            //
    case 0x5F:                                      // Emission requirements to which vehicle is designed (bit encoded)
      return A;                                     //
    case 0x61:                                      // Driver's demand engine - percent torque (%)
    case 0x62:                                      // Actual engine - percent torque (%)
      return A - 125.0f;                            //
    case 0x63:                                      // Engine reference torque (Nm)
      return (A * 256.0f) + B;                      //
    default:                                        //
      return -4;                                    // Unknown PID
  }
}

int get_DTCs(byte mode) {
  // Request: C2 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcCount = 0;
  String* targetArray = nullptr;

  if (mode == read_storedDTCs) {
    targetArray = storedDTCBuffer;
  } else if (mode == read_pendingDTCs) {
    targetArray = pendingDTCBuffer;
  } else {
    return -1;  // Invalid mode
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

  sendDataToServer();
  return dtcCount;
}

bool clearDTCs() {
  writeData(clear_DTCs, 0x00);
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
      writeData(read_VehicleInfo, read_ID_Length);
    } else if (pid == 0x06) {
      writeData(read_VehicleInfo, read_ID_Num_Length);
    } else {
      return "";
    }

    if (readData()) {
      messageCount = resultBuffer[5];
    } else {
      return "";
    }
  }

  writeData(read_VehicleInfo, pid);

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
  uint8_t *targetArray = nullptr;

  if (mode == read_LiveData) {  // Mode 01
    startByte = 5;
    targetArray = supportedLiveData;
  } else if (mode == read_FreezeFrame) {  // Mode 02
    startByte = 6;
    targetArray = supportedFreezeFrame;
  } else if (mode == test_OxygenSensors) {  // Mode 05
    startByte = 6;
    targetArray = supportedOxygenSensor;
  } else if (mode == test_OtherComponents) {  // Mode 06
    startByte = 6;
    targetArray = supportedOtherComponents;
  } else if (mode == control_OnBoardComponents) {  // Mode 08
    startByte = 5;
    targetArray = supportedControlComponents;
  } else if (mode == read_VehicleInfo) {  // Mode 09
    startByte = 6;
    targetArray = supportedVehicleInfo;
  } else {
    return -1;  // Invalid mode
  }

  uint8_t pidCmds[] = {SUPPORTED_PIDS_1_20, SUPPORTED_PIDS_21_40, SUPPORTED_PIDS_41_60, SUPPORTED_PIDS_61_80, SUPPORTED_PIDS_81_100};

  for (int n = 0; n < 5; n++) {
    // Group 0 is always processed, others must be checked
    if (n != 0 && !isInArray(targetArray, 32, pidCmds[n])) break;

    writeData(mode, pidCmds[n]);
    if (readData() && resultBuffer[3] == 0x40 + mode) {
      for (int i = 0; i < 4; i++) {
        uint8_t value = resultBuffer[i + startByte];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) targetArray[supportedCount++] = pidIndex + 1;
          pidIndex++;
        }
      }
    }
  }

  if (mode == read_LiveData) {
    memcpy(desiredLiveData, supportedLiveData, sizeof(supportedLiveData));
  }

  return supportedCount;
}