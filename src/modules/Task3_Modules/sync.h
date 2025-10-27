#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "state.h"

// Shared control state (set by sensor task, read by LCD task)
extern volatile float gTempC;
extern volatile float gHumRH;
extern volatile DispState gDispState;

extern SemaphoreHandle_t semDispChanged;

bool sync_init();
