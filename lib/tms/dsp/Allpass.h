#pragma once
#include "tms/core/Types.h"
namespace tms { struct Allpass1{ float a=0, z=0; inline void setCoef(float c){a=c;} inline float tick(float x){ float y=-a*x+z; z=x+a*y; return y; } inline void reset(){z=0;} }; }
