# 🌐 WebServer_Code: ESP32 WiFi OBD2 K-Line Scanner with Web Dashboard

This firmware turns an **ESP32** or **ESP8266** into a standalone **WiFi OBD-II diagnostic tool** for K-Line vehicles (ISO 9141-2 and KWP2000 / ISO 14230). The board hosts a mobile-friendly web dashboard, so any phone, tablet or laptop can read **live data**, **trouble codes**, **freeze frame**, **VIN** and more. No app required.

← [Back to the main README](../README.md)

<p align="center">
  <img src="../assets/demo.gif" alt="ESP32 OBD2 web dashboard demo" width="340">
</p>

---

## ✨ What You Get

- 📶 **WiFi Access Point** (`OBD2 Master`) or **Station mode** on your own network, with optional static IP
- ⚡ **Real-time updates** over WebSocket (about every 100 ms)
- 📊 Live data with a **selectable PID list**, DTC read / clear, freeze frame, VIN / calibration IDs, 0–100 km/h timer
- 🔀 **Protocol selection** (Automatic, ISO 9141-2, KWP2000 slow / fast), saved to flash
- 🔄 **OTA firmware + web UI updates** from the browser (ESP32)
- 🔋 **Battery voltage** measurement with oversampling and smoothing
- 🔔 Buzzer melodies and LED for connection status

## 🧰 Requirements

