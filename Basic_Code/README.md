# Basic Code
This code is designed to work with devices such as Arduino Uno, Nano, Pro Mini, and ESP32, which require two UART interfaces for communication. The main UART is used to communicate with the computer and display data on the Serial Monitor, while the second UART is used to connect to a car's OBD-II interface and retrieve information.

For Arduino boards the AltSoftSerial library is used to implement the second UART, while for ESP32, the built-in second hardware UART is utilized.

## 📡 How It Works
* Main UART:
  - The main UART (Serial) communicates with the computer via USB and sends the car's data to the Serial Monitor for display.

* Secondary UART:
  - The secondary UART (AltSoftSerial for Arduino or Serial1/Serial2 for ESP32) connects to the car's OBD-II interface and retrieves vehicle data, which is then forwarded to the main UART for display.

## 🛠️ Hardware Setup
* Arduino Uno/Nano/Pro Mini (AltSoftSerial)
  - RX Pin: Pin 8 (connects to the RX pin as labeled in the schematics)
  - TX Pin: Pin 9 (connects to the TX pin as labeled in the schematics)
* ESP32
  - UART1 RX Pin: GPIO 25 (connects to the RX pin as labeled in the schematics)
  - UART1 TX Pin: GPIO 26 (connects to the TX pin as labeled in the schematics)
 
## ⚙️Instalation
There are two ways to install the code. The first way is to flash the files I have provided on the releases page. The second way is to use the Arduino IDE to manually compile and upload.

### 1. Flash Firmware

### 2. Manuel Instalation using Arduino IDE
* Open Basic_Code.ino file
* Upload the code to your Board

## 🖼️ Example Screenshots of Serial Data
This is a sample output in a serial monitor for a car with ISO9141 Protocol.

<img width=30% src="https://github.com/user-attachments/assets/0ca043ea-6152-40a5-8b22-c516e84031bb">
