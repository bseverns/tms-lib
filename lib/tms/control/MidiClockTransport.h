#pragma once
#include "tms/core/Types.h"
#include <stdint.h>
namespace tms {
// Adapted from lofi-sampler/firmware/platformio/src/ClockTransport.{h,cpp}.
// This turns MIDI realtime bytes into stepped musical transport with optional
// swing. `0xF8` is MIDI Clock, `0xFA` Start, `0xFB` Continue, `0xFC` Stop.
struct MidiClockTransport {
  uint8_t stepsPerBar = 8;
  uint8_t clocksPerStep = 3;
  float swingAmount = 0.0f;

  bool playing = false;
  uint8_t stepIndex = 0;
  uint16_t midiClockCount = 0;

  inline void init(uint8_t steps, uint8_t clocks, float swing) {
    stepsPerBar = steps ? steps : 1;
    clocksPerStep = clocks ? clocks : 1;
    swingAmount = clampf(swing, 0.0f, 1.0f);
    stop();
    stepIndex = 0;
    midiClockCount = 0;
  }

  inline bool handleRealtime(uint8_t statusByte) {
    switch (statusByte) {
    case 0xF8: {
      if (!playing) {
        playing = true;
        midiClockCount = 0;
        stepIndex = 0;
        return true;
      }
      ++midiClockCount;
      uint8_t nextStep = static_cast<uint8_t>((stepIndex + 1u) % stepsPerBar);
      uint8_t clocksNeeded =
          static_cast<uint8_t>(clocksPerStep + swingTicksForStep(nextStep));
      if (midiClockCount >= clocksNeeded) {
        midiClockCount = 0;
        stepIndex = nextStep;
        return true;
      }
      return false;
    }
    case 0xFA:
      playing = true;
      midiClockCount = 0;
      stepIndex = 0;
      return true;
    case 0xFB:
      playing = true;
      return false;
    case 0xFC:
      stop();
      return false;
    default:
      return false;
    }
  }

  inline void stop() { playing = false; }

  inline uint8_t swingTicksForStep(uint8_t nextStep) const {
    static const uint8_t kSwungSteps[] = {1, 3, 5};
    for (uint8_t i = 0; i < sizeof(kSwungSteps) / sizeof(kSwungSteps[0]); ++i) {
      if (nextStep == kSwungSteps[i]) {
        float swing = static_cast<float>(clocksPerStep) * swingAmount;
        if (swing < 0.0f) {
          swing = 0.0f;
        }
        uint8_t ticks = static_cast<uint8_t>(swing + 0.5f);
        uint8_t maxTicks = static_cast<uint8_t>(clocksPerStep / 2u);
        return ticks > maxTicks ? maxTicks : ticks;
      }
    }
    return 0;
  }
};
}
