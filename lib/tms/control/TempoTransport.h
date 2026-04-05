#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from tide-engine/core/TempoTransport.{h,cpp} with the tempo-source
// handoff ideas from DiceLoop/src/tempo_sync.cpp.
struct TempoTransport {
  enum class Source : uint8_t { Internal = 0, Tap = 1, MidiClock = 2 };

  static constexpr float kMinBpm = 40.0f;
  static constexpr float kMaxBpm = 240.0f;
  static constexpr uint8_t kTapHistory = 4;
  static constexpr uint8_t kMidiHistory = 24;
  static constexpr uint32_t kTapResetMs = 2000;
  static constexpr uint32_t kMinTapIntervalMs = 250;
  static constexpr uint32_t kMaxTapIntervalMs = 1500;
  static constexpr uint32_t kMidiTimeoutUs = 500000;
  static constexpr uint32_t kMidiResetUs = 250000;
  static constexpr uint32_t kMinMidiPulseUs = 1000;

  float internalBpm = 120.0f;
  float tapBpm = 120.0f;
  float midiBpm = 120.0f;
  float current = 120.0f;
  Source currentSource = Source::Internal;
  Source manualSource = Source::Internal;

  uint32_t lastTapMs = 0;
  uint32_t tapIntervals[kTapHistory] = {};
  uint8_t tapCount = 0;
  uint8_t tapWrite = 0;

  uint32_t lastMidiClockUs = 0;
  uint32_t midiIntervals[kMidiHistory] = {};
  uint8_t midiCount = 0;
  uint8_t midiWrite = 0;

  static inline float clampBpm(float bpm) {
    return clampf(bpm, kMinBpm, kMaxBpm);
  }

  inline float manualBpm() const {
    return (manualSource == Source::Tap) ? tapBpm : internalBpm;
  }

  inline void reset(float bpm = 120.0f) {
    internalBpm = clampBpm(bpm);
    tapBpm = internalBpm;
    midiBpm = internalBpm;
    current = internalBpm;
    currentSource = Source::Internal;
    manualSource = Source::Internal;
    lastTapMs = 0;
    tapCount = 0;
    tapWrite = 0;
    lastMidiClockUs = 0;
    midiCount = 0;
    midiWrite = 0;
    for (uint8_t i = 0; i < kTapHistory; ++i) {
      tapIntervals[i] = 0;
    }
    for (uint8_t i = 0; i < kMidiHistory; ++i) {
      midiIntervals[i] = 0;
    }
  }

  inline void setInternalBpm(float bpm) {
    internalBpm = clampBpm(bpm);
    if (currentSource == Source::Internal) {
      current = internalBpm;
      manualSource = Source::Internal;
    }
  }

  inline bool applyInternalBpmWithPickup(float bpm,
                                         float pickupWindowBpm = 2.0f) {
    setInternalBpm(bpm);
    if (currentSource == Source::Internal) {
      return true;
    }
    if (fabsf(internalBpm - current) > pickupWindowBpm) {
      return false;
    }
    currentSource = Source::Internal;
    manualSource = Source::Internal;
    tapBpm = internalBpm;
    current = internalBpm;
    return true;
  }

  inline void pushTapInterval(uint32_t intervalMs) {
    tapIntervals[tapWrite] = intervalMs;
    tapWrite = static_cast<uint8_t>((tapWrite + 1u) % kTapHistory);
    if (tapCount < kTapHistory) {
      ++tapCount;
    }
  }

  inline bool registerTap(uint32_t nowMs) {
    if (lastTapMs == 0 || nowMs <= lastTapMs ||
        (nowMs - lastTapMs) > kTapResetMs) {
      lastTapMs = nowMs;
      tapCount = 0;
      tapWrite = 0;
      return false;
    }

    uint32_t intervalMs = nowMs - lastTapMs;
    lastTapMs = nowMs;
    if (intervalMs < kMinTapIntervalMs || intervalMs > kMaxTapIntervalMs) {
      tapCount = 0;
      tapWrite = 0;
      return false;
    }

    pushTapInterval(intervalMs);
    uint32_t sumMs = 0;
    for (uint8_t i = 0; i < tapCount; ++i) {
      sumMs += tapIntervals[i];
    }
    float avgMs = static_cast<float>(sumMs) / static_cast<float>(tapCount);
    tapBpm = clampBpm(60000.0f / avgMs);
    manualSource = Source::Tap;
    if (currentSource != Source::MidiClock) {
      currentSource = Source::Tap;
      current = tapBpm;
    }
    return true;
  }

  inline void pushMidiInterval(uint32_t intervalUs) {
    midiIntervals[midiWrite] = intervalUs;
    midiWrite = static_cast<uint8_t>((midiWrite + 1u) % kMidiHistory);
    if (midiCount < kMidiHistory) {
      ++midiCount;
    }
  }

  inline bool registerMidiClock(uint32_t nowUs) {
    if (lastMidiClockUs != 0 && nowUs > lastMidiClockUs) {
      uint32_t intervalUs = nowUs - lastMidiClockUs;
      if (intervalUs > kMidiResetUs) {
        midiCount = 0;
        midiWrite = 0;
      } else if (intervalUs >= kMinMidiPulseUs) {
        pushMidiInterval(intervalUs);
      }
    }
    lastMidiClockUs = nowUs;

    if (midiCount < 12) {
      return false;
    }

    uint64_t sumUs = 0;
    for (uint8_t i = 0; i < midiCount; ++i) {
      sumUs += midiIntervals[i];
    }
    float avgPulseUs =
        static_cast<float>(sumUs) / static_cast<float>(midiCount);
    midiBpm = clampBpm(60000000.0f / (avgPulseUs * 24.0f));
    currentSource = Source::MidiClock;
    current = midiBpm;
    return true;
  }

  inline bool update(uint32_t nowUs) {
    if (currentSource != Source::MidiClock || lastMidiClockUs == 0 ||
        nowUs <= lastMidiClockUs) {
      return false;
    }
    if ((nowUs - lastMidiClockUs) <= kMidiTimeoutUs) {
      return false;
    }
    stopMidiClock();
    return true;
  }

  inline void resetMidiClock() {
    lastMidiClockUs = 0;
    midiCount = 0;
    midiWrite = 0;
    for (uint8_t i = 0; i < kMidiHistory; ++i) {
      midiIntervals[i] = 0;
    }
  }

  inline void stopMidiClock() {
    resetMidiClock();
    currentSource = manualSource;
    current = manualBpm();
  }

  inline float bpm() const { return current; }
  inline float quarterNoteMs() const { return 60000.0f / current; }
  inline float samplesPerBeat(float sr = static_cast<float>(kSampleRate)) const {
    return (60.0f * sr) / current;
  }
};
}
