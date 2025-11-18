#pragma once
#include "tms/core/Types.h"
#include "tms/dsp/Biquad.h"
namespace tms { struct TiltEQ{ float tilt=0; Biquad low,high;
  void setTilt(float dbPerOct,float sr){ tilt=dbPerOct; float center=800.0f; float amt=dbPerOct*0.8f; low=Biquad::makePeaking(sr,center/2,0.5f, amt); high=Biquad::makePeaking(sr,center*2,0.5f,-amt); }
  inline float tick(float x){ return high.tick(low.tick(x)); } void reset(){low.reset(); high.reset();} }; }
