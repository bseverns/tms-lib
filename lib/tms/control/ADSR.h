#pragma once
#include "tms/core/Types.h"
namespace tms {
// Original linear ADSR: times in ms, sustain/output in [0,1]. Call tick once
// per sample. Positive segment times round up to whole samples. noteOn()
// retriggers from the current level; noteOff() releases from that level.
// Zero-time stages resolve on the next tick without division by zero.
// Settings apply when the next segment starts, including sample-rate changes.
struct ADSR {
  enum class Stage { Idle, Attack, Decay, Sustain, Release };
  Stage stage = Stage::Idle;
  float sampleRate = static_cast<float>(kSampleRate);
  float attackMs = 5.0f, decayMs = 80.0f, sustain = 0.5f, releaseMs = 100.0f;
  float value = 0.0f;
  float start = 0.0f, target = 0.0f;
  uint32_t duration = 0, elapsed = 0;

  static float safeTime(float ms) {
    return isfinite(ms) ? clampf(ms, 0.0f, 60000.0f) : 0.0f;
  }
  void setSampleRate(float sr) {
    if (isfinite(sr) && sr > 0.0f && sr <= 768000.0f) sampleRate = sr;
  }
  void set(float attack, float decay, float sustainLevel, float release) {
    attackMs = safeTime(attack); decayMs = safeTime(decay);
    sustain = isfinite(sustainLevel) ? clampf(sustainLevel, 0.0f, 1.0f) : 0.0f;
    releaseMs = safeTime(release);
  }
  void reset() { stage = Stage::Idle; value = start = target = 0.0f; duration = elapsed = 0; }
  void begin(Stage next, float level, float ms) {
    stage = next; start = value; target = level;
    duration = static_cast<uint32_t>(ceilf(ms * 0.001f * sampleRate)); elapsed = 0;
  }
  void noteOn() { begin(Stage::Attack, 1.0f, attackMs); }
  void noteOff() { if (stage != Stage::Idle) begin(Stage::Release, 0.0f, releaseMs); }
  bool active() const { return stage != Stage::Idle; }
  float tick() {
    // At most attack, decay, release can be skipped on one sample.
    for (int transitions = 0; transitions < 3; ++transitions) {
      if (stage == Stage::Idle || stage == Stage::Sustain) return value;
      if (duration > 0.0f) {
        ++elapsed;
        value = start + (target - start) * (static_cast<float>(elapsed) / static_cast<float>(duration));
        if (elapsed < duration) return value;
      }
      const bool consumedSample = duration > 0.0f;
      value = target;
      Stage completed = stage;
      if (completed == Stage::Attack) begin(Stage::Decay, sustain, decayMs);
      else if (completed == Stage::Decay) stage = Stage::Sustain;
      else stage = Stage::Idle;
      // A timed stage consumes this sample; its successor starts next tick.
      if (consumedSample || completed == Stage::Release) return value;
    }
    return value;
  }
};
}
