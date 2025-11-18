#pragma once
#include "tms/core/Types.h"
namespace tms { struct EnvelopeFollower{ float aA=0.9f,aR=0.99f, env=0, shape=0.5f, rms=0;
  void setup(float atkMs,float relMs){ auto c=[&](float ms){float tau=ms*0.001f; return expf(-1.0f/(tau*kSampleRate));}; aA=c(atkMs); aR=c(relMs); }
  inline float tick(float x){ float absx=fabsf(x); rms=0.99f*rms + 0.01f*(x*x); float meas=(1.0f-shape)*absx + shape*sqrtf(rms);
    float a=(meas>env)?aA:aR; env=a*env + (1.0f-a)*meas; return env; } }; }
