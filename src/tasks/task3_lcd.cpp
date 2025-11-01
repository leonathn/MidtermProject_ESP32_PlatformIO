/**
 * @file task3_lcd.cpp
 * @brief Task 5: LCD Display with Semaphore Waiting
 * 
 * This task updates the LCD display whenever signaled by Task 1.
 * It waits for the semLcdUpdate semaphore before refreshing the display.
 */

#include <Arduino.h>
#include "../config/config.h"
#include "../config/system_types.h"
#include "../hardware/hardware_manager.h"

/**
 * @brief Task 5 Handler - LCD Display
 * 
 * Responsibilities:
 * - Wait for semLcdUpdate semaphore from Task 1
 * - Update LCD with current temperature, humidity, and status
 * - Display format:
 *   Line 1: "T:25.5C  H:55%"
 *   Line 2: "STATE: OK/WARN/CRIT"
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

    Serial.println("[TASK5] LCD display task started");
    Serial.println("[TASK5] Waiting for semLcdUpdate from Task 1...");

    for (;;) {
        // SEMAPHORE WAIT: Block until Task 1 signals update
        if (xSemaphoreTake(semLcdUpdate, portMAX_DELAY) == pdTRUE) {
            float t = gLive.tC;
            float h = gLive.rh;
            TempBand tb = gLive.tBand;

            // Line 1: Temperature and Humidity
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("T:");
            lcd.print(t, 1);
            lcd.print("C ");
            lcd.print("H:");
            lcd.print(h, 0);
            lcd.print("%");

            // Line 2: Status based on temperature band
            lcd.setCursor(0, 1);
            switch (tb) {
                case TempBand::COLD:
                case TempBand::NORMAL:
                    lcd.print("STATE: OK");
                    break;
                case TempBand::HOT:
                    lcd.print("STATE: WARN");
                    break;
                case TempBand::CRITICAL:
                    lcd.print("STATE: CRIT");
                    break;
            }

            gLive.lcd_last_ms = millis();
            gLive.lcd_runs++;
            
            Serial.printf("[TASK5] ✓ LCD updated: T=%.1f°C H=%.1f%% State=%s\n", 
                          t, h, bandName(tb));
        }
    }
}
