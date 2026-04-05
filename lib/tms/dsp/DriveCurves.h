#pragma once
#include "tms/core/Types.h"
#include <stddef.h>
#include <stdint.h>
namespace tms {
// Adapted from dust-press/src/CurveBank.{h,cpp}.
// Several compact nonlinear transfer curves plus optional bias, dirt, and
// deterministic crackle.
struct DriveCurves {
  enum Curve : uint8_t { Tanh = 0, Cubic = 1, Diode = 2, Foldback = 3 };

  uint8_t index = Tanh;
  float bias = 0.0f;
  float dirt = 0.0f;
  float chaos = 0.0f;
  uint32_t chaosState = 1u;

  inline void setIndex(uint8_t idx) { index = idx & 0x3u; }
  inline void setBias(float value) { bias = value; }
  inline void setDirt(float value) { dirt = value; }
  inline void setChaos(float value) { chaos = value; }
  inline void reset(uint32_t seed = 1u) { chaosState = seed ? seed : 1u; }

  static inline float cubicSoftClip(float x) {
    float x2 = x * x;
    return x - (x2 * x) * 0.333333f;
  }

  static inline float diodeClip(float x) {
    float alpha = 3.5f;
    if (x >= 0.0f) {
      return 1.0f - expf(-alpha * x);
    }
    return -(1.0f - expf(alpha * x));
  }

  static inline float foldback(float x) {
    float threshold = 1.0f;
    float magnitude = fabsf(x);
    if (magnitude <= threshold) {
      return x;
    }
    float folded = fmodf(magnitude - threshold, threshold * 2.0f);
    float signedFold =
        (folded <= threshold) ? folded : (threshold * 2.0f - folded);
    return (x >= 0.0f ? 1.0f : -1.0f) * signedFold;
  }

  static inline float hardClip(float x) {
    return clampf(x, -1.0f, 1.0f);
  }

  inline float chaosSample() {
    chaosState = chaosState * 1664525u + 1013904223u;
    return (static_cast<int32_t>(chaosState >> 9) / 16777216.0f) - 1.0f;
  }

  inline float applyCrackle(float x) {
    if (chaos <= 0.0f) {
      return x;
    }
    float chaosNorm = clampf(chaos, 0.0f, 7.0f) / 7.0f;
    chaosState = chaosState * 1103515245u + 12345u;
    float rand01 = (chaosState & 0x00FFFFFFu) / 16777216.0f;
    float probability = 0.01f + chaosNorm * 0.04f;
    if (rand01 < probability) {
      float spike =
          ((chaosState >> 8) & 0x00FFFFFFu) / 8388608.0f - 1.0f;
      return hardClip(x + spike * (0.1f + 0.2f * chaosNorm));
    }
    return x;
  }

  inline float tick(float x) {
    float shaped = x + bias;
    shaped += dirt * shaped * shaped * 0.5f * (shaped >= 0.0f ? 1.0f : -1.0f);
    float chaosNorm = clampf(chaos, 0.0f, 7.0f) / 7.0f;
    shaped += chaosSample() * (0.015f * chaosNorm);

    float y = shaped;
    switch (index & 0x3u) {
    case Tanh:
      y = tanhf(shaped);
      break;
    case Cubic:
      y = cubicSoftClip(shaped);
      break;
    case Diode:
      y = diodeClip(shaped);
      break;
    case Foldback:
    default:
      y = foldback(shaped);
      break;
    }
    return applyCrackle(y);
  }

  inline void processBlock(float* samples, size_t count) {
    for (size_t i = 0; i < count; ++i) {
      samples[i] = tick(samples[i]);
    }
  }
};
}
