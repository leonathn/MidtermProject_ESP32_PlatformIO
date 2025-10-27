#pragma once
#include <Arduino.h>

#ifndef NEOPIXEL_PIN
  #define NEOPIXEL_PIN    45 // data pin to your NeoPixel
#endif
#ifndef NUM_PIXELS
  #define NUM_PIXELS    1     // number of LEDs in your strip
#endif

#ifndef SDA_PIN
  #define SDA_PIN       11    // I2C SDA
#endif
#ifndef SCL_PIN
  #define SCL_PIN       12    // I2C SCL
#endif

#ifndef APP_CPU_NUM
  #define APP_CPU_NUM   tskNO_AFFINITY
#endif
