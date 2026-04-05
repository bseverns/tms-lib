#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from rhythm-weave/lib/RhythmWeaveCore/src/RhythmWeaveMidiClock.{h,cpp}.
// This version is useful when you want a transport that can either emit
// internal MIDI-clock pulses or accept external pulses and report step
// boundaries.
struct ClockPulseTransport {
  static constexpr uint8_t kClocksPerQuarterNote = 24;
  static constexpr uint8_t kClocksPerStep = 6;

  bool isRunning = false;
  uint8_t clocksIntoStep = 0;
  uint32_t usPerClock = 20833;
  uint64_t nextClockUs = 0;

  static inline uint32_t bpmToUsPerClock(float bpm) {
    float safeBpm = (bpm < 1.0f) ? 1.0f : bpm;
    return static_cast<uint32_t>(60000000.0f /
                                 (safeBpm * kClocksPerQuarterNote));
  }

  inline void reset(float bpm, uint64_t nowUs = 0) {
    setTempo(bpm);
    isRunning = false;
    clocksIntoStep = 0;
    nextClockUs = nowUs;
  }

  inline void setTempo(float bpm) { usPerClock = bpmToUsPerClock(bpm); }

  inline void startInternal(uint64_t nowUs) {
    isRunning = true;
    clocksIntoStep = 0;
    nextClockUs = nowUs;
  }

  inline void continueInternal(uint64_t nowUs) {
    isRunning = true;
    nextClockUs = nowUs;
  }

  inline void startExternal() {
    isRunning = true;
    clocksIntoStep = 0;
  }

  inline void continueExternal() { isRunning = true; }

  inline void cueExternalStepBoundary() { clocksIntoStep = 0; }

  inline void stop() { isRunning = false; }

  inline uint8_t drainInternalClocks(uint64_t nowUs) {
    if (!isRunning) {
      return 0;
    }
    uint8_t emitted = 0;
    while (nextClockUs <= nowUs) {
      ++emitted;
      nextClockUs += usPerClock;
      clocksIntoStep =
          static_cast<uint8_t>((clocksIntoStep + 1u) % kClocksPerStep);
    }
    return emitted;
  }

  inline bool pushExternalClock() {
    if (!isRunning) {
      return false;
    }
    bool boundary = clocksIntoStep == 0;
    clocksIntoStep =
        static_cast<uint8_t>((clocksIntoStep + 1u) % kClocksPerStep);
    return boundary;
  }
};
}
