#include "sync.h"

volatile float gTempC = 0.0f;
volatile float gHumRH = 0.0f;
volatile DispState gDispState = DispState::NORMAL;

SemaphoreHandle_t semDispChanged = nullptr;

bool sync_init(){
  if (!semDispChanged)
    semDispChanged = xSemaphoreCreateBinary();
  return semDispChanged != nullptr;
}
