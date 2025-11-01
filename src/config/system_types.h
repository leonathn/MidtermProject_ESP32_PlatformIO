#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <Arduino.h>
#include "config.h"

/* ====== Enumerations ====== */
enum class TempBand : uint8_t { 
    COLD = 0, 
    NORMAL, 
    HOT, 
    CRITICAL 
};

enum class HumBand : uint8_t { 
    DRY = 0, 
    COMFORT, 
    HUMID, 
    WET 
};

/* ====== Global State Structure ====== */
struct LiveState {
    float tC = NAN;
    float rh = NAN;
    TempBand tBand = TempBand::NORMAL;
    HumBand  hBand = HumBand::COMFORT;
    uint8_t ledOn = 0;
    uint32_t onMs = 300, offMs = 300;
    uint32_t giveTemp = 0;
    uint32_t takeTemp = 0;
    uint32_t giveHum  = 0;
    uint32_t takeHum  = 0;
    uint8_t uiMode = 0;   // 0=off, 1=bar, 2=demo

    // Task timing monitor
    uint32_t dht_last_ms = 0;
    uint32_t led_last_ms = 0;
    uint32_t neo_last_ms = 0;
    uint32_t lcd_last_ms = 0;
    uint32_t dht_runs = 0;
    uint32_t led_runs = 0;
    uint32_t neo_runs = 0;
    uint32_t lcd_runs = 0;

    // TinyML inference telemetry
    float tinyml_score = NAN;
    uint32_t tinyml_last_ms = 0;
    uint32_t tinyml_runs = 0;
};

/* ====== Global Variables ====== */
extern volatile LiveState gLive;

// WiFi configuration
extern String gWifiMode;
extern String gStaSsid;
extern String gStaPass;

// Thresholds (editable from web)
extern float T_COLD_MAX;
extern float T_NORMAL_MAX;
extern float T_HOT_MAX;

extern float H_DRY_MAX;
extern float H_COMF_MAX;
extern float H_HUMID_MAX;

/* ====== Helper Functions ====== */
const char* bandName(TempBand b);
const char* humName(HumBand b);
TempBand classifyTemp(float tC);
HumBand classifyHum(float h);
void bandToBlink(TempBand b, uint32_t& onMs, uint32_t& offMs);

#endif // SYSTEM_TYPES_H
