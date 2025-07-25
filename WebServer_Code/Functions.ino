byte calculateChecksum(const byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum % 256;
}

bool isInArray(byte arr[], int size, byte value) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == value) {
      return true;
    }
  }
  return false;
}

// int getArrayLength(byte arr[]) {
//   int count = 0;
//   while (arr[count] != 0) {
//     count++;
//   }
//   return count;
// }

String convertHexToAscii(byte* hexBytes, size_t length) {
  String asciiString = "";
  for (int i = 0; i < length; i++) {
    if (hexBytes[i] >= 0x20 && hexBytes[i] <= 0x7E) {
      char asciiChar = (char)hexBytes[i];
      asciiString += asciiChar;
    }
  }
  return asciiString;
}

String convertBytesToHexString(byte* buffer, int length) {
  String hexString = "";
  for (int i = 0; i < length; i++) {
    if (buffer[i] < 0x10) {
      hexString += "0";
    }
    hexString += String(buffer[i], HEX);
  }
  hexString.toUpperCase();
  return hexString;
}

String convertBytesToHexStringWithComma(byte arr[], int length) {
  String hexString = "";
  for (int i = 0; i < length; i++) {
    if (arr[i] == 0) {
      break;
    }
    if (i > 0) {
      hexString += ", ";
    }
    if (arr[i] < 0x10) {
      hexString += "0";
    }
    hexString += String(arr[i], HEX);
  }
  hexString.toUpperCase();
  return hexString;
}

String joinStringsWithComma(String arr[], int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    if (arr[i] == 0) {
      break;
    }
    if (i > 0) {
      result += ", ";
    }
    result += arr[i];
  }
  return result;
}


void begin_K_Serial() {
#ifdef ESP32
  K_Serial.begin(10400, SERIAL_8N1, K_line_RX, K_line_TX);
#elif defined(ESP8266)
  K_Serial.begin(10400, SERIAL_8N1);
#endif
}




void connectMelody() {
  tone(Buzzer, 660, 50);
  delay(80);
  tone(Buzzer, 1320, 55);
  delay(70);
  tone(Buzzer, 1760, 100);
}

void disconnectMelody() {
  tone(Buzzer, 660, 50);
  delay(80);
  tone(Buzzer, 1760, 55);
  delay(70);
  tone(Buzzer, 1320, 100);
}

void Melody1() {
  tone(Buzzer, 1100, 50);
  delay(10);
  tone(Buzzer, 1300, 50);
}

void Melody2() {
  tone(Buzzer, 1320, 50);
  delay(10);
  tone(Buzzer, 1570, 50);
}

void Melody3() {
  tone(Buzzer, 500, 40);
  tone(Buzzer, 600, 60);
}

void BlinkLed(int time, int count) {
  for (int i = 1; i <= count; i++) {
    digitalWrite(Led, LOW);
    delay(time);
    digitalWrite(Led, HIGH);
    if (i != count) {
      delay(time);
    }
  }
}

struct PidMapping {
  byte pid;
  const char* jsonKey;
  int value;
  const char* unit;
};

