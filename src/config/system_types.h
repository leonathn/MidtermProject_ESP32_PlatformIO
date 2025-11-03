/**
 * @file system_types.h
 * @brief System Data Types - Structures, enums, and global state declarations
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This header defines:
 * - Temperature and humidity band classifications
 * - Global system state structure (LiveState)
 * - Configuration variables (WiFi, thresholds)
 * - Helper function prototypes for classification and conversion
 * 
 * This file is included by all modules that need access to system state.
 */

#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <Arduino.h>
#include "config.h"

/* ====== Enumerations ====== */

/**
 * @brief Temperature classification bands
 * @details Four levels based on configurable thresholds:
 *          - COLD (0): Below T_COLD_MAX (default < 20°C)
 *          - NORMAL (1): Between T_COLD_MAX and T_NORMAL_MAX (20-30°C)
 *          - HOT (2): Between T_NORMAL_MAX and T_HOT_MAX (30-40°C)
 *          - CRITICAL (3): Above T_HOT_MAX (default > 40°C)
 * @note Used to trigger different LED blink patterns
 */
enum class TempBand : uint8_t { 
    COLD = 0,     ///< Temperature below cold threshold
    NORMAL,       ///< Temperature in normal range
    HOT,          ///< Temperature in hot range
    CRITICAL      ///< Temperature in critical range (danger)
};

/**
 * @brief Humidity classification bands
 * @details Four levels based on configurable thresholds:
 *          - DRY (0): Below H_DRY_MAX (default < 40%)
 *          - COMFORT (1): Between H_DRY_MAX and H_COMF_MAX (40-60%)
 *          - HUMID (2): Between H_COMF_MAX and H_HUMID_MAX (60-80%)
 *          - WET (3): Above H_HUMID_MAX (default > 80%)
 * @note Used to determine NeoPixel color (blue → green → yellow → red)
 */
enum class HumBand : uint8_t { 
    DRY = 0,      ///< Humidity below dry threshold (blue LED)
    COMFORT,      ///< Humidity in comfort range (green LED)
    HUMID,        ///< Humidity in humid range (yellow LED)
    WET           ///< Humidity above wet threshold (red LED)
};

/* ====== Global State Structure ====== */

/**
 * @brief Global system state structure
 * @details Contains all real-time sensor data, control flags, and telemetry.
 *          Updated by tasks and read by web server for JSON API responses.
 * @note Declared as 'volatile' to ensure visibility across tasks
 * @warning No mutex protection - designed for single-writer (tasks), multi-reader (web) pattern
 */
struct LiveState {
    // Sensor readings (from Task 1)
    float tC = NAN;              ///< Temperature in Celsius (NAN until first reading)
    float rh = NAN;              ///< Relative humidity in % (NAN until first reading)
    TempBand tBand = TempBand::NORMAL;  ///< Current temperature band
    HumBand  hBand = HumBand::COMFORT;  ///< Current humidity band
    
    // LED state (from Task 2)
    uint8_t ledOn = 0;           ///< LED state: 0=off, 1=on
    uint32_t onMs = 300;         ///< LED on-time duration in milliseconds
    uint32_t offMs = 300;        ///< LED off-time duration in milliseconds
    
    // Semaphore telemetry (debugging/monitoring)
    uint32_t giveTemp = 0;       ///< Count of semBandChanged given (by Task 1)
    uint32_t takeTemp = 0;       ///< Count of semBandChanged taken (by Task 2)
    uint32_t giveHum  = 0;       ///< Count of semHumChanged given (by Task 1)
    uint32_t takeHum  = 0;       ///< Count of semHumChanged taken (by Task 3)
    
    // UI control (from web dashboard)
    uint8_t uiMode = 0;          ///< NeoPixel UI mode: 0=off, 1=bar, 2=demo, 3=sos, 4=blink
    
    // Task timing monitor (for performance analysis)
    uint32_t dht_last_ms = 0;    ///< Timestamp of last DHT20 reading (millis)
    uint32_t led_last_ms = 0;    ///< Timestamp of last LED update (millis)
    uint32_t neo_last_ms = 0;    ///< Timestamp of last NeoPixel update (millis)
    uint32_t lcd_last_ms = 0;    ///< Timestamp of last LCD update (millis)
    uint32_t dht_runs = 0;       ///< Total DHT20 task executions
    uint32_t led_runs = 0;       ///< Total LED task executions
    uint32_t neo_runs = 0;       ///< Total NeoPixel task executions
    uint32_t lcd_runs = 0;       ///< Total LCD task executions
    
    // TinyML inference telemetry
    float tinyml_score = NAN;    ///< Latest anomaly detection score (0.0-1.0)
    uint32_t tinyml_last_ms = 0; ///< Timestamp of last inference (millis)
    uint32_t tinyml_runs = 0;    ///< Total inference executions
};

/* ====== Global Variables ====== */

/**
 * @brief Global live system state instance
 * @note Volatile ensures changes are visible across tasks and ISRs
 */
extern volatile LiveState gLive;

// WiFi configuration (runtime modifiable)

/**
 * @brief Current WiFi mode ("ap" or "sta")
 */
extern String gWifiMode;

/**
 * @brief Station mode target SSID
 */
extern String gStaSsid;

/**
 * @brief Station mode WiFi password
 */
extern String gStaPass;

// Temperature thresholds (runtime modifiable via web API)

/**
 * @brief Upper temperature limit of COLD band (°C)
 * @note Can be modified via /set API endpoint
 */
extern float T_COLD_MAX;

/**
 * @brief Upper temperature limit of NORMAL band (°C)
 * @note Can be modified via /set API endpoint
 */
extern float T_NORMAL_MAX;

/**
 * @brief Upper temperature limit of HOT band (°C)
 * @note Above this is CRITICAL; can be modified via /set API endpoint
 */
extern float T_HOT_MAX;

// Humidity thresholds (runtime modifiable via web API)

/**
 * @brief Upper humidity limit of DRY band (%)
 * @note Can be modified via /set API endpoint
 */
extern float H_DRY_MAX;

/**
 * @brief Upper humidity limit of COMFORT band (%)
 * @note Can be modified via /set API endpoint
 */
extern float H_COMF_MAX;

/**
 * @brief Upper humidity limit of HUMID band (%)
 * @note Above this is WET; can be modified via /set API endpoint
 */
extern float H_HUMID_MAX;

/* ====== Helper Functions ====== */

/**
 * @brief Convert temperature band to string name
 * @param b Temperature band enum
 * @return String representation ("COLD", "NORMAL", "HOT", "CRITICAL")
 */
const char* bandName(TempBand b);

/**
 * @brief Convert humidity band to string name
 * @param b Humidity band enum
 * @return String representation ("DRY", "COMFORT", "HUMID", "WET")
 */
const char* humName(HumBand b);

/**
 * @brief Classify temperature value into band
 * @param tC Temperature in Celsius
 * @return Temperature band classification
 */
TempBand classifyTemp(float tC);

/**
 * @brief Classify humidity value into band
 * @param h Humidity percentage (0-100)
 * @return Humidity band classification
 */
HumBand classifyHum(float h);

/**
 * @brief Get LED blink timing for temperature band
 * @param b Temperature band
 * @param[out] onMs LED on-time in milliseconds
 * @param[out] offMs LED off-time in milliseconds
 */
void bandToBlink(TempBand b, uint32_t& onMs, uint32_t& offMs);

#endif // SYSTEM_TYPES_H
