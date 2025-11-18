#pragma once
#include <stdint.h>
namespace tms { struct ClockEngine{ float bpm=120.f; inline uint32_t usPerStep() const { return (uint32_t)(60000000.0f/(bpm*4.0f)); } }; }
