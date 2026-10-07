#pragma once
#include "tms/sample/SampleView.h"
namespace tms {
// One tick per output sample. Rate is source samples / output sample (1 =
// original speed at matching rates); negative plays backwards. No allocation.
// trigger snapshots rate/mode/fade; retrigger starts at the chosen endpoint.
// Fades are linear and measured in source samples, capped at half the view.
// Loop crossfade overlaps tail with head and skips the consumed head on wrap:
// after the initial traversal, the loop period is size - fade samples.
struct SamplePlayer {
  SampleView source;
  double position = 0.0;
  double step = 1.0;
  size_t fade = 0;
  bool looping = false, reverse = false, playing = false;

  void reset() { position = 0; playing = false; }
  void setSource(SampleView view) { source = view; reset(); }
  void stop() { playing = false; }
  bool active() const { return playing; }
  void trigger(double rate = 1.0, bool loop = false, size_t fadeSamples = 0) {
    reset();
    if (!source.data || !source.size || !isfinite(rate) || rate == 0) return;
    reverse = rate < 0;
    step = fabs(rate);
    looping = loop;
    fade = fadeSamples > source.size / 2 ? source.size / 2 : fadeSamples;
    playing = true;
  }
  float readOriented(double p, bool wrap) const {
    return source.read(reverse ? static_cast<double>(source.size - 1) - p : p, wrap);
  }
  float tick() {
    if (!playing) return 0;
    float output = readOriented(position, looping && fade == 0);
    if (fade) {
      if (looping && position >= static_cast<double>(source.size - fade)) {
        double head = position - static_cast<double>(source.size - fade);
        float mix = static_cast<float>(head / fade);
        output = output * (1 - mix) + readOriented(head, false) * mix;
      } else if (!looping) {
        // Both endpoint samples are zero, suppressing trigger/stop clicks.
        float gain = static_cast<float>(fmin(1.0, fmin(position / fade,
            (static_cast<double>(source.size - 1) - position) / fade)));
        output *= fmaxf(0, gain);
      }
    }
    position += step;
    if (position >= static_cast<double>(source.size)) {
      if (looping) {
        double period = static_cast<double>(source.size - fade);
        position = fade + fmod(position - source.size, period);
      } else stop();
    }
    return output;
  }
};
}
