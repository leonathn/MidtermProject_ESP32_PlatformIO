#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hw_pins.h"
#include "hum_band.h"
#include "sync.h"

static DHT20 dht;

static void task_dht20(void*){
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  Wire.setTimeOut(50);
  dht.begin();

  // discard first read
  dht.read(); vTaskDelay(pdMS_TO_TICKS(100));

  // initial publish
  dht.read();
  float t0 = dht.getTemperature();
  float h0 = dht.getHumidity();
  if (!isnan(h0)){
    gHumBand = classifyRH(h0);
    Serial.printf("[DHT20] INIT  T=%.2f C  RH=%.2f %% -> %s (give)\n", t0, h0, humBandName(gHumBand));
    xSemaphoreGive(semHumChanged);
  } else {
    Serial.println("[DHT20] INIT NaN, waiting...");
  }

  HumBand last = gHumBand;

  while(true){
    dht.read();
    float t = dht.getTemperature();
    float h = dht.getHumidity();

    if (isnan(h)){
      Serial.println("[DHT20] RH NaN -> skip");
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    HumBand now = classifyRH(h);
    Serial.printf("[DHT20]      T=%.2f C  RH=%.2f %% -> %s\n", t, h, humBandName(now));

    if (now != last){
      gHumBand = now;
      last = now;
      Serial.printf("[EVENT] Hum band changed -> %s (give)\n", humBandName(now));
      xSemaphoreGive(semHumChanged);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void dht20_reader_start(){
  xTaskCreatePinnedToCore(task_dht20, "DHT20", 4096, nullptr, 2, nullptr, APP_CPU_NUM);
}
