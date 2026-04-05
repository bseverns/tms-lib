#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from DiceLoop/src/audio_pipeline.cpp.
// The source control was named `noiseNorm`: 0 keeps more bits, 1 keeps fewer.
struct BitCrusher {
  float noiseNorm = 0.0f;

  inline void setNoise(float amount) { noiseNorm = clampf(amount, 0.0f, 1.0f); }

  inline int crushBits() const {
    int bits = static_cast<int>(roundf(2.0f + (1.0f - noiseNorm) * 6.0f));
    if (bits < 2) {
      bits = 2;
    }
    if (bits > 8) {
      bits = 8;
    }
    return bits;
  }

  inline float tick(float sample) const {
    int steps = 1 << crushBits();
    int crushed = static_cast<int>(sample * static_cast<float>(steps));
    return static_cast<float>(crushed) / static_cast<float>(steps);
  }
};
}
