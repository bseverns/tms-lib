#pragma once
#include "tms/core/Types.h"
#include <stddef.h>
namespace tms {
// Symmetric Hann window for a finite grain: both endpoint samples are zero.
// Lengths below three cannot form a useful Hann grain and return silence.
inline float hannWindow(size_t index, size_t length) {
  if (length < 3 || index == 0 || index >= length - 1) return 0.0f;
  double phase = static_cast<double>(index) / static_cast<double>(length - 1);
  return static_cast<float>(0.5 - 0.5 * cos(6.28318530717958647692 * phase));
}
}
