/**
 * @file hardware_manager.cpp
 * @brief Hardware Abstraction Layer - Device initialization and management
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This file implements hardware initialization and provides global access to
 * peripheral objects. It abstracts the hardware layer from application logic.
 * 
 * Managed Hardware:
 * - DHT20 temperature/humidity sensor (I2C)
 * - 16x2 LCD display (I2C)
 * - NeoPixel LED strips (WS2812B)
 * - FreeRTOS synchronization primitives (binary semaphores)
 */

#include "hardware_manager.h"

/* ====== Hardware Object Definitions ====== */

/**
 * @brief DHT20 sensor object
 * @details I2C temperature and humidity sensor
 *          - Default address: 0x38
 *          - Resolution: 0.01°C, 0.024% RH
 *          - Update rate: 2 Hz maximum
 */
DHT20 dht;

/**
 * @brief LCD display object (16x2 characters)
 * @details I2C LCD with PCF8574 I/O expander
 *          - Address: 0x21 (configurable via LCD_I2C_ADDR)
 *          - Display: 16 columns × 2 rows
 *          - Backlight: Controlled via I2C
 */
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

/**
 * @brief NeoPixel humidity indicator (single pixel on GPIO 45)
 * @details WS2812B RGB LED for humidity level indication
 *          - Color coding: Blue (dry) → Green (comfort) → Yellow (humid) → Red (wet)
 *          - Protocol: NEO_GRB (Green-Red-Blue bit order)
 *          - Frequency: 800 KHz (NEO_KHZ800)
 */
Adafruit_NeoPixel stripHum(NEOPIXEL_HUM_NUM, NEOPIXEL_HUM_PIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief NeoPixel UI bar (4 pixels on GPIO 6)
 * @details WS2812B RGB LED strip for user interface
 *          - Modes: OFF, BAR (humidity graph), DEMO (rainbow), SOS, BLINK
 *          - Protocol: NEO_GRB (Green-Red-Blue bit order)
 *          - Frequency: 800 KHz (NEO_KHZ800)
 */
Adafruit_NeoPixel stripUI(NEOPIXEL_UI_NUM, NEOPIXEL_UI_PIN, NEO_GRB + NEO_KHZ800);

/* ====== Semaphore Definitions ====== */

/**
 * @brief Binary semaphore for temperature band changes
 * @details Task 1 (sensor) gives this when temperature band changes
 *          Task 2 (LED) takes this to update blink pattern
 *          Flow: Task1 → semBandChanged → Task2
 */
SemaphoreHandle_t semBandChanged = NULL;

/**
 * @brief Binary semaphore for humidity band changes
 * @details Task 1 (sensor) gives this when humidity band changes
 *          Task 3 (NeoPixel) takes this to update color
 *          Flow: Task1 → semHumChanged → Task3
 */
SemaphoreHandle_t semHumChanged = NULL;

/**
 * @brief Binary semaphore for LCD updates
 * @details Task 1 (sensor) gives this on every reading (500ms)
 *          Task 5 (LCD) takes this to refresh display
 *          Flow: Task1 → semLcdUpdate → Task5
 */
SemaphoreHandle_t semLcdUpdate = NULL;

/* ====== Initialization Functions ====== */

/**
 * @brief Initialize all hardware peripherals
 * @details Performs sequential initialization of:
 *          1. I2C bus (for DHT20 and LCD)
 *          2. DHT20 temperature/humidity sensor
 *          3. LCD display with startup message
 *          4. Both NeoPixel strips (clear to black)
 * 
 * @note Call this function once during system setup before creating tasks
 * @warning Failure to initialize hardware may cause task crashes
 */
void initHardware() {
    // Initialize I2C bus with custom pins (SDA=11, SCL=12)
    // Both DHT20 and LCD share this bus
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // Initialize DHT20 sensor
    // Sensor needs ~2 seconds warm-up time for accurate readings
    dht.begin();
    
    // Initialize LCD display
    lcd.init();                    // Initialize LCD driver
    lcd.backlight();               // Turn on backlight (via PCF8574 pin)
    lcd.clear();                   // Clear any garbage characters
    lcd.setCursor(0, 0);           // Position cursor at top-left (column 0, row 0)
    lcd.print("ESP32-S3 LAB");     // Display startup message
    
    // Initialize NeoPixel strips
    // begin() configures GPIO pins for WS2812B protocol
    // show() clears all LEDs to black (off state)
    stripHum.begin();              // Initialize humidity indicator (GPIO 45)
    stripHum.show();               // Clear to black
    
    stripUI.begin();               // Initialize UI bar (GPIO 6)
    stripUI.show();                // Clear to black
    
    Serial.println("[HW] Hardware initialized successfully");
}

/**
 * @brief Initialize FreeRTOS binary semaphores for inter-task communication
 * @details Creates three binary semaphores:
 *          - semBandChanged: Temperature band change notification
 *          - semHumChanged: Humidity band change notification
 *          - semLcdUpdate: LCD display update trigger
 * 
 * Binary semaphores behavior:
 * - Initial state: Empty (must be given before taking)
 * - xSemaphoreGive(): Signal event (producer)
 * - xSemaphoreTake(): Wait for event (consumer)
 * 
 * @note Call this after hardware initialization but before task creation
 * @warning If creation fails, system will log error but continue (tasks will hang)
 */
void initSemaphores() {
    // Create binary semaphores (not counting semaphores)
    // Binary semaphores can be given/taken only once before needing to be reset
    semBandChanged = xSemaphoreCreateBinary();
    semHumChanged  = xSemaphoreCreateBinary();
    semLcdUpdate   = xSemaphoreCreateBinary();
    
    // Verify all semaphores were created successfully
    // NULL indicates memory allocation failure
    if (semBandChanged == NULL || semHumChanged == NULL || semLcdUpdate == NULL) {
        Serial.println("[ERROR] Failed to create semaphores!");
        // System continues but tasks will hang on xSemaphoreTake()
    } else {
        Serial.println("[SYNC] Semaphores created successfully");
    }
}
