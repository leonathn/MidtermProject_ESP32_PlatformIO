/**
 * @file task1_sensor.cpp
 * @brief Task 1: DHT20 Sensor Reading with Semaphore Signaling
 * 
 * This task reads temperature and humidity from the DHT20 sensor,
 * classifies the readings into bands, and signals other tasks when
 * significant changes occur using semaphores.
 */

#include <Arduino.h>
#include "../config/config.h"
#include "../config/system_types.h"
#include "../hardware/hardware_manager.h"

/**
 * @brief Task 1 Handler - DHT20 Sensor Reading
 * 
 * Responsibilities:
 * - Read DHT20 sensor every 500ms
 * - Classify temperature into bands (COLD, NORMAL, HOT, CRITICAL)
 * - Classify humidity into bands (DRY, COMFORT, HUMID, WET)
 * - Signal Task 2 (LED) when temperature band changes via semBandChanged
 * - Signal Task 3 (NeoPixel) when humidity band changes via semHumChanged
 * - Always signal LCD update via semLcdUpdate
 * 
 * @param pv Unused parameter (FreeRTOS requirement)
 */
void task_read_dht20(void* pv) {
    // Initial readings to stabilize sensor
    dht.read(); 
    vTaskDelay(pdMS_TO_TICKS(100)); 
    dht.read();

    TempBand lastT = TempBand::NORMAL;
    HumBand  lastH = HumBand::COMFORT;
    bool firstReading = true;  // Flag for first reading

    Serial.println("[TASK1] DHT20 sensor task started");
    Serial.println("[TASK1] Will signal:");
    Serial.println("        - semBandChanged → Task 2 (LED)");
    Serial.println("        - semHumChanged → Task 3 (NeoPixel)");
    Serial.println("        - semLcdUpdate → Task 5 (LCD)");

    for (;;) {
        // Read sensor data
        dht.read();
        float t = dht.getTemperature();
        float h = dht.getHumidity();

        // Update global state (shared with web server)
        gLive.tC = t;
        gLive.rh = h;
        gLive.dht_last_ms = millis();
        gLive.dht_runs++;

        // Classify current readings
        TempBand nowT = classifyTemp(t);
        HumBand  nowH = classifyHum(h);

        // SEMAPHORE SIGNALING: Temperature band change (or first reading)
        if (nowT != lastT || firstReading) {
            gLive.tBand = nowT;
            xSemaphoreGive(semBandChanged);  // ← Signal Task 2 (LED)
            gLive.giveTemp++;
            lastT = nowT;
            if (firstReading) {
                Serial.printf("[TASK1] ✓ First reading: Temp=%s (%.1f°C) → semBandChanged given\n", 
                              bandName(nowT), t);
            } else {
                Serial.printf("[TASK1] ✓ Temp band changed: %s (%.1f°C) → semBandChanged given\n", 
                              bandName(nowT), t);
            }
            
            // AUTO-RESET SOS MODE: If temperature drops from CRITICAL and SOS mode is active
            if (lastT == TempBand::CRITICAL && nowT != TempBand::CRITICAL && gLive.uiMode == 3) {
                gLive.uiMode = 1;  // Switch back to BAR mode (safe visual indicator)
                Serial.println("[TASK1] ✓ Temperature safe → Auto-resetting SOS mode to BAR mode");
            }
        }

        // SEMAPHORE SIGNALING: Humidity band change (or first reading)
        if (nowH != lastH || firstReading) {
            gLive.hBand = nowH;
            xSemaphoreGive(semHumChanged);  // ← Signal Task 3 (NeoPixel)
            gLive.giveHum++;
            lastH = nowH;
            if (firstReading) {
                Serial.printf("[TASK1] ✓ First reading: Hum=%s (%.1f%%) → semHumChanged given\n", 
                              humName(nowH), h);
            } else {
                Serial.printf("[TASK1] ✓ Hum band changed: %s (%.1f%%) → semHumChanged given\n", 
                              humName(nowH), h);
            }
        }

        // SEMAPHORE SIGNALING: Always update LCD
        xSemaphoreGive(semLcdUpdate);  // ← Signal Task 5 (LCD)

        firstReading = false;  // Clear flag after first reading

        // Wait 500ms before next reading
        vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS));
    }
}
