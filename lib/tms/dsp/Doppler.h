#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from orbit-looper/src/Doppler.{h,cpp}.
// Fractional stereo sample reader for moving read-head and orbiting-source
// examples.
struct Doppler {
  const int16_t* buffer = nullptr;
  uint32_t frames = 0;
  uint32_t channels = 2;

  inline void setBuffer(const int16_t* buf, uint32_t frameCount,
                        uint32_t channelCount) {
    buffer = buf;
    frames = frameCount;
    channels = channelCount;
  }

  inline void readAt(float pos, float& outL, float& outR) const {
    if (!buffer || frames == 0 || channels < 2) {
      outL = 0.0f;
      outR = 0.0f;
      return;
    }

    while (pos < 0.0f) {
      pos += static_cast<float>(frames);
    }
    while (pos >= static_cast<float>(frames)) {
      pos -= static_cast<float>(frames);
    }

    uint32_t idx0 = static_cast<uint32_t>(pos);
    uint32_t idx1 = idx0 + 1;
    if (idx1 >= frames) {
      idx1 = 0;
    }
    float frac = pos - static_cast<float>(idx0);

    int16_t s0L = buffer[idx0 * channels + 0];
    int16_t s1L = buffer[idx1 * channels + 0];
    int16_t s0R = buffer[idx0 * channels + 1];
    int16_t s1R = buffer[idx1 * channels + 1];

    outL = static_cast<float>(s0L) +
           (static_cast<float>(s1L) - static_cast<float>(s0L)) * frac;
    outR = static_cast<float>(s0R) +
           (static_cast<float>(s1R) - static_cast<float>(s0R)) * frac;
  }

  inline void process(float& readPos, float stepSize, float& outL,
                      float& outR) const {
    readAt(readPos, outL, outR);
    readPos += stepSize;
    while (readPos >= static_cast<float>(frames)) {
      readPos -= static_cast<float>(frames);
    }
  }
};
}
