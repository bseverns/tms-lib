#pragma once
#include "tms/core/Types.h"
#include <array>
#include <stddef.h>
namespace tms {
// Radix-2 complex FFT adapted from fog-bank/src/SpectralFrame.cpp.
// Copyright (c) 2025 Ben Severns. MIT; see repository LICENSE.
// Forward is unnormalized, exp(-j*w); inverse divides by N. In-place arrays.
template<size_t N>
struct FFT {
  static_assert(N >= 2 && (N & (N-1)) == 0, "FFT size must be power of two >= 2");
  using Buffer = std::array<float,N>;
  static void transform(Buffer& real, Buffer& imag, bool inverse = false) {
    size_t j=0;
    for(size_t i=1;i<N;++i) {
      size_t bit=N/2;
      while(j & bit) { j^=bit; bit/=2; }
      j^=bit;
      if(i<j) {
        float r=real[i], q=imag[i]; real[i]=real[j]; imag[i]=imag[j]; real[j]=r; imag[j]=q;
      }
    }
    for(size_t length=2;;length*=2) {
      double angle=(inverse ? 1 : -1)*6.28318530717958647692/length;
      float wr=static_cast<float>(cos(angle)), wi=static_cast<float>(sin(angle));
      for(size_t start=0;start<N;start+=length) {
        float r=1,q=0;
        for(size_t offset=0;offset<length/2;++offset) {
          size_t a=start+offset,b=a+length/2;
          float br=real[b]*r-imag[b]*q, bi=real[b]*q+imag[b]*r;
          float ar=real[a],ai=imag[a];
          real[a]=ar+br; imag[a]=ai+bi; real[b]=ar-br; imag[b]=ai-bi;
          float next=r*wr-q*wi; q=r*wi+q*wr; r=next;
        }
      }
      if(length==N) break;
    }
    if(inverse) for(size_t i=0;i<N;++i) { real[i]/=N; imag[i]/=N; }
  }
};
}
