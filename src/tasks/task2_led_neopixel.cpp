/**
 * @file task2_led_neopixel.cpp
 * @brief Task 2 & Task 3: LED Control and NeoPixel Indicators with Semaphore Waiting
 * 
 * Task 2: LED blinking based on temperature bands (waits for semBandChanged)
 * Task 3: NeoPixel humidity indicator (waits for semHumChanged)
 * Task 4: NeoPixel UI bar (user-controlled, no semaphore)
 */

#include <Arduino.h>
#include "../config/config.h"
#include "../config/system_types.h"
#include "../hardware/hardware_manager.h"

/**
 * @brief Task 2 Handler - LED Temperature Indicator
 * 
 * Responsibilities:
 * - Wait for semBandChanged semaphore from Task 1
 * - Adjust LED blinking pattern based on temperature band:
 *   * COLD: Slow blink (1000ms/1000ms)
 *   * NORMAL: Medium blink (300ms/300ms)
 *   * HOT: Fast blink (120ms/120ms)
 *   * CRITICAL: Always ON (no blinking)
 * 
 * Semaphore Usage:
 * - WAITS on semBandChanged (given by Task 1 when temp band changes)
 * 
 * @param pv Unused parameter (FreeRTOS requirement)
 */
void task_led(void* pv) {
    pinMode((int)LED_GPIO, OUTPUT);
    digitalWrite((int)LED_GPIO, LOW);
    gLive.ledOn = 0;

    Serial.println("[TASK2] LED control task started");
    Serial.println("[TASK2] Waiting for semBandChanged from Task 1...");

    // SEMAPHORE WAIT: Block until first temperature reading
    xSemaphoreTake(semBandChanged, portMAX_DELAY);  // ← Wait for Task 1
    gLive.takeTemp++;
    Serial.println("[TASK2] ✓ Received first semBandChanged");

    bool ledState = false;

    for (;;) {
        uint32_t onMs, offMs;
        bandToBlink(gLive.tBand, onMs, offMs);
        gLive.onMs = onMs; 
        gLive.offMs = offMs;

        // CRITICAL state: LED always ON
        if (gLive.tBand == TempBand::CRITICAL) {
            digitalWrite((int)LED_GPIO, HIGH);
            gLive.ledOn = 1;
            gLive.led_last_ms = millis();
            gLive.led_runs++;
            
            // SEMAPHORE WAIT: Check for band change with timeout
            if (xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(100)) == pdTRUE) {
                gLive.takeTemp++;
                Serial.println("[TASK2] ✓ Received semBandChanged (was CRITICAL)");
            }
            continue;
        }

        // Normal blinking
        digitalWrite((int)LED_GPIO, ledState ? HIGH : LOW);
        gLive.ledOn = ledState ? 1 : 0;
        gLive.led_last_ms = millis();
        gLive.led_runs++;

        uint32_t slice = ledState ? onMs : offMs;
        if (slice == 0) slice = 1;

        // SEMAPHORE WAIT: Wait for timeout or band change
        if (xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(slice)) == pdTRUE) {
            gLive.takeTemp++;
            Serial.printf("[TASK2] ✓ Received semBandChanged (new band: %s)\n", 
                          bandName(gLive.tBand));
            continue;  // Restart blink cycle with new pattern
        }

        ledState = !ledState;
    }
}

/**
 * @brief Task 3 Handler - NeoPixel Humidity Indicator
 * 
 * Responsibilities:
 * - Wait for semHumChanged semaphore from Task 1
 * - Update single NeoPixel (GPIO 45) color based on humidity band:
 *   * DRY (< 40%): Blue
 *   * COMFORT (40-60%): Green
 *   * HUMID (60-80%): Yellow
 *   * WET (> 80%): Red
 * 
 * Semaphore Usage:
 * - WAITS on semHumChanged (given by Task 1 when humidity band changes)
 * 
 * @param pv Unused parameter (FreeRTOS requirement)
 */
