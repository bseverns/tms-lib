#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from orbit-looper/src/SyncUtils.{h,cpp}.
// Tiny helpers for converting tempo periods into loop lengths and snapping an
// existing loop length to a musical multiplier table.
inline float syncPeriodMsToLoopSeconds(float periodMs, float minSec,
                                       float maxSec) {
  float sec = periodMs * 0.001f;
  if (sec < minSec) {
    sec = minSec;
  }
  if (sec > maxSec) {
    sec = maxSec;
  }
  return sec;
}

inline float syncQuantizeSeconds(float baseSec, float currentSec,
                                 const float* multipliers, int count) {
  if (!multipliers || count <= 0 || baseSec <= 0.0f) {
    return baseSec;
  }
  float best = baseSec * multipliers[0];
  float bestDiff = fabsf(currentSec - best);
  for (int i = 1; i < count; ++i) {
    float candidate = baseSec * multipliers[i];
    float diff = fabsf(currentSec - candidate);
    if (diff < bestDiff) {
      bestDiff = diff;
      best = candidate;
    }
  }
  return best;
}
}
