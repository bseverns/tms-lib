#pragma once
#include <stdint.h>
namespace tms { struct Profiler{ uint32_t startUs=0,maxUs=0; void start(uint32_t t){startUs=t;} void stop(uint32_t t){uint32_t d=t-startUs; if(d>maxUs) maxUs=d;} }; }
