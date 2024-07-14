byte calculateChecksum(const byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum % 256;
}

bool isInArray(String array[], int arrayLength, String searchString) {
  for (int i = 0; i < arrayLength; i++) {
    if (array[i] == searchString) {
      return true;
    }
  }
  return false;
}

int getArrayLength(byte arr[]) {
  int count = 0;
  while (arr[count] != 0) {
    count++;
  }
  return count;
}

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