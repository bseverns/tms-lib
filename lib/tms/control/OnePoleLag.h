#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from seedbox/src/util/Smoother.h.
// This is the five-line control smoothing example: one state, one alpha.
struct OnePoleLag {
  float state = 0.0f;
  float alpha = 0.15f;

  inline void reset(float value) { state = value; }

  inline void setAlpha(float value) {
    alpha = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
  }

  inline float tick(float target) {
    state += alpha * (target - state);
    return state;
  }
};
}
