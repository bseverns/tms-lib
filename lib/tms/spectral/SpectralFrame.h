#pragma once
#include "tms/spectral/FFT.h"
namespace tms {
// Mono streaming STFT/weighted overlap-add, informed by fog-bank SpectralFrame.
// Copyright (c) 2025 Ben Severns. MIT; see repository LICENSE.
// Fixed N samples latency, Hop samples per frame. Square-root PERIODIC Hann
// analysis/synthesis windows; accumulated weights normalize reconstruction.
// Input is zero-padded before startup. Callback receives full complex spectrum
// and must preserve conjugate symmetry for real output. One tick per sample.
template<size_t N = 512, size_t Hop = N/4>
class SpectralFrame {
  static_assert(N >= 4 && Hop > 0 && Hop <= N/2 && N%Hop==0,
                "Use FFT length >= 4 with a dividing hop <= N/2");
  using Buffer = typename FFT<N>::Buffer;
  Buffer input_={}, window_={}, real_={}, imag_={};
  std::array<float,2*N> output_={}, weight_={};
  size_t head_=0, cursor_=0, untilFrame_=Hop;
public:
  SpectralFrame() {
    for(size_t i=0;i<N;++i)
      window_[i]=static_cast<float>(sqrt(0.5-0.5*cos(6.28318530717958647692*i/N)));
  }
  static constexpr size_t latencySamples() { return N; }
  void reset() {
    input_.fill(0); real_.fill(0); imag_.fill(0); output_.fill(0); weight_.fill(0);
    head_=cursor_=0; untilFrame_=Hop;
  }
  template<class Processor>
  float tick(float input, Processor&& processSpectrum) {
    input_[head_]=input; if(++head_==N) head_=0;
    if(--untilFrame_==0) {
      untilFrame_=Hop;
      for(size_t i=0;i<N;++i) { real_[i]=input_[(head_+i)%N]*window_[i]; imag_[i]=0; }
      FFT<N>::transform(real_,imag_);
      processSpectrum(real_,imag_);
      FFT<N>::transform(real_,imag_,true);
      for(size_t i=0;i<N;++i) {
        size_t index=(cursor_+1+i)%(2*N);
        output_[index]+=real_[i]*window_[i]; weight_[index]+=window_[i]*window_[i];
      }
    }
    float result=weight_[cursor_]>1e-6f ? output_[cursor_]/weight_[cursor_] : 0;
    output_[cursor_]=weight_[cursor_]=0;
    if(++cursor_==2*N) cursor_=0;
    return result;
  }
  float tick(float input) { return tick(input, [](Buffer&,Buffer&){}); }
};
}
