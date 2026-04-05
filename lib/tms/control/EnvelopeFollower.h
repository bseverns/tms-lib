#pragma once
#include "tms/core/Types.h"
#include <stddef.h>
namespace tms {
// Adapted from dust-press/src/EnvelopeFollower.{h,cpp}.
// This is the standard rectifier + attack/release smoother used for
// compressors, followers, transient meters, and modulation sources.
struct EnvelopeFollower {
  float sampleRate = static_cast<float>(kSampleRate);
  float attackMs = 5.0f;
  float releaseMs = 50.0f;
  float attackCoeff = 0.0f;
  float releaseCoeff = 0.0f;
  float state = 0.0f;

  inline EnvelopeFollower() { updateCoeffs(); }

  inline void setSampleRate(float sr) {
    sampleRate = (sr > 0.0f) ? sr : static_cast<float>(kSampleRate);
    updateCoeffs();
  }

  inline void setAttackMs(float ms) {
    attackMs = ms;
    updateCoeffs();
  }

  inline void setReleaseMs(float ms) {
    releaseMs = ms;
    updateCoeffs();
  }

  inline float tick(float input) {
    float rectified = fabsf(input);
    float coeff = (rectified > state) ? attackCoeff : releaseCoeff;
    state = coeff * state + (1.0f - coeff) * rectified;
    return state;
  }

  inline void processBlock(const float* input, float* envelope, size_t count) {
    for (size_t i = 0; i < count; ++i) {
      envelope[i] = tick(input[i]);
    }
  }

  inline void reset(float value = 0.0f) {
    state = value;
    updateCoeffs();
  }

  inline void updateCoeffs() {
    float attackSamples = (attackMs * 0.001f) * sampleRate;
    float releaseSamples = (releaseMs * 0.001f) * sampleRate;
    attackCoeff = (attackSamples > 1.0f) ? expf(-1.0f / attackSamples) : 0.0f;
    releaseCoeff =
        (releaseSamples > 1.0f) ? expf(-1.0f / releaseSamples) : 0.0f;
  }
};
}