void task_neopixel_hum(void* pv) {
    stripHum.begin();
    stripHum.show();

    Serial.println("[TASK3] NeoPixel humidity indicator started");
    Serial.println("[TASK3] Waiting for semHumChanged from Task 1...");

    // SEMAPHORE WAIT: Block until first humidity reading
    xSemaphoreTake(semHumChanged, portMAX_DELAY);  // ← Wait for Task 1
    gLive.takeHum++;
    Serial.println("[TASK3] ✓ Received first semHumChanged");

    for (;;) {
        // Set color based on humidity band
        uint32_t color;
        switch (gLive.hBand) {
            case HumBand::DRY:     
                color = stripHum.Color(0, 0, 255);  // Blue
                Serial.println("[TASK3] Setting color: BLUE (DRY)");
                break;
            case HumBand::COMFORT: 
                color = stripHum.Color(0, 255, 0);  // Green
                Serial.println("[TASK3] Setting color: GREEN (COMFORT)");
                break;
            case HumBand::HUMID:   
                color = stripHum.Color(255, 255, 0);  // Yellow
                Serial.println("[TASK3] Setting color: YELLOW (HUMID)");
                break;
            case HumBand::WET:     
                color = stripHum.Color(255, 0, 0);  // Red
                Serial.println("[TASK3] Setting color: RED (WET)");
                break;
        }
        
        stripHum.setPixelColor(0, color);
        stripHum.show();
        gLive.neo_last_ms = millis();
        gLive.neo_runs++;

        // SEMAPHORE WAIT: Block until next humidity change
        if (xSemaphoreTake(semHumChanged, portMAX_DELAY) == pdTRUE) {
            gLive.takeHum++;
            Serial.printf("[TASK3] ✓ Received semHumChanged (new band: %s)\n", 
                          humName(gLive.hBand));
        }
    }
}

/**
 * @brief Task 4 Handler - NeoPixel UI Bar
 * 
 * Responsibilities:
 * - Control 4-pixel NeoPixel strip (GPIO 6) for user interface
 * - Support 3 modes (no semaphore, user-controlled from web):
 *   * Mode 0: OFF - All pixels off
 *   * Mode 1: BAR - Show humidity as bar graph (0-100% → 0-4 LEDs)
 *   * Mode 2: DEMO - Rainbow animation
 * 
 * Semaphore Usage:
 * - NONE (runs independently, no synchronization needed)
 * 
 * @param pv Unused parameter (FreeRTOS requirement)
 */
void task_neopixel_ui(void* pv) {
    stripUI.begin();
    stripUI.show();
    uint32_t hue = 0;

    Serial.println("[TASK4] NeoPixel UI bar started");
    Serial.println("[TASK4] No semaphore - runs independently (user-controlled)");

    for (;;) {
        if (gLive.uiMode == 0) {
            // Mode 0: OFF - all pixels off
            for (int i = 0; i < NEOPIXEL_UI_NUM; i++) {
                stripUI.setPixelColor(i, 0);
            }
            stripUI.show();
            
        } else if (gLive.uiMode == 1) {
            // Mode 1: BAR - show humidity as bar graph
            float h = gLive.rh;
            int ledsOn = map((int)h, 0, 100, 0, NEOPIXEL_UI_NUM);
            if (ledsOn < 0) ledsOn = 0;
            if (ledsOn > NEOPIXEL_UI_NUM) ledsOn = NEOPIXEL_UI_NUM;
            
            for (int i = 0; i < NEOPIXEL_UI_NUM; i++) {
                if (i < ledsOn) {
                    stripUI.setPixelColor(i, stripUI.Color(0, 100, 255));
                } else {
                    stripUI.setPixelColor(i, 0);
                }
            }
            stripUI.show();
            
        } else {
            // Mode 2: DEMO - rainbow animation
            for (int i = 0; i < NEOPIXEL_UI_NUM; i++) {
                uint8_t r = (uint8_t)((sin((hue + i * 40) * 0.02f) + 1) * 127);
                uint8_t g = (uint8_t)((sin((hue + i * 40) * 0.02f + 2.1f) + 1) * 127);
                uint8_t b = (uint8_t)((sin((hue + i * 40) * 0.02f + 4.2f) + 1) * 127);
                stripUI.setPixelColor(i, stripUI.Color(r, g, b));
            }
            stripUI.show();
            hue += 12;
        }
        
        vTaskDelay(pdMS_TO_TICKS(UI_STRIP_UPDATE_MS));
    }
}
