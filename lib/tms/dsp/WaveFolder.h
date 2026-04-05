#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from DiceLoop/src/audio_pipeline.cpp.
// The short memory term slows down the fold motion so tiny control changes
// feel like smear instead of zippering.
struct WaveFolder {
  float noiseNorm = 0.0f;
  float memory = 0.0f;

  inline void reset() { memory = 0.0f; }
  inline void setNoise(float amount) { noiseNorm = clampf(amount, 0.0f, 1.0f); }

  inline float tick(float sample) {
    float drive = 1.0f + noiseNorm * 5.0f;
    float folded = sinf(sample * drive * 3.14159265f);
    memory = 0.92f * memory + 0.08f * folded;
    return clampf(folded + 0.5f * memory, -1.0f, 1.0f);
  }
};
}
