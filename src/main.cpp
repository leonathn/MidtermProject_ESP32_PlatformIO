#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#define BOOT_BUTTON_GPIO GPIO_NUM_0 // ESP32-S3 default BOOT button
#define LED_GPIO GPIO_NUM_48        // LED on GPIO48
SemaphoreHandle_t xBinarySemaphore;

void task_monitor_button(void *pvParameters)
{
  while (1)
  {
    // Check if button is pressed (active-low)
    if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
    {
      // Debounce delay
      vTaskDelay(pdMS_TO_TICKS(50));
      if (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
      {
        // Give the semaphore to signal the LED task
        xSemaphoreGive(xBinarySemaphore);
        // Wait until button is released
        while (gpio_get_level(BOOT_BUTTON_GPIO) == 0)
        {
          vTaskDelay(pdMS_TO_TICKS(10));
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_blink_led(void *pvParameters)
{
  while (1)
  {
    // Wait for the semaphore
    if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY))
    {
      // Blink LED 3 times
      for (int i = 0; i < 3; i++)
      {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
      }
    }
  }
}

void setup(void)
{
  pinMode(BOOT_BUTTON_GPIO, INPUT_PULLUP);
  pinMode(LED_GPIO, OUTPUT);
  // Create binary semaphore
  xBinarySemaphore = xSemaphoreCreateBinary();
  // Create tasks
  xTaskCreate(task_monitor_button, "MonitorButton", 2048, NULL, 10, NULL);
  xTaskCreate(task_blink_led, "BlinkLED", 2048, NULL, 5, NULL);
}

void loop()
{
}