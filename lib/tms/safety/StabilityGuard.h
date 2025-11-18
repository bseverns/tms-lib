#pragma once
#include "tms/core/Types.h"
namespace tms { struct StabilityGuard{ inline float clampFeedback(float g){ return clampf(g,-0.98f,0.98f);} }; }
