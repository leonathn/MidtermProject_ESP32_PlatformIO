/**
 * @file config.h
 * @brief System Configuration - Hardware pins, thresholds, and task parameters
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This file contains all compile-time configuration constants for the system.
 * Modify these values to customize hardware connections, timing, and behavior.
 * 
 * Key Configuration Areas:
 * - GPIO pin assignments for all peripherals
 * - I2C device addresses and parameters
 * - WiFi network credentials
 * - Temperature and humidity classification thresholds
 * - FreeRTOS task stack sizes and priorities
 * - Timing intervals for sensor polling and updates
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/* ====== Hardware Pins ====== */

/**
 * @brief LED pin for temperature indicator
 * @note GPIO 48 is used for the status LED on YOLO Uno board
 * @details LED blinks at different rates based on temperature band:
 *          - COLD: 1000ms on/off
 *          - NORMAL: 300ms on/off
 *          - HOT: 120ms on/off
 *          - CRITICAL: Always ON
 */
#ifndef LED_GPIO
  #define LED_GPIO GPIO_NUM_48
#endif

/**
 * @brief I2C bus pins (shared by DHT20 and LCD)
 * @note These pins must support I2C hardware peripheral
 */
static const int SDA_PIN = 11;  ///< I2C Serial Data Line
static const int SCL_PIN = 12;  ///< I2C Serial Clock Line

/* ====== I2C Device Addresses ====== */

/**
 * @brief LCD I2C address
 * @note Common addresses: 0x27, 0x3F, 0x21 (use I2C scanner to verify)
 */
#define LCD_I2C_ADDR 0x21

/**
 * @brief LCD display dimensions
 * @note Standard 1602 LCD: 16 columns × 2 rows
 */
#define LCD_COLS     16  ///< Number of characters per row
#define LCD_ROWS     2   ///< Number of rows

/* ====== NeoPixel Configuration ====== */

/**
 * @brief Humidity indicator NeoPixel (single pixel)
 * @details Shows humidity level with color coding:
 *          - Blue: DRY (< 40%)
 *          - Green: COMFORT (40-60%)
 *          - Yellow: HUMID (60-80%)
 *          - Red: WET (> 80%)
 */
#define NEOPIXEL_HUM_PIN 45  ///< GPIO pin for humidity indicator
#define NEOPIXEL_HUM_NUM 1   ///< Number of pixels (single LED)

/**
 * @brief UI bar NeoPixel strip (4 pixels)
 * @details Multi-mode display strip with:
 *          - OFF mode: All pixels dark
 *          - BAR mode: Visual humidity percentage (0-4 LEDs)
 *          - DEMO mode: Rainbow animation
 *          - SOS mode: Emergency pattern (··· ─── ···)
 *          - BLINK mode: Synchronized flashing
 */
#define NEOPIXEL_UI_PIN 6    ///< GPIO pin for UI bar
#define NEOPIXEL_UI_NUM 4    ///< Number of pixels in strip

/**
 * @brief CPU core assignment for FreeRTOS tasks
 * @note tskNO_AFFINITY allows FreeRTOS scheduler to assign tasks to any core
 * @details ESP32-S3 has 2 cores (PRO_CPU and APP_CPU)
 *          Using tskNO_AFFINITY provides better load balancing
 */
#ifndef APP_CPU_NUM
  #define APP_CPU_NUM tskNO_AFFINITY
#endif

/* ====== WiFi Configuration ====== */

/**
 * @brief Default WiFi Access Point SSID
 * @note This network name will be visible when ESP32 creates its own WiFi AP
 */
#define AP_SSID_DEFAULT "ESP32-S3-LAB"

/**
 * @brief Default WiFi Access Point password
 * @note Minimum 8 characters required for WPA2 security
 */
#define AP_PASS_DEFAULT "12345678"

/* ====== Default Thresholds ====== */

/**
 * @brief Temperature band thresholds (in degrees Celsius)
 * @details Classification ranges:
 *          - COLD: temperature < 20°C
 *          - NORMAL: 20°C ≤ temperature < 30°C
 *          - HOT: 30°C ≤ temperature < 40°C
 *          - CRITICAL: temperature ≥ 40°C
 * @note These can be modified at runtime via web dashboard
 */
