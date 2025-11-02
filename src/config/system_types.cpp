#include "system_types.h"
#include <WiFi.h>

/* ====== Global Variable Definitions ====== */
volatile LiveState gLive;
HistoryBuffer gHistory;
SystemHealth gHealth;
AlertConfig gAlerts;

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

/* ====== Historical Data Management ====== */
void addHistoryPoint(float temp, float hum) {
    gHistory.temp[gHistory.index] = temp;
    gHistory.hum[gHistory.index] = hum;
    gHistory.timestamps[gHistory.index] = millis();
    
    gHistory.index++;
    if (gHistory.index >= HISTORY_SIZE) {
        gHistory.index = 0;
        gHistory.filled = true;
    }
}

/* ====== System Health Monitoring ====== */
void updateSystemHealth() {
    // Memory stats
    gHealth.freeHeap = ESP.getFreeHeap();
    gHealth.minFreeHeap = ESP.getMinFreeHeap();
    
    // WiFi signal strength (only in STA mode)
    if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED) {
        gHealth.wifiRSSI = WiFi.RSSI();
    } else if (WiFi.getMode() == WIFI_AP) {
        gHealth.wifiRSSI = 0;  // AP mode doesn't have RSSI
    }
    
    // Uptime
    gHealth.uptime = millis() / 1000;  // Convert to seconds
    
    // Reset reason (read once at startup, but stored here)
    // This is typically called from main setup
}

/* ====== Alert System ====== */
void checkAlerts() {
    if (!gAlerts.enabled) return;
    
    bool newAlert = false;
    
    // Check temperature critical
    if (gLive.tBand == TempBand::CRITICAL && !gAlerts.tempCritical) {
        gAlerts.tempCritical = true;
        newAlert = true;
    } else if (gLive.tBand != TempBand::CRITICAL) {
        gAlerts.tempCritical = false;
    }
    
    // Check humidity critical
    if (gLive.hBand == HumBand::WET && !gAlerts.humCritical) {
        gAlerts.humCritical = true;
        newAlert = true;
    } else if (gLive.hBand != HumBand::WET) {
        gAlerts.humCritical = false;
    }
    
    // Check TinyML anomaly (score > 0.7)
    if (!isnan(gLive.tinyml_score) && gLive.tinyml_score > 0.7 && !gAlerts.anomalyDetected) {
        gAlerts.anomalyDetected = true;
        newAlert = true;
    } else if (isnan(gLive.tinyml_score) || gLive.tinyml_score <= 0.7) {
        gAlerts.anomalyDetected = false;
    }
    
    // Update alert counter and timestamp
    if (newAlert) {
        gAlerts.lastAlertTime = millis();
        gAlerts.alertCount++;
    }
}
