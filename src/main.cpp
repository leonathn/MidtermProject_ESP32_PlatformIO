/**
 * @file main.cpp
 * @brief ESP32-S3 Multi-Task Environmental Monitoring System
 * @details Professional FreeRTOS-based system with modular architecture
 * 
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * Features:
 * - Task 1: DHT20 sensor reading with change detection
 * - Task 2: LED temperature indicator with variable blink patterns
 * - Task 3: NeoPixel humidity indicator (single pixel, GPIO 45)
 * - Task 4: NeoPixel UI bar (4 pixels, GPIO 6) with multiple modes
 * - Task 5: LCD display with real-time status
 * - Web dashboard for configuration and monitoring
 * - WiFi Access Point with station mode switching
 */

#include <Arduino.h>

// Configuration and types
#include "config/config.h"
#include "config/system_types.h"

// Hardware management
#include "hardware/hardware_manager.h"

// Task definitions
#include "tasks/tasks.h"

// Web server
#include "web/web_server.h"

/**
 * @brief System initialization
 * Initializes hardware, semaphores, WiFi, web server, and creates tasks
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(50);
    
    Serial.println("\n╔════════════════════════════════════════════════╗");
    Serial.println("║   ESP32-S3 Multi-Task Environmental Monitor    ║");
    Serial.println("║   FreeRTOS + DHT20 + LCD + NeoPixel + Web      ║");
    Serial.println("╚════════════════════════════════════════════════╝\n");
    
    // Initialize hardware components
    Serial.println("=== System Initialization ===");
    initHardware();
    
    // Initialize synchronization primitives
    initSemaphores();
    
    // Initialize WiFi Access Point
    initWiFi();
    
    // Initialize web server
    initWebServer();
    
    // Create all FreeRTOS tasks
    createAllTasks();
    
    Serial.println("\n╔════════════════════════════════════════════════╗");
    Serial.println("║              System Ready                      ║");
    Serial.println("║  Connect to: ESP32-S3-LAB (password: 12345678)║");
    Serial.println("║  Dashboard: http://192.168.4.1                 ║");
    Serial.println("╚════════════════════════════════════════════════╝\n");
    
    Serial.println("[MAIN] Entering main loop - handling web requests");
}

/**
 * @brief Main loop
 * Handles web server client requests
 * Note: FreeRTOS tasks run independently in the background
 */
void loop() {
    handleWebServer();
    delay(2);
}
