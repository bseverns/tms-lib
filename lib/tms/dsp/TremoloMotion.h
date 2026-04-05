#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from DiceLoop/src/audio_pipeline.cpp.
// This is intentionally shallow: it adds movement without fully gating the
// signal away.
struct TremoloMotion {
  float sampleRate = static_cast<float>(kSampleRate);
  float phase = 0.0f;

  inline void setSampleRate(float sr) {
    sampleRate = (sr > 0.0f) ? sr : static_cast<float>(kSampleRate);
  }

  inline void reset() { phase = 0.0f; }

  inline float gain(float densityNorm) {
    densityNorm = clampf(densityNorm, 0.0f, 1.0f);
    float rate = 0.35f + densityNorm * 7.5f;
    phase += 6.28318531f * (rate / sampleRate);
    if (phase > 6.28318531f) {
      phase -= 6.28318531f;
    }
    return 0.75f + 0.25f * sinf(phase);
  }

  inline float tick(float sample, float densityNorm) {
    return sample * gain(densityNorm);
  }
};
}
