#include "sync.h"

bool sync_init() {
  if (!semBandChanged) {
    semBandChanged = xSemaphoreCreateBinary();
  }
  return semBandChanged != nullptr;
}
