#pragma once
#include "tms/control/BlockParam.h"
namespace tms {
// Adapted from tide-engine/core/WowFlutter.{h,cpp}.
// Returns a delay modulation offset in milliseconds: slow wow plus faster
// flutter, both smoothed per block.
struct WowFlutter {
  float sampleRate = static_cast<float>(kSampleRate);
  float wowPhase = 0.0f;
  float flutterPhase = 0.0f;
  BlockParam wowDepthMs;
  BlockParam wowRateHz;
  BlockParam flutterDepthMs;
  BlockParam flutterRateHz;

  static inline float triangle(float phase) {
    float v = phase - 0.5f;
    if (v < 0.0f) {
      v = -v;
    }
    return 1.0f - 4.0f * v;
  }

  inline void setSampleRate(float sr) {
    sampleRate = (sr > 0.0f) ? sr : static_cast<float>(kSampleRate);
  }

  inline void reset(float wowDepth = 0.0f, float wowRate = 0.35f,
                    float flutterDepth = 0.0f, float flutterRate = 6.0f) {
    wowDepthMs.reset(clampf(wowDepth, 0.0f, 35.0f));
    wowRateHz.reset(clampf(wowRate, 0.05f, 2.5f));
    flutterDepthMs.reset(clampf(flutterDepth, 0.0f, 12.0f));
    flutterRateHz.reset(clampf(flutterRate, 4.0f, 18.0f));
  }

  inline void setWow(float depthMs, float rateHz) {
    wowDepthMs.setTarget(clampf(depthMs, 0.0f, 35.0f));
    wowRateHz.setTarget(clampf(rateHz, 0.05f, 2.5f));
  }

  inline void setFlutter(float depthMs, float rateHz) {
    flutterDepthMs.setTarget(clampf(depthMs, 0.0f, 12.0f));
    flutterRateHz.setTarget(clampf(rateHz, 4.0f, 18.0f));
  }

  inline void syncToTime(double seconds) {
    if (seconds < 0.0) {
      seconds = 0.0;
    }
    double wowCycles = seconds * static_cast<double>(wowRateHz.cur);
    double flutterCycles = seconds * static_cast<double>(flutterRateHz.cur);
    wowPhase = static_cast<float>(wowCycles - floor(wowCycles));
    flutterPhase = static_cast<float>(flutterCycles - floor(flutterCycles));
  }

  inline void beginBlock(int nFrames) {
    wowDepthMs.beginBlock(nFrames);
    wowRateHz.beginBlock(nFrames);
    flutterDepthMs.beginBlock(nFrames);
    flutterRateHz.beginBlock(nFrames);
  }

  inline float tick() {
    float wowDepth = wowDepthMs.next();
    float wowRate = wowRateHz.next();
    float flutterDepth = flutterDepthMs.next();
    float flutterRate = flutterRateHz.next();

    wowPhase += wowRate / sampleRate;
    if (wowPhase >= 1.0f) {
      wowPhase -= 1.0f;
    }
    flutterPhase += flutterRate / sampleRate;
    if (flutterPhase >= 1.0f) {
      flutterPhase -= 1.0f;
    }

    float wow = triangle(wowPhase) * wowDepth;
    float flutter = triangle(flutterPhase) * flutterDepth;
    return wow + flutter;
  }
};
}
