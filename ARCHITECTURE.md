# Project Structure Documentation

## 📁 Professional Modular Architecture

The project has been refactored into a clean, professional modular structure for better maintainability, readability, and scalability.

### Directory Structure

```
src/
├── main.cpp                      # Main entry point (setup/loop)
│
├── config/                       # Configuration and type definitions
│   ├── config.h                 # Hardware pins, WiFi, task configuration
│   ├── system_types.h           # Enums, structs, global declarations
│   └── system_types.cpp         # Global variable definitions & helpers
│
├── hardware/                     # Hardware abstraction layer
│   ├── hardware_manager.h       # Hardware object declarations
│   └── hardware_manager.cpp     # DHT20, LCD, NeoPixel, semaphore init
│
├── tasks/                        # FreeRTOS task implementations
│   ├── tasks.h                  # Task function declarations
│   └── tasks.cpp                # All 5 task implementations
│
└── web/                          # Web server components
    ├── web_server.h             # Web server function declarations
    ├── web_server.cpp           # HTTP handlers & WiFi management
    └── web_pages.h              # HTML/CSS/JavaScript dashboard
```

## 🎯 Module Responsibilities

### 1. **config/** - Configuration Layer
- **config.h**: All hardware pin definitions, WiFi credentials, task stack sizes, priorities, and timing constants
- **system_types.h**: Type definitions (TempBand, HumBand), global state structure (LiveState), function prototypes
- **system_types.cpp**: Global variable implementations and helper functions (classifyTemp, bandToBlink, etc.)

### 2. **hardware/** - Hardware Abstraction Layer
- **hardware_manager.h**: External declarations for hardware objects
- **hardware_manager.cpp**: 
  - Hardware object instantiation (DHT20, LCD, NeoPixels)
  - Semaphore creation and initialization
  - Hardware initialization functions

### 3. **tasks/** - Application Logic Layer
- **tasks.h**: Public task function declarations
- **tasks.cpp**: Complete implementations of all 5 FreeRTOS tasks:
  - `task_read_dht20()` - Sensor reading & band classification
  - `task_led()` - Temperature-based LED blinking
  - `task_neopixel_hum()` - Humidity indicator (GPIO 45)
  - `task_neopixel_ui()` - User interface bar (GPIO 6)
  - `task_lcd()` - Display updates

### 4. **web/** - Web Interface Layer
- **web_server.h**: Public web server API
- **web_server.cpp**: 
  - WiFi initialization (AP/STA modes)
  - HTTP route handlers (/state, /set, /ui/*, /wifi)
  - Server lifecycle management
- **web_pages.h**: Complete HTML dashboard with CSS and JavaScript

### 5. **main.cpp** - Application Entry Point
- Clean, minimal setup() and loop()
- Orchestrates initialization of all modules
- Handles web server in main loop

## 🔧 Benefits of This Architecture

### ✅ Separation of Concerns
Each module has a single, well-defined responsibility:
- Config files only define constants and types
- Hardware layer only manages physical components
- Tasks layer only implements business logic
- Web layer only handles HTTP communication

### ✅ Maintainability
- Easy to find and modify specific functionality
- Changes to one module don't affect others
- Clear dependencies between modules

### ✅ Testability
- Individual modules can be tested in isolation
- Mock hardware interfaces for unit testing
- Easy to add new tasks without modifying existing code

### ✅ Scalability
- Add new sensors by extending hardware layer
- Add new tasks without touching others
- Add new web endpoints in web_server.cpp

### ✅ Readability
- main.cpp is < 70 lines (was 676 lines!)
- Each file has a single, clear purpose
- Comprehensive documentation and comments

## 📝 Compilation

All files are automatically compiled by PlatformIO. The build system:
1. Compiles all `.cpp` files in `src/` and subdirectories
2. Links them together
3. Generates final firmware binary

No changes needed to `platformio.ini` - it works out of the box!

## 🚀 Migration Guide

### Old Code Location → New Location

| Old Location | New Location | Description |
|-------------|-------------|-------------|
| Lines 1-100 in main.cpp | config/config.h + system_types.h | Pin definitions, enums, types |
| Lines 100-135 in main.cpp | config/system_types.cpp | Helper functions |
| Lines 135-350 in main.cpp | web/web_pages.h | HTML dashboard |
| Lines 350-460 in main.cpp | web/web_server.cpp | HTTP handlers |
| Lines 460-550 in main.cpp | tasks/tasks.cpp (task_read_dht20) | Sensor task |
| Lines 550-615 in main.cpp | tasks/tasks.cpp (task_led) | LED task |
| Lines 615-660 in main.cpp | tasks/tasks.cpp (task_neopixel_*) | NeoPixel tasks |
| Lines 660-676 in main.cpp | main.cpp (setup/loop) | Main entry point |

### What Changed?

**Nothing functionally!** The code does exactly the same thing, just organized better:

- ✅ All tasks work identically
- ✅ Web dashboard unchanged
- ✅ Same pin assignments
- ✅ Same behavior and performance
- ✅ Only structure improved

## 🔍 How to Navigate

**To change hardware pins:**
→ Edit `config/config.h`

**To modify temperature/humidity thresholds:**
→ Edit `config/config.h` (defaults) or use web dashboard (runtime)

**To add a new task:**
1. Declare in `tasks/tasks.h`
2. Implement in `tasks/tasks.cpp`
3. Call from `createAllTasks()` in `tasks/tasks.cpp`

**To add a new web endpoint:**
1. Declare handler in `web/web_server.cpp`
2. Register route in `initWebServer()`

**To modify dashboard UI:**
→ Edit `web/web_pages.h`

## 📊 File Size Comparison

| File | Old (monolithic) | New (modular) | Change |
|------|-----------------|---------------|--------|
| main.cpp | 676 lines | 67 lines | ✅ -90% |
| config/* | 0 lines | ~150 lines | New module |
| hardware/* | 0 lines | ~50 lines | New module |
| tasks/* | 0 lines | ~250 lines | New module |
| web/* | 0 lines | ~350 lines | New module |

**Result**: Same functionality, better organized, easier to maintain!

## 🎓 Best Practices Applied

- ✅ Single Responsibility Principle
- ✅ Separation of Concerns
- ✅ DRY (Don't Repeat Yourself)
- ✅ Clear naming conventions
- ✅ Comprehensive documentation
- ✅ Modular architecture
- ✅ Header guards on all headers
- ✅ Forward declarations where appropriate
- ✅ Const correctness
- ✅ PROGMEM for large strings

---

**Old main.cpp has been preserved as `main_old.cpp` for reference.**
