#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @file tasks.h
 * @brief Task declarations and semaphore synchronization documentation
 * 
 * SEMAPHORE SYNCHRONIZATION DIAGRAM:
 * 
 *     Task 1 (Sensor)              Task 2 (LED)           Task 3 (NeoPixel)       Task 5 (LCD)
 *     ===============              ============           =================       ============
 *          |                            |                        |                     |
 *          |-- Read DHT20               |                        |                     |
 *          |                            |                        |                     |
 *          |-- Classify Temp            |                        |                     |
 *          |   (if changed)             |                        |                     |
 *          |                            |                        |                     |
 *          |---- semBandChanged ------->|                        |                     |
 *          |       (GIVE)               |                        |                     |
 *          |                            |<-- TAKE (wait)         |                     |
 *          |                            |                        |                     |
 *          |                            |-- Update LED           |                     |
 *          |                            |   blink pattern        |                     |
 *          |                            |                        |                     |
 *          |-- Classify Humidity        |                        |                     |
 *          |   (if changed)             |                        |                     |
 *          |                            |                        |                     |
 *          |---- semHumChanged ----------------------->          |                     |
 *          |       (GIVE)               |                        |                     |
 *          |                            |                        |<-- TAKE (wait)      |
 *          |                            |                        |                     |
 *          |                            |                        |-- Update color      |
 *          |                            |                        |                     |
 *          |---- semLcdUpdate --------------------------------------------------->    |
 *          |       (GIVE)               |                        |                     |
 *          |                            |                        |                     |<-- TAKE (wait)
 *          |                            |                        |                     |
 *          |                            |                        |                     |-- Update LCD
 *          |                            |                        |                     |
 *          |-- Wait 500ms               |                        |                     |
 *          |                            |                        |                     |
 *          └--> (repeat)                └--> (repeat)            └--> (repeat)         └--> (repeat)
 * 
 * 
 * Task 4 accessories run independently with no semaphore synchronization.
 */

/* ====== Task Function Declarations ====== */

/**
 * @brief Task 1: DHT20 Sensor Reading (PRODUCER)
 * @file task1_sensor.cpp
 * 
 * Reads temperature and humidity from DHT20 sensor every 500ms
 * 
 * Semaphores GIVEN:
 * - semBandChanged → Signals Task 2 when temperature band changes
 * - semHumChanged → Signals Task 3 when humidity band changes
 * - semLcdUpdate → Signals Task 5 every reading
 */
void task_read_dht20(void* pv);

/**
 * @brief Task 2: LED Temperature Indicator (CONSUMER)
 * @file task2_led_neopixel.cpp
 * 
 * Controls LED blinking pattern based on temperature band
 * 
 * Semaphores TAKEN:
 * - semBandChanged ← Waits for Task 1 temperature band changes
 */
void task_led(void* pv);

/**
 * @brief Task 3: NeoPixel Humidity Indicator (CONSUMER)
 * @file task2_led_neopixel.cpp
 * 
 * Controls single NeoPixel (GPIO 45) color based on humidity band
 * 
 * Semaphores TAKEN:
 * - semHumChanged ← Waits for Task 1 humidity band changes
 */
void task_neopixel_hum(void* pv);

/**
 * @brief Task 4: Fan & LED Accessory Control (INDEPENDENT)
 * @file task4_fan_led.cpp
 * 
 * Controls the D3 mini fan and D5 NeoPixel LED module.
 * Fan supports OFF / ON / AUTO (based on temperature band).
 * LED module supports OFF / RED / GREEN / BLUE / RAINBOW.
 * 
 * Semaphores: NONE (polls shared state directly).
 */
void task_fan_led(void* pv);

/**
 * @brief Task 4b: NeoPixel UI Bar (INDEPENDENT)
 * @file task2_led_neopixel.cpp
 * 
 * Controls 4-pixel NeoPixel strip (GPIO 6) for user interface
 * Supports OFF, BAR (humidity %), and DEMO (color cycle) modes
 * 
 * Semaphores: NONE (runs independently, no synchronization)
 */
void task_neopixel_ui(void* pv);

/**
 * @brief Task 5: LCD Display (CONSUMER)
 * @file task3_lcd.cpp
 * 
 * Updates LCD screen with temperature, humidity, and status
 * 
 * Semaphores TAKEN:
 * - semLcdUpdate ← Waits for Task 1 to signal new data available
 */
void task_lcd(void* pv);

/* ====== Task Management ====== */

/**
 * @brief Creates all FreeRTOS tasks with proper priorities and core affinity
 * 
 * Task Configuration:
 * - Task 1 (DHT20): Core 1, Priority 3, Stack 4096
 * - Task 2 (LED): Core 0, Priority 2, Stack 3072
 * - Task 3 (NeoPixel Hum): Core 0, Priority 2, Stack 3072
 * - Task 4 (Fan & LED): Core 0, Priority 1, Stack 3072
 * - Task 4b (NeoPixel UI): Core 0, Priority 1, Stack 3072
 * - Task 5 (LCD): Core 0, Priority 1, Stack 3072
 * - Task 6 (TinyML): Core 0, Priority 1, Stack 8192
 */
void createAllTasks();

#endif // TASKS_H
