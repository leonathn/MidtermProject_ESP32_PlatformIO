#include "system_types.h"

/* ====== Global Variable Definitions ====== */
volatile LiveState gLive;

String gWifiMode = "ap";
String gStaSsid  = "";
String gStaPass  = "";

float T_COLD_MAX   = DEFAULT_T_COLD_MAX;
float T_NORMAL_MAX = DEFAULT_T_NORMAL_MAX;
float T_HOT_MAX    = DEFAULT_T_HOT_MAX;

float H_DRY_MAX    = DEFAULT_H_DRY_MAX;
float H_COMF_MAX   = DEFAULT_H_COMF_MAX;
float H_HUMID_MAX  = DEFAULT_H_HUMID_MAX;

/* ====== Helper Function Implementations ====== */
const char* bandName(TempBand b) {
    switch (b) {
        case TempBand::COLD: return "COLD";
        case TempBand::NORMAL: return "NORMAL";
        case TempBand::HOT: return "HOT";
        case TempBand::CRITICAL: return "CRITICAL";
    }
    return "?";
}

const char* humName(HumBand b) {
    switch (b) {
        case HumBand::DRY: return "DRY";
        case HumBand::COMFORT: return "COMFORT";
        case HumBand::HUMID: return "HUMID";
        case HumBand::WET: return "WET";
    }
    return "?";
}

const char* fanModeName(FanMode m) {
    switch (m) {
        case FanMode::FAN_OFF:  return "OFF";
        case FanMode::FAN_ON:   return "ON";
        case FanMode::FAN_AUTO: return "AUTO";
    }
    return "?";
}

const char* ledModuleModeName(LedModuleMode m) {
    switch (m) {
        case LedModuleMode::LED_OFF:     return "OFF";
        case LedModuleMode::LED_RED:     return "RED";
        case LedModuleMode::LED_GREEN:   return "GREEN";
        case LedModuleMode::LED_BLUE:    return "BLUE";
        case LedModuleMode::LED_RAINBOW: return "RAINBOW";
    }
    return "?";
}

TempBand classifyTemp(float tC) {
    if (tC < T_COLD_MAX)   return TempBand::COLD;
    if (tC < T_NORMAL_MAX) return TempBand::NORMAL;
    if (tC < T_HOT_MAX)    return TempBand::HOT;
    return TempBand::CRITICAL;
}

HumBand classifyHum(float h) {
    if (h < H_DRY_MAX)     return HumBand::DRY;
    if (h < H_COMF_MAX)    return HumBand::COMFORT;
    if (h < H_HUMID_MAX)   return HumBand::HUMID;
    return HumBand::WET;
}

void bandToBlink(TempBand b, uint32_t& onMs, uint32_t& offMs) {
    switch (b) {
        case TempBand::COLD:     onMs = 1000; offMs = 1000; break;
        case TempBand::NORMAL:   onMs = 300;  offMs = 300;  break;
        case TempBand::HOT:      onMs = 120;  offMs = 120;  break;
        case TempBand::CRITICAL: onMs = 0;    offMs = 0;    break;
    }
}
