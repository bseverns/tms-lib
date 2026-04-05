#pragma once
#include "tms/core/Types.h"
namespace tms {
// Adapted from fog-bank/src/PresenceKeeper.{h,cpp}.
// It estimates how much upper-mid "presence" exists in the wet signal and
// pushes that band back up when it falls behind.
struct PresenceKeeper {
  struct ChannelState {
    float lowState = 0.0f;
    float highState = 0.0f;
  };

  float sampleRate = static_cast<float>(kSampleRate);
  float strength = 0.75f;
  float targetRatio = 0.20f;
  float lowCoeff = 0.0f;
  float highCoeff = 0.0f;
  float attackCoeff = 0.0f;
  float releaseCoeff = 0.0f;
  float lastPresenceRatio = 1.0f;
  float lastPresenceDeficit = 0.0f;
  float wetEnv = 0.0f;
  float bandEnv = 0.0f;
  ChannelState left;
  ChannelState right;

  static constexpr float kLowHz = 900.0f;
  static constexpr float kHighHz = 3200.0f;
  static constexpr float kAttackMs = 5.0f;
  static constexpr float kReleaseMs = 90.0f;
  static constexpr float kMaxBoostDb = 6.0f;
  static constexpr float kMinActivity = 0.015f;

  inline PresenceKeeper() { updateCoefficients(); }

  static inline float coeffFor(float sr, float hz) {
    return 1.0f - expf((-2.0f * 3.14159265f * hz) / sr);
  }

  static inline float smoothingCoeff(float sr, float timeMs) {
    float seconds = timeMs * 0.001f;
    return 1.0f - expf(-1.0f / (sr * seconds));
  }

  static inline void updateEnvelope(float input, float atk, float rel,
                                    float& envelope) {
    float coeff = (input > envelope) ? atk : rel;
    envelope += coeff * (input - envelope);
  }

  static inline float processBand(ChannelState& st, float input, float lowC,
                                  float highC) {
    st.lowState += lowC * (input - st.lowState);
    st.highState += highC * (input - st.highState);
    return st.highState - st.lowState;
  }

  inline void setSampleRate(float sr) {
    sampleRate = (sr > 1.0f) ? sr : static_cast<float>(kSampleRate);
    updateCoefficients();
  }

  inline void setStrength(float value) { strength = clampf(value, 0.0f, 1.0f); }

  inline void setTargetRatio(float ratio) {
    targetRatio = clampf(ratio, 0.05f, 0.5f);
  }

  inline void reset() {
    lastPresenceRatio = 1.0f;
    lastPresenceDeficit = 0.0f;
    wetEnv = 0.0f;
    bandEnv = 0.0f;
    left = ChannelState{};
    right = ChannelState{};
  }

  inline void updateCoefficients() {
    lowCoeff = coeffFor(sampleRate, kLowHz);
    highCoeff = coeffFor(sampleRate, kHighHz);
    attackCoeff = smoothingCoeff(sampleRate, kAttackMs);
    releaseCoeff = smoothingCoeff(sampleRate, kReleaseMs);
  }

  inline void process(float& inL, float& inR) {
    float bandL = processBand(left, inL, lowCoeff, highCoeff);
    float bandR = processBand(right, inR, lowCoeff, highCoeff);

    float wetMag = 0.5f * (fabsf(inL) + fabsf(inR));
    float bandMag = 0.5f * (fabsf(bandL) + fabsf(bandR));
    updateEnvelope(wetMag, attackCoeff, releaseCoeff, wetEnv);
    updateEnvelope(bandMag, attackCoeff, releaseCoeff, bandEnv);

    lastPresenceRatio = bandEnv / (wetEnv + 1.0e-5f);
    float activity =
        clampf((wetEnv - kMinActivity) / (0.08f - kMinActivity), 0.0f, 1.0f);
    float deficit =
        clampf((targetRatio - lastPresenceRatio) / targetRatio, 0.0f, 1.0f);
    lastPresenceDeficit = deficit * activity;

    float maxBoostLin = dbToLinear(kMaxBoostDb);
    float boost = 1.0f + (maxBoostLin - 1.0f) * lastPresenceDeficit * strength;
    inL += bandL * (boost - 1.0f);
    inR += bandR * (boost - 1.0f);
  }
};
}
