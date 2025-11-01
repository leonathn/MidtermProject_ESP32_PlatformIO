/**
 * @file tasks.cpp
 * @brief Task Management - Creates and manages all FreeRTOS tasks
 * 
 * Task implementations are split into separate files:
 * - task1_sensor.cpp: DHT20 sensor reading (signals semaphores)
 * - task2_led_neopixel.cpp: LED + NeoPixel control (waits for semaphores)
 * - task3_lcd.cpp: LCD display (waits for semaphore)
 */

#include "tasks.h"
#include "../config/config.h"
#include "../ml/tinyml.h"

// Task function prototypes (implemented in separate files)
extern void task_read_dht20(void* pv);
extern void task_led(void* pv);
extern void task_neopixel_hum(void* pv);
extern void task_neopixel_ui(void* pv);
extern void task_lcd(void* pv);

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

    createTinyMLTask();
    
    Serial.println("[TASKS] All tasks created successfully");
}
