#pragma once
#include <stdint.h>
namespace tms { struct ChaosEngine{ uint32_t seed=1; int level=0; void reseed(uint32_t s){seed=s?s:1;} void setLevel(int l){level=l<0?0:(l>7?7:l);} float rand01(){ seed=1664525u*seed+1013904223u; return (seed>>8)*(1.0f/16777216.0f);} }; }
