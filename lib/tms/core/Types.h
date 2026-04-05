#pragma once
// See docs/TEACHING_GUIDE.md for pedagogy behind these choices.
#include <stdint.h>
#include <math.h>
namespace tms {
  using f32=float; using f64=double;
  constexpr f32 kPi=3.14159265f;
  constexpr int kSampleRate=44100;   // global sample rate (Hz)
  constexpr int kBlockSize =128;     // audio callback quantum (samples)
  struct StereoFrame{ f32 left=0.0f,right=0.0f; };
  inline f32 clampf(f32 x,f32 lo,f32 hi){return x<lo?lo:(x>hi?hi:x);}
  inline f32 dbToLinear(f32 db){return powf(10.0f, db*0.05f);}
  inline f32 linearToDb(f32 x){return 20.0f*log10f(fmaxf(x,1e-12f));}
  inline f32 midiNoteToHz(f32 n){return 440.0f*powf(2.0f,(n-69.0f)/12.0f);}
  inline f32 zapDenorm(f32 x){return (fabsf(x)<1e-30f)?0.0f:x;}
}
