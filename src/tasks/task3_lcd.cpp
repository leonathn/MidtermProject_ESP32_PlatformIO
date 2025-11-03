/**
 * @file task3_lcd.cpp
 * @brief Task 3: LCD Display with Task 1 & Task 2 Conditions
 * 
 * This task updates the LCD display to show:
 * - Line 1: Task 1 sensor data (Temperature & Humidity)
 * - Line 2: Task 2 LED status (band state and LED on/off)
 */

#include <Arduino.h>
#include "../config/config.h"
#include "../config/system_types.h"
#include "../hardware/hardware_manager.h"

/**
 * @brief Task 3 Handler - LCD Display (Combined Task 1 & Task 2 Info)
 * 
 * Responsibilities:
 * - Wait for semLcdUpdate semaphore from Task 1
 * - Display Task 1 data: Temperature and Humidity readings
 * - Display Task 2 status: Temperature band and LED state
 * - Display format:
 *   Line 1: "T:25.5C  H:55.0%"
 *   Line 2: "HOT LED:ON"
 * 
 * Semaphore Usage:
 * - WAITS on semLcdUpdate (given by Task 1 every time sensor is read)
 * 
 * @param pv Unused parameter (FreeRTOS requirement)
 */
void task_lcd(void* pv) {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESP32-S3 LAB");
    lcd.setCursor(0, 1);
    lcd.print("Task 1 & 2 Info");

    Serial.println("[TASK3] LCD display task started");
    Serial.println("[TASK3] Showing Task 1 (Sensor) & Task 2 (LED) conditions");
    Serial.println("[TASK3] Waiting for semLcdUpdate from Task 1...");

    vTaskDelay(pdMS_TO_TICKS(2000)); // Show startup message for 2 seconds

    for (;;) {
        // SEMAPHORE WAIT: Block until Task 1 signals update
        if (xSemaphoreTake(semLcdUpdate, portMAX_DELAY) == pdTRUE) {
            float t = gLive.tC;
            float h = gLive.rh;
            TempBand tb = gLive.tBand;
            HumBand hb = gLive.hBand;

            // Line 1: Task 1 - Actual Temperature and Humidity values
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("T:");
            lcd.print(t, 1);
            lcd.print("C ");
            lcd.print("H:");
            lcd.print(h, 0);
            lcd.print("%");

            // Line 2: Task 2 - Status/Condition of both Temperature and Humidity
            lcd.setCursor(0, 1);
            
            // Show temperature status (abbreviated)
            switch (tb) {
                case TempBand::COLD:
                    lcd.print("T:COLD ");
                    break;
                case TempBand::NORMAL:
                    lcd.print("T:NORM ");
                    break;
                case TempBand::HOT:
                    lcd.print("T:HOT ");
                    break;
                case TempBand::CRITICAL:
                    lcd.print("T:CRIT ");
                    break;
            }

            // Show humidity status (abbreviated)
            switch (hb) {
                case HumBand::DRY:
                    lcd.print("H:DRY");
                    break;
                case HumBand::COMFORT:
                    lcd.print("H:OK");
                    break;
                case HumBand::HUMID:
                    lcd.print("H:HUM");
                    break;
                case HumBand::WET:
                    lcd.print("H:WET");
                    break;
            }

            gLive.lcd_last_ms = millis();
            gLive.lcd_runs++;
            
            Serial.printf("[TASK3] ✓ LCD updated - Values: T=%.1f°C H=%.1f%% | Status: T=%s H=%s\n", 
                          t, h, bandName(tb), humName(hb));
        }
    }
}
