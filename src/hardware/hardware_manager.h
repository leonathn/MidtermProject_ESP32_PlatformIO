/**
 * @file hardware_manager.h
 * @brief Hardware Abstraction Layer - Interface declarations
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This header provides global access to all hardware peripherals and
 * synchronization primitives used throughout the system.
 * 
 * Exported Objects:
 * - Sensor: DHT20 temperature/humidity sensor
 * - Display: 16x2 I2C LCD
 * - LEDs: Two NeoPixel strips (humidity indicator + UI bar)
 * - Synchronization: Three binary semaphores for task coordination
 * 
 * Usage:
 *   Include this header in any source file that needs hardware access.
 *   Objects are defined in hardware_manager.cpp and linked globally.
 */

#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../config/config.h"

/* ====== Hardware Objects ====== */

/**
 * @brief Global DHT20 sensor object
 * @details I2C temperature/humidity sensor (address 0x38)
 * @note Provides getTemperature() and getHumidity() methods
 */
extern DHT20 dht;

/**
 * @brief Global LCD display object
 * @details 16x2 character display via I2C (PCF8574 expander)
 * @note Provides print(), setCursor(), clear() methods
 */
extern LiquidCrystal_I2C lcd;

/**
 * @brief Global NeoPixel strip for humidity indicator (1 pixel, GPIO 45)
 * @details Single RGB LED showing humidity level via color
 * @note Use setPixelColor() and show() to control
 */
extern Adafruit_NeoPixel stripHum;

/**
 * @brief Global NeoPixel strip for UI bar (4 pixels, GPIO 6)
 * @details Multi-pixel strip for user interface and animations
 * @note Use setPixelColor() and show() to control
 */
extern Adafruit_NeoPixel stripUI;

/* ====== Semaphores ====== */

/**
 * @brief Binary semaphore: Temperature band change notification
 * @details Given by: Task 1 (sensor) when temperature band changes
 *          Taken by: Task 2 (LED) to update blink pattern
 * @note Use xSemaphoreGive() to signal, xSemaphoreTake() to wait
 */
extern SemaphoreHandle_t semBandChanged;

/**
 * @brief Binary semaphore: Humidity band change notification
 * @details Given by: Task 1 (sensor) when humidity band changes
 *          Taken by: Task 3 (NeoPixel) to update color
 * @note Use xSemaphoreGive() to signal, xSemaphoreTake() to wait
 */
extern SemaphoreHandle_t semHumChanged;

/**
 * @brief Binary semaphore: LCD display update trigger
 * @details Given by: Task 1 (sensor) on every reading (500ms)
 *          Taken by: Task 5 (LCD) to refresh display
 * @note Use xSemaphoreGive() to signal, xSemaphoreTake() to wait
 */
extern SemaphoreHandle_t semLcdUpdate;

/* ====== Initialization ====== */

/**
 * @brief Initialize all hardware peripherals
 * @details Initializes I2C bus, DHT20, LCD, and NeoPixel strips
 * @note Call once during setup() before creating tasks
 * @warning Must be called before any hardware access
 */
void initHardware();

/**
 * @brief Initialize FreeRTOS binary semaphores
 * @details Creates semaphores for inter-task communication
 * @note Call after hardware init but before task creation
 * @warning Tasks will hang if semaphores not created
 */
void initSemaphores();

#endif // HARDWARE_MANAGER_H
