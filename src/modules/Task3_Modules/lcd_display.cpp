#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "hw_pins.h"
#include "state.h"
#include "sync.h"

// LCD object
static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

static void showLines(const char* l1, const char* l2){
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(l1);
  if (LCD_ROWS > 1){ lcd.setCursor(0,1); lcd.print(l2); }
}

static void drawState(DispState s, float tC, float rh){
  char l1[21], l2[21];
  // Line1: values
  snprintf(l1, sizeof(l1), "T:%5.1fC  H:%5.1f%%", tC, rh);

  // Line2: state text + simple indicator
  switch(s){
    case DispState::NORMAL:
      snprintf(l2, sizeof(l2), "STATE: NORMAL     ");
      lcd.backlight(); // steady
      break;
    case DispState::WARNING:
      snprintf(l2, sizeof(l2), "STATE: WARNING !  ");
      lcd.backlight(); // steady
      break;
    case DispState::CRITICAL:
      snprintf(l2, sizeof(l2), "STATE: CRITICAL!! ");
      // blink backlight to draw attention
      lcd.backlight(); delay(50); lcd.noBacklight(); delay(50); lcd.backlight();
      break;
  }
  showLines(l1, l2);
}

static void task_lcd(void*){
  lcd.init();
  lcd.backlight();
  showLines("LCD Init...", "Waiting data");

  // Wait for first notification
  xSemaphoreTake(semDispChanged, portMAX_DELAY);

  TickType_t last = xTaskGetTickCount();

  while(true){
    // Snapshot shared values
    float t = gTempC;
    float h = gHumRH;
    DispState s = gDispState;

    drawState(s, t, h);

    // We refresh periodically (e.g., every 1s) but react instantly to changes
    const TickType_t period = pdMS_TO_TICKS(1000);
    if (xSemaphoreTake(semDispChanged, period) == pdTRUE){
      // state changed -> loop immediately and redraw
      continue;
    }
    vTaskDelayUntil(&last, period);
  }
}

void lcd_display_start(){
  xTaskCreatePinnedToCore(task_lcd, "LCD", 3072, nullptr, 1, nullptr, APP_CPU_NUM);
}
