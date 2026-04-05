#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from horizon/src/MSMatrix.h.
// Mid/side is often the cleanest way to explain stereo width processing:
// encode L/R to M/S, shape M or S, then decode back.
struct MSMatrix {
  inline void encode(float l, float r, float& m, float& s) const {
    m = 0.5f * (l + r);
    s = 0.5f * (l - r);
  }

  inline void decode(float m, float s, float& l, float& r) const {
    l = m + s;
    r = m - s;
  }
};
}
