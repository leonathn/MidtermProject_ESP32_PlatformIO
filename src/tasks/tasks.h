#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ====== Task Function Declarations ====== */

/**
 * @brief Task 1: DHT20 Sensor Reading
 * Reads temperature and humidity from DHT20 sensor
 * Signals other tasks when significant changes occur
 */
void task_read_dht20(void* pv);

/**
 * @brief Task 2: LED Temperature Indicator
 * Controls LED blinking pattern based on temperature band
 * Responds to temperature change signals
 */
void task_led(void* pv);

/**
 * @brief Task 3: NeoPixel Humidity Indicator
 * Controls single NeoPixel (GPIO 45) color based on humidity band
 * Responds to humidity change signals
 */
void task_neopixel_hum(void* pv);

/**
 * @brief Task 4: NeoPixel UI Bar
 * Controls 4-pixel NeoPixel strip (GPIO 6) for user interface
 * Supports OFF, BAR (humidity %), and DEMO (color cycle) modes
 */
void task_neopixel_ui(void* pv);

/**
 * @brief Task 5: LCD Display
 * Updates LCD screen with temperature, humidity, and status
 * Responds to display update signals
 */
void task_lcd(void* pv);

/* ====== Task Management ====== */
void createAllTasks();

#endif // TASKS_H
