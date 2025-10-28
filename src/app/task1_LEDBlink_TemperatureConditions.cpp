#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Task1 modules
#include "modules/Task1_Modules/hw_pins.h"
#include "modules/Task1_Modules/band.h"
#include "modules/Task1_Modules/sync.h"
#include "modules/Task1_Modules/dht20_reader.h"
#include "modules/Task1_Modules/led_blinker.h"

// Define shared objects declared in sync.h
volatile TempBand gBand = TempBand::NORMAL;
SemaphoreHandle_t semBandChanged = nullptr;

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n=== ESP32-S3 Task1: DHT20 + LED via semaphore (Task1_Modules) ===");

  if (!sync_init()) {
    Serial.println("[FATAL] Failed to create semaphore");
    while (true) { delay(1000); }
  }

  // Start module tasks
  dht20_reader_start();
  led_blinker_start();
}

void loop() {
  // idle – all work happens in tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}
