# 🚗OBD2 K-line Reader

![GitHub forks](https://img.shields.io/github/forks/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/OBD2_K-line_Reader?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/OBD2_K-line_Reader?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/OBD2_K-line_Reader)

This code is for reading the K-Linke in Cars. With this code you can read sensor values, troubleshoot codes and more. It is compatible with ISO9141 and ISO14230(KWP slow and fast) protocols.
I have shared schematics to communicate with the car. You can use these schematics or you can make another one. I used Arduino nano and ESP32 C3 as microcontrollers, but you can use another microcontrollers like STM32, ESP8266 and much more.

I will share more information about K-Line protocols and communication later. Stay tuned 😉.

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
* Change Partition Scheme to "Huge APP (3MB No OTA, 1MB SPIFFS)" in Tools menu
* Upload the code to your Board
* Upload the Website to SPIFFS.
  - To upload the Web Site, you can use PlatformIO or you can upload with tool for Arduino IDE v1.x.x</br>
  [Here](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) is documentation how to upload files to SPIFFS with Arduino IDE.


## 📱A picture of the application I made with the ESP32
<img width=20% src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/ec4e65b2-c6a1-4438-9ee7-89229c2815b3">
<img width=22.14% src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/303f1157-73bc-4cd4-859d-7097c8e56081">
<img width=20% src="https://github.com/user-attachments/assets/863cb9f6-7c27-48e2-b40a-7c3f9d069def">
<img width=20% src="https://github.com/user-attachments/assets/bbb49c73-4edd-4ff9-b605-560be9446b55">
<img width=20% src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/ae6cf20b-fa70-4343-80c3-f289de2fe90f">
<img width=20% src="https://github.com/user-attachments/assets/9c1cf815-f33b-4e6f-8166-e37143766833">
<img width=20% src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/54613d4c-ea31-48cc-a426-a7199aa9eaff">


## 🛠️Schematics for communication
#### This is the schematic with Transistors
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Transistor%20Schematic.png" width=70%>

#### This is the schematic with L9637D
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/L9637D.png" width=70%>

## The device I made with ESP32 C3 SuperMini.
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/e6de1715-fa77-4e7f-b723-4eebebad2242" width=70%>
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/39cb36cf-74dd-46c8-902c-27799c8c8f58" width=70%>

