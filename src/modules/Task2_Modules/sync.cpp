#include "sync.h"

volatile HumBand gHumBand = HumBand::COMFY;
SemaphoreHandle_t semHumChanged = nullptr;

bool sync_init(){
  if (!semHumChanged)
    semHumChanged = xSemaphoreCreateBinary();
  return semHumChanged != nullptr;
}
