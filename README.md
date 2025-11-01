# ESP32-S3 Multi-Task Environmental Monitoring System

## 📋 Project Overview
A professional FreeRTOS-based environmental monitoring system for ESP32-S3 featuring modular architecture, semaphore synchronization, and real-time web dashboard. Demonstrates advanced multitasking capabilities with producer-consumer pattern implementation.

## ✨ Features
- **Task 1 (Sensor Producer)**: DHT20 I2C sensor reading with automatic band classification and semaphore signaling
- **Task 2 (LED Consumer)**: Temperature-responsive LED blinking patterns (4 modes: COLD, NORMAL, HOT, CRITICAL)
- **Task 3 (NeoPixel Consumer)**: Humidity indicator with 4 color bands (DRY=Blue, COMFORT=Green, HUMID=Yellow, WET=Red)
- **Task 4 (UI Independent)**: 4-pixel NeoPixel bar with 3 modes (OFF, BAR, DEMO)
- **Task 5 (LCD Consumer)**: Real-time display with temperature, humidity, and system status
- **Web Dashboard**: WiFi Access Point with live monitoring, threshold configuration, and mode control
- **Semaphore Synchronization**: Producer-consumer pattern with 3 binary semaphores
- **Multi-core Processing**: Task distribution across ESP32-S3 dual cores (Xtensa LX7)

## 🔧 Hardware Requirements

| Component | Model/Type | Connection | Notes |
|-----------|------------|------------|-------|
| Microcontroller | ESP32-S3 (YOLO UNO) | USB-C | Dual-core Xtensa LX7 @ 240MHz |
| Temperature/Humidity | DHT20 (AHT20) | I2C (SDA: GPIO 11, SCL: GPIO 12) | Digital I2C sensor, Address 0x38 |
| Status LED | Built-in LED | GPIO 48 | Temperature band indicator |
| Humidity NeoPixel | WS2812B (1 pixel) | GPIO 45 | Humidity band visualization |
| UI NeoPixel Bar | WS2812B (4 pixels) | GPIO 6 | User interface bar |
| LCD Display | 16x2 I2C LCD | I2C Address: 0x21 | Real-time status display |

## 📌 Pin Configuration

```cpp
// Digital GPIO Pins
#define LED_GPIO            48      // Built-in LED (temperature indicator)
#define NEOPIXEL_HUM_PIN    45      // Single NeoPixel (humidity indicator)
#define NEOPIXEL_HUM_NUM    1       // Number of humidity pixels
#define NEOPIXEL_UI_PIN     6       // 4-pixel NeoPixel bar (UI)
#define NEOPIXEL_UI_NUM     4       // Number of UI pixels

// I2C Communication
#define SDA_PIN             11      // I2C Data line
#define SCL_PIN             12      // I2C Clock line

// I2C Device Addresses
// DHT20:                 0x38   (hardcoded in sensor library)
#define LCD_I2C_ADDR        0x21   // LCD I2C address
#define LCD_COLS            16     // LCD columns
#define LCD_ROWS            2      // LCD rows
```

## 📁 Project Structure (Modular Architecture)
```
MidtermProject_ESP32_PlatformIO/
│
├── platformio.ini                  # PlatformIO build configuration
├── README.md                       # Project documentation (this file)
├── ARCHITECTURE.md                 # Detailed architecture documentation
├── .gitignore                      # Git ignore rules
│
└── src/
    ├── main.cpp                    # Application entry point (setup/loop)
    │
    ├── config/                     # Configuration Layer
    │   ├── config.h                # Hardware pins, WiFi, task config
    │   ├── system_types.h          # Type definitions & global declarations
    │   └── system_types.cpp        # Global variables & helper functions
    │
    ├── hardware/                   # Hardware Abstraction Layer
    │   ├── hardware_manager.h      # Hardware object declarations
    │   └── hardware_manager.cpp    # DHT20, LCD, NeoPixel, semaphore init
    │
    ├── tasks/                      # Application Logic Layer (3 files)
    │   ├── tasks.h                 # Task declarations & semaphore diagram
    │   ├── tasks.cpp               # Task creation & management
    │   ├── task1_sensor.cpp        # Task 1: Sensor reading (PRODUCER)
    │   ├── task2_led_neopixel.cpp  # Task 2-4: LED & NeoPixels (CONSUMERS)
    │   └── task3_lcd.cpp           # Task 5: LCD display (CONSUMER)
    │
    └── web/                        # Web Interface Layer
        ├── web_server.h            # Web server function declarations
        ├── web_server.cpp          # HTTP handlers & WiFi management
        └── web_pages.h             # HTML dashboard (CSS/JS included)
```

### 📊 Semaphore Synchronization Flow

