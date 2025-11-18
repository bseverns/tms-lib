#pragma once
#include "tms/core/Types.h"
namespace tms { struct Biquad{ float b0=1,b1=0,b2=0,a0=1,a1=0,a2=0,z1=0,z2=0;
  inline void reset(){z1=z2=0;} inline float tick(float x){ float y=b0*x+z1; z1=b1*x - a1*y + z2; z2=b2*x - a2*y; return y; }
  inline void norm(){ b0/=a0;b1/=a0;b2/=a0;a1/=a0;a2/=a0;a0=1; }
  static Biquad makeLowpass(float sr,float fc,float Q){ Biquad q; float w0=2*3.14159265f*fc/sr; float a=sinf(w0)/(2*Q); float c=cosf(w0);
    q.b0=(1-c)/2; q.b1=1-c; q.b2=(1-c)/2; q.a0=1+a; q.a1=-2*c; q.a2=1-a; q.norm(); return q; }
  static Biquad makeHighpass(float sr,float fc,float Q){ Biquad q; float w0=2*3.14159265f*fc/sr; float a=sinf(w0)/(2*Q); float c=cosf(w0);
    q.b0=(1+c)/2; q.b1=-(1+c); q.b2=(1+c)/2; q.a0=1+a; q.a1=-2*c; q.a2=1-a; q.norm(); return q; }
  static Biquad makePeaking(float sr,float fc,float Q,float dB){ Biquad q; float A=powf(10,dB/40.0f); float w0=2*3.14159265f*fc/sr; float a=sinf(w0)/(2*Q); float c=cosf(w0);
    q.b0=1+a*A; q.b1=-2*c; q.b2=1-a*A; q.a0=1+a/A; q.a1=-2*c; q.a2=1-a/A; q.norm(); return q; } }; }
