#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hw_pins.h"
#include "state.h"
#include "sync.h"

static DHT20 dht;

static void task_dht20(void*){
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  Wire.setTimeOut(50);
  dht.begin();

  // Discard first read
  dht.read(); vTaskDelay(pdMS_TO_TICKS(100));

  // Initial publish
  dht.read();
  float t0 = dht.getTemperature();
  float h0 = dht.getHumidity();
  bool sensorError = isnan(t0) || isnan(h0);
  if (!sensorError){
    gTempC = t0; gHumRH = h0;
    DispState s = decideState(classifyT(t0), classifyRH(h0), false);
    gDispState = s;
    Serial.printf("[DHT20] INIT  T=%.2fC  RH=%.2f%%  -> %s (give)\n", t0, h0, dispStateName(s));
    xSemaphoreGive(semDispChanged);
  } else {
    gDispState = DispState::CRITICAL;
    Serial.println("[DHT20] INIT invalid -> CRITICAL (give)");
    xSemaphoreGive(semDispChanged);
  }

  DispState last = gDispState;

  while(true){
    dht.read();
    float t = dht.getTemperature();
    float h = dht.getHumidity();
    bool err = isnan(t) || isnan(h);

    if (err){
      gDispState = DispState::CRITICAL;
      // only give if state actually changed
      if (last != gDispState){
        last = gDispState;
        Serial.println("[EVENT] Sensor error -> CRITICAL (give)");
        xSemaphoreGive(semDispChanged);
      }
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    gTempC = t; gHumRH = h;
    DispState now = decideState(classifyT(t), classifyRH(h), false);
    if (now != last){
      last = now; gDispState = now;
      Serial.printf("[EVENT] State -> %s (give)  T=%.2f  RH=%.2f\n", dispStateName(now), t, h);
      xSemaphoreGive(semDispChanged);
    } else {
      Serial.printf("[DHT20]        T=%.2fC  RH=%.2f%%  (%s)\n", t, h, dispStateName(now));
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void dht20_reader_start(){
  xTaskCreatePinnedToCore(task_dht20, "DHT20", 4096, nullptr, 2, nullptr, APP_CPU_NUM);
}
