#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "hw_pins.h"
#include "hum_band.h"
#include "sync.h"

static Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

static inline uint32_t packColorScaled(RGB c, uint8_t brightness){
  auto sc = [&](uint8_t x)->uint8_t { return (uint16_t)x * brightness / 255; };
  return pixels.Color(sc(c.r), sc(c.g), sc(c.b));
}

static void task_neopixel(void*){
  pixels.begin();
  pixels.setBrightness(255);
  pixels.clear();
  pixels.show();

  Serial.println("[PIXEL] Waiting for first humidity notification...");
  xSemaphoreTake(semHumChanged, portMAX_DELAY);

  uint32_t lastChangeMs = millis();
  const uint8_t baseBrightness = 140;
  const uint8_t breatheDepth   = 70;
  const uint16_t breathePeriod = 2000;

  while(true){
    HumBand b = gHumBand;

    // breathing brightness
    uint32_t ms = millis();
    float phase = (float)((ms - lastChangeMs) % breathePeriod) / breathePeriod;
    float wave = 0.5f - 0.5f * cosf(2.0f * 3.1415926f * phase);
    uint8_t bri = baseBrightness - breatheDepth/2 + (uint8_t)(wave * breatheDepth);

    uint32_t col = packColorScaled(humBandColor(b), bri);
    for (uint16_t i=0;i<NUM_PIXELS;i++) pixels.setPixelColor(i, col);
    pixels.show();

    // react fast to band changes
    if (xSemaphoreTake(semHumChanged, pdMS_TO_TICKS(50)) == pdTRUE){
      lastChangeMs = millis();
    }
  }
}

void neopixel_led_start(){
  xTaskCreatePinnedToCore(task_neopixel, "NEOPIX", 3072, nullptr, 1, nullptr, APP_CPU_NUM);
}
