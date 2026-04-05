#pragma once
#include "tms/core/Types.h"
#include <stddef.h>
namespace tms {
// Adapted from silt/src/ScaleMap.{h,cpp}.
// Maps integer degrees into scale ratios so resonators and quantized systems
// can share the same tuning tables.
struct ScaleMap {
  enum Scale : int {
    Chromatic = 0,
    PentatonicMinor = 1,
    JustIntonation = 2,
    Pelog = 3,
    Gamelan = 4,
  };

  inline float tableRatioForDegree(const float* ratios, size_t count,
                                   size_t degree) const {
    if (count == 0) {
      return 1.0f;
    }
    size_t octave = degree / count;
    size_t step = degree % count;
    return ratios[step] * powf(2.0f, static_cast<float>(octave));
  }

  inline float ratioForDegree(size_t degree, Scale scale) const {
    static const float kPentatonicMinorRatios[] = {
        1.0f, 1.1892071f, 1.3348398f, 1.4983071f, 1.7817974f,
    };
    static const float kJustIntonationRatios[] = {
        1.0f, 16.0f / 15.0f, 9.0f / 8.0f,  6.0f / 5.0f,
        5.0f / 4.0f, 4.0f / 3.0f,  45.0f / 32.0f, 3.0f / 2.0f,
        8.0f / 5.0f, 5.0f / 3.0f,  9.0f / 5.0f,   15.0f / 8.0f,
    };
    static const float kPelogRatios[] = {
        1.0f, 1.0699845f, 1.2009369f, 1.4142135f,
        1.4983071f, 1.6817929f, 1.8877486f,
    };
    static const float kGamelanRatios[] = {
        1.0f, 1.1486983f, 1.3195080f, 1.5157166f, 1.7411011f,
    };

    switch (scale) {
    case Chromatic:
      return powf(2.0f, static_cast<float>(degree) / 12.0f);
    case PentatonicMinor:
      return tableRatioForDegree(
          kPentatonicMinorRatios,
          sizeof(kPentatonicMinorRatios) / sizeof(kPentatonicMinorRatios[0]),
          degree);
    case JustIntonation:
      return tableRatioForDegree(
          kJustIntonationRatios,
          sizeof(kJustIntonationRatios) / sizeof(kJustIntonationRatios[0]),
          degree);
    case Pelog:
      return tableRatioForDegree(kPelogRatios,
                                 sizeof(kPelogRatios) / sizeof(kPelogRatios[0]),
                                 degree);
    case Gamelan:
      return tableRatioForDegree(
          kGamelanRatios,
          sizeof(kGamelanRatios) / sizeof(kGamelanRatios[0]), degree);
    }
    return 1.0f;
  }

  inline float frequencyForDegree(float baseHz, size_t degree, Scale scale) const {
    if (baseHz <= 0.0f) {
      return 0.0f;
    }
    return baseHz * ratioForDegree(degree, scale);
  }

  inline size_t notesPerOctave(Scale scale) const {
    switch (scale) {
    case Chromatic:
    case JustIntonation:
      return 12;
    case PentatonicMinor:
    case Gamelan:
      return 5;
    case Pelog:
      return 7;
    }
    return 12;
  }
};
}