```
Task 1 (Sensor) ──┬──> semBandChanged ──> Task 2 (LED)
    PRODUCER      ├──> semHumChanged  ──> Task 3 (NeoPixel Humidity)
                  └──> semLcdUpdate   ──> Task 5 (LCD Display)

Task 4 (NeoPixel UI) runs independently (no semaphore)
```

---

## 🚀 Getting Started

### Prerequisites
1. **Install PlatformIO**:
   - **VS Code Extension** (Recommended):
     - Open VS Code
     - Go to Extensions (Ctrl+Shift+X)
     - Search for "PlatformIO IDE"
     - Click Install
   
   - **PlatformIO CLI**:
     ```bash
     pip install platformio
     ```

2. **Install Drivers**:
   - ESP32-S3 USB Driver: [CP210x Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
   - Or CH340 Driver if applicable

### 📥 Upload Instructions

#### Method 1: Using VS Code PlatformIO Extension

1. **Open Project**:
   ```
   File → Open Folder → Select "MidtermProject_ESP32_PlatformIO"
   ```

2. **Connect ESP32**:
   - Connect ESP32-S3 to computer via USB cable
   - Wait for driver recognition

3. **Upload Code**:
   - Click the **→ (Upload)** button on the bottom toolbar
   - Or press `Ctrl+Alt+U`
   - Or open PlatformIO menu → Upload

4. **Monitor Serial Output**:
   - Click the **🔌 (Serial Monitor)** button
   - Or press `Ctrl+Alt+S`
   - Set baud rate to **115200**

#### Method 2: Using PlatformIO CLI

1. **Navigate to Project Directory**:
   ```bash
   cd "c:\Users\Hoai Nhan\Documents\Embedded Systems\MidtermProject_ESP32_PlatformIO"
   ```

2. **Build Project**:
   ```bash
   pio run
   ```

3. **Upload to ESP32**:
   ```bash
   pio run --target upload
   ```

4. **Monitor Serial Output**:
   ```bash
   pio device monitor
   ```

5. **Build + Upload + Monitor (All-in-one)**:
   ```bash
   pio run --target upload && pio device monitor
   ```

#### Method 3: Using PlatformIO Core Tasks (VS Code)

1. Press `Ctrl+Shift+P`
2. Type "PlatformIO: Upload"
3. Select from available tasks:
   - `PlatformIO: Build` - Compile only
   - `PlatformIO: Upload` - Upload to device
   - `PlatformIO: Upload and Monitor` - Upload + open serial monitor
   - `PlatformIO: Clean` - Clean build files

### 🔍 Troubleshooting Upload Issues

#### Issue 1: Port Not Detected
```bash
# List all connected devices
pio device list

# Manually specify port in platformio.ini
upload_port = COM3  # Windows
upload_port = /dev/ttyUSB0  # Linux
upload_port = /dev/cu.usbserial-*  # macOS
```

#### Issue 2: Permission Denied (Linux/macOS)
```bash
# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER
sudo chmod 666 /dev/ttyUSB0

# Then logout and login again
```

#### Issue 3: Upload Failed - Flash Size Mismatch
- Check `platformio.ini` board settings
- Ensure correct board type: `esp32-s3-devkitc-1`
- Verify flash size matches your hardware

#### Issue 4: Stuck at "Connecting..."
1. **Hold BOOT button** on ESP32-S3
2. Click **Upload**
3. Release BOOT button when "Connecting..." appears
4. Wait for upload to complete

#### Issue 5: Serial Monitor Shows Garbage
- Set correct baud rate: **115200**
- In VS Code: Check bottom toolbar
- In CLI: `pio device monitor -b 115200`

### 📊 Verify Upload Success

After successful upload, you should see in the **Serial Monitor (115200 baud)**:

```
╔════════════════════════════════════════════════╗
║   ESP32-S3 Multi-Task Environmental Monitor    ║
║   FreeRTOS + DHT20 + LCD + NeoPixel + Web      ║
╚════════════════════════════════════════════════╝

=== System Initialization ===
[HW] Hardware initialized successfully
[SYNC] Semaphores created successfully
[WiFi] AP IP: 192.168.4.1
[WEB] Web server started on port 80

[TASKS] Creating FreeRTOS tasks...
[TASK1] DHT20 sensor task started
[TASK2] LED control task started
[TASK3] NeoPixel humidity indicator started
[TASK4] NeoPixel UI bar started
[TASK5] LCD display task started

╔════════════════════════════════════════════════╗
║              System Ready                      ║
║  Connect to: ESP32-S3-LAB (password: 12345678)║
║  Dashboard: http://192.168.4.1                 ║
╚════════════════════════════════════════════════╝
```

**Expected Behavior:**
- ✅ **Task 1**: DHT20 readings every 500ms, semaphores given when bands change
- ✅ **Task 2**: LED blinking pattern changes based on temperature band
- ✅ **Task 3**: Single NeoPixel (GPIO 45) color changes with humidity
- ✅ **Task 4**: 4-pixel bar (GPIO 6) shows OFF/BAR/DEMO mode
- ✅ **Task 5**: LCD displays temperature, humidity, and status
- ✅ **Web Server**: Dashboard accessible at http://192.168.4.1

### 🌐 Access Web Dashboard

1. **Connect to WiFi Access Point:**
   - SSID: `ESP32-S3-LAB`
   - Password: `12345678`

2. **Open Browser:**
   - Navigate to: `http://192.168.4.1`

3. **Dashboard Features:**
   - 📊 Real-time sensor data (temperature & humidity)
   - 🎨 Live humidity bar visualization
   - ⚙️ Adjust temperature/humidity thresholds
   - 🎛️ Control NeoPixel UI modes (OFF/BAR/DEMO)
   - 📡 WiFi mode switching (AP/Station)
   - 📈 Task execution statistics

---

## 🎯 Task Details & Semaphore Usage

### Task 1: DHT20 Sensor (Producer - Core 1, Priority 3)
- Reads sensor every 500ms
- Classifies into bands: `COLD/NORMAL/HOT/CRITICAL` (temp), `DRY/COMFORT/HUMID/WET` (humidity)
- **Gives semaphores:** `semBandChanged`, `semHumChanged`, `semLcdUpdate`

### Task 2: LED Controller (Consumer - Core 0, Priority 2)
- **Takes semaphore:** `semBandChanged`
- Blink patterns: COLD (1s/1s), NORMAL (300ms/300ms), HOT (120ms/120ms), CRITICAL (always ON)

### Task 3: NeoPixel Humidity (Consumer - Core 0, Priority 2)
- **Takes semaphore:** `semHumChanged`
- Colors: DRY (Blue), COMFORT (Green), HUMID (Yellow), WET (Red)

### Task 4: NeoPixel UI Bar (Independent - Core 0, Priority 1)
- **No semaphore** (runs independently)
- Modes: OFF, BAR (humidity %), DEMO (rainbow)

### Task 5: LCD Display (Consumer - Core 0, Priority 1)
- **Takes semaphore:** `semLcdUpdate`
- Displays: "T:25.5C H:55%" / "STATE: OK/WARN/CRIT"

---

## 📦 Dependencies (Managed by PlatformIO)

```ini
lib_deps = 
    robtillaart/DHT20@^0.3.1           # DHT20/AHT20 I2C sensor
    adafruit/Adafruit NeoPixel@^1.15.2 # WS2812B LED control
    marcoschwartz/LiquidCrystal_I2C@^1.1.4  # I2C LCD driver
```

Built-in libraries (no installation needed):
- `WiFi` - ESP32 WiFi support
- `WebServer` - HTTP server
- `Wire` - I2C communication
- `FreeRTOS` - Multitasking (included in ESP32 Arduino core)

---

## 📝 Deliverables

* **PDF Report** – Detailed design, implementation, results, and team contributions
* **GitHub Repository** – Fully commented, modular source code (this repository)
* **Live Demo** – Working hardware demonstration with web dashboard

---

## 👥 Team

* **Vo Phuc Thien** – Head Engineer, Hardware Integration
* **Tran Hoai Nhan** – Project Manager, Software Architecture

---

## 📚 Quick Reference

### Serial Monitor Commands
```bash
# View output
pio device monitor -b 115200

# Filter task messages
pio device monitor -b 115200 | findstr "TASK"
```

### Common Configuration Changes

**Change LCD Address** (if different):
```cpp
// src/config/config.h
#define LCD_I2C_ADDR 0x21  // Change to your LCD address
```

**Adjust Temperature Thresholds** (runtime via web or compile-time):
```cpp
// src/config/config.h
#define DEFAULT_T_COLD_MAX   20.0f  // Below this = COLD
#define DEFAULT_T_NORMAL_MAX 30.0f  // Below this = NORMAL
#define DEFAULT_T_HOT_MAX    40.0f  // Below this = HOT, above = CRITICAL
```

**Change WiFi Credentials**:
```cpp
// src/config/config.h
#define AP_SSID_DEFAULT "ESP32-S3-LAB"
#define AP_PASS_DEFAULT "12345678"
```

### File Modification Guide
- **Add/modify pins**: `src/config/config.h`
- **Change task logic**: `src/tasks/task1_sensor.cpp`, `task2_led_neopixel.cpp`, `task3_lcd.cpp`
- **Modify web UI**: `src/web/web_pages.h`
- **Add HTTP endpoints**: `src/web/web_server.cpp`
- **Adjust thresholds**: Web dashboard or `src/config/system_types.cpp`

---

## 🔗 Related Documentation
- [ARCHITECTURE.md](ARCHITECTURE.md) - Detailed modular architecture explanation
- [PlatformIO Docs](https://docs.platformio.org/)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)

---

**Last Updated:** October 31, 2025  
**Version:** 2.0 - Modular Architecture with 3 Task Files

