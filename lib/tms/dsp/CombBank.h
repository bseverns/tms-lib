#pragma once
#include "tms/control/ScaleMap.h"
#include "tms/core/Types.h"
#include <array>
#include <stddef.h>
#include <stdint.h>
namespace tms {
// Adapted from silt/src/CombBank.{h,cpp}.
// Tuned bank of feedback combs for Karplus/plate-adjacent resonator studies.
struct CombBank {
  static constexpr size_t kMaxCombs = 16;
  static constexpr size_t kMaxDelaySamples = 1024;

  std::array<std::array<float, kMaxDelaySamples>, kMaxCombs> buffers = {};
  std::array<size_t, kMaxCombs> writeHeads = {};
  std::array<size_t, kMaxCombs> delaySamples = {};
  std::array<float, kMaxCombs> lowpassStates = {};
  std::array<float, kMaxCombs> feedbacks = {};
  std::array<float, kMaxCombs> pan = {};
  std::array<float, kMaxCombs> shakeCents = {};

  ScaleMap scaleMap = {};
  ScaleMap::Scale scale = ScaleMap::Chromatic;
  float baseHz = 110.0f;
  float damping = 0.6f;
  float brightness = 0.4f;
  float detuneCents = 6.0f;
  float spread = 0.3f;
  size_t combCount = 8;

  inline CombBank() {
    reset();
    retune();
  }

  static inline float centsToRatio(float cents) {
    return powf(2.0f, cents / 1200.0f);
  }

  inline void reset() {
    for (size_t i = 0; i < kMaxCombs; ++i) {
      buffers[i].fill(0.0f);
    }
    writeHeads.fill(0);
    lowpassStates.fill(0.0f);
  }

  inline void setBaseHz(float hz) {
    baseHz = clampf(hz, 55.0f, 1760.0f);
    retune();
  }

  inline void setScale(ScaleMap::Scale value) {
    scale = value;
    retune();
  }

  inline void setCombCount(size_t count) {
    combCount = count < 1 ? 1 : (count > kMaxCombs ? kMaxCombs : count);
    retune();
  }

  inline void setDamping(float value) {
    damping = clampf(value, 0.0f, 1.0f);
    retune();
  }

  inline void setBrightness(float value) {
    brightness = clampf(value, 0.0f, 1.0f);
  }

  inline void setDetuneCents(float cents) {
    detuneCents = clampf(cents, 0.0f, 50.0f);
    retune();
  }

  inline void setSpread(float value) {
    spread = clampf(value, 0.0f, 1.0f);
    retune();
  }

  inline void setShake(float amount, uint32_t seed) {
    float shakeRange = clampf(amount, 0.0f, 1.0f) * 14.0f;
    uint32_t state = seed ? seed : 0x9E3779B9u;
    for (size_t i = 0; i < kMaxCombs; ++i) {
      state ^= state << 13;
      state ^= state >> 17;
      state ^= state << 5;
      float normalized = static_cast<float>(state & 0xffffu) / 65535.0f;
      shakeCents[i] = (normalized * 2.0f - 1.0f) * shakeRange;
    }
    retune();
  }

  inline float jitteredDetuneForVoice(size_t voice) const {
    if (combCount <= 1) {
      return shakeCents[voice];
    }
    float centered =
        (static_cast<float>(voice) / static_cast<float>(combCount - 1)) * 2.0f -
        1.0f;
    return centered * detuneCents + shakeCents[voice];
  }

  inline void retune() {
    float resonance = 1.0f - damping;
    float baseFeedback =
        clampf(0.08f + resonance * resonance * 0.9f, 0.08f, 0.985f);

    for (size_t i = 0; i < kMaxCombs; ++i) {
      float hz = scaleMap.frequencyForDegree(baseHz, i, scale) *
                 centsToRatio(jitteredDetuneForVoice(i));
      float clampedHz = hz < 40.0f ? 40.0f : hz;
      size_t delay =
          static_cast<size_t>(roundf(static_cast<float>(kSampleRate) / clampedHz));
      if (delay < 8) {
        delay = 8;
      }
      if (delay >= kMaxDelaySamples) {
        delay = kMaxDelaySamples - 1;
      }
      delaySamples[i] = delay;

      if (combCount <= 1) {
        pan[i] = 0.0f;
      } else {
        float centered = (static_cast<float>(i) /
                          static_cast<float>(combCount - 1)) *
                             2.0f -
                         1.0f;
        pan[i] = centered * spread;
      }

      float voiceTrim = 0.0125f * static_cast<float>(i % 4);
      feedbacks[i] = clampf(baseFeedback - voiceTrim, 0.25f, 0.97f);
    }
  }

  inline StereoFrame process(float input) {
    StereoFrame output;
    float bright = 0.03f + brightness * brightness * 0.94f;

    for (size_t i = 0; i < combCount; ++i) {
      size_t writeIndex = writeHeads[i];
      size_t delay = delaySamples[i];
      size_t readIndex =
          (writeIndex + kMaxDelaySamples - delay) % kMaxDelaySamples;

      float delayed = buffers[i][readIndex];
      lowpassStates[i] += (delayed - lowpassStates[i]) * bright;
      float filtered = lowpassStates[i];

      float excite =
          input * (0.7f + 0.35f * brightness + 0.1f * (1.0f - damping));
      buffers[i][writeIndex] = excite + filtered * feedbacks[i];
      writeHeads[i] = (writeIndex + 1) % kMaxDelaySamples;

      float leftWeight = 0.5f * (1.0f - pan[i]);
      float rightWeight = 0.5f * (1.0f + pan[i]);
      output.left += filtered * leftWeight;
      output.right += filtered * rightWeight;
    }

    float normalize = 0.34f / sqrtf(static_cast<float>(combCount));
    output.left *= normalize;
    output.right *= normalize;
    return output;
  }
};
}
