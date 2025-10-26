#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/* ---- Hardware ---- */
#define LED_GPIO  GPIO_NUM_48
static const int SDA_PIN = 11;   // your pins
static const int SCL_PIN = 12;

/* ---- DHT20 ---- */
DHT20 dht;

/* ---- Sync ---- */
SemaphoreHandle_t semBandChanged;

/* ---- Bands ---- */
enum class TempBand : uint8_t { COLD=0, NORMAL, HOT, CRITICAL };
volatile TempBand gBand = TempBand::NORMAL;

/* Thresholds (°C) */
const float T_COLD_MAX   = 20.0f;   // <20      -> COLD
const float T_NORMAL_MAX = 30.0f;   // 20..29.9 -> NORMAL
const float T_HOT_MAX    = 40.0f;   // 30..39.9 -> HOT
// >=40 -> CRITICAL

static inline const char* bandName(TempBand b) {
  switch (b) {
    case TempBand::COLD: return "COLD";
    case TempBand::NORMAL: return "NORMAL";
    case TempBand::HOT: return "HOT";
    case TempBand::CRITICAL: return "CRITICAL";
  } return "?";
}
static inline TempBand classify(float tC) {
  if (tC < T_COLD_MAX)   return TempBand::COLD;
  if (tC < T_NORMAL_MAX) return TempBand::NORMAL;
  if (tC < T_HOT_MAX)    return TempBand::HOT;
  return TempBand::CRITICAL;
}
static inline void bandToBlink(TempBand b, uint32_t& onMs, uint32_t& offMs) {
  switch (b) {
    case TempBand::COLD:     onMs = 1000; offMs = 1000; break; // slow
    case TempBand::NORMAL:   onMs = 300;  offMs = 300;  break; // medium
    case TempBand::HOT:      onMs = 120;  offMs = 120;  break; // fast
    case TempBand::CRITICAL: onMs = 0;    offMs = 0;    break; // solid ON
  }
}

/* ----------------- Tasks ----------------- */

void task_read_dht20(void* pv) {
  // discard first read (can be stale)
  dht.read();
  vTaskDelay(pdMS_TO_TICKS(100));

  TempBand last = gBand;

  while (true) {
    dht.read();  // no check per your request
    float t = dht.getTemperature();
    float h = dht.getHumidity();

    TempBand now = classify(t);
    Serial.printf("[DHT20] T=%.2f C, RH=%.2f %%  -> band=%s\n", t, h, bandName(now));

    if (now != last) {
      gBand = now;
      last  = now;
      Serial.printf("[EVENT] Band changed -> %s (semaphore give)\n", bandName(now));
      xSemaphoreGive(semBandChanged);
    }

    vTaskDelay(pdMS_TO_TICKS(500)); // ~2 Hz
  }
}

void task_led(void* pv) {
  pinMode((int)LED_GPIO, OUTPUT);
  digitalWrite((int)LED_GPIO, LOW);

  Serial.println("[LED] Waiting for first band notification...");
  xSemaphoreTake(semBandChanged, portMAX_DELAY);

  bool ledState = false;
  while (true) {
    uint32_t onMs, offMs;
    bandToBlink(gBand, onMs, offMs);
    Serial.printf("[LED] Mode=%s  on=%lu ms  off=%lu ms\n",
                  bandName(gBand), (unsigned long)onMs, (unsigned long)offMs);

    if (gBand == TempBand::CRITICAL) {
      digitalWrite((int)LED_GPIO, HIGH);
      // poll occasionally to react quickly to changes
      xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(50));
      continue;
    }

    digitalWrite((int)LED_GPIO, ledState ? HIGH : LOW);
    uint32_t slice = ledState ? onMs : offMs;
    if (slice == 0) slice = 1;

    // interruptable wait: if a new band arrives, apply immediately
    if (xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(slice)) == pdTRUE) continue;
    ledState = !ledState;
  }
}

/* --------------- Arduino --------------- */

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n=== ESP32-S3 DHT20 + Binary Semaphore LED (no checks) ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  Wire.setTimeOut(50);

  dht.begin(); // no return check
  Serial.println("[DHT20] begin() called");

  semBandChanged = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(task_read_dht20, "DHT20", 3072, nullptr, 2, nullptr, APP_CPU_NUM);
  xTaskCreatePinnedToCore(task_led,       "LED",   2048, nullptr, 1, nullptr, APP_CPU_NUM);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000)); // idle
}
