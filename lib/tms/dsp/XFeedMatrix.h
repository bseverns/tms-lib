#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from tide-engine/core/XFeedMatrix.{h,cpp}.
// Simple stereo crossfeed: each channel keeps its own signal and receives a
// small amount of the opposite channel.
struct XFeedMatrix {
  float lr = 0.2f;
  float rl = 0.2f;

  inline void set(float leftToRight, float rightToLeft) {
    lr = clampf(leftToRight, 0.0f, 0.6f);
    rl = clampf(rightToLeft, 0.0f, 0.6f);
  }

  inline void process(float inL, float inR, float& outL, float& outR) const {
    outL = inL + rl * inR;
    outR = inR + lr * inL;
  }
};
}
