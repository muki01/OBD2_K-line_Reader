<div align="center">

# 🚗 OBD2 K-Line Reader — Car Diagnostics <br>(ISO 9141 / ISO 14230)

**Read live sensor data, read & clear trouble codes (DTCs), view freeze-frame data, vehicle info (VIN) and battery voltage from older K-Line vehicles — supporting ISO 9141-2 and ISO 14230 / KWP2000 (slow & fast init) on Arduino, ESP32, ESP8266, STM32 and Raspberry Pi Pico. Comes with multiple hardware interface schematics and an optional WiFi web dashboard with OTA updates.**

![GitHub Repo stars](https://img.shields.io/github/stars/muki01/OBD2_K-line_Reader?style=flat)
![GitHub forks](https://img.shields.io/github/forks/muki01/OBD2_K-line_Reader?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/OBD2_K-line_Reader?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/OBD2_K-line_Reader?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/OBD2_K-line_Reader)
![ESP32](https://img.shields.io/badge/ESP32-000000?logo=espressif&logoColor=red)
![Arduino](https://img.shields.io/badge/Arduino-00979D?logo=arduino&logoColor=white)
[![Sponsor](https://img.shields.io/badge/Sponsor-PCBWay-blue)](https://www.pcbway.com/)

</div>

---

## 📌 Overview

**OBD2 K-Line Reader** is a complete diagnostic firmware for communicating with a vehicle's ECU over the **K-Line** interface. Through the standard OBD-II connector it can read **live sensor data**, read and **clear diagnostic trouble codes (DTCs)**, capture **freeze-frame** snapshots, retrieve **vehicle info (VIN & ECU IDs)**, run an **acceleration/speed test**, and measure **battery voltage** — no dedicated scan tool required.

It supports the **ISO 9141-2** and **ISO 14230 (KWP2000, slow & fast init)** protocols used by most European and Asian vehicles built roughly between **2000 and 2010**, with runtime protocol switching and auto-detection. Tested on **Arduino** and **ESP32**, it also runs on STM32, ESP8266, Raspberry Pi Pico and similar microcontrollers.

Two builds are included: a lightweight **Serial Monitor** version (`Basic_Code`) for quick testing, and a **WebServer** version (`WebServer_Code`) that turns an ESP32 / ESP8266 into a standalone WiFi diagnostic tool — running as a **Station or Access Point**, updatable **over-the-air (OTA)**, and served through a browser-based dashboard. Full hardware schematics are included so you can build the interface yourself.

This project is part of a larger **OBD2 diagnostics toolkit** — libraries, a CAN-bus reader and a web dashboard. See [Related Projects](#-related-projects).

🔗 **Looking for the Web UI?** See the dedicated front-end: [OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI).

## 💼 Hire Me

I design custom automotive diagnostic tools, firmware and apps — this is what I do professionally. Whether you need a full product or just the communication layer, I can help.

- **Custom protocol implementation** — K-Line (ISO 9141-2 / KWP2000), CAN bus, and manufacturer-specific protocols (BMW I/K-Bus, VAG KW1281, and more)
- **Custom bus & protocol communication code** — low-level drivers and communication routines for any automotive bus or proprietary/custom protocol you need
- **ECU security access** — seed-key (security access) algorithms and unlock routines for KWP2000 / UDS ECUs
- **Custom mobile & desktop apps** — Android / iOS / web companion apps to visualize, log and control your device
- **Bespoke hardware** — diagnostic shields & PCBs designed around your requirements
- **ECU communication & reverse engineering** — PID logging, DTC handling, freeze-frame, VIN, undocumented buses
- **Embedded firmware** — Arduino, ESP32, ESP8266, STM32, Raspberry Pi Pico

Need something else related to **automotive communication or electronics**? Just reach out — if it involves cars, buses or embedded hardware, I can most likely help.

**Have a project in this space?** → 📧 **[muksin.muksin04@gmail.com](mailto:muksin.muksin04@gmail.com)**

## ✨ Features

**Diagnostics**
- 📊 **Live sensor data** — read real-time PIDs from the ECU.
- ⚠️ **Trouble codes** — read **and clear** DTCs.
- ❄️ **Freeze-frame data** — snapshot of sensor values captured when a fault occurred.
- 🚗 **Vehicle info** — read VIN and ECU identification data.
- 🚦 **Acceleration test** — measure vehicle performance (speed test).
- 🔋 **Battery voltage** — read directly from the OBD-II connector.

**Connectivity & Firmware**
- 🔀 **Selectable protocols** — ISO 9141-2 and ISO 14230 (KWP2000 slow & fast init), switchable at runtime.
- 📶 **WiFi — STA or AP mode** — connect to your network or host its own access point *(WebServer build)*.
- 🔄 **OTA updates** — update the firmware over the air, no cable needed *(WebServer build)*.
- 🔌 **Multi-platform** — Arduino, ESP32, ESP8266, STM32, Raspberry Pi Pico and more.
- 🌐 **Two builds** — Serial-monitor `Basic_Code` and browser-based `WebServer_Code`.
- 🛠️ **Multiple schematics** — transistor, comparator and dedicated automotive-IC options.

## 📱Pictures of the application I made

<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width="90%" src="https://github.com/user-attachments/assets/9b3aebe5-998d-4731-85bc-a0d7666fd116" />
</a>
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width="90%" src="https://github.com/user-attachments/assets/8544df16-cf62-4a80-8f19-cbd0daadfb51" />
</a>

📂 *The UI for this application has been moved to a separate repository. You can check out the source code and details here:* **[Explore the Web UI Repository](https://github.com/muki01/OBD2-Diagnostic-UI)**

## 📡 Supported Protocols

| Protocol | Standard | Initialization | Status |
|----------|----------|----------------|--------|
| ISO 9141-2 | ISO 9141-2 | 5-baud slow init | ✅ Tested |
| KWP2000 (slow init) | ISO 14230-4 | 5-baud slow init | ✅ Tested |
| KWP2000 (fast init) | ISO 14230-4 | Fast init | ✅ Tested |
| Auto-detect | — | Tries all of the above | ✅ Default |

## 🚀 Quick Start

First, build a hardware interface from one of the [Schematics](#-schematics-for-communication) below. Then choose the build that fits your goal — each has its own step-by-step setup guide:

### 🖥️ Just testing? → `Basic_Code`
Serial-monitor version for Arduino & ESP32 — wire it up, upload, and read live data in the Serial Monitor.

👉 **Setup guide:** [Basic_Code README](Basic_Code/README.md)

### 🌐 Want the web dashboard? → `WebServer_Code`
For ESP32 / ESP8266 — turns the board into a standalone WiFi diagnostic tool with a browser dashboard (STA/AP) and OTA updates.

👉 **Setup guide:** [WebServer_Code README](WebServer_Code/README.md)

> ⚠️ **Disclaimer:** This is a hobby / development project. I am not responsible for any issues or damage that may occur during testing. Never use it while the vehicle is in motion — proceed at your own risk.

## 🔗 Related Projects

Part of a full OBD2 / automotive diagnostics ecosystem:

| Firmware & Readers | Libraries | Manufacturer Protocols | UI |
|--------------------|-----------|------------------------|-----|
| [OBD2 K-line Reader](https://github.com/muki01/OBD2_K-line_Reader) | [OBD2 K-Line Library](https://github.com/muki01/OBD2_KLine_Library) | [BMW I/K Bus](https://github.com/muki01/I-K_Bus) | [OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI) |
| [OBD2 CAN Bus Reader](https://github.com/muki01/OBD2_CAN_Bus_Reader) | [OBD2 CAN Bus Library](https://github.com/muki01/OBD2_CAN_Bus_Library) | [VAG KW1281](https://github.com/muki01/VAG_KW1281) | |

## 🛠️ Schematics for Communication

K-Line communication operates at different voltage and signal levels than microcontroller pins. These circuits provide the required level shifting and protection for safe, stable operation. Pick the approach that suits your project:

### 🔹 Transistor-based
<img src="https://raw.githubusercontent.com/muki01/OBD2_K-line_Reader/main/Schematics/Transistor%20Schematic.png" width="70%">

A simple, low-cost discrete-transistor interface, ideal for basic implementations and prototyping. The **R6** resistor is sized for **3.3V** microcontrollers — for a **5V** MCU, change **R6** to **5.3 kΩ**.

### 🔹 Comparator-based
<img src="https://raw.githubusercontent.com/muki01/OBD2_K-line_Reader/main/Schematics/Comparator.png" width="70%">

Uses a cheap comparator IC (e.g. **LM393**) to produce a clean digital level. Better noise immunity and well-defined logic thresholds than the transistor design, at a slightly higher component count — a great balance of cost and reliability.

### 🔹 Dedicated automotive IC
<p align="start">
  <img src="https://raw.githubusercontent.com/muki01/OBD2_K-line_Reader/main/Schematics/L9637D.png" width="45%" />
  <img src="https://raw.githubusercontent.com/muki01/OBD2_K-line_Reader/main/Schematics/MC33290.png" width="42%" />
</p>
<p align="start">
  <img src="https://raw.githubusercontent.com/muki01/OBD2_K-line_Reader/main/Schematics/Si9241.png" width="43%" />
  <img src="https://raw.githubusercontent.com/muki01/OBD2_K-line_Reader/main/Schematics/SN65HVDA195.png" width="45%" />
</p>

Uses purpose-built K-Line / ISO 9141 transceiver ICs (**L9637D, MC33290, Si9241, SN65HVDA195**, etc.) with built-in level shifting and protection. Fully standards-compliant with the highest reliability — recommended for production-grade, long-term designs.

## 🧩 PCB Design & Sponsorship

<table>
  <tr>
    <td width="20%" valign="middle">
      <a href="https://pcbway.com/g/SD5aQu">
        <img src="https://github.com/user-attachments/assets/793d8b14-11d8-4dde-9778-d24fd80c78ea" alt="PCBWay" width="100%" />
      </a>
    </td>
    <td width="80%" valign="middle">
      <strong>The custom-designed PCBs used in this project were manufactured with sponsorship from <a href="https://www.pcbway.com/" target="_blank">PCBWay</a>.</strong>
      PCBWay provides high-quality, reliable PCB manufacturing services to electronics developers worldwide. I was thoroughly satisfied with the board quality and support, and I'd like to thank them for the excellent manufacturing, fast delivery and affordable pricing that truly added value to this project.
    </td>
  </tr>
</table>

### 📷 PCBs manufactured for this project:

<img width="26%" src="https://github.com/user-attachments/assets/3a34b38d-cd39-4f5f-b4dd-d671399bff53" />
<img width="35%" src="https://github.com/user-attachments/assets/935a801a-722a-49b1-afa2-417eeee0fc73" />
<img width="35%" src="https://github.com/user-attachments/assets/f2bfb41b-f75f-4a12-8223-ec67dbc38678" />

👉 Need professional PCB manufacturing for your own projects? 🔗 [Check out PCBWay](https://pcbway.com/g/SD5aQu)

## ☕ Support My Work

If you enjoy my projects and want to support me, you can do so through the links below:

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/-Sponsor%20Me%20on%20GitHub-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

---

## 📬 Contact

For custom development, job offers, collaboration, sponsorship, or purchasing my devices, feel free to reach out.

📧 **Email:** [muksin.muksin04@gmail.com](mailto:muksin.muksin04@gmail.com)

---

<div align="center">

Created by [**Muki**](https://github.com/muki01) · If you find this useful, consider giving it a ⭐

</div>