PidMapping liveDataMappings[] = {
  { 0x01, "Monitor Status Since DTC Cleared", 0, "BIT" },
  { 0x03, "Fuel System Status", 0, "BIT" },
  { 0x04, "Engine Load", 0, "%" },
  { 0x05, "Coolant Temp", 0, "°C" },
  { 0x06, "Short Term Fuel Trim Bank 1", 0, "%" },
  { 0x07, "Long Term Fuel Trim Bank 1", 0, "%" },
  { 0x08, "Short Term Fuel Trim Bank 2", 0, "%" },
  { 0x09, "Long Term Fuel Trim Bank 2", 0, "%" },
  { 0x0A, "Fuel Pressure", 0, "kPa" },
  { 0x0B, "Intake Manifold Abs Pressure", 0, "kPa" },
  { 0x0C, "RPM", 0, "rpm" },
  { 0x0D, "Speed", 0, "km/h" },
  { 0x0E, "Timing Advance (before TDC)", 0, "°" },
  { 0x0F, "Intake Air Temp", 0, "°C" },
  { 0x10, "MAF Flow Rate", 0, "g/s" },
  { 0x11, "Throttle Position", 0, "%" },
  { 0x12, "Commanded Secondary Air Status", 0, "BIT" },
  { 0x13, "Oxygen Sensors Present 2 Banks", 0, "BIT" },
  { 0x14, "Oxygen Sensor 1A", 0, "V %" },
  { 0x15, "Oxygen Sensor 2A", 0, "V %" },
  { 0x16, "Oxygen Sensor 3A", 0, "V %" },
  { 0x17, "Oxygen Sensor 4A", 0, "V %" },
  { 0x18, "Oxygen Sensor 5A", 0, "V %" },
  { 0x19, "Oxygen Sensor 6A", 0, "V %" },
  { 0x1A, "Oxygen Sensor 7A", 0, "V %" },
  { 0x1B, "Oxygen Sensor 8A", 0, "V %" },
  { 0x1C, "OBD Standards", 0, "BIT" },
  { 0x1D, "Oxygen Sensors Present 4 Banks", 0, "BIT" },
  { 0x1E, "Aux Input Status", 0, "BIT" },
  { 0x1F, "Run Time Since Engine Start", 0, "sec" },

  { 0x21, "Distance Traveled With MIL On", 0, "km" },
  { 0x22, "Fuel Rail Pressure", 0, "kPa" },
  { 0x23, "Fuel Rail Guage Pressure", 0, "kPa" },
  { 0x24, "Oxygen Sensor 1B", 0, "ratio V" },
  { 0x25, "Oxygen Sensor 2B", 0, "ratio V" },
  { 0x26, "Oxygen Sensor 3B", 0, "ratio V" },
  { 0x27, "Oxygen Sensor 4B", 0, "ratio V" },
  { 0x28, "Oxygen Sensor 5B", 0, "ratio V" },
  { 0x29, "Oxygen Sensor 6B", 0, "ratio V" },
  { 0x2A, "Oxygen Sensor 7B", 0, "ratio V" },
  { 0x2B, "Oxygen Sensor 8B", 0, "ratio V" },
  { 0x2C, "Commanded EGR", 0, "%" },
  { 0x2D, "EGR Error", 0, "%" },
  { 0x2E, "Commanded Evaporative Purge", 0, "%" },
  { 0x2F, "Fuel Tank Level Input", 0, "%" },
  { 0x30, "Warm UPS Since Codes Cleared", 0, "count" },
  { 0x31, "Dist Trav Since Codes Cleared", 0, "km" },
  { 0x32, "Evap System Vapor Pressure", 0, "Pa" },
  { 0x33, "ABS Barometric Pressure", 0, "kPa" },
  { 0x34, "Oxygen Sensor 1C", 0, "ratio mA" },
  { 0x35, "Oxygen Sensor 2C", 0, "ratio mA" },
  { 0x36, "Oxygen Sensor 3C", 0, "ratio mA" },
  { 0x37, "Oxygen Sensor 4C", 0, "ratio mA" },
  { 0x38, "Oxygen Sensor 5C", 0, "ratio mA" },
  { 0x39, "Oxygen Sensor 6C", 0, "ratio mA" },
  { 0x3A, "Oxygen Sensor 7C", 0, "ratio mA" },
  { 0x3B, "Oxygen Sensor 8C", 0, "ratio mA" },
  { 0x3C, "Catalyst Temp Bank 1 Sensor 1", 0, "°C" },
  { 0x3D, "Catalyst Temp Bank 2 Sensor 1", 0, "°C" },
  { 0x3E, "Catalyst Temp Bank 1 Sensor 2", 0, "°C" },
  { 0x3F, "Catalyst Temp Bank 2 Sensor 2", 0, "°C" },

  { 0x41, "Monitor Status This Drive Cycle", 0, "BIT" },
  { 0x42, "Control Module Voltage", 0, "V" },
  { 0x43, "Absolute Load Value", 0, "%" },
  { 0x44, "Commanded Fuel/Air Equivalence Ratio", 0, "ratio" },
  { 0x45, "Relative Throttle Position", 0, "%" },
  { 0x46, "Ambient Air Temperature", 0, "°C" },
  { 0x47, "Absolute Throttle Position B", 0, "%" },
  { 0x48, "Absolute Throttle Position C", 0, "%" },
  { 0x49, "Absolute Throttle Position D", 0, "%" },
  { 0x4A, "Absolute Throttle Position E", 0, "%" },
  { 0x4B, "Absolute Throttle Position F", 0, "%" },
  { 0x4C, "Commanded Throttle Actuator", 0, "%" },
  { 0x4D, "Time Run with MIL On", 0, "min" },
  { 0x4E, "Time Since Trouble Codes Cleared", 0, "min" },
  { 0x4F, "Max Values: Equiv Ratio, Voltage, Current, Pressure", 0, "ratio V mA kPa" },
  { 0x50, "Max MAF Flow Rate", 0, "g/s" },
  { 0x51, "Fuel Type", 0, "ref table" },
  { 0x52, "Ethanol Fuel Percentage", 0, "%" },
  { 0x53, "Absolute Evap System Vapor Pressure", 0, "kPa" },
  { 0x54, "Evap System Vapor Pressure", 0, "Pa" },
  { 0x55, "Short Term Secondary O2 Sensor Trim Bank 1–3", 0, "%" },
  { 0x56, "Long Term Secondary O2 Sensor Trim Bank 1–3", 0, "%" },
  { 0x57, "Short Term Secondary O2 Sensor Trim Bank 2–4", 0, "%" },
  { 0x58, "Long Term Secondary O2 Sensor Trim Bank 2–4", 0, "%" },
  { 0x59, "Fuel Rail Absolute Pressure", 0, "kPa" },
  { 0x5A, "Relative Accelerator Pedal Position", 0, "%" },
  { 0x5B, "Hybrid Battery Remaining Life", 0, "%" },
  { 0x5C, "Engine Oil Temperature", 0, "°C" },
  { 0x5D, "Fuel Injection Timing", 0, "°" },
  { 0x5E, "Engine Fuel Rate", 0, "L/h" },
  { 0x5F, "Emission Requirements", 0, "BIT" }
};

