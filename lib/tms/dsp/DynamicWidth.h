#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from horizon/src/DynWidth.{h,cpp}.
// Low side energy stays anchored near mono while upper side energy breathes
// wider on sustains and narrows on transient hits.
struct DynamicWidth {
  float baseWidth = 0.6f;
  float dynAmount = 0.35f;
  float lowAnchorHz = 100.0f;
  float lowSideState = 0.0f;
  float lowAlpha = 0.0f;
  float sampleRate = static_cast<float>(kSampleRate);
  float lastWidth = 0.6f;

  inline DynamicWidth() { setLowAnchorHz(lowAnchorHz); }

  inline void reset() {
    lowSideState = 0.0f;
    lastWidth = baseWidth;
  }

  inline void setBaseWidth(float w) {
    baseWidth = clampf(w, 0.0f, 1.0f);
    lastWidth = baseWidth;
  }

  inline void setDynAmount(float amount) {
    dynAmount = clampf(amount, 0.0f, 1.0f);
  }

  inline void setLowAnchorHz(float hz) {
    lowAnchorHz = clampf(hz, 40.0f, 250.0f);
    float omega = 6.28318531f * lowAnchorHz / sampleRate;
    lowAlpha = 1.0f - expf(-omega);
  }

  inline void setSampleRate(float sr) {
    if (sr <= 0.0f) {
      return;
    }
    sampleRate = sr;
    setLowAnchorHz(lowAnchorHz);
  }

  inline void process(float& mid, float& side, float transientActivity) {
    (void)mid;
    float t = clampf(transientActivity, 0.0f, 1.0f);
    lowSideState += lowAlpha * (side - lowSideState);
    float low = lowSideState;
    float high = side - low;

    float widen = baseWidth + dynAmount;
    float narrow = baseWidth * (1.0f - 0.9f * dynAmount);
    if (widen > 1.0f) {
      widen = 1.0f;
    }
    if (narrow < 0.0f) {
      narrow = 0.0f;
    }

    float widthNow = narrow * t + widen * (1.0f - t);
    lastWidth = widthNow;

    float lowWidthScale = 0.25f;
    high *= widthNow;
    low *= widthNow * lowWidthScale;
    side = low + high;
  }
};
}
