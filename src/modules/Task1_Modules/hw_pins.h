#pragma once
#include <Arduino.h>

// ===== Hardware pins =====
#ifndef LED_GPIO
  #define LED_GPIO  GPIO_NUM_48
#endif

#ifndef SDA_PIN
  #define SDA_PIN   11
#endif

#ifndef SCL_PIN
  #define SCL_PIN   12
#endif

// ===== Task core pinning =====
#ifndef APP_CPU_NUM
  #define APP_CPU_NUM tskNO_AFFINITY   // safe fallback
#endif
