# 🚗OBD2 K-line Reader

![GitHub forks](https://img.shields.io/github/forks/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/OBD2_K-line_Reader?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/OBD2_K-line_Reader?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/OBD2_K-line_Reader)
[![Sponsor](https://img.shields.io/badge/Sponsor-PCBWay-blue)](https://www.pcbway.com/)

This code is for reading the K-Linke in Cars. With this code you can read sensor values, troubleshoot codes and more. It is compatible with ISO9141 and ISO14230(KWP slow and fast) protocols.
I have shared schematics to communicate with the car. You can use these schematics or you can make another one. I used Arduino nano and ESP32 C3 as microcontrollers, but you can use another microcontrollers like STM32, ESP8266 and much more.

I will share more information about K-Line protocols and communication later. Stay tuned 😉.

You can also see my other car projects:
1. [Тhis](https://github.com/muki01/I-K_Bus) project is for BMW with I/K bus system. 
2. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Reader) project is for Cars with CAN Bus.
3. [Тhis](https://github.com/muki01/OBD2_KLine_Library) is my OBD2 K-Line Communication Library for Arduino IDE.

## ⚙️Instalation
* If you just want to test, you can use the [Basic_Code](https://github.com/muki01/OBD2_K-line_Reader/tree/main/Basic_Code). This code extracts the data to the serial monitor. It is compatible with Arduino and ESP32. [Here](https://github.com/muki01/OBD2_K-line_Reader/blob/main/Basic_Code/README.md) are the instructions on how this code works and how to install.
* If you want to see the retreaved data in the Web Site you can use [WebServer_Code](https://github.com/muki01/OBD2_K-line_Reader/tree/main/WebServer_Code). It is compatible with All ESP32 and ESP8266 board. [Here](https://github.com/muki01/OBD2_K-line_Reader/tree/main/WebServer_Code/README.md) are the instructions on how this code works and how to install.

> [!WARNING] 
> I am not responsible for any issues or damages that may occur during your testing. Please proceed with caution and at your own risk.


## 📱Pictures of the application I made
<img width=90% src="https://github.com/user-attachments/assets/766e178a-b956-4bdb-8f64-1919da479c65">
<img width=90% src="https://github.com/user-attachments/assets/24f8f3cd-4056-44de-8414-635a4de0d60c">


## 🛠️Schematics for communication
#### This is the schematic with Transistors
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Transistor%20Schematic.png" width=70%>

The **R6** resistor in this schematic is designed for **3.3V** microcontrollers. If you are using a **5V** microcontroller, you need to change the **R6** value to **5.3kΩ**.

Additionally, I have observed that many test devices use a **1kΩ** value for **R4**. However, according to the K-Line documentation, the recommended value for **R4** is **510Ω**. It is advisable to follow this value. That being said, using **1kΩ** for **R4** will not cause any issues in the circuit. However, if you prefer to adhere to the documentation, **510Ω** is the recommended value.

#### This is the schematic with L9637D
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/L9637D.png" width=70%>

<!--## The device I made with ESP32 C3 SuperMini.
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/e6de1715-fa77-4e7f-b723-4eebebad2242" width=70%>
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/39cb36cf-74dd-46c8-902c-27799c8c8f58" width=70%>
-->

## 🧩 PCB Design and Sponsorship

<table>
  <tr>
    <td style="width: 150px; vertical-align: top;">
      <a href="https://www.pcbway.com/">
        <img src="https://github.com/user-attachments/assets/793d8b14-11d8-4dde-9778-d24fd80c78ea" alt="PCBWay" width="120"/>
      </a>
    </td>
    <td style="vertical-align: top;">
      <p>
        <strong>The custom-designed PCBs used in this project were manufactured with sponsorship from <a href="https://www.pcbway.com/" target="_blank">PCBWay</a>.</strong><br/>
        <a href="https://www.pcbway.com/" target="_blank">PCBWay</a> is a well-established company that provides high-quality and reliable PCB manufacturing services to electronics developers worldwide.
      </p>
      <p>
        I was thoroughly satisfied with the board quality and customer support throughout the production process.<br/>
        I'd like to thank <a href="https://www.pcbway.com/" target="_blank">PCBWay</a> for their excellent manufacturing, fast delivery, and affordable pricing that truly added value to this project.
      </p>
    </td>
  </tr>
</table>



📷 Below is a photo of the PCB manufactured for this project:

<img width=36% src="https://github.com/user-attachments/assets/3a34b38d-cd39-4f5f-b4dd-d671399bff53" />

<br>
<br>

👉 If you're looking for professional PCB manufacturing for your own projects: </br>
🔗  [Check out PCBWay](https://www.pcbway.com/)

## 🔧 The Device I Made
I am currently assembling the final version of the device using this PCB.
The board is designed to work with the ESP32-S3 Zero module, which will serve as the core of the system.

In the coming days, I will upload photos and detailed information about the completed hardware.
📷 Stay tuned — real-world images of the working device are on the way!

In the future, I may also design new versions of the PCB or explore alternative microcontrollers, depending on the project's evolution.

