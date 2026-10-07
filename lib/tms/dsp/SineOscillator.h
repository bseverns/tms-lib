#pragma once
#include "tms/core/PhaseAccumulator.h"
namespace tms {
// One oscillator tick per sample. Output is [-1,1]; cosine leads sine by a
// quarter cycle. Uses visible trig math rather than a wavetable. Frequencies
// at or above Nyquist alias; callers choose appropriate rates for their patch.
struct SineOscillator {
  PhaseAccumulator clock;
  void setSampleRate(float sr) { clock.setSampleRate(sr); }
  void setFrequency(float hz) { clock.setFrequency(hz); }
  void reset(float cycles = 0.0f) { clock.reset(cycles); }
  float tick() { return sinf(2.0f * kPi * clock.tick()); }
  void tickQuadrature(float& sine, float& cosine) {
    float angle = 2.0f * kPi * clock.tick();
    sine = sinf(angle);
    cosine = cosf(angle);
  }
};
}
