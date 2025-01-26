# 🚗OBD2 K-line Reader

![GitHub forks](https://img.shields.io/github/forks/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/OBD2_K-line_Reader?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/OBD2_K-line_Reader?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/OBD2_K-line_Reader)

This code is for reading the K-Linke in Cars. With this code you can read sensor values, troubleshoot codes and more. It is compatible with ISO9141 and ISO14230(KWP slow and fast) protocols.
I have shared schematics to communicate with the car. You can use these schematics or you can make another one. I used Arduino nano and ESP32 C3 as microcontrollers, but you can use another microcontrollers like STM32, ESP8266 and much more.

I will share more information about K-Line protocols and communication later. Stay tuned 😉.

You can also see [this](https://github.com/muki01/I-K_Bus) project for I/K Bus for BMW cars

## ⚙️Instalation
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
* If you are using ESP32 C3,S2 or S3 board, you need to disable "USB CDC On Boot" option in Tools menu
* Upload the code to your Board
* Upload the Website to SPIFFS.
  - To upload the Web Site, you can use PlatformIO or you can upload with tool for Arduino IDE v1.x.x</br>
  [Here](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) is documentation how to upload files to SPIFFS with Arduino IDE.


## 📱Pictures of the application I made
<img width=90% src="https://github.com/user-attachments/assets/766e178a-b956-4bdb-8f64-1919da479c65">
<img width=90% src="https://github.com/user-attachments/assets/24f8f3cd-4056-44de-8414-635a4de0d60c">


## 🛠️Schematics for communication
#### This is the schematic with Transistors
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Transistor%20Schematic.png" width=70%>

#### This is the schematic with L9637D
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/L9637D.png" width=70%>

## The device I made with ESP32 C3 SuperMini.
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/e6de1715-fa77-4e7f-b723-4eebebad2242" width=70%>
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/39cb36cf-74dd-46c8-902c-27799c8c8f58" width=70%>

