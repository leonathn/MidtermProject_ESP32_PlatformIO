/**
 * @file tasks.cpp
 * @brief Task Management - Creates and manages all FreeRTOS tasks
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This file is responsible for creating all FreeRTOS tasks that run concurrently.
 * Task implementations are split into separate files for better organization:
 * - task1_sensor.cpp: DHT20 sensor reading (data producer, signals semaphores)
 * - task2_led_neopixel.cpp: LED + NeoPixel control (data consumers, wait for semaphores)
 * - task3_lcd.cpp: LCD display (data consumer, waits for semaphore)
 * - task5_tinyml.cpp: TinyML inference (independent processor)
 * 
 * All tasks are pinned to APP_CPU_NUM (tskNO_AFFINITY) for automatic load balancing.
 */

#include "tasks.h"
#include "../config/config.h"
#include "../ml/tinyml.h"

/* ====== Task Function Prototypes ====== */

/**
 * @brief Task 1: DHT20 sensor reading and band classification
 * @note Implemented in task1_sensor.cpp
 */
extern void task_read_dht20(void* pv);

/**
 * @brief Task 2: LED temperature indicator with variable blink rate
 * @note Implemented in task2_led_neopixel.cpp
 */
extern void task_led(void* pv);

/**
 * @brief Task 3: NeoPixel humidity indicator (single LED color)
 * @note Implemented in task2_led_neopixel.cpp
 */
extern void task_neopixel_hum(void* pv);

/**
 * @brief Task 4: NeoPixel UI bar (4-LED strip with multiple modes)
 * @note Implemented in task2_led_neopixel.cpp
 */
extern void task_neopixel_ui(void* pv);

/**
 * @brief Task 5: LCD display (shows sensor data and system status)
 * @note Implemented in task3_lcd.cpp
 */
extern void task_lcd(void* pv);

/* ====== Task Creation ====== */

/**
 * @brief Create all FreeRTOS tasks for the system
 * @details Creates 6 concurrent tasks:
 *          1. DHT20 sensor (Priority 3 - highest)
 *          2. LED control (Priority 2)
 *          3. NeoPixel humidity (Priority 2)
 *          4. NeoPixel UI bar (Priority 1)
 *          5. LCD display (Priority 1)
 *          6. TinyML inference (Priority 1)
 * 
 * @note Tasks are pinned using xTaskCreatePinnedToCore() with APP_CPU_NUM
 * @note All tasks run in infinite loops and never return
 * @warning Call this AFTER hardware and semaphore initialization
 * @warning Do not call this multiple times (will create duplicate tasks)
 */
void createAllTasks() {
    Serial.println("[TASKS] Creating FreeRTOS tasks...");
    
    // Task 1: DHT20 Sensor Reader (highest priority - data source)
    // Reads temperature/humidity every 500ms and signals dependent tasks
    xTaskCreatePinnedToCore(
        task_read_dht20,          // Task function
        "DHT20",                  // Task name (for debugging)
        TASK_DHT_STACK_SIZE,      // Stack size: 4096 bytes
        nullptr,                  // Task parameter (unused)
        TASK_DHT_PRIORITY,        // Priority: 3 (highest)
        nullptr,                  // Task handle (not needed)
        APP_CPU_NUM               // CPU core: tskNO_AFFINITY (auto-balance)
    );
    
    // Task 2: LED Temperature Indicator (high priority - time-sensitive)
    // Blinks LED at different rates based on temperature band
    xTaskCreatePinnedToCore(
        task_led,                 // Task function
        "LED",                    // Task name
        TASK_LED_STACK_SIZE,      // Stack size: 3072 bytes
        nullptr,                  // Task parameter (unused)
        TASK_LED_PRIORITY,        // Priority: 2 (high)
        nullptr,                  // Task handle (not needed)
        APP_CPU_NUM               // CPU core: tskNO_AFFINITY
    );
    
    // Task 3: NeoPixel Humidity Indicator (high priority - time-sensitive)
    // Single LED shows humidity level via color (blue → green → yellow → red)
    xTaskCreatePinnedToCore(
        task_neopixel_hum,        // Task function
        "NEO_H",                  // Task name
        TASK_NEO_HUM_STACK_SIZE,  // Stack size: 3072 bytes
        nullptr,                  // Task parameter (unused)
        TASK_NEO_PRIORITY,        // Priority: 2 (high)
        nullptr,                  // Task handle (not needed)
        APP_CPU_NUM               // CPU core: tskNO_AFFINITY
    );
    
    // Task 4: NeoPixel UI Bar (low priority - user interface)
    // 4-LED strip with modes: OFF, BAR, DEMO, SOS, BLINK
    xTaskCreatePinnedToCore(
        task_neopixel_ui,         // Task function
        "NEO_UI",                 // Task name
        TASK_NEO_UI_STACK_SIZE,   // Stack size: 3072 bytes
        nullptr,                  // Task parameter (unused)
        TASK_LCD_PRIORITY,        // Priority: 1 (low)
        nullptr,                  // Task handle (not needed)
        APP_CPU_NUM               // CPU core: tskNO_AFFINITY
    );
    
    // Task 5: LCD Display (low priority - display updates)
    // 16x2 LCD shows temperature, humidity, and status
    xTaskCreatePinnedToCore(
        task_lcd,                 // Task function
        "LCD",                    // Task name
        TASK_LCD_STACK_SIZE,      // Stack size: 3072 bytes
        nullptr,                  // Task parameter (unused)
        TASK_LCD_PRIORITY,        // Priority: 1 (low)
        nullptr,                  // Task handle (not needed)
        APP_CPU_NUM               // CPU core: tskNO_AFFINITY
    );

    // Task 6: TinyML Anomaly Detection (low priority - ML inference)
    // Runs TensorFlow Lite Micro model for anomaly detection
    // Created by separate function in task5_tinyml.cpp
    createTinyMLTask();
    
    Serial.println("[TASKS] All tasks created successfully");
}
