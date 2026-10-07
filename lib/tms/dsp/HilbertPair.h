#pragma once
#include "tms/dsp/FIR.h"
#include "tms/dsp/Window.h"
namespace tms {
// Original windowed ideal Hilbert FIR, not FieldKitFX's allpass implementation.
// Produces delayed real I and approximate Hilbert Q (cosine -> sine).
// Odd length, Hann window, h[m]=2/(pi*m) for odd m, zero for even m.
// Latency is (Taps-1)/2 samples. Approximation weakens near DC and Nyquist.
template<size_t Taps = 129>
class HilbertPair {
  static_assert(Taps >= 3 && Taps % 2 == 1, "Hilbert FIR requires odd length >= 3");
  static constexpr size_t kDelay = (Taps - 1) / 2;
  FIR<Taps> quadrature_;
  std::array<float, kDelay + 1> realHistory_ = {};
  size_t head_ = 0;
public:
  static constexpr size_t latencySamples() { return kDelay; }
  HilbertPair() {
    std::array<float, Taps> coefficients = {};
    for (size_t i = 0; i < Taps; ++i) {
      double offset = static_cast<double>(i) - static_cast<double>(kDelay);
      if (i % 2 != kDelay % 2)
        coefficients[i] = static_cast<float>(2.0 / (3.14159265358979323846 * offset)) * hannWindow(i, Taps);
    }
    quadrature_.load(coefficients.data(), Taps);
  }
  void reset() { quadrature_.reset(); realHistory_.fill(0); head_ = 0; }
  void tick(float input, float& real, float& quadrature) {
    realHistory_[head_] = input;
    if (++head_ == realHistory_.size()) head_ = 0;
    real = realHistory_[head_];
    quadrature = quadrature_.tick(input);
  }
};
}
