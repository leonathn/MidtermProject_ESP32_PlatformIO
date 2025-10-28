#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Task3 modules
#include "modules/Task3_Modules/hw_pins.h"
#include "modules/Task3_Modules/state.h"
#include "modules/Task3_Modules/sync.h"
#include "modules/Task3_Modules/dht20_reader.h"
#include "modules/Task3_Modules/lcd_display.h"

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n=== Task 3: Temperature & Humidity Monitoring on LCD (Semaphore) ===");

  Serial.println("+---------------- Display State Mapping -------------------+");
  Serial.println("| NORMAL   : T < 30°C  and  30% ≤ RH < 60%                |");
  Serial.println("| WARNING  : (30°C ≤ T < 40°C) OR (60% ≤ RH < 80%)        |");
  Serial.println("| CRITICAL : (T ≥ 40°C) OR (RH ≥ 80%) OR (sensor error)   |");
  Serial.println("+----------------------------------------------------------+");

  if (!sync_init()) {
    Serial.println("[FATAL] Failed to create semaphore");
    while (true) { delay(1000); }
  }

  dht20_reader_start();   // producer: updates gDispState + gives semaphore when state changes
  lcd_display_start();    // consumer: takes semaphore and refreshes LCD
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
