#pragma once
#include "tms/core/Types.h"
#include <string.h>
namespace tms {
struct DelayLine{
  static constexpr int kMax=2*kSampleRate; float buf[kMax]; int w=0, len=kMax;
  DelayLine(){clear();}
  void clear(){ memset(buf,0,sizeof(buf)); w=0;}
  void setMaxDelayMs(float ms){ int want=(int)(ms*0.001f*kSampleRate); if(want<2)want=2; if(want>kMax)want=kMax; len=want; clear();}
  inline void write(float s){ buf[w]=s; w=(w+1)%len; }
  inline float read(float d){
    float idx=(float)w - d; while(idx<0) idx+=len; while(idx>=len) idx-=len;
    int i0=(int)idx; int i1=(i0+1)%len; float a=idx-i0; return (1-a)*buf[i0] + a*buf[i1];
  }
};}
