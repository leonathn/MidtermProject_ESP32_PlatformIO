#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../config/config.h"

/* ====== Hardware Objects ====== */
extern DHT20 dht;
extern LiquidCrystal_I2C lcd;
extern Adafruit_NeoPixel stripHum;
extern Adafruit_NeoPixel stripUI;

/* ====== Semaphores ====== */
extern SemaphoreHandle_t semBandChanged;
extern SemaphoreHandle_t semHumChanged;
extern SemaphoreHandle_t semLcdUpdate;

/* ====== Initialization ====== */
void initHardware();
void initSemaphores();

#endif // HARDWARE_MANAGER_H
