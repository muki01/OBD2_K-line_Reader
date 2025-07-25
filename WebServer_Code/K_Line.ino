byte resultBuffer[64];
String dtcBuffer[32];
byte supportedLiveData[32];
byte desiredLiveData[32];
byte supportedFreezeFrame[32];
byte supportedVehicleInfo[32];
byte supportedComponentMonitoring[32];

void obdTask() {
  //Check DTCs in page -1, 0, 2, 3, 5, 6
  if (page == -1 || page == 0 || page == 2 || page == 3 || page == 5 || page == 6) {
    if (page != -1) {
      if (millis() - lastDTCTime >= 1000) {
        get_DTCs();
        lastDTCTime = millis();
      }
    } else {
      get_DTCs();
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
    if (dtcBuffer[0] != "") {
      if (isInArray(supportedLiveData, sizeof(supportedLiveData), DISTANCE_TRAVELED_WITH_MIL_ON)) {
        getPID(read_LiveData, DISTANCE_TRAVELED_WITH_MIL_ON);
      }
    }
  }
  //Get All FreezeFrame values in page 3 if detected DTCs
  else if (page == 3) {
    if (dtcBuffer[0] != "") {
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

bool init_OBD2() {
  // Request: C1 33 F1 81 66
  // Response: 83 F1 11 C1 8F EF C4

  if (protocol == "Automatic" || protocol == "ISO14230_Slow" || protocol == "ISO9141") {
    debugPrintln("Trying ISO9141 or ISO14230_Slow");
    K_Serial.end();
    pinMode(K_line_RX, INPUT_PULLUP);
    pinMode(K_line_TX, OUTPUT);
    digitalWrite(K_line_TX, HIGH), delay(3000);
    send5baud(0x33);

    begin_K_Serial();
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

int readData() {
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

void getPID(byte mode, byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  if (mode != 0x01 && mode != 0x02) return;

  writeData(mode, pid);

  if (!readData()) return;             // Yanıt okunamadıysa çık
  if (resultBuffer[4] != pid) return;  // Gelen PID doğru mu kontrol et

  byte A = resultBuffer[(mode == 1) ? 5 : 6];
  byte B = resultBuffer[(mode == 1) ? 6 : 7];

  PidMapping* mappings = (mode == 1) ? liveDataMappings : freezeFrameMappings;  // Doğru mapping dizisini seç

  for (int i = 0; i < 64; i++) {
    if (mappings[i].pid == pid) {

      if (pid == FUEL_SYSTEM_STATUS) {                     // PID: 0x03
        mappings[i].value = A;                             // unit: raw
      } else if (pid == ENGINE_LOAD) {                     // PID: 0x04
        mappings[i].value = (100.0 / 255) * A;             // unit: %
      } else if (pid == ENGINE_COOLANT_TEMP) {             // PID: 0x05
        mappings[i].value = A - 40;                        // unit: °C
      } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_1) {     // PID: 0x06
        mappings[i].value = (A / 1.28) - 100.0;            // unit: %
      } else if (pid == LONG_TERM_FUEL_TRIM_BANK_1) {      // PID: 0x07
        mappings[i].value = (A / 1.28) - 100.0;            // unit: %
      } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_2) {     // PID: 0x08
        mappings[i].value = (A / 1.28) - 100.0;            // unit: %
      } else if (pid == LONG_TERM_FUEL_TRIM_BANK_2) {      // PID: 0x09
        mappings[i].value = (A / 1.28) - 100.0;            // unit: %
      } else if (pid == FUEL_PRESSURE) {                   // PID: 0x0A
        mappings[i].value = 3 * A;                         // unit: kPa
      } else if (pid == INTAKE_MANIFOLD_ABS_PRESSURE) {    // PID: 0x0B
        mappings[i].value = A;                             // unit: kPa
      } else if (pid == ENGINE_RPM) {                      // PID: 0x0C
        mappings[i].value = (256 * A + B) / 4;             // unit: rpm
      } else if (pid == VEHICLE_SPEED) {                   // PID: 0x0D
        mappings[i].value = A;                             // unit: km/h
      } else if (pid == TIMING_ADVANCE) {                  // PID: 0x0E
        mappings[i].value = (A / 2) - 64;                  // unit: °BTDC
      } else if (pid == INTAKE_AIR_TEMP) {                 // PID: 0x0F
        mappings[i].value = A - 40;                        // unit: °C
      } else if (pid == MAF_FLOW_RATE) {                   // PID: 0x10
        mappings[i].value = (256 * A + B) / 100.0;         // unit: g/s
      } else if (pid == THROTTLE_POSITION) {               // PID: 0x11
        mappings[i].value = (100.0 / 255) * A;             // unit: %
      } else if (pid == COMMANDED_SECONDARY_AIR_STATUS) {  // PID: 0x12
        mappings[i].value = A;                             // unit: raw
      } else if (pid == OXYGEN_SENSORS_PRESENT_2_BANKS) {  // PID: 0x13
        mappings[i].value = A;                             // unit: raw
      } else if (pid == OXYGEN_SENSOR_1_A) {               // PID: 0x14
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim1 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_2_A) {               // PID: 0x15
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim2 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_3_A) {               // PID: 0x16
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim3 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_4_A) {               // PID: 0x17
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim4 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_5_A) {               // PID: 0x18
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim5 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_6_A) {               // PID: 0x19
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim6 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_7_A) {               // PID: 0x1A
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim7 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OXYGEN_SENSOR_8_A) {               // PID: 0x1B
        mappings[i].value = A / 200.0;                     // unit: volts
        shortTermFuelTrim8 = (100.0 / 128) * B - 100.0;    // unit: %
      } else if (pid == OBD_STANDARDS) {                   // PID: 0x1C
        mappings[i].value = A;                             // unit: raw
      } else if (pid == OXYGEN_SENSORS_PRESENT_4_BANKS) {  // PID: 0x1D
        mappings[i].value = A;                             // unit: raw
      } else if (pid == AUX_INPUT_STATUS) {                // PID: 0x1E
        mappings[i].value = A;                             // unit: raw
      } else if (pid == RUN_TIME_SINCE_ENGINE_START) {     // PID: 0x1F
        mappings[i].value = 256 * A + B;                   // unit: seconds
      }

      else if (pid == DISTANCE_TRAVELED_WITH_MIL_ON) {            // PID: 0x21
        mappings[i].value = 256 * A + B;                          // unit: km
      } else if (pid == FUEL_RAIL_PRESSURE) {                     // PID: 0x22
        mappings[i].value = 0.079 * (256 * A + B);                // unit: kPa
      } else if (pid == FUEL_RAIL_GAUGE_PRESSURE) {               // PID: 0x23
        mappings[i].value = 10 * (256 * A + B);                   // unit: kPa
      } else if (pid == OXYGEN_SENSOR_1_B) {                      // PID: 0x24
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_2_B) {                      // PID: 0x25
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_3_B) {                      // PID: 0x26
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_4_B) {                      // PID: 0x27
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_5_B) {                      // PID: 0x28
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_6_B) {                      // PID: 0x29
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_7_B) {                      // PID: 0x2A
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == OXYGEN_SENSOR_8_B) {                      // PID: 0x2B
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio V
      } else if (pid == COMMANDED_EGR) {                          // PID: 0x2C
        mappings[i].value = (100.0 / 255) * A;                    // unit: %
      } else if (pid == EGR_ERROR) {                              // PID: 0x2D
        mappings[i].value = (100.0 / 128) * A - 100;              // unit: %
      } else if (pid == COMMANDED_EVAPORATIVE_PURGE) {            // PID: 0x2E
        mappings[i].value = (100.0 / 255) * A;                    // unit: %
      } else if (pid == FUEL_TANK_LEVEL_INPUT) {                  // PID: 0x2F
        mappings[i].value = (100.0 / 255) * A;                    // unit: %
      } else if (pid == WARMUPS_SINCE_CODES_CLEARED) {            // PID: 0x30
        mappings[i].value = A;                                    // unit: counts
      } else if (pid == DISTANCE_TRAVELED_SINCE_CODES_CLEARED) {  // PID: 0x31
        mappings[i].value = 256 * A + B;                          // unit: km
      } else if (pid == EVAPORATION_SYSTEM_VAPOR_PRESSURE) {      // PID: 0x32
        mappings[i].value = (256 * A + B) / 4;                    // unit: Pa
      } else if (pid == ABSOLUTE_BAROMETRIC_PRESSURE) {           // PID: 0x33
        mappings[i].value = A;                                    // unit: kPa
      } else if (pid == OXYGEN_SENSOR_1_C) {                      // PID: 0x34
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_2_C) {                      // PID: 0x35
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_3_C) {                      // PID: 0x36
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_4_C) {                      // PID: 0x37
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_5_C) {                      // PID: 0x38
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_6_C) {                      // PID: 0x39
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_7_C) {                      // PID: 0x3A
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == OXYGEN_SENSOR_8_C) {                      // PID: 0x3B
        mappings[i].value = (256 * A + B) / 32768.0;              // unit: ratio mA
      } else if (pid == CATALYST_TEMP_BANK_1_SENSOR_1) {          // PID: 0x3C
        mappings[i].value = (256 * A + B) / 10 - 40;              // unit: °C
      } else if (pid == CATALYST_TEMP_BANK_2_SENSOR_1) {          // PID: 0x3D
        mappings[i].value = (256 * A + B) / 10 - 40;              // unit: °C
      } else if (pid == CATALYST_TEMP_BANK_1_SENSOR_2) {          // PID: 0x3E
        mappings[i].value = (256 * A + B) / 10 - 40;              // unit: °C
      } else if (pid == CATALYST_TEMP_BANK_2_SENSOR_2) {          // PID: 0x3F
        mappings[i].value = (256 * A + B) / 10 - 40;              // unit: °C
      }

      else if (pid == MONITOR_STATUS_THIS_DRIVE_CYCLE) {     // PID: 0x41
        mappings[i].value = A;                               // unit: raw
      } else if (pid == CONTROL_MODULE_VOLTAGE) {            // PID: 0x42
        mappings[i].value = (256 * A + B) / 1000.0;          // unit: V
      } else if (pid == ABS_LOAD_VALUE) {                    // PID: 0x43
        mappings[i].value = (100.0 / 255) * (256 * A + B);   // unit: %
      } else if (pid == FUEL_AIR_COMMANDED_EQUIV_RATIO) {    // PID: 0x44
        mappings[i].value = (256 * A + B) / 32768.0;         // unit: ratio
      } else if (pid == RELATIVE_THROTTLE_POSITION) {        // PID: 0x45
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == AMBIENT_AIR_TEMP) {                  // PID: 0x46
        mappings[i].value = A - 40;                          // unit: °C
      } else if (pid == ABS_THROTTLE_POSITION_B) {           // PID: 0x47
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == ABS_THROTTLE_POSITION_C) {           // PID: 0x48
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == ABS_THROTTLE_POSITION_D) {           // PID: 0x49
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == ABS_THROTTLE_POSITION_E) {           // PID: 0x4A
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == ABS_THROTTLE_POSITION_F) {           // PID: 0x4B
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == COMMANDED_THROTTLE_ACTUATOR) {       // PID: 0x4C
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == TIME_RUN_WITH_MIL_ON) {              // PID: 0x4D
        mappings[i].value = 256 * A + B;                     // unit: min
      } else if (pid == TIME_SINCE_CODES_CLEARED) {          // PID: 0x4E
        mappings[i].value = 256 * A + B;                     // unit: min
      } else if (pid == MAX_VALUES_EQUIV_V_I_PRESSURE) {     // PID: 0x4F
        mappings[i].value = B;                               // unit: ratio V mA kPa
      } else if (pid == MAX_MAF_RATE) {                      // PID: 0x50
        mappings[i].value = A * 10;                          // unit: g/s
      } else if (pid == FUEL_TYPE) {                         // PID: 0x51
        mappings[i].value = A;                               // unit: ref table
      } else if (pid == ETHANOL_FUEL_PERCENT) {              // PID: 0x52
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == ABS_EVAP_SYS_VAPOR_PRESSURE) {       // PID: 0x53
        mappings[i].value = (256 * A + B) / 200;             // unit: kPa
      } else if (pid == EVAP_SYS_VAPOR_PRESSURE) {           // PID: 0x54
        mappings[i].value = 256 * A + B;                     // unit: Pa
      } else if (pid == SHORT_TERM_SEC_OXY_SENS_TRIM_1_3) {  // PID: 0x55
        mappings[i].value = (100.0 / 128.0) * A - 100;       // unit: %
      } else if (pid == LONG_TERM_SEC_OXY_SENS_TRIM_1_3) {   // PID: 0x56
        mappings[i].value = (100.0 / 128.0) * A - 100;       // unit: %
      } else if (pid == SHORT_TERM_SEC_OXY_SENS_TRIM_2_4) {  // PID: 0x57
        mappings[i].value = (100.0 / 128.0) * A - 100;       // unit: %
      } else if (pid == LONG_TERM_SEC_OXY_SENS_TRIM_2_4) {   // PID: 0x58
        mappings[i].value = (100.0 / 128.0) * A - 100;       // unit: %
      } else if (pid == FUEL_RAIL_ABS_PRESSURE) {            // PID: 0x59
        mappings[i].value = 10.0 * (256 * A + B);            // unit: kPa
      } else if (pid == RELATIVE_ACCELERATOR_PEDAL_POS) {    // PID: 0x5A
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == HYBRID_BATTERY_REMAINING_LIFE) {     // PID: 0x5B
        mappings[i].value = (100.0 / 255) * A;               // unit: %
      } else if (pid == ENGINE_OIL_TEMP) {                   // PID: 0x5C
        mappings[i].value = A - 40;                          // unit: °C
      } else if (pid == FUEL_INJECTION_TIMING) {             // PID: 0x5D
        mappings[i].value = (256 * A + B) / 128.0 - 210;     // unit: °
      } else if (pid == ENGINE_FUEL_RATE) {                  // PID: 0x5E
        mappings[i].value = (256 * A + B) / 20.0;            // unit: L/h
      } else if (pid == EMISSION_REQUIREMENTS) {             // PID: 0x5F
        mappings[i].value = A;                               // unit: raw
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
  writeData(clear_DTCs, 0x00);
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
  byte* targetArray = nullptr;

  if (mode == read_LiveData) {
    startByte = 5;
    targetArray = supportedLiveData;
  } else if (mode == read_FreezeFrame) {
    startByte = 6;
    targetArray = supportedFreezeFrame;
  } else if (mode == read_VehicleInfo) {
    startByte = 6;
    targetArray = supportedVehicleInfo;
  } else if (mode == component_Monitoring) {
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

  if (mode == read_LiveData) {
    memcpy(desiredLiveData, supportedLiveData, sizeof(supportedLiveData));
  }

  return supportedCount;
}