#pragma once
#include <stdint.h>

enum class TempBand : uint8_t { COLD=0, NORMAL, HOT, CRITICAL };

static inline const char* bandName(TempBand b) {
  switch (b) {
    case TempBand::COLD: return "COLD";
    case TempBand::NORMAL: return "NORMAL";
    case TempBand::HOT: return "HOT";
    case TempBand::CRITICAL: return "CRITICAL";
  }
  return "?";
}

static constexpr float T_COLD_MAX   = 20.0f;  // <20      -> COLD
static constexpr float T_NORMAL_MAX = 30.0f;  // 20..29.9 -> NORMAL
static constexpr float T_HOT_MAX    = 40.0f;  // 30..39.9 -> HOT

static inline TempBand classify(float tC) {
  if (tC < T_COLD_MAX)   return TempBand::COLD;
  if (tC < T_NORMAL_MAX) return TempBand::NORMAL;
  if (tC < T_HOT_MAX)    return TempBand::HOT;
  return TempBand::CRITICAL;
}

static inline void bandToBlink(TempBand b, uint32_t& onMs, uint32_t& offMs) {
  switch (b) {
    case TempBand::COLD:     onMs = 1000; offMs = 1000; break;
    case TempBand::NORMAL:   onMs = 300;  offMs = 300;  break;
    case TempBand::HOT:      onMs = 120;  offMs = 120;  break;
    case TempBand::CRITICAL: onMs = 0;    offMs = 0;    break;
  }
}
