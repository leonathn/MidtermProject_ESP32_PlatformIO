#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Task2 modules
#include "modules/Task2_Modules/hw_pins.h"
#include "modules/Task2_Modules/hum_band.h"
#include "modules/Task2_Modules/sync.h"
#include "modules/Task2_Modules/dht20_reader.h"
#include "modules/Task2_Modules/neopixel_led.h"

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n=== Task 2: NeoPixel LED Control Based on Humidity (Semaphore) ===");

  Serial.println("+---------------- Humidity → Color mapping ----------------+");
  Serial.println("|   RH < 30%     -> DRY    -> Blue                         |");
  Serial.println("|   30%..59%     -> COMFY  -> Green                        |");
  Serial.println("|   60%..79%     -> HUMID  -> Orange                       |");
  Serial.println("|   RH >= 80%    -> WET    -> Red                          |");
  Serial.println("+----------------------------------------------------------+");

  if (!sync_init()) {
    Serial.println("[FATAL] Failed to create humidity semaphore");
    while (true) { delay(1000); }
  }

  dht20_reader_start();   // producer: computes band, gives semaphore on change
  neopixel_led_start();   // consumer: takes semaphore, updates color
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000)); // idle
}
