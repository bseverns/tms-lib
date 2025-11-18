#pragma once
#include "tms/core/Types.h"
namespace tms { struct LimiterLookahead{ float ceiling=dbToLinear(-1.0f); float releaseMs=50.0f;
  inline void setCeilingDb(float db){ ceiling=dbToLinear(db);} inline void setReleaseMs(float ms){releaseMs=ms;}
  inline float tick(float x){ if(x>ceiling) return ceiling; if(x<-ceiling) return -ceiling; return x; } }; }
