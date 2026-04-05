#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from seedbox/include/util/ScaleQuantizer.h and
// seedbox/src/util/ScaleQuantizer.cpp.
// Snap arbitrary semitone offsets back onto a chosen scale.
struct ScaleQuantizer {
  enum class Scale : uint8_t {
    Chromatic = 0,
    Major,
    Minor,
    PentatonicMajor,
    PentatonicMinor,
  };

  static inline int clampRoot(int root) {
    int normalized = root % 12;
    return normalized < 0 ? normalized + 12 : normalized;
  }

  static inline void degreesFor(Scale scale, const int*& degrees, int& count) {
    static const int kChromatic[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    static const int kMajor[] = {0, 2, 4, 5, 7, 9, 11};
    static const int kMinor[] = {0, 2, 3, 5, 7, 8, 10};
    static const int kPentMaj[] = {0, 2, 4, 7, 9};
    static const int kPentMin[] = {0, 3, 5, 7, 10};
    switch (scale) {
    case Scale::Chromatic:
      degrees = kChromatic;
      count = 12;
      return;
    case Scale::Major:
      degrees = kMajor;
      count = 7;
      return;
    case Scale::Minor:
      degrees = kMinor;
      count = 7;
      return;
    case Scale::PentatonicMajor:
      degrees = kPentMaj;
      count = 5;
      return;
    case Scale::PentatonicMinor:
    default:
      degrees = kPentMin;
      count = 5;
      return;
    }
  }

  static inline float snap(float semitoneOffset, uint8_t root, Scale scale,
                           int direction) {
    const int* degrees = nullptr;
    int count = 0;
    degreesFor(scale, degrees, count);
    if (!degrees || count <= 0) {
      return semitoneOffset;
    }

    int normalizedRoot = clampRoot(root);
    float target = semitoneOffset;
    float relative = target - static_cast<float>(normalizedRoot);
    int baseOctave = static_cast<int>(floorf(relative / 12.0f));
    float bestPitch = target;
    float bestDistance = 1.0e30f;

    for (int octave = baseOctave - 2; octave <= baseOctave + 2; ++octave) {
      for (int i = 0; i < count; ++i) {
        float candidate =
            static_cast<float>(octave * 12 + normalizedRoot + degrees[i]);
        float delta = candidate - target;
        float absDelta = fabsf(delta);
        if (direction > 0 && delta < 0.0f) {
          continue;
        }
        if (direction < 0 && delta > 0.0f) {
          continue;
        }
        if (absDelta < bestDistance ||
            (absDelta == bestDistance && candidate < bestPitch)) {
          bestPitch = candidate;
          bestDistance = absDelta;
        }
      }
    }
    return bestPitch;
  }

  static inline float nearest(float semitoneOffset, uint8_t root, Scale scale) {
    return snap(semitoneOffset, root, scale, 0);
  }

  static inline float up(float semitoneOffset, uint8_t root, Scale scale) {
    return snap(semitoneOffset, root, scale, 1);
  }

  static inline float down(float semitoneOffset, uint8_t root, Scale scale) {
    return snap(semitoneOffset, root, scale, -1);
  }
};
}
