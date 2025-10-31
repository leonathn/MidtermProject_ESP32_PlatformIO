# ESP32 Multi-Task Temperature & Humidity Monitor

## 📋 Project Overview
A professional FreeRTOS-based monitoring system for ESP32-S3 that demonstrates multitasking capabilities with semaphore synchronization. The system monitors temperature and humidity using a DHT20 sensor and provides real-time feedback through multiple output devices.

## ✨ Features
- **Task 1 (Sensor)**: DHT20 sensor reading with automatic change detection
- **Task 2 (LED)**: Temperature-responsive LED blinking patterns (3 modes)
- **Task 3 (Display)**: Dual output - LCD status display and humidity-based NeoPixel color control
- **Task 4 (Web Server)**: WiFi AP dashboard with device control and real-time monitoring
- **Synchronization**: Efficient semaphore-based inter-task communication
- **Multi-core Processing**: Optimized task distribution across ESP32-S3 dual cores

## 🔧 Hardware Requirements

| Component | Model/Type | Connection | Notes |
|-----------|------------|------------|-------|
| Microcontroller | ESP32-S3 (YOLO UNO) | - | Dual-core Xtensa LX7 |
| Temperature/Humidity | DHT20 | I2C (SDA: GPIO 11, SCL: GPIO 12) | Digital I2C sensor |
| Status LED | Standard LED | GPIO 48 | Temperature indicator |
| RGB LED | NeoPixel WS2812B | GPIO 45 | Humidity visualization |
| LCD Display | 16x2 I2C LCD | I2C Address: 0x27 | System status display |

## 📌 Pin Configuration

```cpp
// Digital GPIO Pins
#define LED_PIN         48      // Status LED output
#define NEOPIXEL_PIN    45      // NeoPixel RGB LED data
#define NUM_PIXELS      1       // Single NeoPixel LED

// I2C Communication
#define SDA_PIN         11      // I2C Data line
#define SCL_PIN         12      // I2C Clock line

// I2C Device Addresses
// DHT20:              0x38    (hardcoded in sensor)
#define LCD_ADDR        0x27    // LCD I2C address
#define LCD_COLS        16      // LCD columns
#define LCD_ROWS        2       // LCD rows
```

## 📁 Project Structure
```
MidtermProject_ESP32_PlatformIO/
│
├── platformio.ini                  # Build configuration
├── README.md                       # This documentation
├── .gitignore                      # Git ignore rules
│
└── src/
    ├── main.cpp                    # Application entry point
    │
    ├── core/
    │   ├── task_manager.h          # Task management interface
    │   └── task_manager.cpp        # Task initialization & lifecycle
    │
    └── modules/
        ├── config/
        │   ├── pins.h              # Hardware pin definitions
        │   └── types.h             # Shared data structures
        │
        └── tasks/
            ├── tasks.h             # Task function declarations
            ├── task1.cpp           # DHT20 Sensor Reading
            ├── task2.cpp           # LED Temperature Indicator
            └── task3.cpp           # LCD & NeoPixel Display
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

After successful upload, you should see:
- Task 1: DHT20 sensor readings in serial monitor
- Task 2: LED blinking according to temperature
- Task 3: LCD displaying status, NeoPixel color changing with humidity
- Task 4: Web server running, accessible via ESP32 AP

---

## Deliverables

* **PDF Report** – detailing design, implementation, results, and member contributions.
* **GitHub Repository** – fully commented source code (this repository).

## Team

* **[Vo Phuc Thien]** – Head Engineer
* **[Tran Hoai Nhan]** – Project Manager, Assistant Engineer


---

