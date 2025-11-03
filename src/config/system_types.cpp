/**
 * @file system_types.cpp
 * @brief System Data Types - Global state and helper function implementations
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This file implements:
 * - Global system state variables (shared between tasks and web server)
 * - Temperature/humidity classification functions
 * - LED blink pattern mapping
 * - String conversion utilities for enums
 * 
 * All variables are global to allow efficient access from tasks and web API.
 */

#include "system_types.h"
#include <WiFi.h>

/* ====== Global Variable Definitions ====== */

/**
 * @brief Global live system state
 * @details Volatile to ensure visibility across tasks and interrupts
 *          Contains all real-time sensor data, task telemetry, and control flags
 * @note Shared between FreeRTOS tasks and web server (no mutex needed for reads)
 */
volatile LiveState gLive;

/**
 * @brief Current WiFi mode ("ap" or "sta")
 * @details "ap" = Access Point mode (ESP32 creates network)
 *          "sta" = Station mode (ESP32 connects to existing network)
 */
String gWifiMode = "ap";

/**
 * @brief Station mode SSID (when connecting to external WiFi)
 * @note Empty string when in AP mode
 */
String gStaSsid  = "";

/**
 * @brief Station mode password (when connecting to external WiFi)
 * @note Empty string when in AP mode
 */
String gStaPass  = "";

/**
 * @brief Runtime-modifiable temperature thresholds
 * @details Initialized from DEFAULT_* constants in config.h
 *          Can be updated via web API (/set endpoint)
 * @note Changes take effect immediately in classification logic
 */
float T_COLD_MAX   = DEFAULT_T_COLD_MAX;    ///< Upper limit of COLD band (default 20°C)
float T_NORMAL_MAX = DEFAULT_T_NORMAL_MAX;  ///< Upper limit of NORMAL band (default 30°C)
float T_HOT_MAX    = DEFAULT_T_HOT_MAX;     ///< Upper limit of HOT band (default 40°C)

/**
 * @brief Runtime-modifiable humidity thresholds
 * @details Initialized from DEFAULT_* constants in config.h
 *          Can be updated via web API (/set endpoint)
 * @note Changes take effect immediately in classification logic
 */
float H_DRY_MAX    = DEFAULT_H_DRY_MAX;     ///< Upper limit of DRY band (default 40%)
float H_COMF_MAX   = DEFAULT_H_COMF_MAX;    ///< Upper limit of COMFORT band (default 60%)
float H_HUMID_MAX  = DEFAULT_H_HUMID_MAX;   ///< Upper limit of HUMID band (default 80%)

/* ====== Helper Function Implementations ====== */

/**
 * @brief Convert temperature band enum to string name
 * @param b Temperature band enum value
 * @return Const string name of the band ("COLD", "NORMAL", "HOT", "CRITICAL")
 * @note Used for serial debugging and web API JSON responses
 */
const char* bandName(TempBand b) {
    switch (b) {
        case TempBand::COLD:     return "COLD";
        case TempBand::NORMAL:   return "NORMAL";
        case TempBand::HOT:      return "HOT";
        case TempBand::CRITICAL: return "CRITICAL";
    }
    return "?";  // Should never reach here
}

/**
 * @brief Convert humidity band enum to string name
 * @param b Humidity band enum value
 * @return Const string name of the band ("DRY", "COMFORT", "HUMID", "WET")
 * @note Used for serial debugging and web API JSON responses
 */
const char* humName(HumBand b) {
    switch (b) {
        case HumBand::DRY:     return "DRY";
        case HumBand::COMFORT: return "COMFORT";
        case HumBand::HUMID:   return "HUMID";
        case HumBand::WET:     return "WET";
    }
    return "?";  // Should never reach here
}

/**
 * @brief Classify temperature reading into band
 * @param tC Temperature in degrees Celsius
 * @return Temperature band enum (COLD, NORMAL, HOT, or CRITICAL)
 * @details Uses runtime-modifiable thresholds:
 *          - tC < T_COLD_MAX → COLD
 *          - tC < T_NORMAL_MAX → NORMAL
 *          - tC < T_HOT_MAX → HOT
 *          - tC ≥ T_HOT_MAX → CRITICAL
 * @note This function is called by Task 1 (sensor) on every reading
 */
TempBand classifyTemp(float tC) {
    if (tC < T_COLD_MAX)   return TempBand::COLD;
    if (tC < T_NORMAL_MAX) return TempBand::NORMAL;
    if (tC < T_HOT_MAX)    return TempBand::HOT;
    return TempBand::CRITICAL;  // Temperature ≥ T_HOT_MAX
}

/**
 * @brief Classify humidity reading into band
 * @param h Humidity in percentage (0-100%)
 * @return Humidity band enum (DRY, COMFORT, HUMID, or WET)
 * @details Uses runtime-modifiable thresholds:
 *          - h < H_DRY_MAX → DRY
 *          - h < H_COMF_MAX → COMFORT
 *          - h < H_HUMID_MAX → HUMID
 *          - h ≥ H_HUMID_MAX → WET
 * @note This function is called by Task 1 (sensor) on every reading
 */
HumBand classifyHum(float h) {
    if (h < H_DRY_MAX)     return HumBand::DRY;
    if (h < H_COMF_MAX)    return HumBand::COMFORT;
    if (h < H_HUMID_MAX)   return HumBand::HUMID;
    return HumBand::WET;  // Humidity ≥ H_HUMID_MAX
}

/**
 * @brief Map temperature band to LED blink timing
 * @param b Temperature band enum
 * @param[out] onMs Reference to store LED on-time in milliseconds
 * @param[out] offMs Reference to store LED off-time in milliseconds
 * @details Blink patterns by band:
 *          - COLD: Slow (1000ms on, 1000ms off) - 0.5 Hz
 *          - NORMAL: Medium (300ms on, 300ms off) - 1.67 Hz
 *          - HOT: Fast (120ms on, 120ms off) - 4.17 Hz
 *          - CRITICAL: Solid ON (0ms = special case for always-on)
 * @note Used by Task 2 (LED control) to set blink rate
 */
void bandToBlink(TempBand b, uint32_t& onMs, uint32_t& offMs) {
    switch (b) {
        case TempBand::COLD:     onMs = 1000; offMs = 1000; break;  // Slow blink
        case TempBand::NORMAL:   onMs = 300;  offMs = 300;  break;  // Medium blink
        case TempBand::HOT:      onMs = 120;  offMs = 120;  break;  // Fast blink
        case TempBand::CRITICAL: onMs = 0;    offMs = 0;    break;  // Always ON (no blink)
    }
}
