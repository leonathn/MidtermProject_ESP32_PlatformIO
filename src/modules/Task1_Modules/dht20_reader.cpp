#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hw_pins.h"
#include "band.h"
#include "sync.h"

static DHT20 dht;

static void task_read_dht20(void* pv) {
  // I2C & sensor init (idempotent if already done elsewhere)
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(50);
  dht.begin();

  // discard first read (can be stale)
  dht.read();
  vTaskDelay(pdMS_TO_TICKS(100));

  // First measurement -> set band and wake LED once
  dht.read();
  float t0 = dht.getTemperature();
  float h0 = dht.getHumidity();
  if (!isnan(t0)) {
    gBand = classify(t0);
    Serial.printf("[DHT20] INIT T=%.2fC RH=%.2f%% -> %s (give)\n", t0, h0, bandName(gBand));
    xSemaphoreGive(semBandChanged);
  } else {
    Serial.println("[DHT20] INIT NaN, not giving");
  }

  TempBand last = gBand;

  while (true) {
    dht.read();
    float t = dht.getTemperature();
    float h = dht.getHumidity();
    if (isnan(t)) {
      Serial.println("[DHT20] NaN -> skip");
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    TempBand now = classify(t);
    Serial.printf("[DHT20] T=%.2fC RH=%.2f%% -> %s\n", t, h, bandName(now));
    if (now != last) {
      gBand = now;
      last  = now;
      Serial.printf("[EVENT] Band -> %s (give)\n", bandName(now));
      xSemaphoreGive(semBandChanged);
    }
    vTaskDelay(pdMS_TO_TICKS(500)); // ~2 Hz
  }
}

void dht20_reader_start() {
  xTaskCreatePinnedToCore(task_read_dht20, "DHT20", 3072, nullptr, 2, nullptr, APP_CPU_NUM);
}