#define DEFAULT_T_COLD_MAX   20.0f  ///< Upper limit of COLD range
#define DEFAULT_T_NORMAL_MAX 30.0f  ///< Upper limit of NORMAL range
#define DEFAULT_T_HOT_MAX    40.0f  ///< Upper limit of HOT range (above = CRITICAL)

/**
 * @brief Humidity band thresholds (in percentage %)
 * @details Classification ranges:
 *          - DRY: humidity < 40%
 *          - COMFORT: 40% ≤ humidity < 60%
 *          - HUMID: 60% ≤ humidity < 80%
 *          - WET: humidity ≥ 80%
 * @note These can be modified at runtime via web dashboard
 */
#define DEFAULT_H_DRY_MAX    40.0f  ///< Upper limit of DRY range
#define DEFAULT_H_COMF_MAX   60.0f  ///< Upper limit of COMFORT range
#define DEFAULT_H_HUMID_MAX  80.0f  ///< Upper limit of HUMID range (above = WET)

/* ====== Task Configuration ====== */

/**
 * @brief FreeRTOS task stack sizes (in bytes)
 * @details Stack allocation for each task's local variables and call stack
 *          Increase if task experiences stack overflow
 * @warning Too small = stack overflow; Too large = wasted RAM
 * @note Monitor actual usage with uxTaskGetStackHighWaterMark()
 */
#define TASK_DHT_STACK_SIZE     4096  ///< DHT20 sensor task (I2C communication needs buffer)
#define TASK_LED_STACK_SIZE     3072  ///< LED control task (simple GPIO operations)
#define TASK_NEO_HUM_STACK_SIZE 3072  ///< NeoPixel humidity indicator (color calculations)
#define TASK_NEO_UI_STACK_SIZE  3072  ///< NeoPixel UI bar (animations require buffer)
#define TASK_LCD_STACK_SIZE     3072  ///< LCD display task (text buffer)
#define TASK_TINYML_STACK_SIZE  8192  ///< TinyML task (ML inference needs large stack)

/**
 * @brief FreeRTOS task priorities (0 = lowest, higher number = higher priority)
 * @details Priority assignment rationale:
 *          - Priority 3: DHT20 sensor (highest - data source for all tasks)
 *          - Priority 2: LED & NeoPixel (time-sensitive visual feedback)
 *          - Priority 1: LCD & TinyML (can tolerate slight delays)
 * @note FreeRTOS scheduler preempts lower priority tasks when higher priority tasks are ready
 * @warning Same priority tasks share CPU time through round-robin scheduling
 */
#define TASK_DHT_PRIORITY       3  ///< Highest - sensor is data source
#define TASK_LED_PRIORITY       2  ///< High - time-sensitive blinking
#define TASK_NEO_PRIORITY       2  ///< High - visual indicators
#define TASK_LCD_PRIORITY       1  ///< Low - display updates less critical
#define TASK_TINYML_PRIORITY    1  ///< Low - inference can be delayed

/* ====== Timing ====== */

/**
 * @brief DHT20 sensor reading interval
 * @note DHT20 sensor has 0.5s minimum sampling period
 * @details Faster polling won't get new data and wastes CPU
 */
#define DHT_READ_INTERVAL_MS    500  ///< 500ms = 2 readings per second

/**
 * @brief NeoPixel UI bar animation update rate
 * @note 120ms provides smooth animations without excessive CPU usage
 */
#define UI_STRIP_UPDATE_MS      120  ///< ~8 fps for UI animations

/**
 * @brief Web server state polling interval (unused in current implementation)
 */
#define WEB_POLL_INTERVAL_MS    500  ///< Web state update rate

/**
 * @brief TinyML task timing parameters
 * @details Controls ML inference behavior and retry logic
 */
#define TINYML_WAIT_FOR_DATA_MS 500   ///< Wait time when sensor data unavailable
#define TINYML_RETRY_DELAY_MS   1000  ///< Delay after inference failure
#define TINYML_INFERENCE_MS     5000  ///< Normal interval between inferences (5 seconds)

#endif // CONFIG_H
