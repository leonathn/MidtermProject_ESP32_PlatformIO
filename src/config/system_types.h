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

/* ====== Historical Data Buffer ====== */
#define HISTORY_SIZE 50  // Keep last 50 readings (25 seconds at 500ms rate)

struct HistoryBuffer {
    float temp[HISTORY_SIZE];
    float hum[HISTORY_SIZE];
    uint32_t timestamps[HISTORY_SIZE];
    uint8_t index = 0;
    bool filled = false;
};

/* ====== System Health Structure ====== */
struct SystemHealth {
    uint32_t freeHeap = 0;
    uint32_t minFreeHeap = 0;
    uint8_t cpuCore0 = 0;  // CPU usage percentage (0-100)
    uint8_t cpuCore1 = 0;
    int8_t wifiRSSI = 0;   // WiFi signal strength (dBm)
    uint32_t uptime = 0;
    uint8_t resetReason = 0;
};

/* ====== Alert Configuration ====== */
struct AlertConfig {
    bool enabled = true;
    bool tempCritical = false;  // Alert triggered flags
    bool humCritical = false;
    bool anomalyDetected = false;
    uint32_t lastAlertTime = 0;
    uint32_t alertCount = 0;
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
extern HistoryBuffer gHistory;
extern SystemHealth gHealth;
extern AlertConfig gAlerts;

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

// Historical data management
void addHistoryPoint(float temp, float hum);
void updateSystemHealth();
void checkAlerts();

#endif // SYSTEM_TYPES_H
