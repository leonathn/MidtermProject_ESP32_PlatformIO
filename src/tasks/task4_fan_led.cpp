#include <Arduino.h>
#include "../config/config.h"
#include "../config/system_types.h"
#include "../hardware/hardware_manager.h"

namespace
{
    LedModuleMode lastStaticMode = LedModuleMode::LED_OFF;
    bool fanState = false;
    uint16_t rainbowHue = 0;

    uint32_t colorWheel(uint8_t pos)
    {
        pos = 255 - pos;
        if (pos < 85)
        {
            return stripAccessories.Color(255 - pos * 3, 0, pos * 3);
        }
        if (pos < 170)
        {
            pos -= 85;
            return stripAccessories.Color(0, pos * 3, 255 - pos * 3);
        }
        pos -= 170;
        return stripAccessories.Color(pos * 3, 255 - pos * 3, 0);
    }

    uint32_t solidColorForMode(LedModuleMode mode)
    {
        switch (mode)
        {
            case LedModuleMode::LED_RED:
                return stripAccessories.Color(255, 0, 0);
            case LedModuleMode::LED_GREEN:
                return stripAccessories.Color(0, 255, 0);
            case LedModuleMode::LED_BLUE:
                return stripAccessories.Color(0, 0, 255);
            default:
                return 0;
        }
    }

    void applyStaticColor(LedModuleMode mode)
    {
        uint32_t color = solidColorForMode(mode);
        for (int i = 0; i < LED_MODULE_COUNT; ++i)
        {
            stripAccessories.setPixelColor(i, color);
        }
        stripAccessories.show();
        gLive.led_module_last_ms = millis();
        gLive.led_module_runs++;
        lastStaticMode = mode;
        Serial.printf("[TASK4] LED module -> %s\n", ledModuleModeName(mode));
    }

    void applyRainbowStep()
    {
        for (int i = 0; i < LED_MODULE_COUNT; ++i)
        {
            uint8_t offset = (rainbowHue + i * 32) & 0xFF;
            stripAccessories.setPixelColor(i, colorWheel(offset));
        }
        stripAccessories.show();
        rainbowHue = (rainbowHue + 4) & 0xFF;
        gLive.led_module_last_ms = millis();
        gLive.led_module_runs++;
    }

    bool computeFanDesired()
    {
        switch (gLive.fanMode)
        {
            case FanMode::FAN_ON:
                return true;
            case FanMode::FAN_AUTO:
                return (gLive.tBand == TempBand::HOT || gLive.tBand == TempBand::CRITICAL);
            case FanMode::FAN_OFF:
            default:
                return false;
        }
    }

    void updateFan()
    {
        bool desired = computeFanDesired();
        if (desired != fanState)
        {
            digitalWrite((int)FAN_GPIO, desired ? HIGH : LOW);
            fanState = desired;
            Serial.printf("[TASK4] Fan output -> %s (mode %s)\n",
                          fanState ? "ON" : "OFF",
                          fanModeName(gLive.fanMode));
        }
        gLive.fanActive = fanState ? 1 : 0;
        gLive.fan_last_ms = millis();
        gLive.fan_runs++;
    }
}

void task_fan_led(void *pvParameters)
{
    // Ensure known defaults
    digitalWrite((int)FAN_GPIO, LOW);
    gLive.fanMode = FanMode::FAN_OFF;
    gLive.fanActive = 0;
    lastStaticMode = LedModuleMode::LED_OFF;

    stripAccessories.clear();
    stripAccessories.show();

    Serial.println("[TASK4] Fan & LED accessory task started");

    for (;;)
    {
        updateFan();

        LedModuleMode mode = gLive.ledModuleMode;
        if (mode == LedModuleMode::LED_RAINBOW)
        {
            if (lastStaticMode != LedModuleMode::LED_RAINBOW)
            {
                Serial.println("[TASK4] LED module -> RAINBOW");
                lastStaticMode = LedModuleMode::LED_RAINBOW;
            }
            applyRainbowStep();
        }
        else if (mode == LedModuleMode::LED_OFF)
        {
            if (lastStaticMode != LedModuleMode::LED_OFF)
            {
                stripAccessories.clear();
                stripAccessories.show();
                gLive.led_module_last_ms = millis();
                gLive.led_module_runs++;
                lastStaticMode = LedModuleMode::LED_OFF;
                Serial.println("[TASK4] LED module -> OFF");
            }
        }
        else if (mode != lastStaticMode)
        {
            applyStaticColor(mode);
        }

        vTaskDelay(pdMS_TO_TICKS(FAN_LED_UPDATE_MS));
    }
}
