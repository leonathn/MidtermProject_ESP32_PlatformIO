#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hum_band.h"

// Shared control state
extern volatile HumBand gHumBand;
extern SemaphoreHandle_t semHumChanged;

bool sync_init();  // creates semaphore and initializes state
