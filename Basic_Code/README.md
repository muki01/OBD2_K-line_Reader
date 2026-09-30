# 🖥️ Basic_Code: OBD2 K-Line Reader for the Serial Monitor

A lightweight **Arduino / ESP32** sketch that talks to your car's ECU over **K-Line** (ISO 9141-2 and KWP2000 / ISO 14230) and prints live sensor data, trouble codes, freeze-frame data and supported PIDs to the **Serial Monitor**.

Use it to verify your K-Line interface, learn the protocol, or as a starting point for your own firmware. For the WiFi dashboard, see **[WebServer_Code](../WebServer_Code/README.md)**.

← [Back to the main README](../README.md)

---

## 📡 How It Works

The sketch uses **two UARTs**:

| UART | Purpose | Arduino (AVR) | ESP32 |
|---|---|---|---|
| **Main** (`Serial`) | USB link to your computer / Serial Monitor | Hardware `Serial` | Hardware / USB-CDC `Serial` |
| **K-Line** (`K_Serial`) | Vehicle communication at 10.4 kbaud through the interface circuit | `AltSoftSerial` (software UART) | Hardware `Serial1` |

On start-up the firmware wakes the ECU (5-baud or fast init), detects the protocol and then keeps polling. It reconnects automatically if the link drops.

## 🛠️ Hardware Setup

Build one of the [K-Line interface schematics](../README.md#-interface-schematics) and connect its **RX** and **TX** to:

| Board | K-Line RX | K-Line TX | Status LED | Notes |
|---|:---:|:---:|:---:|---|
| Arduino Uno / Nano / Pro Mini | **D8** | **D9** | D13 | Pins are fixed by AltSoftSerial |
| ESP32-S3 / C3 / C6 | **GPIO 10** | **GPIO 11** | GPIO 6 | Configurable in `Basic_Code.ino` |

> [!NOTE]
> On a classic **ESP32-WROOM**, GPIO 10 / 11 are connected to the SPI flash. Change `K_line_RX` / `K_line_TX` to free pins (for example GPIO 16 / 17).

Connect the interface to **OBD-II pin 7** (K-Line), **pin 16** (+12 V) and **pins 4 / 5** (GND). Full pinout is in the [main README](../README.md#obd-ii-connector-pinout-k-line).

## ⚙️ Installation

### Option 1: Arduino IDE

1. Install the **[Arduino IDE](https://www.arduino.cc/en/software)** (2.x recommended).
2. For ESP32, add the **Espressif ESP32** board package via *Boards Manager*.
3. For Arduino AVR boards, install **AltSoftSerial** via *Library Manager*.
4. Open `Basic_Code/Basic_Code.ino`. The other `.ino` files in the folder open as tabs automatically.
5. Select your board and port, then click **Upload**.
6. Open the **Serial Monitor** at **9600 baud**.

### Option 2: Pre-built firmware

When pre-compiled binaries are published on the **[Releases](https://github.com/muki01/OBD2_K-line_Reader/releases)** page, you can flash them directly with [esptool](https://docs.espressif.com/projects/esptool/) or the [ESP Web Flasher](https://espressif.github.io/esptool-js/).

## 🔧 Configuration

Everything is at the top of `Basic_Code.ino`:

```cpp
String selectedProtocol = "Automatic";   // "Automatic", "ISO9141", "ISO14230_Slow", "ISO14230_Fast"

int _byteWriteInterval = 5;   // Delay between transmitted bytes (5 - 20 ms)
int _interByteTimeout  = 60;  // Wait after the last received byte (55 - 5000 ms)
int _readTimeout       = 1000;
```

- Leave `selectedProtocol` on **`Automatic`** unless you already know your car's protocol. Forcing it skips the detection step and connects faster.
- Comment out `#define DEBUG_Serial` to disable all serial output.

## 📋 What Gets Printed

Each polling cycle prints:

- **Live data**: vehicle speed, engine RPM, coolant temperature, intake air temperature, throttle position, timing advance, engine load, MAF flow rate
- **Stored DTCs** (Mode 03) and **pending DTCs** (Mode 07)
- **Freeze frame** (Mode 02): speed, RPM, coolant temperature and engine load at the time of the fault
- **Supported PIDs** for live data, freeze frame and vehicle info

## 🖼️ Example Output

Sample Serial Monitor output from an **ISO 9141-2** vehicle:

<img src="https://github.com/user-attachments/assets/0ca043ea-6152-40a5-8b22-c516e84031bb" alt="Serial Monitor output of the OBD2 K-Line Reader showing live data and DTCs from an ISO 9141-2 car" width="320">

## 🩹 Troubleshooting

| Symptom | What to check |
|---|---|
| No response / init fails | Ignition **on** (engine can be off), pin 7 populated, common ground, interface RX/TX not swapped |
| Garbage characters | Serial Monitor must be at **9600 baud** |
| Connects then drops | Increase `_interByteTimeout`, check the 12 V supply of the interface |
| ESP32 reboots or won't boot | Avoid flash / strapping pins for K-Line RX/TX |
