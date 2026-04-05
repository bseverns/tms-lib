#pragma once
#include "tms/core/Types.h"
#include <array>
#include <stddef.h>
namespace tms {
// Adapted from silt/src/Plate.{h,cpp}.
// Small four-line plate-style feedback network for compact stereo wash.
struct Plate {
  static constexpr size_t kDelayCount = 4;
  static constexpr size_t kMaxDelaySamples = 576;

  std::array<std::array<float, kMaxDelaySamples>, kDelayCount> buffers = {};
  std::array<size_t, kDelayCount> writeHeads = {};
  std::array<size_t, kDelayCount> delaySamples = {257, 331, 419, 557};
  std::array<float, kDelayCount> damped = {};

  float damping = 0.6f;
  float brightness = 0.4f;

  inline Plate() { reset(); }

  inline void reset() {
    for (size_t i = 0; i < kDelayCount; ++i) {
      buffers[i].fill(0.0f);
    }
    writeHeads.fill(0);
    damped.fill(0.0f);
  }

  inline void setDamping(float value) {
    damping = clampf(value, 0.0f, 1.0f);
  }

  inline void setBrightness(float value) {
    brightness = clampf(value, 0.0f, 1.0f);
  }

  inline StereoFrame process(float input) {
    std::array<float, kDelayCount> delayed = {};
    float resonance = 1.0f - damping;
    float bright = 0.02f + brightness * brightness * 0.93f;
    float decay =
        clampf(0.2f + resonance * resonance * 0.77f, 0.2f, 0.985f);

    for (size_t i = 0; i < kDelayCount; ++i) {
      size_t readIndex =
          (writeHeads[i] + kMaxDelaySamples - delaySamples[i]) % kMaxDelaySamples;
      delayed[i] = buffers[i][readIndex];
      damped[i] += (delayed[i] - damped[i]) * bright;
    }

    float dampedSum = damped[0] + damped[1] + damped[2] + damped[3];
    float common = dampedSum * (0.36f + 0.14f * resonance);
    float excite = input * (0.22f + 0.55f * resonance + 0.18f * brightness);

    for (size_t i = 0; i < kDelayCount; ++i) {
      buffers[i][writeHeads[i]] = excite + decay * (common - damped[i]);
      writeHeads[i] = (writeHeads[i] + 1) % kMaxDelaySamples;
    }

    StereoFrame output;
    output.left =
        (damped[0] + 0.82f * damped[2] - 0.28f * damped[3]) * 0.31f;
    output.right =
        (damped[1] + 0.82f * damped[3] - 0.28f * damped[2]) * 0.31f;
    return output;
  }
};
}
