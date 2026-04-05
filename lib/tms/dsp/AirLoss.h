#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from orbit-looper/src/AirLoss.{h,cpp}.
// Distance darkens the signal by lowering the low-pass cutoff and trimming
// gain a bit as the source moves farther away.
struct AirLoss {
  float sampleRate = static_cast<float>(kSampleRate);
  float nearHz = 12000.0f;
  float farHz = 800.0f;
  float amount = 0.0f;
  float distanceNorm = 0.0f;
  float alphaNear = 0.0f;
  float alphaFar = 0.0f;
  float alpha = 0.0f;
  float gain = 1.0f;
  float stateL = 0.0f;
  float stateR = 0.0f;

  inline AirLoss() {
    updateAlphaEndpoints();
    updateCoefficients();
  }

  inline void setSampleRate(float sr) {
    if (sr > 1000.0f) {
      sampleRate = sr;
      updateAlphaEndpoints();
      updateCoefficients();
    }
  }

  inline void setCutoffs(float nearCutoffHz, float farCutoffHz) {
    nearHz = clampf(nearCutoffHz, 2000.0f, 20000.0f);
    farHz = clampf(farCutoffHz, 200.0f, nearHz);
    updateAlphaEndpoints();
    updateCoefficients();
  }

  inline void setAmount(float amt) {
    amount = clampf(amt, 0.0f, 1.0f);
    updateCoefficients();
  }

  inline void setDistanceNorm(float d) {
    distanceNorm = clampf(d, 0.0f, 1.0f);
    updateCoefficients();
  }

  inline void reset() {
    stateL = 0.0f;
    stateR = 0.0f;
  }

  inline void updateAlphaEndpoints() {
    float aNear = expf(-2.0f * 3.14159265f * nearHz / sampleRate);
    float aFar = expf(-2.0f * 3.14159265f * farHz / sampleRate);
    alphaNear = 1.0f - aNear;
    alphaFar = 1.0f - aFar;
  }

  inline void updateCoefficients() {
    float t = clampf(amount * distanceNorm, 0.0f, 1.0f);
    alpha = alphaNear + (alphaFar - alphaNear) * t;
    gain = 1.0f / (1.0f + amount * distanceNorm * 1.5f);
  }

  inline void process(float& l, float& r) {
    stateL += alpha * (l - stateL);
    stateR += alpha * (r - stateR);
    l = stateL * gain;
    r = stateR * gain;
  }
};
}
