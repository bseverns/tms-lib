#pragma once
#include "tms/core/Types.h"
namespace tms {
struct SoftSaturation{ enum Mode{TANH=0,CUBIC=1,BIAS_TANH=2}; Mode mode=TANH; float drive=1.0f,bias=0.0f;
  inline float process(float x) const { x*=drive; switch(mode){case TANH:return tanhf(x); case CUBIC:return x-(x*x*x/3.0f); case BIAS_TANH:return tanhf(x+bias);} return x; } };
}
