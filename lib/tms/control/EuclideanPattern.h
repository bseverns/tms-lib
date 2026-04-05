#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from seedbox/src/engine/EuclidEngine.cpp.
// Generates a Euclidean gate mask from steps, fills, and rotation.
struct EuclideanPattern {
  static constexpr uint8_t kMaxSteps = 32;

  uint8_t steps = 16;
  uint8_t fills = 4;
  uint8_t rotate = 0;
  uint8_t cursor = 0;
  uint8_t mask[kMaxSteps] = {};

  static inline uint8_t clampSteps(int32_t value) {
    if (value < 1) {
      return 1;
    }
    if (value > kMaxSteps) {
      return kMaxSteps;
    }
    return static_cast<uint8_t>(value);
  }

  static inline uint8_t clampFills(uint8_t stepCount, int32_t value) {
    if (value < 0) {
      return 0;
    }
    if (value > stepCount) {
      return stepCount;
    }
    return static_cast<uint8_t>(value);
  }

  static inline uint8_t wrapRotate(uint8_t stepCount, int32_t value) {
    if (stepCount == 0) {
      return 0;
    }
    int32_t mod = value % stepCount;
    if (mod < 0) {
      mod += stepCount;
    }
    return static_cast<uint8_t>(mod);
  }

  inline void reset() {
    cursor = 0;
    rebuild();
  }

  inline void set(uint8_t stepCount, uint8_t fillCount, uint8_t rotation) {
    steps = clampSteps(stepCount);
    fills = clampFills(steps, fillCount);
    rotate = wrapRotate(steps, rotation);
    rebuild();
  }

  inline void rebuild() {
    for (uint8_t i = 0; i < kMaxSteps; ++i) {
      mask[i] = 0;
    }
    if (fills == 0 || steps == 0) {
      cursor = 0;
      return;
    }
    for (uint8_t i = 0; i < steps; ++i) {
      uint32_t start = static_cast<uint32_t>(i) * fills;
      uint32_t end = static_cast<uint32_t>(i + 1) * fills;
      bool gate = (end / steps) > (start / steps);
      uint8_t rotated =
          static_cast<uint8_t>((i + steps - (rotate % steps)) % steps);
      mask[rotated] = gate ? 1u : 0u;
    }
    if (cursor >= steps) {
      cursor %= steps;
    }
  }

  inline bool gateAt(uint8_t index) const {
    if (steps == 0) {
      return false;
    }
    return mask[index % steps] != 0;
  }

  inline bool tick() {
    if (steps == 0) {
      return false;
    }
    bool gate = mask[cursor] != 0;
    cursor = static_cast<uint8_t>((cursor + 1u) % steps);
    return gate;
  }
};
}
