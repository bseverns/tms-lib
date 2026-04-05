#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from seedbox/src/util/Smoother.h.
// Separate rise/fall rates are useful when you want fast attacks but slower
// releases, or physically plausible motion in UI/automation.
struct SlewLimiter {
  float state = 0.0f;
  float riseStep = 0.01f;
  float fallStep = 0.02f;

  inline void reset(float value) { state = value; }

  inline void setSteps(float rise, float fall) {
    riseStep = rise < 0.0f ? 0.0f : rise;
    fallStep = fall < 0.0f ? 0.0f : fall;
  }

  inline float tick(float target) {
    float delta = target - state;
    if (delta > riseStep) {
      state += riseStep;
    } else if (delta < -fallStep) {
      state -= fallStep;
    } else {
      state = target;
    }
    return state;
  }
};
}
