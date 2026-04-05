#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from fog-bank/src/PhaseBlur.{h,cpp}.
// Produces smoothly wandering offset/crossmix values for frozen-buffer or
// delay-cloud style effects.
struct PhaseBlur {
  float sampleRate = static_cast<float>(kSampleRate);
  float blur = 0.0f;
  float drift = 0.0f;
  float scanDepth = 0.0f;
  float spanSamples = 16.0f;
  float offsetAValue = 0.0f;
  float offsetBValue = 0.0f;
  float targetA = 0.0f;
  float targetB = 0.0f;
  float crossmixValue = 0.0f;
  float crossmixTarget = 0.0f;
  float scanPhase = 0.0f;
  float scanValue = 0.0f;
  float slew = 0.02f;
  int samplesUntilRetarget = 1;
  uint32_t state = 1u;

  inline void setSampleRate(float sr) {
    sampleRate = (sr > 1.0f) ? sr : static_cast<float>(kSampleRate);
  }

  inline void setBlur(float value) { blur = clampf(value, 0.0f, 1.0f); }
  inline void setDrift(float value) { drift = clampf(value, 0.0f, 1.0f); }
  inline void setScanDepth(float value) {
    scanDepth = clampf(value, 0.0f, 1.0f);
  }

  inline void setSpan(float samples) {
    spanSamples = (samples > 1.0f) ? samples : 1.0f;
  }

  inline void reset(uint32_t seed = 1u) {
    state = seed ? seed : 1u;
    offsetAValue = 0.0f;
    offsetBValue = 0.0f;
    targetA = 0.0f;
    targetB = 0.0f;
    crossmixValue = 0.0f;
    crossmixTarget = 0.0f;
    scanPhase = 0.0f;
    scanValue = 0.0f;
    samplesUntilRetarget = 1;
  }

  inline float nextSignedRandom() {
    state = state * 1664525u + 1013904223u;
    float unit =
        static_cast<float>((state >> 8) & 0x00FFFFFFu) / 16777215.0f;
    return unit * 2.0f - 1.0f;
  }

  inline void retarget() {
    float width = blur * spanSamples;
    targetA = nextSignedRandom() * width;
    targetB = nextSignedRandom() * width;
    crossmixTarget = 0.5f * blur * nextSignedRandom();
    float driftSeconds = 0.18f - (0.18f - 0.01f) * drift;
    samplesUntilRetarget =
        static_cast<int>(fmaxf(1.0f, sampleRate * driftSeconds));
  }

  inline void step() {
    slew = 0.0025f + drift * 0.06f;
    if (--samplesUntilRetarget <= 0) {
      retarget();
    }

    offsetAValue += (targetA - offsetAValue) * slew;
    offsetBValue += (targetB - offsetBValue) * slew;
    crossmixValue += (crossmixTarget - crossmixValue) * slew;

    float scanRateHz = 0.03f + drift * 0.42f;
    scanPhase += (6.28318531f * scanRateHz) / sampleRate;
    if (scanPhase > 6.28318531f) {
      scanPhase -= 6.28318531f;
    }
    scanValue = sinf(scanPhase) * scanDepth * spanSamples * 3.0f;
  }

  inline float offsetA() const { return offsetAValue; }
  inline float offsetB() const { return offsetBValue; }
  inline float crossmix() const { return crossmixValue; }
  inline float scanOffset() const { return scanValue; }
};
}
