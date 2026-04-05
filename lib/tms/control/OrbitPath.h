#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from orbit-looper/src/PathGen.{h,cpp}.
// Generates simple 2D motion paths for panning, spatial modulation, or UI.
struct OrbitPath {
  enum PathType : int { Circle = 0, Ellipse = 1, Lemniscate = 2 };

  int type = Ellipse;
  float radius = 1.0f;
  float height = 0.0f;

  inline void setType(int pathType) {
    if (pathType < Circle) {
      pathType = Circle;
    }
    if (pathType > Lemniscate) {
      pathType = Lemniscate;
    }
    type = pathType;
  }

  inline void setRadius(float r) { radius = clampf(r, 0.1f, 5.0f); }
  inline void setHeight(float h) { height = clampf(h, -1.0f, 1.0f); }

  inline static float wrap01(float phase) {
    phase -= floorf(phase);
    if (phase < 0.0f) {
      phase += 1.0f;
    }
    return phase;
  }

  inline void getXY(float phase, float& x, float& y) const {
    float p = wrap01(phase);
    float s = sinf(p * 6.28318531f);
    float c = cosf(p * 6.28318531f);
    switch (type) {
    case Circle:
      x = c * radius;
      y = s * radius;
      break;
    case Ellipse:
      x = c * radius;
      y = s * radius * 0.6f;
      break;
    case Lemniscate:
    default:
      x = c * radius;
      y = (s * c) * (2.0f * radius);
      break;
    }
  }
};
}