PidMapping freezeFrameMappings[] = {
  { 0x01, "Monitor Status Since DTC Cleared", 0, "BIT" },
  { 0x03, "Fuel System Status", 0, "BIT" },
  { 0x04, "Engine Load", 0, "%" },
  { 0x05, "Coolant Temp", 0, "°C" },
  { 0x06, "Short Term Fuel Trim Bank 1", 0, "%" },
  { 0x07, "Long Term Fuel Trim Bank 1", 0, "%" },
  { 0x08, "Short Term Fuel Trim Bank 2", 0, "%" },
  { 0x09, "Long Term Fuel Trim Bank 2", 0, "%" },
  { 0x0A, "Fuel Pressure", 0, "kPa" },
  { 0x0B, "Intake Manifold Abs Pressure", 0, "kPa" },
  { 0x0C, "RPM", 0, "rpm" },
  { 0x0D, "Speed", 0, "km/h" },
  { 0x0E, "Timing Advance (before TDC)", 0, "°" },
  { 0x0F, "Intake Air Temp", 0, "°C" },
  { 0x10, "MAF Flow Rate", 0, "g/s" },
  { 0x11, "Throttle Position", 0, "%" },
  { 0x12, "Commanded Secondary Air Status", 0, "BIT" },
  { 0x13, "Oxygen Sensors Present 2 Banks", 0, "BIT" },
  { 0x14, "Oxygen Sensor 1A", 0, "V %" },
  { 0x15, "Oxygen Sensor 2A", 0, "V %" },
  { 0x16, "Oxygen Sensor 3A", 0, "V %" },
  { 0x17, "Oxygen Sensor 4A", 0, "V %" },
  { 0x18, "Oxygen Sensor 5A", 0, "V %" },
  { 0x19, "Oxygen Sensor 6A", 0, "V %" },
  { 0x1A, "Oxygen Sensor 7A", 0, "V %" },
  { 0x1B, "Oxygen Sensor 8A", 0, "V %" },
  { 0x1C, "OBD Standards", 0, "BIT" },
  { 0x1D, "Oxygen Sensors Present 4 Banks", 0, "BIT" },
  { 0x1E, "Aux Input Status", 0, "BIT" },
  { 0x1F, "Run Time Since Engine Start", 0, "sec" },

  { 0x21, "Distance Traveled With MIL On", 0, "km" },
  { 0x22, "Fuel Rail Pressure", 0, "kPa" },
  { 0x23, "Fuel Rail Guage Pressure", 0, "kPa" },
  { 0x24, "Oxygen Sensor 1B", 0, "ratio V" },
  { 0x25, "Oxygen Sensor 2B", 0, "ratio V" },
  { 0x26, "Oxygen Sensor 3B", 0, "ratio V" },
  { 0x27, "Oxygen Sensor 4B", 0, "ratio V" },
  { 0x28, "Oxygen Sensor 5B", 0, "ratio V" },
  { 0x29, "Oxygen Sensor 6B", 0, "ratio V" },
  { 0x2A, "Oxygen Sensor 7B", 0, "ratio V" },
  { 0x2B, "Oxygen Sensor 8B", 0, "ratio V" },
  { 0x2C, "Commanded EGR", 0, "%" },
  { 0x2D, "EGR Error", 0, "%" },
  { 0x2E, "Commanded Evaporative Purge", 0, "%" },
  { 0x2F, "Fuel Tank Level Input", 0, "%" },
  { 0x30, "Warm UPS Since Codes Cleared", 0, "count" },
  { 0x31, "Dist Trav Since Codes Cleared", 0, "km" },
  { 0x32, "Evap System Vapor Pressure", 0, "Pa" },
  { 0x33, "ABS Barometric Pressure", 0, "kPa" },
  { 0x34, "Oxygen Sensor 1C", 0, "ratio mA" },
  { 0x35, "Oxygen Sensor 2C", 0, "ratio mA" },
  { 0x36, "Oxygen Sensor 3C", 0, "ratio mA" },
  { 0x37, "Oxygen Sensor 4C", 0, "ratio mA" },
  { 0x38, "Oxygen Sensor 5C", 0, "ratio mA" },
  { 0x39, "Oxygen Sensor 6C", 0, "ratio mA" },
  { 0x3A, "Oxygen Sensor 7C", 0, "ratio mA" },
  { 0x3B, "Oxygen Sensor 8C", 0, "ratio mA" },
  { 0x3C, "Catalyst Temp Bank 1 Sensor 1", 0, "°C" },
  { 0x3D, "Catalyst Temp Bank 2 Sensor 1", 0, "°C" },
  { 0x3E, "Catalyst Temp Bank 1 Sensor 2", 0, "°C" },
  { 0x3F, "Catalyst Temp Bank 2 Sensor 2", 0, "°C" },

  { 0x41, "Monitor Status This Drive Cycle", 0, "BIT" },
  { 0x42, "Control Module Voltage", 0, "V" },
  { 0x43, "Absolute Load Value", 0, "%" },
  { 0x44, "Commanded Fuel/Air Equivalence Ratio", 0, "ratio" },
  { 0x45, "Relative Throttle Position", 0, "%" },
  { 0x46, "Ambient Air Temperature", 0, "°C" },
  { 0x47, "Absolute Throttle Position B", 0, "%" },
  { 0x48, "Absolute Throttle Position C", 0, "%" },
  { 0x49, "Absolute Throttle Position D", 0, "%" },
  { 0x4A, "Absolute Throttle Position E", 0, "%" },
  { 0x4B, "Absolute Throttle Position F", 0, "%" },
  { 0x4C, "Commanded Throttle Actuator", 0, "%" },
  { 0x4D, "Time Run with MIL On", 0, "min" },
  { 0x4E, "Time Since Trouble Codes Cleared", 0, "min" },
  { 0x4F, "Max Values: Equiv Ratio, Voltage, Current, Pressure", 0, "ratio V mA kPa" },
  { 0x50, "Max MAF Flow Rate", 0, "g/s" },
  { 0x51, "Fuel Type", 0, "ref table" },
  { 0x52, "Ethanol Fuel Percentage", 0, "%" },
  { 0x53, "Absolute Evap System Vapor Pressure", 0, "kPa" },
  { 0x54, "Evap System Vapor Pressure", 0, "Pa" },
  { 0x55, "Short Term Secondary O2 Sensor Trim Bank 1–3", 0, "%" },
  { 0x56, "Long Term Secondary O2 Sensor Trim Bank 1–3", 0, "%" },
  { 0x57, "Short Term Secondary O2 Sensor Trim Bank 2–4", 0, "%" },
  { 0x58, "Long Term Secondary O2 Sensor Trim Bank 2–4", 0, "%" },
  { 0x59, "Fuel Rail Absolute Pressure", 0, "kPa" },
  { 0x5A, "Relative Accelerator Pedal Position", 0, "%" },
  { 0x5B, "Hybrid Battery Remaining Life", 0, "%" },
  { 0x5C, "Engine Oil Temperature", 0, "°C" },
  { 0x5D, "Fuel Injection Timing", 0, "°" },
  { 0x5E, "Engine Fuel Rate", 0, "L/h" },
  { 0x5F, "Emission Requirements", 0, "BIT" }
};

void updatePidValue(byte pid, int newValue) {
  for (int i = 0; i < 64; i++) {
    if (liveDataMappings[i].pid == pid) {
      liveDataMappings[i].value = newValue;
      return;
    }
  }
}