#pragma once
#include "tms/sample/GrainVoice.h"
#include "tms/core/XorShift32.h"
#include <array>
namespace tms {
// Fixed-capacity, sample-clocked periodic scheduler. Position spray is in
// SOURCE samples; interval and duration are OUTPUT samples. One tick/sample.
// Full pools drop new grains (no stealing). Summed output is not normalized.
template<size_t Voices = 8>
struct GrainCloud {
  static_assert(Voices > 0, "GrainCloud needs voices");
  std::array<GrainVoice, Voices> voices = {};
  SampleView source;
  XorShift32 random;
  size_t interval = 1, duration = 3, untilNext = 0;
  size_t launched = 0, dropped = 0;
  double position = 0, rate = 1;
  float spray = 0, gain = 0.2f;
  bool running = false;
  // Restart clears voices/counters and seeds randomness. Invalid setup stops.
  bool start(SampleView view, size_t spacing, size_t length, double startPosition,
             double speed = 1, float spread = 0, float level = 0.2f,
             uint32_t seed = 1) {
    stop(); launched = dropped = 0; untilNext = 0;
    if (!view.data || !view.size || !spacing || length < 3 ||
        !isfinite(startPosition) || !isfinite(speed) || !isfinite(spread) ||
        spread < 0 || !isfinite(level) || level < 0) return false;
    source = view; interval = spacing; duration = length;
    position = startPosition; rate = speed; spray = spread;
    gain = clampf(level, 0, 1); random.seed(seed); running = true;
    return true;
  }
  void stop() { running = false; for (auto& voice : voices) voice.stop(); }
  size_t activeVoices() const {
    size_t count = 0; for (const auto& voice : voices) count += voice.active();
    return count;
  }
  float tick() {
    if (!running) return 0;
    if (untilNext == 0) {
      // Draw on every scheduled event, including drops, for stable RNG cadence.
      double offset = static_cast<double>(random.uniformSigned()) * spray;
      bool admitted = false;
      for (auto& voice : voices) {
        if (!voice.active()) {
          admitted = voice.trigger(source, position + offset, rate, duration, gain);
          break;
        }
      }
      if (admitted) ++launched; else ++dropped;
      untilNext = interval;
    }
    --untilNext;
    float out = 0; for (auto& voice : voices) out += voice.tick();
    return out;
  }
};
}
