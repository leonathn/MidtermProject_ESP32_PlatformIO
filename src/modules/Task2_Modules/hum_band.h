#pragma once
#include <stdint.h>

enum class HumBand : uint8_t { DRY=0, COMFY, HUMID, WET };

static inline const char* humBandName(HumBand b){
  switch(b){
    case HumBand::DRY:   return "DRY";
    case HumBand::COMFY: return "COMFY";
    case HumBand::HUMID: return "HUMID";
    case HumBand::WET:   return "WET";
  } return "?";
}

// Ranges (%RH)
static constexpr float RH_DRY_MAX   = 30.0f;  // <30
static constexpr float RH_COMFY_MAX = 60.0f;  // 30..59
static constexpr float RH_HUMID_MAX = 80.0f;  // 60..79
// >=80 -> WET

static inline HumBand classifyRH(float rh){
  if (rh < RH_DRY_MAX)   return HumBand::DRY;
  if (rh < RH_COMFY_MAX) return HumBand::COMFY;
  if (rh < RH_HUMID_MAX) return HumBand::HUMID;
  return HumBand::WET;
}

// Simple RGB container
struct RGB { uint8_t r,g,b; };

// Base colors (no brightness scaling)
static inline RGB humBandColor(HumBand b){
  switch(b){
    case HumBand::DRY:   return {  0,  0,255}; // Blue
    case HumBand::COMFY: return {  0,255,  0}; // Green
    case HumBand::HUMID: return {255,120,  0}; // Orange
    case HumBand::WET:   return {255,  0,  0}; // Red
  }
  return {0,0,0};
}
