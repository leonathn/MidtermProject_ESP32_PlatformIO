#include "tasks.h"
#include "../config/config.h"
#include "../config/system_types.h"
#include "../hardware/hardware_manager.h"

/* ====== Task 1: DHT20 Sensor Reading ====== */
void task_read_dht20(void* pv) {
    // Initial readings to stabilize sensor
    dht.read(); 
    vTaskDelay(pdMS_TO_TICKS(100)); 
    dht.read();

    TempBand lastT = TempBand::NORMAL;
    HumBand  lastH = HumBand::COMFORT;

    Serial.println("[TASK1] DHT20 sensor task started");

    for (;;) {
        dht.read();
        float t = dht.getTemperature();
        float h = dht.getHumidity();

        // Update global state
        gLive.tC = t;
        gLive.rh = h;
        gLive.dht_last_ms = millis();
        gLive.dht_runs++;

        // Classify current readings
        TempBand nowT = classifyTemp(t);
        HumBand  nowH = classifyHum(h);

        // Signal temperature band change
        if (nowT != lastT) {
            gLive.tBand = nowT;
            xSemaphoreGive(semBandChanged);
            gLive.giveTemp++;
            lastT = nowT;
            Serial.printf("[TASK1] Temp band changed: %s (%.1f°C)\n", bandName(nowT), t);
        }

        // Signal humidity band change
        if (nowH != lastH) {
            gLive.hBand = nowH;
            xSemaphoreGive(semHumChanged);
            gLive.giveHum++;
            lastH = nowH;
            Serial.printf("[TASK1] Hum band changed: %s (%.1f%%)\n", humName(nowH), h);
        }

        // Always signal LCD to update
        xSemaphoreGive(semLcdUpdate);

        vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS));
    }
}

/* ====== Task 2: LED Temperature Indicator ====== */
void task_led(void* pv) {
    pinMode((int)LED_GPIO, OUTPUT);
    digitalWrite((int)LED_GPIO, LOW);
    gLive.ledOn = 0;

    Serial.println("[TASK2] LED control task started");

    // Wait for first temperature reading
    xSemaphoreTake(semBandChanged, portMAX_DELAY);
    gLive.takeTemp++;

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
            
            if (xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(100)) == pdTRUE) {
                gLive.takeTemp++;
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

        // Wait for timeout or band change
        if (xSemaphoreTake(semBandChanged, pdMS_TO_TICKS(slice)) == pdTRUE) {
            gLive.takeTemp++;
            continue;
        }

        ledState = !ledState;
    }
}

/* ====== Task 3: NeoPixel Humidity Indicator ====== */
void task_neopixel_hum(void* pv) {
    stripHum.begin();
    stripHum.show();

    Serial.println("[TASK3] NeoPixel humidity indicator started");

    // Wait for first humidity reading
    xSemaphoreTake(semHumChanged, portMAX_DELAY);
    gLive.takeHum++;

    for (;;) {
        // Set color based on humidity band
        switch (gLive.hBand) {
            case HumBand::DRY:     
                stripHum.setPixelColor(0, stripHum.Color(0, 0, 255)); // Blue
                break;
            case HumBand::COMFORT: 
                stripHum.setPixelColor(0, stripHum.Color(0, 255, 0)); // Green
                break;
            case HumBand::HUMID:   
                stripHum.setPixelColor(0, stripHum.Color(255, 255, 0)); // Yellow
                break;
            case HumBand::WET:     
                stripHum.setPixelColor(0, stripHum.Color(255, 0, 0)); // Red
                break;
        }
        stripHum.show();
        gLive.neo_last_ms = millis();
        gLive.neo_runs++;

        // Wait for humidity change
        if (xSemaphoreTake(semHumChanged, portMAX_DELAY) == pdTRUE) {
            gLive.takeHum++;
        }
    }
}

/* ====== Task 4: NeoPixel UI Bar ====== */
void task_neopixel_ui(void* pv) {
    stripUI.begin();
    stripUI.show();
    uint32_t hue = 0;

    Serial.println("[TASK4] NeoPixel UI bar started");

    for (;;) {
        if (gLive.uiMode == 0) {
            // OFF mode: all pixels off
            for (int i = 0; i < NEOPIXEL_UI_NUM; i++) {
                stripUI.setPixelColor(i, 0);
            }
            stripUI.show();
            
        } else if (gLive.uiMode == 1) {
            // BAR mode: show humidity as bar graph
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
            // DEMO mode: rainbow animation
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

/* ====== Task 5: LCD Display ====== */
void task_lcd(void* pv) {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESP32-S3 LAB");

    Serial.println("[TASK5] LCD display task started");

    for (;;) {
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
        }
    }
}

/* ====== Task Creation ====== */
void createAllTasks() {
    Serial.println("[TASKS] Creating FreeRTOS tasks...");
    
    xTaskCreatePinnedToCore(
        task_read_dht20, 
        "DHT20", 
        TASK_DHT_STACK_SIZE, 
        nullptr, 
        TASK_DHT_PRIORITY, 
        nullptr, 
        APP_CPU_NUM
    );
    
    xTaskCreatePinnedToCore(
        task_led, 
        "LED", 
        TASK_LED_STACK_SIZE, 
        nullptr, 
        TASK_LED_PRIORITY, 
        nullptr, 
        APP_CPU_NUM
    );
    
    xTaskCreatePinnedToCore(
        task_neopixel_hum, 
        "NEO_H", 
        TASK_NEO_HUM_STACK_SIZE, 
        nullptr, 
        TASK_NEO_PRIORITY, 
        nullptr, 
        APP_CPU_NUM
    );
    
    xTaskCreatePinnedToCore(
        task_neopixel_ui, 
        "NEO_UI", 
        TASK_NEO_UI_STACK_SIZE, 
        nullptr, 
        TASK_LCD_PRIORITY, 
        nullptr, 
        APP_CPU_NUM
    );
    
    xTaskCreatePinnedToCore(
        task_lcd, 
        "LCD", 
        TASK_LCD_STACK_SIZE, 
        nullptr, 
        TASK_LCD_PRIORITY, 
        nullptr, 
        APP_CPU_NUM
    );
    
    Serial.println("[TASKS] All tasks created successfully");
}
