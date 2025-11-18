#pragma once
#include "tms/core/Types.h"
namespace tms { enum class Scale{Linear,Log}; struct ParamSpec{ const char* id; float minv,maxv,defv; Scale scale; float smoothMs; };
struct Param{ ParamSpec spec; float cur=0, tgt=0, a=0;
  void init(const ParamSpec& s){ spec=s; cur=tgt=s.defv; setSmoothMs(s.smoothMs); }
  void setSmoothMs(float ms){ float tau=ms*0.001f; a= (tau>0)? expf(-1.0f/(tau*kSampleRate)) : 0.0f; }
  void setTarget(float v){ tgt=clampf(v,spec.minv,spec.maxv); }
  inline float tick(){ cur = a*cur + (1.0f-a)*tgt; return cur; } };
}
