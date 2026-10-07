#pragma once
#include "tms/spectral/SpectralFrame.h"
namespace tms {
// Mono magnitude/phase-advance freeze informed by fog-bank SpectralFrame.
// Copyright (c) 2025 Ben Severns. MIT; see repository LICENSE.
// Captures the next completed frame when hold is enabled; later held frames
// ignore live input, retaining magnitudes and advancing phase. Optional decay
// is a time constant in seconds (zero = infinite). Release resumes live STFT.
template<size_t N = 512, size_t Hop = N/4>
class SpectralHold {
  static constexpr size_t Bins=N/2+1;
  SpectralFrame<N,Hop> frame_;
  std::array<float,Bins> magnitude_={}, phase_={}, previous_={}, advance_={};
  bool requested_=false,captured_=false,initialized_=false;
  float sampleRate_=static_cast<float>(kSampleRate), decaySeconds_=0, decay_=1;
  static float wrap(float phase) { return static_cast<float>(remainder(phase,6.28318530717958647692)); }
  void updateDecay() { decay_=decaySeconds_>0 ? expf(-static_cast<float>(Hop)/(sampleRate_*decaySeconds_)) : 1; }
  template<class Buffer>
  void process(Buffer& real,Buffer& imag) {
    if(!requested_ || !captured_) {
      if(captured_) initialized_=false;
      for(size_t bin=0;bin<Bins;++bin) {
        float phase=atan2f(imag[bin],real[bin]);
        float expected=static_cast<float>(6.28318530717958647692*Hop*bin/N);
        advance_[bin]=expected+(initialized_ ? wrap(phase-previous_[bin]-expected) : 0);
        magnitude_[bin]=hypotf(real[bin],imag[bin]);
        phase_[bin]=previous_[bin]=phase;
      }
      initialized_=true; captured_=requested_;
      // Capture frame itself is unchanged: phase is not advanced prematurely.
      return;
    }
    for(size_t bin=0;bin<Bins;++bin) {
      magnitude_[bin]*=decay_;
      phase_[bin]=wrap(phase_[bin]+advance_[bin]);
      real[bin]=magnitude_[bin]*cosf(phase_[bin]); imag[bin]=magnitude_[bin]*sinf(phase_[bin]);
    }
    imag[0]=imag[N/2]=0;
    for(size_t bin=1;bin<N/2;++bin) { real[N-bin]=real[bin]; imag[N-bin]=-imag[bin]; }
  }
public:
  static constexpr size_t latencySamples() { return N; }
  void setHold(bool enabled) { requested_=enabled; }
  bool holding() const { return requested_ && captured_; }
  bool setSampleRate(float sr) {
    if(!isfinite(sr) || sr<=0) return false;
    sampleRate_=sr; updateDecay(); return true;
  }
  bool setDecaySeconds(float seconds) {
    if(!isfinite(seconds) || seconds<0) return false;
    decaySeconds_=seconds; updateDecay(); return true;
  }
  // Reset silences history and clears the hold request; rate/decay are retained.
  void reset() {
    frame_.reset(); magnitude_.fill(0); phase_.fill(0); previous_.fill(0); advance_.fill(0);
    requested_=captured_=initialized_=false;
  }
  float tick(float input) {
    return frame_.tick(input,[this](auto& real,auto& imag){this->process(real,imag);});
  }
};
}