| | |
|---|---|
| **Board** | ESP32, ESP32-S3, ESP32-C3, ESP32-C6 or ESP8266 |
| **Interface** | Any [K-Line interface schematic](../README.md#-interface-schematics) |
| **Arduino core** | [Espressif ESP32](https://github.com/espressif/arduino-esp32) or [ESP8266](https://github.com/esp8266/Arduino) board package |
| **Libraries** | [ESPAsyncWebServer](https://github.com/ESP32Async/ESPAsyncWebServer), [AsyncTCP](https://github.com/ESP32Async/AsyncTCP) (ESP32) or [ESPAsyncTCP](https://github.com/ESP32Async/ESPAsyncTCP) (ESP8266), [ArduinoJson](https://arduinojson.org/) v7 |

## 🛠️ Hardware Setup

Default pin mapping (edit the `#define`s at the top of `WebServer_Code.ino` to match your board):

| Signal | ESP32 / ESP32-S3 | ESP8266 |
|---|:---:|:---:|
| K-Line RX | **GPIO 10** (`Serial1`) | **GPIO 3** (UART0) |
| K-Line TX | **GPIO 11** (`Serial1`) | **GPIO 1** (UART0) |
| Status LED | GPIO 6 | GPIO 2 |
| Buzzer | GPIO 8 | GPIO 4 |
| Battery voltage (ADC) | GPIO 1 | GPIO 5 |

```cpp
#define K_Serial Serial1
#define K_line_RX 10
#define K_line_TX 11
#define Led 6
#define Buzzer 8
#define voltagePin 1
```

> [!NOTE]
> - On a classic **ESP32-WROOM**, GPIO 10 / 11 are used by the SPI flash, so move K-Line RX / TX to free pins (for example GPIO 16 / 17).
> - On **ESP8266**, K-Line uses UART0, which is shared with the USB-serial chip. Debug output is therefore disabled, and you shouldn't use the USB port while the K-Line interface is connected.

**Battery voltage** is read through a resistor divider (defaults **R1 = 47 kΩ**, **R2 = 10 kΩ**) from OBD-II pin 16 to the ADC pin. Adjust `R1`, `R2` and `CALIBRATION_FACTOR` in `WebServer_Code.ino` to match your parts.

## ⚙️ Installation

### 1. Flash the firmware

1. Open `WebServer_Code/WebServer_Code.ino` in the **Arduino IDE**.
2. Install the libraries listed above via *Library Manager*.
3. Select your board. For OTA updates on ESP32, pick a partition scheme that includes **OTA and SPIFFS** (the default 4 MB scheme works).
4. *(ESP32-S3 / C3 / C6)* Enable **USB CDC On Boot** if you want debug output on the native USB port.
5. Click **Upload**.

### 2. Upload the web dashboard to SPIFFS

The dashboard lives in [`data/`](data) (pre-gzipped HTML, CSS, JS and fonts). Upload it to the board's SPIFFS partition with one of these:

| Tool | How |
|---|---|
| **Arduino IDE 2.x** | Install the [arduino-spiffs-upload](https://github.com/espx-cz/arduino-spiffs-upload) plugin, then press `Ctrl` + `Shift` + `P` and choose **Upload SPIFFS to Pico/ESP8266/ESP32** |
| **Arduino IDE 1.8.x** | [ESP32 Sketch Data Upload tool](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) |
| **PlatformIO** | `pio run --target uploadfs` |

> [!IMPORTANT]
> If SPIFFS is missing or can't be mounted, the firmware halts and the LED blinks continuously. Upload the `data/` folder before first use.

### 3. Pre-built firmware *(optional)*

When pre-compiled binaries are published on the **[Releases](https://github.com/muki01/OBD2-K-Line-Reader/releases)** page, flash them with [esptool](https://docs.espressif.com/projects/esptool/) or the [ESP Web Flasher](https://espressif.github.io/esptool-js/).

## 📱 First Connection

1. Plug the device into the OBD-II port and switch the ignition **on**.
2. On your phone, join the WiFi network **`OBD2 Master`**. The password is **`12345678`**.
3. Open **http://192.168.4.1** in a browser.
4. The two icons in the header show the **WebSocket** and **vehicle** connection state (green means connected).

### Join your own WiFi network (Station mode)

In **Settings → Network Configuration**, enter your SSID and password (and optionally a static IP), then press **Update Network**. The device restarts and joins your network. If it can't connect within a few seconds, it falls back to Access Point mode automatically, so you never get locked out.

> [!TIP]
> Change `AP_password` in `WEB_SERVER.ino` before using the device regularly.

## 🧭 Using the Dashboard

| Page | What it does |
|---|---|
| **Main Menu** | Battery voltage (green > 12.6 V, yellow 12.0–12.6 V, red < 12.0 V) and navigation |
| **Live Data** | Real-time values of the PIDs selected in *Settings → Monitor Parameters* |
| **Error Codes** | Stored DTCs with descriptions, plus **Clear Error Codes** (Mode 04) |
| **Freeze Frame** | Sensor values the ECU captured when the fault code was stored |
| **Speed Test** | Press **Start** at standstill; the timer starts above 0 km/h and stops at 100 km/h |
| **Vehicle Info** | VIN, calibration ID, calibration version and supported PID maps |
| **Settings** | Theme, protocol, PID picker, WiFi and firmware update |

> [!TIP]
> K-Line is slow (10.4 kbaud), so every PID adds latency. **Select only the PIDs you need** in *Monitor Parameters* for the fastest Live Data refresh.

The protocol list in Settings also shows CAN options, because the dashboard is shared with the [OBD2 CAN Bus Reader](https://github.com/muki01/OBD2_CAN_Bus_Reader). This firmware uses the K-Line options: **Automatic**, **ISO 9141-2**, **ISO 14230-4 KWP (Slow)** and **ISO 14230-4 KWP (Fast)**.

## 🔄 OTA Updates (ESP32)

Open **Settings → System Maintenance**, choose the **Resource Pack (SPIFFS `.bin`)** and **Core Firmware (`.bin`)** files, then press **Update Firmware**. A progress bar shows the upload; when it finishes the device reboots, and you reconnect to its WiFi.

- Firmware binary: Arduino IDE → **Sketch → Export Compiled Binary**
- SPIFFS image: generated by your filesystem upload tool, or `pio run --target buildfs` in PlatformIO

## 🔌 API (for your own apps)

Everything the dashboard shows is available to your own apps and scripts:

| Endpoint | Type | Description |
|---|---|---|
| `ws://<device-ip>/ws` | WebSocket | Pushes a JSON frame about every 100 ms. Send `page0` … `page6` to choose which data set is streamed, `clear_dtc` to clear codes, `beep` to sound the buzzer |
| `GET /api/getData` | REST | One-shot JSON snapshot: selected live data, DTCs, voltage and protocol |
| `GET /api/clearDTCs` | REST | Clears stored trouble codes |
| `POST /protocolOptions` | Form | `protocol=Automatic \| ISO9141 \| ISO14230_Slow \| ISO14230_Fast` |
| `POST /pidSelect` | Form | Sets the PIDs shown in Live Data |
| `POST /wifiOptions` | Form | `SSID`, `WifiPassword`, `ipAddr`, `subnetMask`, `gateway` (device restarts) |

Example live-data frame (`page1`):

```json
{
  "LiveData": {
    "RPM":          { "value": 812,  "unit": "rpm" },
    "Coolant Temp": { "value": 88,   "unit": "°C" },
    "Engine Load":  { "value": 24.3, "unit": "%" }
  },
  "selectedProtocol": "Automatic",
  "connectedProtocol": "ISO14230_Fast",
  "Voltage": 14.08,
  "vehicleStatus": true
}
```

## 🩹 Troubleshooting

| Symptom | What to check |
|---|---|
| LED blinks forever after boot | SPIFFS not uploaded; see step 2 |
| `OBD2 Master` WiFi not visible | Wait about 5 s after power-up; the device first tries the saved Station network |
| Page loads but vehicle icon stays red | Ignition on? Pin 7 populated? Try forcing a protocol in Settings |
| Live Data updates slowly | Reduce the number of selected PIDs |
| Wrong battery voltage | Calibrate `R1`, `R2`, `CALIBRATION_FACTOR` |

## 🎨 Web UI Source

The dashboard front-end is maintained separately at **[OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI)**. The files in `data/` are its gzipped production build.
