#pragma once
#include <stdint.h>
namespace tms { struct LEDBar{ uint8_t level=0; inline void set(uint8_t v){ level=(v>7)?7:v; } }; }
