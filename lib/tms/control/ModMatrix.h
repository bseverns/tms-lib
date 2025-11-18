#pragma once
#include "tms/core/Types.h"
namespace tms { struct ModSlot{ int src=-1,dst=-1; float depth=0; }; struct ModMatrix{ static constexpr int kMax=16; ModSlot slots[kMax]; int n=0;
  void clear(){n=0;} bool add(int s,int d,float dep){ if(n>=kMax) return false; slots[n++]={s,d,dep}; return true; } }; }
