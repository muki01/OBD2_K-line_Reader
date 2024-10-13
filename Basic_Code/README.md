# Basic Code
This code is designed to work with devices such as Arduino Uno, Nano, Pro Mini, and ESP32, which require two UART interfaces for communication. The second UART is used to connect to a car's OBD-II interface (or another vehicle data source) and retrieve information, which is then displayed on the Serial Monitor using the main UART.

For Arduino boards like Uno, Nano, and Pro Mini, AltSoftSerial library is used to implement the second UART, while for ESP32, the built-in second hardware UART is utilized.

## 📡 How It Works
The main UART (Serial) communicates with the computer and outputs the car's data to the Serial Monitor.
The secondary UART (AltSoftSerial for Arduino or Serial1 for ESP32) is connected to the car and retrieves vehicle data.

## 🛠️ Hardware Setup
* Arduino Uno/Nano/Pro Mini
 - RX Pin (AltSoftSerial): Pin 8 (connects to the RX pin as labeled in the schematics)
 - TX Pin (AltSoftSerial): Pin 9 (connects to the TX pin as labeled in the schematics)
* ESP32
 - UART1 RX Pin: GPIO 16 (connects to the RX pin as labeled in the schematics)
 - UART1 TX Pin: GPIO 17 (connects to the TX pin as labeled in the schematics)

## 🖼️ Example Screenshots of Serial Data
