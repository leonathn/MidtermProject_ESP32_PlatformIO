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

        // SEMAPHORE SIGNALING: Temperature band change
        if (nowT != lastT) {
            gLive.tBand = nowT;
            xSemaphoreGive(semBandChanged);  // ← Signal Task 2 (LED)
            gLive.giveTemp++;
            lastT = nowT;
            Serial.printf("[TASK1] ✓ Temp band changed: %s (%.1f°C) → semBandChanged given\n", 
                          bandName(nowT), t);
        }

        // SEMAPHORE SIGNALING: Humidity band change
        if (nowH != lastH) {
            gLive.hBand = nowH;
            xSemaphoreGive(semHumChanged);  // ← Signal Task 3 (NeoPixel)
            gLive.giveHum++;
            lastH = nowH;
            Serial.printf("[TASK1] ✓ Hum band changed: %s (%.1f%%) → semHumChanged given\n", 
                          humName(nowH), h);
        }

        // SEMAPHORE SIGNALING: Always update LCD
        xSemaphoreGive(semLcdUpdate);  // ← Signal Task 5 (LCD)

        // Wait 500ms before next reading
        vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS));
    }
}
