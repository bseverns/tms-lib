#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from horizon/src/AirEQ.{h,cpp}.
// One-pole "air band" EQ: split into low + high, then scale the high band.
struct AirEQ {
  float alpha = 0.0f;
  float low = 0.0f;
  float highGain = 1.0f;

  inline AirEQ() { setFreqAndGain(10000.0f, 0.0f); }

  inline void setFreqAndGain(float freqHz, float gainDb) {
    freqHz = clampf(freqHz, 4000.0f, 16000.0f);
    float omega = 6.28318531f * freqHz / static_cast<float>(kSampleRate);
    alpha = 1.0f - expf(-omega);
    highGain = dbToLinear(clampf(gainDb, -6.0f, 6.0f));
  }

  inline void reset() { low = 0.0f; }

  inline float tick(float x) {
    low += alpha * (x - low);
    float high = x - low;
    return low + high * highGain;
  }
};
}
