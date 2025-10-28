#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "hw_pins.h"
#include "band.h"
#include "sync.h"

static void task_led(void* pv) {
  pinMode((int)LED_GPIO, OUTPUT);
  digitalWrite((int)LED_GPIO, LOW);

  Serial.println("[LED] Waiting for first band notification...");
  xSemaphoreTake(semBandChanged, portMAX_DELAY);

  bool ledState = false;
  while (true) {
    uint32_t onMs, offMs;
    bandToBlink(gBand, onMs, offMs);
    Serial.printf("[LED] Mode=%s  on=%lu off=%lu\n",
                  bandName(gBand), (unsigned long)onMs, (unsigned long)offMs);

    if (gBand == TempBand::CRITICAL) {
      digitalWrite((int)LED_GPIO, HIGH);
      // check frequently so we react to band changes ASAP
      (void)xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(50));
      continue;
    }

    // blinking behavior
    digitalWrite((int)LED_GPIO, ledState ? HIGH : LOW);
    uint32_t slice = ledState ? onMs : offMs;
    if (slice == 0) slice = 1;

    // This delay is interruptible by semaphore — no vTaskDelayUntil()
    if (xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(slice)) == pdTRUE) {
      continue; // band changed — recompute timings instantly
    }

    ledState = !ledState;
  }
}


void led_blinker_start() {
  xTaskCreatePinnedToCore(task_led, "LED", 2048, nullptr, 1, nullptr, APP_CPU_NUM);
}
