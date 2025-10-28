#pragma once
#include <stdint.h>

// Bands & Display state
enum class TempBand : uint8_t { COLD=0, NORMAL, HOT, CRITICAL };
enum class HumBand  : uint8_t { DRY=0, COMFY, HUMID, WET };
enum class DispState: uint8_t { NORMAL=0, WARNING, CRITICAL };

// Thresholds
static constexpr float T_COLD_MAX   = 20.0f;   // <20 -> COLD
static constexpr float T_NORMAL_MAX = 30.0f;   // 20..29.9 -> NORMAL
static constexpr float T_HOT_MAX    = 40.0f;   // 30..39.9 -> HOT
// >=40 -> CRITICAL

static constexpr float RH_DRY_MAX   = 30.0f;   // <30   -> DRY
static constexpr float RH_COMFY_MAX = 60.0f;   // 30..59-> COMFY
static constexpr float RH_HUMID_MAX = 80.0f;   // 60..79-> HUMID
// >=80 -> WET

static inline TempBand classifyT(float tC){
  if (tC < T_COLD_MAX)   return TempBand::COLD;
  if (tC < T_NORMAL_MAX) return TempBand::NORMAL;
  if (tC < T_HOT_MAX)    return TempBand::HOT;
  return TempBand::CRITICAL;
}
static inline HumBand classifyRH(float rh){
  if (rh < RH_DRY_MAX)   return HumBand::DRY;
  if (rh < RH_COMFY_MAX) return HumBand::COMFY;
  if (rh < RH_HUMID_MAX) return HumBand::HUMID;
  return HumBand::WET;
}

// Worst-of logic → display state
static inline DispState decideState(TempBand tb, HumBand hb, bool sensorError){
  if (sensorError) return DispState::CRITICAL;

  bool tempCritical = (tb == TempBand::CRITICAL);
  bool humCritical  = (hb == HumBand::WET);
  if (tempCritical || humCritical) return DispState::CRITICAL;

  bool tempWarn = (tb == TempBand::HOT);
  bool humWarn  = (hb == HumBand::HUMID);
  if (tempWarn || humWarn) return DispState::WARNING;

  return DispState::NORMAL;
}

static inline const char* dispStateName(DispState s){
  switch(s){
    case DispState::NORMAL:   return "NORMAL";
    case DispState::WARNING:  return "WARNING";
    case DispState::CRITICAL: return "CRITICAL";
  } return "?";
}
