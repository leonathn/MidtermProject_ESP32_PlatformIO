#pragma once
#include <Arduino.h>

#ifndef SDA_PIN
  #define SDA_PIN 11
#endif
#ifndef SCL_PIN
  #define SCL_PIN 12
#endif

#ifndef LCD_I2C_ADDR
  #define LCD_I2C_ADDR 33   // common PCF8574 backpack address: 0x27 or 0x3F
#endif
#ifndef LCD_COLS
  #define LCD_COLS 16
#endif
#ifndef LCD_ROWS
  #define LCD_ROWS 2
#endif

#ifndef APP_CPU_NUM
  #define APP_CPU_NUM tskNO_AFFINITY
#endif
