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
3. [Тhis](https://github.com/muki01/OBD2_K-line_Reader) project is for Cars with ISO9141 and ISO14230 protocols.
4. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Library) is my OBD2 CAN Bus Communication Library for Arduino IDE.
5. [Тhis](https://github.com/muki01/OBD2_KLine_Library) is my OBD2 K-Line Communication Library for Arduino IDE.
6. [Тhis](https://github.com/muki01/VAG_KW1281) project is for VAG Cars with KW1281 protocol.
<!--7. [Тhis](https://github.com/muki01/I-K_Bus_Library) is my I/K Bus Communication Library for Arduino IDE.-->


## ⚙️Instalation
* If you just want to test, you can use the [Basic_Code](https://github.com/muki01/OBD2_K-line_Reader/tree/main/Basic_Code). This code extracts the data to the serial monitor. It is compatible with Arduino and ESP32. [Here](https://github.com/muki01/OBD2_K-line_Reader/blob/main/Basic_Code/README.md) are the instructions on how this code works and how to install.
* If you want to see the retreaved data in the Web Site you can use [WebServer_Code](https://github.com/muki01/OBD2_K-line_Reader/tree/main/WebServer_Code). It is compatible with All ESP32 and ESP8266 board. [Here](https://github.com/muki01/OBD2_K-line_Reader/tree/main/WebServer_Code/README.md) are the instructions on how this code works and how to install.

> [!WARNING] 
> I am not responsible for any issues or damages that may occur during your testing. Please proceed with caution and at your own risk.


## 📱Pictures of the application I made
<img width=90% src="https://github.com/user-attachments/assets/766e178a-b956-4bdb-8f64-1919da479c65">
<img width=90% src="https://github.com/user-attachments/assets/24f8f3cd-4056-44de-8414-635a4de0d60c">


## 🛠️Schematics for communication
### 🔹 Transistor-Based Schematic
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Transistor%20Schematic.png" width=70%>

This schematic uses a discrete transistor-based approach to interface the K-Line with a microcontroller.
It is a simple and low-cost solution suitable for basic implementations and prototyping.

The **R6** resistor in this schematic is designed for **3.3V** microcontrollers. If you are using a **5V** microcontroller, you need to change the **R6** value to **5.3kΩ**.

### 🔹 Comparator-Based Schematic
<img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Comparator.png" width=70%>

This design uses a low-cost comparator IC to process the K-Line signal and convert it into a clean digital level for the microcontroller.
It offers a good balance between cost, simplicity, and signal reliability.

- Can be implemented using cheap and widely available comparators such as LM393
- Better noise immunity than discrete transistor-based designs
- Provides well-defined logic thresholds
- Suitable for low-budget projects that require improved signal stability
- Slightly higher component count compared to the transistor solution, but still cost-effective

### 🔹 Dedicated Automotive IC Schematic
<p align="start">
  <img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/L9637D.png" width="45%" />
  <img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/MC33290.png" width="42%" />
</p>

<p align="start">
  <img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/Si9241.png" width="43%" />
  <img src="https://github.com/muki01/OBD2_K-line_Reader/blob/main/Schematics/SN65HVDA195.png" width="45%" />
</p>


This schematic category uses dedicated automotive communication ICs (e.g. L9637D, MCZ33290, Si9241, SN65HVDA195 etc.) specifically designed for K-Line / ISO 9141 applications.

- Built-in voltage level shifting and protection
- Fully compliant with automotive communication standards
- Highest reliability and signal stability
- Recommended for production-grade and long-term use designs

<!--## The device I made with ESP32 C3 SuperMini.
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/e6de1715-fa77-4e7f-b723-4eebebad2242" width=70%>
<img src="https://github.com/muki01/OBD2_K-line_Reader/assets/75759731/39cb36cf-74dd-46c8-902c-27799c8c8f58" width=70%>
-->

## 🧩 PCB Design and Sponsorship

<a href="https://pcbway.com/g/SD5aQu">
  <img align="left" src="https://github.com/user-attachments/assets/793d8b14-11d8-4dde-9778-d24fd80c78ea" alt="PCBWay" width="18%" />
</a>
<p>
   <strong>The custom-designed PCBs used in this project were manufactured with sponsorship from <a href="https://www.pcbway.com/" target="_blank">PCBWay</a>.</strong>
   PCBWay is a well-established company that provides high-quality and reliable PCB manufacturing services to electronics developers worldwide.
</p>
<p>
   I was thoroughly satisfied with the board quality and customer support throughout the production process.
   I'd like to thank PCBWay for their excellent manufacturing, fast delivery, and affordable pricing that truly added value to this project.
</p>

#### 📷 Below are photos of the PCBs manufactured for this project:

<img width=36% src="https://github.com/user-attachments/assets/3a34b38d-cd39-4f5f-b4dd-d671399bff53" />
<img width=39% src="https://github.com/user-attachments/assets/1a794aea-b9b8-4cdd-bebb-17b25fe7fd7b" />

</br>
</br>

👉 If you're looking for professional PCB manufacturing for your own projects: </br>
🔗 [Check out PCBWay](https://pcbway.com/g/SD5aQu)

## 🔧 The Device I Made
<img width=60% src="https://github.com/user-attachments/assets/262a9871-beb2-41da-b0f4-117eae2c9fd5" />


In the future, I may also design new versions of the PCB or explore alternative microcontrollers, depending on the project's evolution.

---

## ☕ Support My Work

If you enjoy my projects and want to support me, you can do so through the links below:

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/-Sponsor%20Me%20on%20GitHub-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

---

## 📬 Contact

For information, job offers, collaboration, sponsorship, or purchasing my devices, you can contact me via email.

📧 Email: muksin.muksin04@gmail.com

---

