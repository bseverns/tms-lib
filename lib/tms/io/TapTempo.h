#pragma once
#include <stdint.h>
namespace tms { struct TapTempo{ static constexpr int kMax=8; uint32_t taps[kMax]; int n=0;
  void reset(){n=0;} void tap(uint32_t ms){ if(n<kMax) taps[n++]=ms; else { for(int i=1;i<kMax;i++) taps[i-1]=taps[i]; taps[kMax-1]=ms; } }
  float bpm() const { if(n<2) return 120.0f; int total=0; for(int i=1;i<n;i++) total += (int)(taps[i]-taps[i-1]); float avg= (float)total/(float)(n-1); if(avg<1) avg=1; return 60000.0f/avg; } }; }
