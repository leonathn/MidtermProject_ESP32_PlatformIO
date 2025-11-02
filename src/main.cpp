/**
 * @file main.cpp
 * @brief ESP32-S3 Multi-Task Environmental Monitoring System - Main Entry Point
 * @details Professional FreeRTOS-based embedded system with modular architecture
 * 
 * @author ESP32-S3 Lab
 * @date 2025
 * @version 1.0
 * 
 * System Architecture:
 * ====================
 * This is a real-time environmental monitoring system built on ESP32-S3 with
 * FreeRTOS multi-tasking. The system uses binary semaphores for efficient
 * inter-task communication, ensuring responsive behavior without polling.
 * 
 * Key Features:
 * - Task 1 (Priority 3): DHT20 I2C sensor reading every 500ms with band detection
 * - Task 2 (Priority 2): LED temperature indicator with adaptive blink patterns
 * - Task 3 (Priority 2): NeoPixel humidity indicator (color-coded, GPIO 45)
 * - Task 4 (Priority 1): NeoPixel UI bar (4 LEDs, GPIO 6, multi-mode display)
 * - Task 5 (Priority 1): LCD 16x2 display with real-time sensor data
 * - Task 6 (Priority 1): TinyML anomaly detection using TensorFlow Lite Micro
 * - Web Dashboard: Full-featured HTTP interface for monitoring and configuration
 * - WiFi: Dual-mode support (Access Point and Station modes)
 * - GPIO Control: Remote pin manipulation via web API
 * 
 * Task Synchronization:
 * =====================
 * Tasks communicate via three binary semaphores:
 * 1. semBandChanged: Task 1 → Task 2 (temperature band change)
 * 2. semHumChanged:  Task 1 → Task 3 (humidity band change)
 * 3. semLcdUpdate:   Task 1 → Task 5 (LCD refresh trigger)
 * 
 * This event-driven design eliminates busy-waiting and reduces CPU usage.
 * 
 * Hardware Requirements:
 * ======================
 * - ESP32-S3 microcontroller (YOLO Uno board or compatible)
 * - DHT20 I2C temperature/humidity sensor (address 0x38)
 * - 16x2 LCD with I2C adapter (address 0x21)
 * - WS2812B NeoPixel: 1 pixel on GPIO 45, 4 pixels on GPIO 6
 * - Status LED on GPIO 48
 * - I2C bus: SDA=GPIO11, SCL=GPIO12
 * 
 * Network Access:
 * ===============
 * Default SSID: ESP32-S3-LAB
 * Password: 12345678
 * Dashboard URL: http://192.168.4.1
 * 
 * Build Instructions:
 * ===================
 * Platform: PlatformIO
 * Framework: Arduino (ESP32-S3)
 * Build: Ctrl+Alt+B (or 'pio run')
 * Upload: Ctrl+Alt+U (or 'pio run --target upload')
 * Monitor: Ctrl+Alt+S (or 'pio device monitor')
 */

#include <Arduino.h>

// Configuration and types
#include "config/config.h"          // Hardware pins, timing constants
#include "config/system_types.h"    // Data structures, enums, global state

// Hardware management
#include "hardware/hardware_manager.h"  // Device initialization, semaphores

// Task definitions
#include "tasks/tasks.h"            // FreeRTOS task creation

// Web server
#include "web/web_server.h"         // HTTP server, WiFi management

/**
 * @brief System initialization (runs once at boot)
 * @details Performs sequential initialization in the correct order:
 *          1. Serial communication (115200 baud for debugging)
 *          2. Hardware peripherals (I2C, sensors, displays)
 *          3. FreeRTOS synchronization primitives (semaphores)
 *          4. WiFi network (Access Point mode by default)
 *          5. HTTP web server (port 80)
 *          6. FreeRTOS tasks (6 concurrent tasks)
 * 
 * After setup() completes, FreeRTOS scheduler is already running and
 * all tasks execute concurrently. The main loop() only handles web requests.
 * 
 * @note This function is called automatically by Arduino framework
 * @note Do not call this function manually
 * @warning Incorrect initialization order may cause system instability
 */
void setup() {
    // Step 1: Initialize serial communication for debugging
    // Baud rate: 115200 (standard for ESP32)
    // Delay ensures USB CDC is ready on ESP32-S3
    Serial.begin(115200);
    delay(50);  // Give USB CDC time to initialize
    
    // Print startup banner
    Serial.println("\n╔════════════════════════════════════════════════╗");
    Serial.println("║   ESP32-S3 Multi-Task Environmental Monitor    ║");
    Serial.println("║   FreeRTOS + DHT20 + LCD + NeoPixel + Web      ║");
    Serial.println("╚════════════════════════════════════════════════╝\n");
    
    // Step 2: Initialize hardware components
    // Sets up I2C bus, DHT20 sensor, LCD display, NeoPixel strips
    Serial.println("=== System Initialization ===");
    initHardware();
    
    // Step 3: Initialize FreeRTOS synchronization primitives
    // Creates three binary semaphores for inter-task communication
    initSemaphores();
    
    // Step 4: Initialize WiFi in Access Point mode
    // Creates "ESP32-S3-LAB" network with IP 192.168.4.1
    initWiFi();
    
    // Step 5: Initialize web server and register HTTP routes
    // Starts HTTP server on port 80 with all API endpoints
    initWebServer();
    
    // Step 6: Create all FreeRTOS tasks
    // Tasks start running immediately after creation
    // FreeRTOS scheduler automatically manages task switching
    createAllTasks();
    
    // Print ready message with connection instructions
    Serial.println("\n╔════════════════════════════════════════════════╗");
    Serial.println("║              System Ready                      ║");
    Serial.println("║  Connect to: ESP32-S3-LAB (password: 12345678)║");
    Serial.println("║  Dashboard: http://192.168.4.1                 ║");
    Serial.println("╚════════════════════════════════════════════════╝\n");
    
    Serial.println("[MAIN] Entering main loop - handling web requests");
}

/**
 * @brief Main program loop (runs continuously)
 * @details The loop's only responsibility is to process incoming HTTP requests.
 *          All sensor reading, LED control, and display updates happen in
 *          FreeRTOS tasks running concurrently in the background.
 * 
 * Loop Behavior:
 * - Calls handleWebServer() to process one HTTP request (non-blocking)
 * - Delays 2ms to prevent watchdog timer issues
 * - Repeats indefinitely
 * 
 * @note This function is called automatically by Arduino framework
 * @note FreeRTOS tasks continue running independently of this loop
 * @note The 2ms delay yields CPU time to other tasks and prevents watchdog resets
 * @warning Do not add blocking code here (blocks web server)
 * @warning Do not remove delay (may trigger watchdog timer)
 * 
 * CPU Usage:
 * - loop() runs on PRO_CPU core (core 0)
 * - FreeRTOS tasks run on APP_CPU core (core 1) or both (tskNO_AFFINITY)
 * - Web server is not thread-safe, so must run on PRO_CPU
 */
void loop() {
    // Process incoming HTTP client requests
    // Non-blocking: returns immediately if no request pending
    handleWebServer();
    
    // Small delay to:
    // 1. Prevent watchdog timer timeout
    // 2. Yield CPU time to FreeRTOS tasks
    // 3. Reduce power consumption slightly
    delay(2);  // 2ms delay = ~500 iterations per second
}
