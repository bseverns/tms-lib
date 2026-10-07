#pragma once
#include "tms/dsp/HilbertPair.h"
#include "tms/dsp/SineOscillator.h"
namespace tms {
// Mono single-sideband modulation: I*cos(theta) - Q*sin(theta).
// Shift is signed Hz: positive moves spectral components upwards, negative
// downwards. This is additive frequency translation, not ratio-based pitch.
// One tick/output sample; finite audio expected, no allocation or clipping.
template<size_t Taps = 129>
class FrequencyShifter {
  HilbertPair<Taps> pair_;
  SineOscillator oscillator_;
public:
  static constexpr size_t latencySamples() { return HilbertPair<Taps>::latencySamples(); }
  // Finite shift within +/- Nyquist is accepted; invalid settings retain state.
  bool setShiftHz(float hz) {
    if (!isfinite(hz) || fabsf(hz) >= oscillator_.clock.sampleRate * 0.5f) return false;
    oscillator_.setFrequency(hz);
    return true;
  }
  bool setSampleRate(float sr) {
    if (!isfinite(sr) || sr <= 0 || fabsf(oscillator_.clock.frequencyHz) >= sr * 0.5f) return false;
    oscillator_.setSampleRate(sr);
    return true;
  }
  void reset() { pair_.reset(); oscillator_.reset(); }
  float tick(float input) {
    float real, quadrature, sine, cosine;
    pair_.tick(input, real, quadrature);
    oscillator_.tickQuadrature(sine, cosine);
    return real * cosine - quadrature * sine;
  }
};
}
