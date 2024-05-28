# 🚗OBD2 K-line Reader
This code is for reading the K-Linke in Cars. With this code you can read sensor values, troubleshoot codes and more. It is compatible with ISO9141 and ISO14230(KWP slow and fast) protocols.
I have shared schematics to communicate with the car. You can use these schematics or you can make another one, if you want. I used Arduino nano and ESP32 C3 as microcontrollers, but you can use another microcontrollers like STM32, ESP8266 and much more.

I will share more information about K-Line protocols and communication later. Stay tuned 😉.

## ⚙️Instalation
* Upload .ino file to your Microcontroller with Arduino IDE
* If you are using the code that is for the ESP32, you need to upload the Website to SPIFFS. </br>
 To upload the Web Site, you can use PlatformIO or you can upload with tool for Arduino IDE v1.x.x .</br>
  [Here](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) is documentation how to upload files to SPIFFS with Arduino IDE.

## 📱A picture of the application I made with the ESP32
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/3b923d34-b85f-424e-b796-b458d20b769e" width=28%>
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/318ee48f-2735-4091-ba23-e03e45f6ec02" width=31%>

## 🛠️Schematics for communication
#### This is the schematic with Transistors
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Transistor%20Schematic.png" width=70%>

#### This is the schematic with L9637D
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/L9637D.png" width=70%>

## The device I made with ESP32 C3 SuperMini.
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/e6de1715-fa77-4e7f-b723-4eebebad2242" width=70%>
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/39cb36cf-74dd-46c8-902c-27799c8c8f58" width=70%>

