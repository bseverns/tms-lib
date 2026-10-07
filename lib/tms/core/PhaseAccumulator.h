#pragma once
#include "tms/core/Types.h"
namespace tms {
// Original teaching implementation. Phase is cycles in [0,1), rate is Hz.
// tick() returns the current phase, then advances one sample. Negative rates
// run backwards. Set the rate/sample rate outside the inner processing loop.
struct PhaseAccumulator {
  float phase = 0.0f;
  float sampleRate = static_cast<float>(kSampleRate);
  float frequencyHz = 0.0f;
  float increment = 0.0f;

  static float wrap(float cycles) { return cycles - floorf(cycles); }
  void reset(float cycles = 0.0f) {
    phase = isfinite(cycles) ? wrap(cycles) : 0.0f;
  }
  void setFrequency(float hz) {
    frequencyHz = isfinite(hz) ? hz : 0.0f;
    increment = frequencyHz / sampleRate;
  }
  // Invalid sample rates leave the configuration unchanged.
  void setSampleRate(float sr) {
    if (isfinite(sr) && sr > 0.0f) {
      sampleRate = sr;
      setFrequency(frequencyHz);
    }
  }
  float tick() {
    float out = phase;
    phase = wrap(phase + increment);
    return out;
  }
};
}
