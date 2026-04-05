#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from tide-engine/core/ParamSmoother.{h,cpp}.
// This version ramps linearly across one audio block instead of using an
// exponential one-pole smoother like Param.h.
struct BlockParam {
  float cur = 0.0f;
  float tgt = 0.0f;
  float step = 0.0f;
  int remaining = 0;

  inline void reset(float value) {
    cur = value;
    tgt = value;
    step = 0.0f;
    remaining = 0;
  }

  inline void setTarget(float value) { tgt = value; }

  inline void beginBlock(int nFrames) {
    if (nFrames <= 0) {
      cur = tgt;
      step = 0.0f;
      remaining = 0;
      return;
    }
    step = (tgt - cur) / static_cast<float>(nFrames);
    remaining = nFrames;
  }

  inline float next() {
    float out = cur;
    if (remaining > 0) {
      cur += step;
      --remaining;
      if (remaining == 0) {
        cur = tgt;
      }
    } else {
      cur = tgt;
    }
    return out;
  }

  inline void advance(int nFrames) {
    if (nFrames <= 0 || remaining <= 0) {
      return;
    }
    if (nFrames >= remaining) {
      cur = tgt;
      step = 0.0f;
      remaining = 0;
      return;
    }
    cur += step * static_cast<float>(nFrames);
    remaining -= nFrames;
  }
};
}
