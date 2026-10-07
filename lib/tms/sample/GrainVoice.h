#pragma once
#include "tms/sample/SampleView.h"
#include "tms/dsp/Window.h"
namespace tms {
// Original teaching renderer; see docs/GRANULAR_LAB.md for seedBox references.
// A grain reads an immutable source through a Hann window. Duration is OUTPUT
// samples, start is SOURCE samples, rate is source samples/output sample.
// Signed rates allow reverse; zero rate freezes one source position.
struct GrainVoice {
  SampleView source;
  double position = 0, rate = 1;
  size_t age = 0, duration = 0;
  float gain = 1;
  bool wrap = true;
  void stop() { age = duration = 0; }
  bool active() const { return age < duration; }
  bool trigger(SampleView view, double start, double speed, size_t frames,
               float level = 1, bool loopSource = true) {
    stop();
    if (!view.data || !view.size || frames < 3 || !isfinite(start) ||
        !isfinite(speed) || !isfinite(level) || level < 0) return false;
    source = view; duration = frames; gain = clampf(level, 0, 1);
    wrap = loopSource; position = start; rate = speed;
    if (wrap) {
      position = fmod(position, static_cast<double>(source.size));
      if (position < 0) position += source.size;
      rate = fmod(rate, static_cast<double>(source.size));
    }
    return true;
  }
  float tick() {
    if (!active()) return 0;
    float sample = 0;
    // Non-wrapping grains zero-pad outside the view rather than hold its edge.
    if (wrap || (position >= 0 && position <= static_cast<double>(source.size - 1)))
      sample = source.read(position, wrap);
    float out = sample * gain * hannWindow(age, duration);
    ++age;
    position += rate;
    if (wrap) {
      position = fmod(position, static_cast<double>(source.size));
      if (position < 0) position += source.size;
    }
    return out;
  }
};
}
