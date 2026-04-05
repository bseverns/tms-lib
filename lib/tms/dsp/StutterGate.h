#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from DiceLoop/src/audio_pipeline.cpp.
// This is a tiny sample-and-hold glitcher. Probability mode derives the hold
// window from density; tempo-locked mode expects the caller to set a musical
// window length in samples.
enum class StutterMode { Probability, TempoLocked };

struct StutterGate {
  StutterMode mode = StutterMode::Probability;
  int holdCountdown = 0;
  int tempoWindowSamples = kBlockSize;
  float heldSample = 0.0f;

  inline void reset() {
    holdCountdown = 0;
    heldSample = 0.0f;
  }

  inline void setTempoWindowSamples(int samples) {
    tempoWindowSamples = (samples < 1) ? 1 : samples;
  }

  inline float tick(float sample, float densityNorm) {
    densityNorm = clampf(densityNorm, 0.0f, 1.0f);
    int window = 0;
    if (mode == StutterMode::TempoLocked) {
      window = tempoWindowSamples;
    } else {
      window = 3 + static_cast<int>((1.0f - densityNorm) * 160.0f);
    }
    if (window < 1) {
      window = 1;
    }

    if (--holdCountdown <= 0) {
      holdCountdown = window;
      heldSample = sample;
    }

    float freezeBlend = densityNorm * densityNorm;
    return heldSample * freezeBlend + sample * (1.0f - freezeBlend);
  }
};
}
