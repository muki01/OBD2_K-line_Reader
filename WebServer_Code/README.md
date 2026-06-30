# WebServer Code
This code is designed to be used with devices like ESP8266 and ESP32. It creates a Web Server that allows you to view the car's values.
The ESP8266 or ESP32 creates a WiFi AP (access point) named "OBD2 Master".After connecting to this WiFi network, you should open `192.168.4.1` address in your browser.

## 📡 How It Works
  - The main UART (UART0) connects to the OBD-II interface of the car and retrieves vehicle data which is then sent to the WebSite via WebSocket. 

## 🛠️ Hardware Setup
UART0 should be used because it works best with the others UARTs don't work well because I think they are not as accurate with time but if you want you can try with them too.

* ESP32, ESP8266
  - UART0 RX Pin: GPIO 3 (connects to the RX pin as labeled in the schematics)
  - UART0 TX Pin: GPIO 1 (connects to the TX pin as labeled in the schematics)
 
* ESP32 C3
  - UART0 RX Pin: GPIO 20 (connects to the RX pin as labeled in the schematics)
  - UART0 TX Pin: GPIO 21 (connects to the TX pin as labeled in the schematics)
 
* ESP32 C6
  - UART0 RX Pin: GPIO 17 (connects to the RX pin as labeled in the schematics)
  - UART0 TX Pin: GPIO 16 (connects to the TX pin as labeled in the schematics)
 
* ESP32 S3
  - UART0 RX Pin: GPIO 44 (connects to the RX pin as labeled in the schematics)
  - UART0 TX Pin: GPIO 43 (connects to the TX pin as labeled in the schematics)
 
* ESP32 H2
  - UART0 RX Pin: GPIO 23 (connects to the RX pin as labeled in the schematics)
  - UART0 TX Pin: GPIO 24 (connects to the TX pin as labeled in the schematics)

## ⚙️Instalation
There are two ways to install the code. The first way is to flash the files I have provided on the releases page. The second way is to use the Arduino IDE to manually compile and upload.

### 1. Flash Firmware

### 2. Manuel Instalation using Arduino IDE
* Open .ino file
* 📚Instal these Libraries
  ~~~
  - ESPAsyncWebServer
  - AsyncTCP
  - ArduinoJson
  ~~~
* Edit the pins for your Board
     ~~~
     #define K_Serial Serial
     #define K_line_RX 20
     #define K_line_TX 21
     #define Led 8
     #define Buzzer 1
     #define voltagePin 0
     ~~~
* If you are using ESP32 C3, C6, S2, S3 or H2 board, you need to disable "USB CDC On Boot" option in Tools menu
* Upload the code to your Board
* Upload the Website to SPIFFS.
  - To upload the Web Site, you can use PlatformIO or you can upload with tool for Arduino IDE v1.x.x</br>
  [Here](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) is documentation how to upload files to SPIFFS with Arduino IDE.


## 📱Pictures of the application I made
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width=90% src="https://github.com/user-attachments/assets/5a3e0540-b56d-4c3a-a0bf-8c1affcda00c" />
</a>
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width=90% src="https://github.com/user-attachments/assets/8544df16-cf62-4a80-8f19-cbd0daadfb51" />
</a>

📂 *The UI for this application has been moved to a separate repository. You can check out the source code and details here:* **[Explore the Web UI Repository](https://github.com/muki01/OBD2-Diagnostic-UI)**
