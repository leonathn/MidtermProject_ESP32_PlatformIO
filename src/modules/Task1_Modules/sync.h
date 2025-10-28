#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "band.h"

// Declared in your main file:
extern volatile TempBand gBand;
extern SemaphoreHandle_t semBandChanged;

// Create semaphore & init any shared state
bool sync_init();
