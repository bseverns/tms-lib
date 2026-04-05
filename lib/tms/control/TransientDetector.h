#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from horizon/src/TransientDetector.{h,cpp}.
// Envelope follower tuned as a simple "how hit-like is this?" detector.
struct TransientDetector {
  float env = 0.0f;
  float sensitivity = 0.5f;
  float attackCoeff = 0.0f;
  float releaseCoeff = 0.0f;
  float sampleRate = static_cast<float>(kSampleRate);

  inline TransientDetector() { setSampleRate(sampleRate); }

  inline void reset() { env = 0.0f; }

  inline void setSensitivity(float s) {
    sensitivity = clampf(s, 0.0f, 1.0f);
  }

  inline void setSampleRate(float sr) {
    if (sr <= 0.0f) {
      return;
    }
    sampleRate = sr;
    attackCoeff = 1.0f - expf(-1.0f / (0.002f * sampleRate));
    releaseCoeff = 1.0f - expf(-1.0f / (0.080f * sampleRate));
  }

  inline float tick(float x) {
    float ax = fabsf(x);
    float coeff = (ax > env) ? attackCoeff : releaseCoeff;
    env += coeff * (ax - env);

    float threshold = 0.05f + sensitivity * 0.45f;
    float activity = 0.0f;
    if (env > threshold) {
      activity = (env - threshold) / (1.0f - threshold);
    }
    return clampf(activity, 0.0f, 1.0f);
  }
};
}
